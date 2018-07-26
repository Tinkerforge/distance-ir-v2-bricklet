/* distance-ir-v2-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * distance.h Calculate distance from ADC value
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
 
#ifndef DISTANCE_H
#define DISTANCE_H

#include <stdbool.h>
#include <stdint.h>

#include "bricklib2/utility/moving_average.h"
#include "bricklib2/utility/led_flicker.h"

#define DISTANCE_SENSOR_TYPE_PAGE           1
#define DISTANCE_SENSOR_TYPE_MAGIC_POS      0
#define DISTANCE_SENSOR_TYPE_SENSOR_POS     1
#define DISTANCE_SENSOR_TYPE_MAGIC          0x12345678

#define DISTANCE_SENSOR_NUM 3
#define DISTANCE_LOOKUP_SIZE 512

typedef struct {
	MovingAverage moving_average_distance;
	bool first_value;
	uint8_t sensor;
	bool new_sensor;

	uint16_t distance;

	uint32_t last_stage1_value;

	LEDFlickerState led;
} Distance;

extern Distance distance;
extern uint16_t distance_lookup[DISTANCE_SENSOR_NUM][DISTANCE_LOOKUP_SIZE];

void distance_init(void);
void distance_tick(void);
uint16_t distance_get_distance(void);
uint32_t distance_get_analog_value(void);

#endif 
