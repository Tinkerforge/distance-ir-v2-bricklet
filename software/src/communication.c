/* distance-ir-v2-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "distance.h"
#include "configs/config_distance.h"

#define CALLBACK_VALUE_TYPE CALLBACK_VALUE_TYPE_UINT16
#include "bricklib2/utility/callback_value.h" // for distance
#undef CALLBACK_VALUE_TYPE
#define CALLBACK_VALUE_TYPE CALLBACK_VALUE_TYPE_UINT32
#include "bricklib2/utility/callback_value.h" // for analog value
#undef CALLBACK_VALUE_TYPE

CallbackValue_uint16_t callback_value_distance;
CallbackValue_uint32_t callback_value_analog_value;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_DISTANCE: return get_callback_value_uint16_t(message, response, &callback_value_distance);
		case FID_SET_DISTANCE_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_uint16_t(message, &callback_value_distance);
		case FID_GET_DISTANCE_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_uint16_t(message, response, &callback_value_distance);
		case FID_GET_ANALOG_VALUE: return get_callback_value_uint32_t(message, response, &callback_value_analog_value);
		case FID_SET_ANALOG_VALUE_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_uint32_t(message, &callback_value_analog_value);
		case FID_GET_ANALOG_VALUE_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_uint32_t(message, response, &callback_value_analog_value);
		case FID_SET_MOVING_AVERAGE_CONFIGURATION: return set_moving_average_configuration(message);
		case FID_GET_MOVING_AVERAGE_CONFIGURATION: return get_moving_average_configuration(message, response);
		case FID_SET_DISTANCE_LED_CONFIG: return set_distance_led_config(message);
		case FID_GET_DISTANCE_LED_CONFIG: return get_distance_led_config(message, response);
		case FID_SET_SENSOR_TYPE: return set_sensor_type(message);
		case FID_GET_SENSOR_TYPE: return get_sensor_type(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse set_moving_average_configuration(const SetMovingAverageConfiguration *data) {
	if(data->moving_average_length > MOVING_AVERAGE_MAX_LENGTH) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	moving_average_new_length(&distance.moving_average_distance, data->moving_average_length);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_moving_average_configuration(const GetMovingAverageConfiguration *data, GetMovingAverageConfiguration_Response *response) {
	response->header.length         = sizeof(GetMovingAverageConfiguration_Response);
	response->moving_average_length = distance.moving_average_distance.length;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_distance_led_config(const SetDistanceLEDConfig *data) {
	if(data->config > DISTANCE_IR_V2_DISTANCE_LED_CONFIG_SHOW_DISTANCE) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	XMC_GPIO_CONFIG_t config_gpio = {
		.mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
	};

	XMC_GPIO_CONFIG_t config_pwm = {
		.mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};

	switch(data->config) {
		case DISTANCE_IR_V2_DISTANCE_LED_CONFIG_OFF: {
			config_gpio.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
			XMC_GPIO_Init(DISTANCE_LED_PIN, &config_gpio);
			break;
		}

		case DISTANCE_IR_V2_DISTANCE_LED_CONFIG_ON: // fall-through
		case DISTANCE_IR_V2_DISTANCE_LED_CONFIG_SHOW_HEARTBEAT: {
			config_gpio.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
			XMC_GPIO_Init(DISTANCE_LED_PIN, &config_gpio);
			break;
		}

		case DISTANCE_IR_V2_DISTANCE_LED_CONFIG_SHOW_DISTANCE: {
			XMC_GPIO_Init(DISTANCE_LED_PIN, &config_pwm);
			break;
		}
	}

	distance.led.config = data->config;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_distance_led_config(const GetDistanceLEDConfig *data, GetDistanceLEDConfig_Response *response) {
	response->header.length = sizeof(GetDistanceLEDConfig_Response);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_sensor_type(const SetSensorType *data) {
	if(data->sensor > DISTANCE_IR_V2_SENSOR_TYPE_2Y0A02) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(distance.sensor != data->sensor) {
		distance.sensor     = data->sensor;
		distance.new_sensor = true;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_sensor_type(const GetSensorType *data, GetSensorType_Response *response) {
	response->header.length = sizeof(GetSensorType_Response);
	response->sensor        = distance.sensor;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


bool handle_distance_callback(void) {
	return handle_callback_value_callback_uint16_t(&callback_value_distance, FID_CALLBACK_DISTANCE);
}

bool handle_analog_value_callback(void) {
	return handle_callback_value_callback_uint32_t(&callback_value_analog_value, FID_CALLBACK_ANALOG_VALUE);
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	callback_value_init_uint16_t(&callback_value_distance, distance_get_distance);
	callback_value_init_uint32_t(&callback_value_analog_value, distance_get_analog_value);

	communication_callback_init();
}
