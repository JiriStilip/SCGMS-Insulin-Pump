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

#include "auxin_filter.h"
#include "descriptor.h"
#include <FreeRTOS.h>
#include <task.h>
#include "rtl/rattime.h"

#include <dht11.h>
#include <gpio_irq.h>

CAuxIn_Filter::CAuxIn_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CAuxIn_Filter::~CAuxIn_Filter()
{
	//
}

void CAuxIn_Filter::Create_Event(GUID signal_id, double level)
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
	event.signal_id() = signal_id;
	event.level() = level;

	mOutput.Send(event);
}

void dht11_task(void *pvParameters)
{
	CAuxIn_Filter *auxin = (CAuxIn_Filter *)pvParameters;
	const TickType_t xDelay = 15000 / portTICK_RATE_MS;
	while (true)
	{
		vTaskDelay(xDelay);

		sSensor_DHT11.Read();
		auxin->Create_Event(scgms::signal_Air_Temperature, sSensor_DHT11.Get_Temperature());
	}
}

void auxin_task(void *pvParameters)
{
	CAuxIn_Filter *auxin = (CAuxIn_Filter *)pvParameters;
	uint32_t notify_value = 0;
	bool cgm_suspended = false;
	while (true)
	{
		notify_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (notify_value & AUXIN_BTN1)
		{
			if (!cgm_suspended)
			{
				vTaskSuspend(cgm_task_handle);
				cgm_suspended = true;
			}
			else
			{
				vTaskResume(cgm_task_handle);
				cgm_suspended = false;
			}
		}
		else if (notify_value & AUXIN_BTN2)
		{
			auxin->Create_Event(scgms::signal_Remaining_Insulin, 100.0);
		}
	}
}

HRESULT IfaceCalling CAuxIn_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(auxin_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CAuxIn_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	// higher priority (with sufficient task vTaskDelay) since taskENTER/EXIT_CRITICAL() messes with the OLED display
	xTaskCreate(dht11_task, "DHT11 sensor reading", 4 * 1024, this, 2, NULL);

	xTaskCreate(auxin_task, "Auxiliary Input Filter", 4 * 1024, this, 1, &auxin_task_handle);

	return S_OK;
}

HRESULT IfaceCalling CAuxIn_Filter::Do_Execute(scgms::UDevice_Event event)
{
	return mOutput.Send(event);
}
