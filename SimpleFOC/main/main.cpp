/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_simplefoc.h"

BLDCMotor motor = BLDCMotor(6);
BLDCDriver3PWM driver = BLDCDriver3PWM(17, 16, 15);

extern "C" void app_main(void)
{
	SimpleFOCDebug::enable();
	Serial.begin(115200);

	driver.pwm_frequency = 20000;
	driver.voltage_power_supply = 7.4;
	driver.voltage_limit = 7.4;
	driver.init(0);

	motor.linkDriver(&driver);

	motor.velocity_limit = 200;	// Radians per second
	motor.voltage_limit = 0.5;	// Keep low to reduce current, if windings are low resistance
	motor.controller = MotionControlType::velocity_openloop;
	motor.useMonitoring(Serial);

	motor.init();

	while (1) {
		motor.move(40);
		motor.monitor();
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}
