/*
 * WiFi.c
 *
 *  Created on: Mar 2, 2016
 *      Author: Cosmin
 */

#include "WiFi.h"


uint8 connStatus = 0;


void WiFi_Init(void)
{
	struct station_config stationConfig;

	os_memcpy(&stationConfig.ssid, SSID, sizeof(SSID));
	os_memcpy(&stationConfig.password, PASSWORD, sizeof(PASSWORD));
	stationConfig.bssid_set = 0;

	ets_uart_printf("Default mode: %d\r\n", wifi_get_opmode_default());

	//Set the default work mode to Station if not already set
	if(wifi_get_opmode_default() != STATION_MODE)
		wifi_set_opmode(STATION_MODE);

	//Set how will the device appear on the router
	wifi_station_set_hostname(DEVICE_NAME);

	//Configure the AP to connect to
	wifi_station_set_config_current(&stationConfig);	//TODO: Read the config and use the APIs that write it to flash

	wifi_station_connect();
}

void WiFi_CheckConnStatus(void)
{
	struct ip_info DeviceIP;
	uint8 TempBuff[30];

	if(connStatus != wifi_station_get_connect_status())
	{
		connStatus = wifi_station_get_connect_status();
		switch(connStatus)
		{
		case STATION_IDLE:
			ets_uart_printf("Connection Idle!\r\n");
			break;
		case STATION_CONNECTING:
			ets_uart_printf("Connecting...\r\n");
			break;
		case STATION_WRONG_PASSWORD:
			ets_uart_printf("Wrong Password!\r\n");
			break;
		case STATION_NO_AP_FOUND:
			ets_uart_printf("No AP found\r\n");
			break;
		case STATION_CONNECT_FAIL:
			ets_uart_printf("Connection failed\r\n");
			break;
		case STATION_GOT_IP:
			ets_uart_printf("Connected!\r\n");
			wifi_get_ip_info(STATION_IF, &DeviceIP);
			ets_uart_printf("Device IP: %d.%d.%d.%d\r\n",
							(DeviceIP.ip.addr & 0xFF),
							(DeviceIP.ip.addr >> 8 & 0xFF),
							(DeviceIP.ip.addr >> 16 & 0xFF),
							(DeviceIP.ip.addr >> 24));
			break;
		}
	}
}
