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

#include "watchdog_filter.h"
#include "descriptor.h"
#include "pthread.h"
#include <uart_print.h>
#include <FreeRTOS.h>
#include <task.h>

void watchdog_task(void *pvParameters)
{
	print("Starting watchdog task");
	CWatchdog_Filter *wd = (CWatchdog_Filter *)pvParameters;
	while (wd->IsRunning())
	{
		if (!wd->IsAlive())
		{
			wd->Trigger();
		}
		wd->Reset();

		vTaskDelay(wd->GetInterval() / portTICK_RATE_MS);
	}
	while(true)
	{
		// stop watchdog, should delete task instead
	}
}

void CWatchdog_Filter::Kick(int64_t event_time)
{
	if (event_time <= logical_time)
	{
		Trigger();
	}
	else
	{
		logical_time = event_time;
		alive = true;
	}
}

bool CWatchdog_Filter::IsAlive()
{
	return alive;
}

void CWatchdog_Filter::Reset()
{
	alive = false;
}

bool CWatchdog_Filter::IsRunning()
{
	return running;
}

int64_t CWatchdog_Filter::GetInterval()
{
	return interval;
}

void CWatchdog_Filter::Trigger()
{
	print("Filter chain INACTIVE!");
	print("----------------------");
}

CWatchdog_Filter::CWatchdog_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CWatchdog_Filter::~CWatchdog_Filter()
{
	//
}

HRESULT IfaceCalling CWatchdog_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(watchdog_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CWatchdog_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	interval = configuration.Read_Double(watchdog_filter::rsInterval, 10000);
	running = true;
	alive = true;
	logical_time = 0;

	xTaskCreate( watchdog_task, "Watchdog", 4*1024, this, 1, NULL );

	return S_OK;
}

HRESULT IfaceCalling CWatchdog_Filter::Do_Execute(scgms::UDevice_Event event)
{
	if (event.event_code() == scgms::NDevice_Event_Code::Shut_Down)
	{
		running = false;
		print("Watchdog shutdown");
	}
	else
	{
		Kick(event.logical_time());
	}
	return mOutput.Send(event);
};
