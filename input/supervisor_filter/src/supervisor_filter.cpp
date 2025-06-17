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

#include "supervisor_filter.h"
#include "descriptor.h"
#include <FreeRTOS.h>
#include <task.h>
#include <gpio.h>

void watchdog_task(void *pvParameters)
{
	CSupervisor_Filter *filter = (CSupervisor_Filter *)pvParameters;
	const TickType_t xDelay = filter->GetWatchdogInterval() / portTICK_RATE_MS;
	while (true)
	{
		vTaskDelay(xDelay);

		if (!filter->WatchdogIsAlive())
		{
			filter->WatchdogTrigger();
		}
		filter->WatchdogReset();		
	}
}

void CSupervisor_Filter::WatchdogKick(int64_t event_time)
{
	if (event_time <= logical_time)
	{
		WatchdogTrigger();
	}
	else
	{
		logical_time = event_time;
		alive = true;
		gpio_set_output(PIN_LED_WATCHDOG, LOW);
	}
}

bool CSupervisor_Filter::WatchdogIsAlive()
{
	return alive;
}

void CSupervisor_Filter::WatchdogReset()
{
	alive = false;
}

double CSupervisor_Filter::GetWatchdogInterval()
{
	return watchdog_interval;
}

void CSupervisor_Filter::WatchdogTrigger()
{
	gpio_set_output(PIN_LED_WATCHDOG, HIGH);

	Create_BG_Error_Event();
}

void CSupervisor_Filter::Create_BG_Error_Event()
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Error};
	event.signal_id() = scgms::signal_BG;

	mOutput.Send(event);
}

void CSupervisor_Filter::Create_Temp_Warning_Event()
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Warning};
	event.signal_id() = scgms::signal_Air_Temperature;

	mOutput.Send(event);
}

CSupervisor_Filter::CSupervisor_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CSupervisor_Filter::~CSupervisor_Filter()
{
	//
}

HRESULT IfaceCalling CSupervisor_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(supervisor_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CSupervisor_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	watchdog_interval = configuration.Read_Double(supervisor_filter::rsWatchdogInterval, 8000);

	alive = false;
	logical_time = 0;

	gpio_set_function(PIN_LED_WATCHDOG, GPIO_FUNCTION_OUTPUT);

	xTaskCreate( watchdog_task, "CGM Watchdog", 4*1024, this, 1, NULL );

	return S_OK;
}

HRESULT IfaceCalling CSupervisor_Filter::Do_Execute(scgms::UDevice_Event event)
{
	if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		if (event.signal_id() == scgms::signal_BG)
		{
			WatchdogKick(event.logical_time());
		}
		else if (event.signal_id() == scgms::signal_Air_Temperature)
		{
			if (event.level() > Air_Temperature_Overheating)
			{
				// to avoid the warning being overwritten by the level event
				HRESULT res = mOutput.Send(event);
				Create_Temp_Warning_Event();
				return res;
			}
		}
	}

	return mOutput.Send(event);
}
