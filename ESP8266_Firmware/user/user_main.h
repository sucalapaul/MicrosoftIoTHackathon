/*
 * user_main.h
 *
 *  Created on: Mar 1, 2016
 *      Author: Cosmin
 */

#ifndef USER_USER_MAIN_H_
#define USER_USER_MAIN_H_

#include <osapi.h>
#include <os_type.h>
#include <c_types.h>
#include <user_interface.h>
#include "user_config.h"
#include "driver/uart.h"
#include "driver/i2c_bmp180.h"
#include "WiFi.h"
#include "espconn.h"
#include <gpio.h>

#define MAIN_TASK_PERIOD	50		//The period of the MainTask in ms
#define SEC_TASK_PERIOD		19		//19 for 1sec

typedef struct {
	uint8 cnfSSID[16];
	uint8 cnfPsw[16];
	uint32 DevID;
} ConfigData;

static void ICACHE_FLASH_ATTR MainTask(void *arg);
static void SecTask(void);
static void MinTask(void);

static void ConfigApp(void *arg);

#endif /* USER_USER_MAIN_H_ */
