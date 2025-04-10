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

#include "ema_filter.h"
#include "descriptor.h"

CEma_Filter::CEma_Filter(scgms::IFilter *output) : CBase_Filter(output) {
	//
}

CEma_Filter::~CEma_Filter() {
	//
}

HRESULT IfaceCalling CEma_Filter::QueryInterface(const GUID*  riid, void ** ppvObj) {
	if (Internal_Query_Interface<scgms::IFilter>(ema_filter::id, *riid, ppvObj)) return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CEma_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list& error_description) {

	// let us read configured parameters
	auto example_num = configuration.Read_Double(ema_filter::rsAlpha, 0.3);

	if (example_num < 0 || example_num > 1) {
		error_description.push(L"Alpha parameter must be between 0 and 1!");
		return E_INVALIDARG;
	}
	else
	{
		alpha = example_num;
	}

	// return S_OK on success
	return S_OK;
}

HRESULT IfaceCalling CEma_Filter::Do_Execute(scgms::UDevice_Event event) {
	if (event.is_level_event()) {
		if(accumulator == 0)
		{
			accumulator = event.level();
		}
		else
		{
			accumulator = (alpha * event.level()) + (1.0 - alpha) * accumulator;
			event.level() = accumulator;
		}
	}
	return mOutput.Send(event);
};
