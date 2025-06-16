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

#include "print_filter.h"
#include "descriptor.h"
#include "utils/string_utils.h"
#include "rtl/rattime.h"

#include <uart_print.h>
#include "timer.h"

CPrint_Filter::CPrint_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CPrint_Filter::~CPrint_Filter()
{
	//
}

HRESULT IfaceCalling CPrint_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(print_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CPrint_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	return S_OK;
}

HRESULT IfaceCalling CPrint_Filter::Do_Execute(scgms::UDevice_Event event)
{
	print("--------");

	if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		if (event.signal_id() == scgms::signal_BG)
		{
			print("signal_BG level: ");
			print_d(event.level());
			print("Device time:");
			std::string time_str = Rat_Time_To_Local_Time_Str(event.device_time(), "%H:%M:%S");
			print(time_str.c_str());
		}
		else if (event.signal_id() == scgms::signal_Air_Temperature)
		{
			print("signal_Air_Temperature level: ");
			print_d(event.level());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Basal_Rate)
		{
			print("signal_Requested_Insulin_Basal_Rate level: ");
			print_d(event.level());
			print("Device time:");
			std::string time_str = Rat_Time_To_Local_Time_Str(event.device_time(), "%H:%M:%S");
			print(time_str.c_str());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Bolus)
		{
			print("signal_Requested_Insulin_Bolus level: ");
			print_d(event.level());
			print("Device time:");
			std::string time_str = Rat_Time_To_Local_Time_Str(event.device_time(), "%H:%M:%S");
			print(time_str.c_str());
		}
		else if (event.signal_id() == scgms::signal_Remaining_Insulin)
		{
			print("signal_Remaining_Insulin level: ");
			print_d(event.level());
		}
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Masked_Level)
	{
		if (event.signal_id() == scgms::signal_Requested_Insulin_Bolus)
		{
			print("signal_Requested_Insulin_Bolus level (masked): ");
			print_d(event.level());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Basal_Rate)
		{
			print("signal_Requested_Insulin_Basal_Rate level (masked): ");
			print_d(event.level());
		}
	}
	else
	{
		if (event.event_code() == scgms::NDevice_Event_Code::Information)
		{
			print("Information event: ");
		}
		else if (event.event_code() == scgms::NDevice_Event_Code::Warning)
		{
			print("Warning event: ");
		}
		else if (event.event_code() == scgms::NDevice_Event_Code::Error)
		{
			print("Error event: ");
		}
		std::wstring wstr = refcnt::WChar_Container_To_WString(event.info.get());
		std::string str = Narrow_WString(wstr);
		print(str.c_str());
	}
	
	return mOutput.Send(event);
}
