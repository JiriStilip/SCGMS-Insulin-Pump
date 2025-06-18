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

#include "indel_filter.h"
#include "descriptor.h"

double CInDel_Filter::GetReservoirCapacity()
{
	return reservoir_capacity;
}

// refills the reservoir to percentage of its capacity, returns remaining insulin units
double CInDel_Filter::RefillInsulin(double percentage)
{
	remaining_insulin = (percentage / 100.0) * reservoir_capacity;

	return remaining_insulin;
}

bool CInDel_Filter::ReservoirEmpty()
{
	return (remaining_insulin < Minimum_Insulin_Amount) ? true : false;
}

void CInDel_Filter::DeliverInsulin(double device_time)
{
	// amount of insulin corresponding to the elapsed time
	double basal_amount = ((device_time - last_device_time) / scgms::One_Hour) * basal_rate;

	if (basal_amount < Minimum_Insulin_Amount) return;
	// if there is not enough insulin, deliver what is left
	if (basal_amount > remaining_insulin) basal_amount = remaining_insulin;
	// if it exceeds the maximum flow, deliver maximum flow
	if (basal_amount > Maximum_Insulin_Flow) basal_amount = Maximum_Insulin_Flow;

	remaining_insulin -= basal_amount;
	Create_Remaining_Insulin_Level_Event();
	Create_Delivery_Event(scgms::signal_Delivered_Insulin_Basal_Rate, basal_amount, device_time);

	if (ReservoirEmpty())
	{
		Create_Insulin_Warning_Event();
		return;
	}

	double bolus_amount = bolus_to_deliver;
	if (bolus_amount < Minimum_Insulin_Amount) return;
	// deliver no more than Maximum_Insulin_Flow units in total
	if ((bolus_amount + basal_amount) > Maximum_Insulin_Flow) bolus_amount = Maximum_Insulin_Flow - basal_amount;
	// again, if there is not enough insulin, deliver the rest
	if (bolus_amount > remaining_insulin) bolus_amount = remaining_insulin;

	remaining_insulin -= bolus_amount;
	Create_Remaining_Insulin_Level_Event();
	Create_Delivery_Event(scgms::signal_Delivered_Insulin_Bolus, bolus_amount, device_time);
	bolus_to_deliver -= bolus_amount;

	if (ReservoirEmpty()) Create_Insulin_Warning_Event();
}

void CInDel_Filter::Create_Remaining_Insulin_Level_Event()
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
	event.signal_id() = scgms::signal_Remaining_Insulin;
	event.level() = (remaining_insulin / reservoir_capacity) * 100.0;

	mOutput.Send(event);
}

void CInDel_Filter::Create_Insulin_Warning_Event()
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Warning};
	event.signal_id() = scgms::signal_Remaining_Insulin;

	mOutput.Send(event);
}

void CInDel_Filter::Create_Delivery_Event(GUID signal_id, double level, double time)
{
	scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
	event.signal_id() = signal_id;
	event.level() = level;
	event.device_time() = time;

	mOutput.Send(event);
}

CInDel_Filter::CInDel_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CInDel_Filter::~CInDel_Filter()
{
	//
}

HRESULT IfaceCalling CInDel_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(indel_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CInDel_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	reservoir_capacity = configuration.Read_Double(indel_filter::rsReservoirCapacity, 20);

	remaining_insulin = 0.0;
	time_received = false;
	last_device_time = 0.0;
	basal_rate = 0.0;
	bolus_to_deliver = 0.0;

	return S_OK;
}

HRESULT IfaceCalling CInDel_Filter::Do_Execute(scgms::UDevice_Event event)
{
	if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		if (event.signal_id() == scgms::signal_Remaining_Insulin)
		{
			RefillInsulin(event.level());
		}
		// signal_BG means a step in the simulation time
		else if (event.signal_id() == scgms::signal_BG)
		{
			if (!time_received)
			{
				last_device_time = event.device_time();
				time_received = true;
			}
			else 
			{
				HRESULT res = mOutput.Send(event);
				
				if (ReservoirEmpty())
				{
					Create_Insulin_Warning_Event();
					bolus_to_deliver = 0.0;		// so the bolus doses cannot accumulate while there is no insulin
				}
				else 
				{
					DeliverInsulin(event.device_time());
				}
				last_device_time = event.device_time();

				return res;
			}
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Basal_Rate)
		{
			basal_rate = event.level();
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Bolus)
		{
			if (bolus_to_deliver < Minimum_Insulin_Amount)
			{
				bolus_to_deliver = event.level();
			}
		}
	}

	return mOutput.Send(event);
}
