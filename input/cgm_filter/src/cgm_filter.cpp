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

#include "cgm_filter.h"
#include "descriptor.h"
#include "uart_print.h"
#include "input.h"
#include <FreeRTOS.h>
#include <task.h>
#include "rtl/rattime.h"

CCGM_Filter::CCGM_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CCGM_Filter::~CCGM_Filter()
{
	//
}

void CCGM_Filter::Create_Event(int level, double time)
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
	event.level() = level;
	event.signal_id() = scgms::signal_BG;
	event.device_time() = time;

	mOutput.Send(event);
}

double CCGM_Filter::GetTaskPeriod()
{
	return task_period;
}

double CCGM_Filter::GetFirstEventTimestamp()
{
	return first_event_timestamp;
}

double CCGM_Filter::GetMeasurementInterval()
{
	return measurement_interval;
}

void cgmfilter_task(void *pvParameters)
{
	print("Starting generating CGM levels.");
	CCGM_Filter *cgm = (CCGM_Filter *)pvParameters;
	const TickType_t xDelay = cgm->GetTaskPeriod() / portTICK_RATE_MS;
	int level;
	double start_time = cgm->GetFirstEventTimestamp();
	double time = start_time;
	double interval_mins = cgm->GetMeasurementInterval();
	int index = 0;
	while (true)
	{
		level = random_levels[index++];
		cgm->Create_Event(level, time);
		time += interval_mins * scgms::One_Minute;
		vTaskDelay(xDelay);
		if(index >= sizeof(random_levels)/sizeof(random_levels[0]))
		{
			index = 0;
			time = start_time;
			print("Restarting loop.");
		}
	}
}

HRESULT IfaceCalling CCGM_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(cgm_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CCGM_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	task_period = configuration.Read_Double(cgm_filter::rsTaskPeriod, 5000);
	first_event_timestamp = configuration.Read_Double(cgm_filter::rsFirstEventTimestamp, 1.0);
	measurement_interval = configuration.Read_Double(cgm_filter::rsMeasurementInterval, 5);

	xTaskCreate(cgmfilter_task, "CGM Filter", 4 * 1024, this, 1, NULL);

	return S_OK;
}

HRESULT IfaceCalling CCGM_Filter::Do_Execute(scgms::UDevice_Event event)
{
	return mOutput.Send(event);
};
