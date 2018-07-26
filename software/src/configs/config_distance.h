/* distance-ir-v2-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_distance.h: Config for ADC
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

#ifndef CONFIG_DISTANCE_H
#define CONFIG_DISTANCE_H

#include "xmc_gpio.h"

#define DISTANCE_ADC_PIN          P2_1
#define DISTANCE_ADC_CHANNEL      6
#define DISTANCE_ADC_RESULT_REG   10

#define DISTANCE_ADC_IRQ          15
#define DISTANCE_ADC_IRQ_PRIORITY 0

#define DISTANCE_LED_PIN          P1_0
#define DISTANCE_LED_CCU4_SLICE   0

#endif
