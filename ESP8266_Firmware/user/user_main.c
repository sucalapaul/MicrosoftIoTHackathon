/*
 * user_main.c
 *
 *  Created on: Feb 29, 2016
 *      Author: Cosmin
 */

#include "user_main.h"

static os_timer_t mainTaskTmr, configAppTmr;
uint8 SecTskCnt = 0, MinTskCnt = 0;

ConfigData Config;
char TempReqBuff[100];
uint8 DoneFlag = 0;
uint8 DevConfig = 0;


void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(1000000);

	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

	ets_uart_printf("Started!\r\n");

	spi_flash_read(NVM_ADDR * SPI_FLASH_SEC_SIZE,
			(uint32*)&Config,
			sizeof(ConfigData));

	ets_uart_printf("DevID: %u\r\n", Config.DevID);
	if(Config.DevID != -1)	//Valid configuration saved in NVM
	{
		struct station_config stationConfig;

		//Connecting to saved network
		ets_uart_printf("Connecting to saved network..");

		ets_uart_printf("SSID: %s\r\n", Config.cnfSSID);
		ets_uart_printf("Password: %s\r\n", Config.cnfPsw);
		ets_uart_printf("Device ID: %d\r\n", Config.DevID);

		os_memcpy(&stationConfig.ssid, Config.cnfSSID, sizeof(Config.cnfSSID));
		os_memcpy(&stationConfig.password, Config.cnfPsw, sizeof(Config.cnfPsw));
		stationConfig.bssid_set = 0;

		//Set the default work mode to Station if not already set
		if(wifi_get_opmode_default() != STATION_MODE)
			wifi_set_opmode(STATION_MODE);

		//Set how will the device appear on the router
		wifi_station_set_hostname(DEVICE_NAME);

		//Configure the AP to connect to
		wifi_station_set_config_current(&stationConfig);	//TODO: Read the config and use the APIs that write it to flash

		wifi_station_connect();

		//Start the Application Task
		os_timer_disarm(&mainTaskTmr);
		os_timer_setfn(&mainTaskTmr, (os_timer_func_t *)MainTask, (void*)0);
		os_timer_arm(&mainTaskTmr, MAIN_TASK_PERIOD, 1);
	}
	else				//NVM is empty
	{
		ets_uart_printf("Empty NVM!");
		//Init default config AP connection
		WiFi_Init();

		//Start the Application Task
		os_timer_disarm(&configAppTmr);
		os_timer_setfn(&configAppTmr, (os_timer_func_t *)ConfigApp, (void*)0);
		os_timer_arm(&configAppTmr, 500, 1);		//500ms
	}


	//os_printf("Started!\n");

//    if (BMP180_Init())
//    	ets_uart_printf("BMP180 init done.\r\n");
//    else
//    	ets_uart_printf("BMP180 init error.\r\n");


}

void* ConfigCbk(char * response_body, int http_status, char * full_response)
{
	ets_uart_printf("Request response:\r\n");

	os_strcpy(Config.cnfSSID, full_response);
	os_strcpy(Config.cnfPsw, full_response + 16);
	Config.DevID = atoi(full_response + 32);

	ets_uart_printf("Received SSID: %s\r\n", Config.cnfSSID);
	ets_uart_printf("Received password: %s\r\n", Config.cnfPsw);
	ets_uart_printf("Received Device ID: %d\r\n", Config.DevID);

	//Save and restart!
	if((Config.DevID != -1) && Config.DevID)
	{
		spi_flash_write(NVM_ADDR * SPI_FLASH_SEC_SIZE,
				(uint32*)&Config,
				sizeof(ConfigData));

		ets_uart_printf("Restarting..\r\n");
		system_restart();
	}
	return NULL;
}

static void ConfigApp(void *arg)
{
	if((wifi_station_get_connect_status() == STATION_GOT_IP) && (!DoneFlag))
	{
		ets_uart_printf("Requesting Config..\r\n");
		os_sprintf(TempReqBuff, "http://192.168.43.1:8080/config",
				Config.DevID);
		http_get(TempReqBuff, "", ConfigCbk);

		DoneFlag = 1;
	}
}

void* DevConfigCbk(char * response_body, int http_status, char * full_response)
{
	ets_uart_printf("Device configured from server as: %s", response_body);
	DevConfig = atoi(response_body);

	//Configure GPIO
	if(DevConfig == 1)	//In
		GPIO_DIS_OUTPUT(GPIO_PIN);
	if(DevConfig == 2)	//Out
		GPIO_OUTPUT_SET(GPIO_PIN, 0);
}

void* StateRespCbk(char * response_body, int http_status, char * full_response)
{
	uint8 PinState = 0;

	PinState = atoi(response_body);
	ets_uart_printf("State received: %u ", PinState);

	if(PinState)	//High
		gpio_output_set(BIT2, 0, BIT2, 0);
	else			//Low
		gpio_output_set(0, BIT2, BIT2, 0);
}

static void ICACHE_FLASH_ATTR MainTask(void *arg)
{
	WiFi_CheckConnStatus();

	//ets_uart_printf("ConnStatus: %d\r\n", wifi_station_get_connect_status());

	//1 Second Task
	if(SecTskCnt)
		SecTskCnt--;
	else
	{
		SecTskCnt = SEC_TASK_PERIOD;
		SecTask();
	}
}

static void SecTask(void)
{
	//ets_uart_printf("RSSI: %d\r\n", wifi_station_get_rssi());

	//ets_uart_printf("Server: %d\r\n", espconn_mdns_get_servername());
	//ets_uart_printf("Host: %d\r\n", wifi_station_get_hostname());

	if((wifi_station_get_connect_status() == STATION_GOT_IP) && (!DoneFlag))
	{
		ets_uart_printf("Requesting Config..\r\n");
		os_sprintf(TempReqBuff, "http://webapplication220160317031920.azurewebsites.net/api/ESP_config/%u",
				Config.DevID);
		http_get(TempReqBuff, "", DevConfigCbk);

		DoneFlag = 1;
	}

	if((DoneFlag) && (DevConfig == 1))	//Device sends the state of a button
	{
		uint8 PinState = 0;

		//Read GPIO
		if(GPIO_INPUT_GET(GPIO_PIN))
			PinState = 1;
		else
			PinState = 0;

		os_sprintf(TempReqBuff, "http://webapplication220160317031920.azurewebsites.net/api/ESP/%u?value=%u",
			Config.DevID,
			PinState);
		http_get(TempReqBuff, "", NULL);
		ets_uart_printf("Request Done! Sent: %u\r\n", PinState);
	}

	if((DoneFlag) && (DevConfig == 2))	//Device outputs a command from the server
	{
		os_sprintf(TempReqBuff, "http://webapplication220160317031920.azurewebsites.net/api/ESP/%d",
				Config.DevID);
		http_get(TempReqBuff, "", StateRespCbk);
		ets_uart_printf("Request Done!\r\n");
	}

	//1 Minute Task==================================================
	if(MinTskCnt)
		MinTskCnt--;
	else
	{
		MinTskCnt = 59;
		MinTask();
	}
}

static void MinTask(void)
{
//	int32_t temperature, pressure;
//	char buff[20];
//	char req[100];
//
//	temperature = BMP180_GetTemperature();
//	pressure = BMP180_GetPressure(OSS_0);
//
//	ets_uart_printf("Temperature: %s *C\r\n", BMP180_Int2String(buff, temperature));
//	ets_uart_printf("Pressure: %d.%d mmHg\r\n", (int)(pressure * 75 / 10000), (int)((pressure * 75 % 10000) / 1000));
//
//	os_sprintf(req, "http://api.thingspeak.com/update?api_key=KRGKZMYQGBOT3985&field1=%s&field2=%d.%d",
//			BMP180_Int2String(buff, temperature),
//			(int)(pressure * 75 / 10000),
//			(int)((pressure * 75 % 10000) / 1000));
//	http_get(req, "", NULL);
//	ets_uart_printf("Request Done!");
}
