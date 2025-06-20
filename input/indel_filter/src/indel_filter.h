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

#pragma once

#include <rtl/FilterLib.h>
#include <rtl/referencedImpl.h>

#pragma warning(push)
#pragma warning(disable : 4250) // C4250 - 'class1' : inherits 'class2::member' via dominance

constexpr double Maximum_Insulin_Flow = 5.0;
constexpr double Minimum_Insulin_Amount = 0.001;

class CInDel_Filter : public scgms::CBase_Filter
{
private:
	double reservoir_capacity;
	double remaining_insulin;
	bool time_received;
	double last_device_time;
	double basal_rate;
	double bolus_to_deliver;
protected:
	virtual HRESULT Do_Execute(scgms::UDevice_Event event) override final;
	virtual HRESULT Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description) override final;
public:
	double GetReservoirCapacity();
	double RefillInsulin(double percentage);
	bool ReservoirEmpty();
	void DeliverInsulin(double device_time);
	void Create_Remaining_Insulin_Level_Event();
	void Create_Insulin_Warning_Event();
	void Create_Delivery_Event(GUID signal_id, double level, double time);
	CInDel_Filter(scgms::IFilter *output);
	virtual ~CInDel_Filter();
	virtual HRESULT IfaceCalling QueryInterface(const GUID *riid, void **ppvObj) override final;
};

#pragma warning(pop)
