/* distance-ir-v2-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * distance.c: Calculate distance from ADC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "distance.h"

#include <string.h>

#include "configs/config.h"
#include "configs/config_distance.h"

#include "xmc_vadc.h"
#include "xmc_gpio.h"

#include "communication.h"

#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/hal/uartbb/uartbb.h"
#include "bricklib2/hal/ccu4_pwm/ccu4_pwm.h"
#include "bricklib2/utility/util_definitions.h"

#define DISTANCE_STAGE1_MAX_LENGTH 128
#define DISTANCE_DIVIDER (4096*4*DISTANCE_STAGE1_MAX_LENGTH/DISTANCE_LOOKUP_SIZE)

#define distance_adc_irq_handler IRQ_Hdlr_15

// Keep the variables that are used in the interrupts as simple global variables,
// to be sure that the compiler does not need to do any derefernciation or similar.
static uint32_t distance_stage1_length = 0;
static uint32_t distance_stage1_sum    = 0;
static volatile uint32_t distance_stage1_value  = 0;

Distance distance;

void __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) distance_adc_irq_handler(void) {
	distance_stage1_sum += XMC_VADC_GROUP_GetDetailedResult(VADC_G0, DISTANCE_ADC_RESULT_REG) & 0xFFFF;
	distance_stage1_length++;
	if(distance_stage1_length >= DISTANCE_STAGE1_MAX_LENGTH) {
		distance_stage1_value = distance_stage1_sum;
		distance_stage1_length = 0;
		distance_stage1_sum = 0;
	}
}

void distance_sensor_type_write(void)  {
	uint32_t page[EEPROM_PAGE_SIZE/sizeof(uint32_t)];

	page[DISTANCE_SENSOR_TYPE_MAGIC_POS]  = DISTANCE_SENSOR_TYPE_MAGIC;
	page[DISTANCE_SENSOR_TYPE_SENSOR_POS] = distance.sensor;

	bootloader_write_eeprom_page(DISTANCE_SENSOR_TYPE_PAGE, page);
}

void distance_sensor_type_read(void) {
	uint32_t page[EEPROM_PAGE_SIZE/sizeof(uint32_t)];
	bootloader_read_eeprom_page(DISTANCE_SENSOR_TYPE_PAGE, page);

	// The magic number is not where it is supposed to be.
	// This is either our first startup or something went wrong.
	// We initialize the calibration data with sane default values.
	if(page[0] != DISTANCE_SENSOR_TYPE_MAGIC) {
		distance.sensor = DISTANCE_IR_V2_SENSOR_TYPE_2Y0A21;
		distance_sensor_type_write();
		return;
	}

	distance.sensor = page[DISTANCE_SENSOR_TYPE_SENSOR_POS];
}

void distance_init_adc(void) {
	// This structure contains the Global related Configuration.
	const XMC_VADC_GLOBAL_CONFIG_t adc_global_config = {
		.boundary0 = 0, // Lower boundary value for Normal comparison mode
		.boundary1 = 0, // Upper boundary value for Normal comparison mode

		.class0 = {
			.sample_time_std_conv     = 31,                      // The Sample time is (2*tadci)
			.conversion_mode_standard = XMC_VADC_CONVMODE_12BIT, // 12bit conversion Selected
		},
		.class1 = {
			.sample_time_std_conv     = 31,                      // The Sample time is (2*tadci)
		},

		.data_reduction_control         = 0b11, // Accumulate 4 result values
		.wait_for_read_mode             = 0, // GLOBRES Register will not be overwritten until the previous value is read
		.event_gen_enable               = 0, // Result Event from GLOBRES is disabled
		.disable_sleep_mode_control     = 0  // Sleep mode is enabled
	};


	// Global iclass0 configuration
	const XMC_VADC_GLOBAL_CLASS_t adc_global_iclass_config = {
		.conversion_mode_standard = XMC_VADC_CONVMODE_12BIT,
		.sample_time_std_conv	  = 31,
	};

	// Global Result Register configuration structure
	XMC_VADC_RESULT_CONFIG_t adc_global_result_config = {
		.data_reduction_control = 0b11,  // Accumulate 4 result values
		.post_processing_mode   = XMC_VADC_DMM_REDUCTION_MODE,
		.wait_for_read_mode  	= 1, // Enabled
		.part_of_fifo       	= 0, // No FIFO
		.event_gen_enable   	= 0  // Disable Result event
	};

	// LLD Background Scan Init Structure
	const XMC_VADC_BACKGROUND_CONFIG_t adc_background_config = {
		.conv_start_mode   = XMC_VADC_STARTMODE_CIR,       // Conversion start mode selected as cancel inject repeat
		.req_src_priority  = XMC_VADC_GROUP_RS_PRIORITY_1, // Priority of the Background request source in the VADC module
		.trigger_signal    = XMC_VADC_REQ_TR_A,            // If Trigger needed then this denotes the Trigger signal
		.trigger_edge      = XMC_VADC_TRIGGER_EDGE_NONE,   // If Trigger needed then this denotes Trigger edge selected
		.gate_signal       = XMC_VADC_REQ_GT_A,			   // If Gating needed then this denotes the Gating signal
		.timer_mode        = 0,							   // Timer Mode Disabled
		.external_trigger  = 0,                            // Trigger is Disabled
		.req_src_interrupt = 0,                            // Background Request source interrupt Enabled 
		.enable_auto_scan  = 1,
		.load_mode         = XMC_VADC_SCAN_LOAD_OVERWRITE
	};

	const XMC_VADC_GROUP_CONFIG_t group_init_handle0 = {
		.emux_config = {
			.stce_usage                  = 0, 					           // Use STCE when the setting changes
			.emux_mode                   = XMC_VADC_GROUP_EMUXMODE_SWCTRL, // Mode for Emux conversion
			.emux_coding                 = XMC_VADC_GROUP_EMUXCODE_BINARY, // Channel progression - binary format
			.starting_external_channel   = 0,                              // Channel starts at 0 for EMUX
			.connected_channel           = 0                               // Channel connected to EMUX
		},
		.class0 = {
			.sample_time_std_conv        = 31,                             // The Sample time is (2*tadci)
			.conversion_mode_standard    = XMC_VADC_CONVMODE_12BIT,        // 12bit conversion Selected
			.sampling_phase_emux_channel = 0,                              // The Sample time is (2*tadci)
			.conversion_mode_emux        = XMC_VADC_CONVMODE_12BIT         // 12bit conversion Selected
		},
		.class1 = {
			.sample_time_std_conv        = 31,                             // The Sample time is (2*tadci)
			.conversion_mode_standard    = XMC_VADC_CONVMODE_12BIT,        // 12bit conversion Selected
			.sampling_phase_emux_channel = 0,                              // The Sample time is (2*tadci)
			.conversion_mode_emux        = XMC_VADC_CONVMODE_12BIT         // 12bit conversion Selected
		},

		.boundary0                       = 0,                              // Lower boundary value for Normal comparison mode
		.boundary1	                     = 0,                              // Upper boundary value for Normal comparison mode
		.arbitration_round_length        = 0,                              // 4 arbitration slots per round selected (tarb = 4*tadcd) */
		.arbiter_mode                    = XMC_VADC_GROUP_ARBMODE_ALWAYS,  // Determines when the arbiter should run.
	};


	XMC_VADC_CHANNEL_CONFIG_t  channel_a_config = {
		.input_class                =  XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0,    // Global ICLASS 0 selected
		.lower_boundary_select 	    =  XMC_VADC_CHANNEL_BOUNDARY_GROUP_BOUND0,
		.upper_boundary_select 	    =  XMC_VADC_CHANNEL_BOUNDARY_GROUP_BOUND0,
		.event_gen_criteria         =  XMC_VADC_CHANNEL_EVGEN_NEVER,           // Channel Event disabled
		.sync_conversion  		    =  0,                                      // Sync feature disabled
		.alternate_reference        =  XMC_VADC_CHANNEL_REF_INTREF,            // Internal reference selected
		.result_reg_number          =  DISTANCE_ADC_RESULT_REG,                // GxRES[10] selected
		.use_global_result          =  0, 				                       // Use Group result register
		.result_alignment           =  XMC_VADC_RESULT_ALIGN_RIGHT,            // Result alignment - Right Aligned
		.broken_wire_detect_channel =  XMC_VADC_CHANNEL_BWDCH_VAGND,           // No Broken wire mode select
		.broken_wire_detect		    =  0,    		                           // No Broken wire detection
		.bfl                        =  0,                                      // No Boundary flag
		.channel_priority           =  0,                   		           // Lowest Priority 0 selected
		.alias_channel              =  -1                                      // Channel is not Aliased
	};

	XMC_VADC_RESULT_CONFIG_t channel_a_result_config = {
		.data_reduction_control = 0b11,                         // Accumulate 4 result values
		.post_processing_mode   = XMC_VADC_DMM_REDUCTION_MODE,  // Use reduction mode
		.wait_for_read_mode  	= 1,                            // Enabled
		.part_of_fifo       	= 0,                            // No FIFO
		.event_gen_enable   	= 1                             // Disable Result event
	};


	XMC_VADC_GLOBAL_Init(VADC, &adc_global_config);

    XMC_VADC_GROUP_Init(VADC_G0, &group_init_handle0);
    XMC_VADC_GROUP_SetPowerMode(VADC_G0, XMC_VADC_GROUP_POWERMODE_NORMAL);

    XMC_VADC_GLOBAL_DisablePostCalibration(VADC, 0);
    XMC_VADC_GLOBAL_SHS_EnableAcceleratedMode(SHS0, XMC_VADC_GROUP_INDEX_0);
	XMC_VADC_GLOBAL_SHS_SetClockDivider(SHS0, 0);
    XMC_VADC_GLOBAL_SHS_SetAnalogReference(SHS0, XMC_VADC_GLOBAL_SHS_AREF_EXTERNAL_VDD_UPPER_RANGE);

	XMC_VADC_GLOBAL_StartupCalibration(VADC);

	// Initialize the Global Conversion class 0
	XMC_VADC_GLOBAL_InputClassInit(VADC, adc_global_iclass_config, XMC_VADC_GROUP_CONV_STD, 0);

	// Initialize the Background Scan hardware
	XMC_VADC_GLOBAL_BackgroundInit(VADC, &adc_background_config);

	// Initialize the global result register
	XMC_VADC_GLOBAL_ResultInit(VADC, &adc_global_result_config);

    // Initialize for configured channels
    XMC_VADC_GROUP_ChannelInit(VADC_G0, DISTANCE_ADC_CHANNEL, &channel_a_config);

    // Initialize for configured result registers
    XMC_VADC_GROUP_ResultInit(VADC_G0, channel_a_config.result_reg_number, &channel_a_result_config);

	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, DISTANCE_ADC_CHANNEL);
	
	XMC_VADC_GROUP_SetResultInterruptNode(VADC_G0, channel_a_config.result_reg_number, XMC_VADC_SR_SHARED_SR0);
    NVIC_SetPriority(DISTANCE_ADC_IRQ, DISTANCE_ADC_IRQ_PRIORITY);
    NVIC_EnableIRQ(DISTANCE_ADC_IRQ);

	XMC_VADC_GLOBAL_BackgroundTriggerConversion(VADC);
}

void distance_init(void) {
	const XMC_GPIO_CONFIG_t distance_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_LARGE,
	};
	XMC_GPIO_Init(DISTANCE_ADC_PIN, &distance_pin_config);

	const XMC_GPIO_CONFIG_t led_pin_config = {
		.mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};
	XMC_GPIO_Init(DISTANCE_LED_PIN, &led_pin_config);

	memset(&distance, 0, sizeof(Distance));
	distance.first_value = true;
	distance.led.config = DISTANCE_IR_V2_DISTANCE_LED_CONFIG_SHOW_DISTANCE;
	distance_sensor_type_read();

	ccu4_pwm_init(DISTANCE_LED_PIN, DISTANCE_LED_CCU4_SLICE, 100);

	distance_init_adc();
}

void distance_tick(void) {
	if(distance.new_sensor) {
		distance_sensor_type_write();
		distance.new_sensor = false;
	}

	if(distance.led.config == DISTANCE_IR_V2_DISTANCE_LED_CONFIG_SHOW_DISTANCE) {
		const uint16_t distance_min = distance_lookup[distance.sensor][DISTANCE_LOOKUP_SIZE-1]/10;
		const uint16_t distance_max = distance_lookup[distance.sensor][0]/10;
		ccu4_pwm_set_duty_cycle(DISTANCE_LED_CCU4_SLICE, 100-SCALE(distance_get_distance(), distance_min, distance_max, 0, 100));
	} else {
		led_flicker_tick(&distance.led, system_timer_get_ms(), DISTANCE_LED_PIN);
	}

	if(distance_stage1_value > 0) {
		uint32_t value = 0;
		if(distance.first_value) {
			distance.first_value = false;
			moving_average_init(&distance.moving_average_distance, distance_stage1_value, MOVING_AVERAGE_DEFAULT_LENGTH);
			value = distance_stage1_value;
		} else {
			value = moving_average_handle_value(&distance.moving_average_distance, distance_stage1_value);
		}

		distance.last_stage1_value = distance_stage1_value;
		distance_stage1_value = 0;

		uint16_t div_value = value/DISTANCE_DIVIDER;
		uint32_t mod = value % DISTANCE_DIVIDER;

		uint16_t d = ((DISTANCE_DIVIDER - mod)*distance_lookup[distance.sensor][div_value] + 
					mod*distance_lookup[distance.sensor][div_value+1])/DISTANCE_DIVIDER;

		// Make mm
		distance.distance = (d + 5) / 10;
	}
}

uint16_t distance_get_distance(void) {
	return distance.distance;
}

uint32_t distance_get_analog_value(void) {
	if(distance_stage1_value != 0) {
		return distance_stage1_value;
	}
	return distance.last_stage1_value;
}