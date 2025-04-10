/**
 * SmartCGMS - continuous glucose monitoring and controlling framework
 * https://diabetes.zcu.cz/
 *
 * Copyright (c) since 2018 University of West Bohemia.
 *
 * Contact:
 * diabetes@mail.kiv.zcu.cz
 * Medical Informatics, Department of Computer Science and Engineering
 * Faculty of Applied Sciences, University of West Bohemia
 * Univerzitni 8, 301 00 Pilsen
 * Czech Republic
 *
 *
 * Purpose of this software:
 * This software is intended to demonstrate work of the diabetes.zcu.cz research
 * group to other scientists, to complement our published papers. It is strictly
 * prohibited to use this software for diagnosis or treatment of any medical condition,
 * without obtaining all required approvals from respective regulatory bodies.
 *
 * Especially, a diabetic patient is warned that unauthorized use of this software
 * may result into severe injure, including death.
 *
 *
 * Licensing terms:
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these license terms is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 * a) This file is available under the Apache License, Version 2.0.
 * b) When publishing any derivative work or results obtained using this software, you agree to cite the following paper:
 *    Tomas Koutny and Martin Ubl, "SmartCGMS as a Testbed for a Blood-Glucose Level Prediction and/or
 *    Control Challenge with (an FDA-Accepted) Diabetic Patient Simulation", Procedia Computer Science,
 *    Volume 177, pp. 354-362, 2020
 */

#include "data_filter.h"
#include "descriptor.h"
#if defined(RPI)
#include "uart_print.h"
#include "input.h"
#include <FreeRTOS.h>
#include <task.h>
#elif defined(ESP32)
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pthread.h"

#define ECHO_TEST_TXD 1 //(CONFIG_CONSOLE_UART_NUM)
#define ECHO_TEST_RXD 3 //(CONFIG_CONSOLE_UART_NUM)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM (CONFIG_CONSOLE_UART_NUM)
#define ECHO_UART_BAUD_RATE (CONFIG_CONSOLE_UART_BAUDRATE)

#define BUF_SIZE 1024
#elif defined(WASM)
#include "pthread.h"
#endif
CData_Filter::CData_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CData_Filter::~CData_Filter()
{
	//
}

void CData_Filter::Create_Event(int level)
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
	event.level() = level;
	mOutput.Send(event);
}

bool CData_Filter::isRunning()
{
	return running;
}

#if defined(ESP32)
void *uart_thread(void *arg)
{
	CData_Filter *data_filter = (CData_Filter *)arg;

	/* Configure parameters of an UART driver,
	 * communication pins and install the driver */
	uart_config_t uart_config = {
		.baud_rate = ECHO_UART_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_REF_TICK,
	};
	int intr_alloc_flags = 0;

	ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
	ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

	uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
	while (1)
	{
		// Read data from the UART
		int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1),
								  20 / portTICK_PERIOD_MS);

		if (len)
		{
			data[len] = '\0';

			ESP_LOGI("UART", "Recieved data: %s", (char *)data);
			std::string str = (char *)data;
			int level = atoi(str.c_str());

			data_filter->Create_Event(level);
		}
	}
}
#elif defined(RPI)
void datafilter_task(void *pvParameters)
{
	print("Starting generating data");
	CData_Filter *data = (CData_Filter *)pvParameters;
	const TickType_t xDelay = 1000 / portTICK_RATE_MS;
	int level;
	int index = 0;
	while (data->isRunning())
	{
		level = random_levels[index++];
		data->Create_Event(level);
		vTaskDelay(xDelay);
		if(index >= sizeof(random_levels)/sizeof(random_levels[0]))
		{
			index = 0;
			print("Restarting test event loop");
		}
	}
	while (true)
	{
		// stop generating data, should delete task instead
	}
}
#endif
HRESULT IfaceCalling CData_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(data_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CData_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	auto create_task = configuration.Read_Bool(data_filter::rsCreateTask, false);

	running = true;
	events_total = 0;
	level_events = 0;
	info_events = 0;
	error_events = 0;
	warning_events = 0;
#if defined(ESP32)
	if (create_task)
	{
		printf("Creating data filter thread\n");
		int rc = pthread_create(&data_thread, NULL, uart_thread, (void *)this);
		if (rc)
		{
			printf("Error: return code from data filter pthread_create() is %d\n", rc);
		}
	}

#elif defined(RPI)
	if (create_task)
	{
		xTaskCreate(datafilter_task, "Datafilter", 4 * 1024, this, 1, NULL);
	}
#endif
	return S_OK;
}

HRESULT IfaceCalling CData_Filter::Do_Execute(scgms::UDevice_Event event)
{
	events_total++;
	if (event.event_code() == scgms::NDevice_Event_Code::Shut_Down)
	{
		running = false;
#if defined(WASM)
		printf("Total events: %lld\n", events_total);
		printf("Level events: %lld\n", level_events);
		printf("Info events: %lld\n", info_events);
		printf("Error events: %lld\n", error_events);
		printf("Warning events: %lld\n", warning_events);
#elif defined(ESP32)
		printf("Data filter thread shutdown\n");
		printf("Total events: %lld\n", events_total);
		printf("Level events: %lld\n", level_events);
		printf("Info events: %lld\n", info_events);
		printf("Error events: %lld\n", error_events);
		printf("Warning events: %lld\n", warning_events);
		pthread_join(data_thread, NULL);
#elif defined(RPI)
		print("Data filter task shutdown");
		print("Total events:");
		print_i(events_total);
		print("Level events:");
		print_i(level_events);
		print("Info events:");
		print_i(info_events);
		print("Error events:");
		print_i(error_events);
		print("Warning events:");
		print_i(warning_events);
#endif
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		level_events++;
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Information)
	{
		info_events++;
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Error)
	{
		error_events++;
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Warning)
	{
		warning_events++;
	}
	return mOutput.Send(event);
};
