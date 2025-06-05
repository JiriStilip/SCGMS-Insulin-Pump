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

#include "descriptor.h"
#include "cgm_filter.h"

#include <iface/DeviceIface.h>
#include <iface/FilterIface.h>

#include <rtl/FilterLib.h>
#include <rtl/manufactory.h>
#include <utils/descriptor_utils.h>

#include <array>

/*
 * Example filter descriptor block
 */

namespace cgm_filter {

	constexpr size_t param_count = 3;

	const scgms::NParameter_Type param_type[param_count] = {
		scgms::NParameter_Type::ptDouble,
		scgms::NParameter_Type::ptDouble,
		scgms::NParameter_Type::ptDouble
	};

	const wchar_t* ui_param_name[param_count] = {
		L"Task Period Parameter",
		L"First Event Timestamp Parameter",
		L"Simulated Measurements Interval Parameter"
	};

	const wchar_t* rsTaskPeriod = L"task_period";
	const wchar_t* rsFirstEventTimestamp = L"first_event_timestamp";
	const wchar_t* rsMeasurementInterval = L"measurement_interval";

	const wchar_t* config_param_name[param_count] = {
		rsTaskPeriod,
		rsFirstEventTimestamp,
		rsMeasurementInterval
	};

	const wchar_t* ui_param_tooltips[param_count] = {
		L"Sets the period of the data reading task.",
		L"Sets the rattime timestamp of the first event.",
		L"Sets the interval between the simulated measurements."
	};

	const wchar_t* filter_name = L"CGM Input Filter";

	const scgms::TFilter_Descriptor descriptor = {
		id,
		scgms::NFilter_Flags::None,
		filter_name,
		param_count,
		param_type,
		ui_param_name,
		config_param_name,
		ui_param_tooltips
	};
}

/*
 * Array of available filter descriptors
 */

const std::array<scgms::TFilter_Descriptor, 1> filter_descriptions = { { cgm_filter::descriptor } };

/*
 * Filter library interface implementations
 */

extern "C" HRESULT IfaceCalling do_get_filter_descriptors(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end) {

	return do_get_descriptors(filter_descriptions, begin, end);
}

extern "C" HRESULT IfaceCalling do_create_filter(const GUID *id, scgms::IFilter *output, scgms::IFilter **filter) {

	if (*id == cgm_filter::descriptor.id) {
		return Manufacture_Object<CCGM_Filter>(filter, output);
	}

	return E_NOTIMPL;
}
