
#include <rtl/FilterLib.h> // GENERATED
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

#include <rtl/guid.h>

namespace supervisor_filter {

	constexpr GUID id = { 0x095e8653, 0xde85, 0x4d78, { 0xbc, 0xd5, 0xa4, 0xcf, 0x77, 0x8a, 0x86, 0x91 } }; // {095E8653-DE85-4D78-BCD5-A4CF778A8691}

	extern const wchar_t* rsWatchdogInterval;
}

extern "C" HRESULT IfaceCalling do_create_filter_supervisor_filter(const GUID *id, scgms::IFilter *output, scgms::IFilter **filter) ;
extern "C" HRESULT IfaceCalling do_get_filter_descriptors_supervisor_filter(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end) ;