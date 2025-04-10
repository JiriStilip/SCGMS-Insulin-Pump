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

#include "../iface/referencedIface.h"

#include "DeviceLib.h"
#include "rattime.h"
#include "scgmsLib.h"

namespace imported {
	namespace {
		//all these vars have _external suffix not to confuse linker when building the libraries
		scgms::TCreate_Signal create_signal_external = scgms::factory::resolve_symbol<scgms::TCreate_Signal>("create_signal");
		scgms::TCreate_Device_Event create_device_event_external = scgms::factory::resolve_symbol<scgms::TCreate_Device_Event>("create_device_event");
	}
}


bool scgms::SModel_Parameter_Vector::set(const std::vector<double> &params) {
	double *data_ptr = const_cast<double*>(params.data());
	if (!operator bool()) {
		scgms::IModel_Parameter_Vector *new_vector = refcnt::Create_Container<double>(data_ptr, data_ptr + params.size());
		reset(new_vector, [](scgms::IModel_Parameter_Vector* obj_to_release) { if (obj_to_release != nullptr) obj_to_release->Release(); });
		return operator bool();
	} else
		return get()->set(data_ptr, data_ptr + params.size()) == S_OK;
}


bool scgms::SModel_Parameter_Vector::set(const scgms::SModel_Parameter_Vector &params) {
	if (!params) return false;

	double *begin, *end;
	if (params->get(&begin, &end) != S_OK) return false;

	if (!operator bool()) {
		scgms::IModel_Parameter_Vector *new_vector = refcnt::Create_Container<double>(begin, end);
		reset(new_vector, [](scgms::IModel_Parameter_Vector* obj_to_release) { if (obj_to_release != nullptr) obj_to_release->Release(); });
		return operator bool();
	} else
		return get()->set(begin, end) == S_OK;
}

bool scgms::SModel_Parameter_Vector::empty() const {
	if (!operator bool()) return true;
	return get()->empty() == S_OK;
}


scgms::SSignal::SSignal(scgms::STime_Segment segment, const GUID &signal_id) :
	scgms::SSignal::SSignal(segment, signal_id, Invalid_GUID) {
}

scgms::SSignal::SSignal(scgms::STime_Segment segment, const GUID& signal_id, const GUID& approx_id) {
	scgms::ISignal* signal;
	if (imported::create_signal_external(&signal_id, segment.get(), approx_id == Invalid_GUID ? nullptr : &approx_id, &signal) == S_OK) {
		reset(signal, [](scgms::ISignal* obj_to_release) { if (obj_to_release != nullptr) obj_to_release->Release(); });
	}
}

scgms::WSignal::WSignal(ISignal *signal) : mSignal(signal) {};

HRESULT scgms::WSignal::Get_Discrete_Levels(double* const times, double* const levels, const size_t count, size_t *filled) const {
	return mSignal ? mSignal->Get_Discrete_Levels(times, levels, count, filled) : E_FAIL;
}

HRESULT scgms::WSignal::Get_Discrete_Bounds(scgms::TBounds* const time_bounds, scgms::TBounds* const level_bounds, size_t *level_count) const {
	return mSignal ? mSignal->Get_Discrete_Bounds(time_bounds, level_bounds, level_count) : E_FAIL;
}


scgms::WTime_Segment::WTime_Segment(ITime_Segment *segment) : mSegment(segment) {

}

scgms::SSignal scgms::WTime_Segment::Get_Signal(const GUID &signal_id) {
	scgms::SSignal result;
	ISignal *obj = nullptr;

	if (mSegment)
		if (mSegment->Get_Signal(&signal_id, &obj) == S_OK)
			result = refcnt::make_shared_reference_ext<scgms::SSignal, scgms::ISignal>(obj, false);

	return result;
}

scgms::SSignal scgms::STime_Segment::Get_Signal(const GUID &signal_id) {
	scgms::SSignal result;
	ISignal *obj = nullptr;
	scgms::ITime_Segment *segment = get();

	if (segment)
		if (segment->Get_Signal(&signal_id, &obj) == S_OK)
			result = refcnt::make_shared_reference_ext<scgms::SSignal, scgms::ISignal>(obj, false);

	return result;
}

scgms::TDevice_Event* Get_Raw_Event(scgms::IDevice_Event *event) {
	scgms::TDevice_Event* result;
	if ((event == nullptr) || (event->Raw(&result) != S_OK)) result = nullptr;

	return result;
}

scgms::IDevice_Event* Create_Event(const scgms::NDevice_Event_Code code) {
	scgms::IDevice_Event *result;
	if (imported::create_device_event_external(code, &result) != S_OK) result = nullptr;
	assert(result != nullptr);
	return result;
}

scgms::UDevice_Event::UDevice_Event(const scgms::NDevice_Event_Code code) noexcept : UDevice_Event(Create_Event(code)) {
}

scgms::UDevice_Event::UDevice_Event(scgms::UDevice_Event&& event) noexcept: mRaw(event.mRaw) {
	event.release();
}

scgms::UDevice_Event::UDevice_Event(IDevice_Event *event) : std::unique_ptr<IDevice_Event, UDevice_Event_Deleter>(event), mRaw(Get_Raw_Event(event)){
	switch (major_type()) {
		case scgms::UDevice_Event_internal::NDevice_Event_Major_Type::info:
			if (mRaw->info) info = refcnt::make_shared_reference_ext<refcnt::Swstr_container, refcnt::wstr_container>(mRaw->info, true);
			break;

		case scgms::UDevice_Event_internal::NDevice_Event_Major_Type::parameters:
			if (mRaw->parameters) parameters = refcnt::make_shared_reference_ext<scgms::SModel_Parameter_Vector, scgms::IModel_Parameter_Vector>(mRaw->parameters, true);
			break;

		default:
			break;
	}
}

scgms::UDevice_Event_internal::NDevice_Event_Major_Type scgms::UDevice_Event::major_type() const {
	if (mRaw == nullptr) return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::null;
	return UDevice_Event_internal::major_type(mRaw->event_code);
}

scgms::UDevice_Event_internal::NDevice_Event_Major_Type scgms::UDevice_Event_internal::major_type(const scgms::NDevice_Event_Code code) {
	switch (code) {
		case scgms::NDevice_Event_Code::Level:
		case scgms::NDevice_Event_Code::Masked_Level:		return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::level;

		case scgms::NDevice_Event_Code::Parameters:
		case scgms::NDevice_Event_Code::Parameters_Hint:	return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::parameters;

		case scgms::NDevice_Event_Code::Information:
		case scgms::NDevice_Event_Code::Warning:
		case scgms::NDevice_Event_Code::Error:				return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::info;

		case scgms::NDevice_Event_Code::Time_Segment_Start:
		case scgms::NDevice_Event_Code::Time_Segment_Stop:
		case scgms::NDevice_Event_Code::Solve_Parameters:		
		case scgms::NDevice_Event_Code::Shut_Down:
		case scgms::NDevice_Event_Code::Warm_Reset:
		case scgms::NDevice_Event_Code::Suspend_Parameter_Solving:
		case scgms::NDevice_Event_Code::Resume_Parameter_Solving:
															return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::control;

		default:	break;
	}

	return scgms::UDevice_Event_internal::NDevice_Event_Major_Type::null;
}

void scgms::UDevice_Event::reset(IDevice_Event *event) {
	if (operator bool()) {
		auto deleter = get_deleter();
		auto obj = get();
		release();
		deleter(obj);
	}

	if (event) {
		if (event->Raw(&mRaw) == S_OK) event->Release();	//do not forget that we're moving the object!
	}
	
}

scgms::UDevice_Event scgms::UDevice_Event::Clone() {	
	scgms::IDevice_Event* clone;
	if (operator bool() && (get()->Clone(&clone) == S_OK)) {
		return scgms::UDevice_Event{ clone };
	}
	else
		return scgms::UDevice_Event{};
}

const scgms::NDevice_Event_Code& scgms::UDevice_Event::event_code() const {
	return mRaw->event_code;
}

const int64_t& scgms::UDevice_Event::logical_time() const {
	return mRaw->logical_time;
}

GUID& scgms::UDevice_Event::device_id() const {
	return mRaw->device_id;
}

GUID& scgms::UDevice_Event::signal_id() const {
	return mRaw->signal_id;
}

double& scgms::UDevice_Event::device_time() const {
	return mRaw->device_time;
}
	
uint64_t& scgms::UDevice_Event::segment_id() const {
	return mRaw->segment_id;
}

double& scgms::UDevice_Event::level() const {
	return mRaw->level;
}


bool scgms::UDevice_Event::is_level_event() const {
	return major_type() == scgms::UDevice_Event_internal::NDevice_Event_Major_Type::level;
}

bool scgms::UDevice_Event::is_parameters_event() const {
	return major_type() == scgms::UDevice_Event_internal::NDevice_Event_Major_Type::parameters;
}

bool scgms::UDevice_Event::is_info_event() const {
	return major_type() == scgms::UDevice_Event_internal::NDevice_Event_Major_Type::info;
}

bool scgms::UDevice_Event::is_control_event() const {
	return major_type() == scgms::UDevice_Event_internal::NDevice_Event_Major_Type::control;
}


scgms::CTime_Segment::~CTime_Segment()
{
	//
}

HRESULT IfaceCalling scgms::CTime_Segment::Get_Signal(const GUID *signal_id, scgms::ISignal **signal)
{
	auto itr = mSignals.find(*signal_id);
	if (itr != mSignals.end())
	{
		*signal = (*itr).second.get();
		(*signal)->AddRef();
		return S_OK;
	}

	// prefer calculated signal, fall back to measured signal
	if (imported::create_signal_external(signal_id, this, nullptr, signal) != S_OK) {
		return E_NOTIMPL;
	}
	mSignals[*signal_id] = refcnt::make_shared_reference_ext<scgms::SSignal, scgms::ISignal>(*signal, true);  // true due to creating "clone" of pointer with custom reference counter

	return S_OK;
}

scgms::STime_Segment scgms::CTime_Segment::Clone()
{
	// manufacture new segment
	scgms::CTime_Segment* cloned;
	if (Manufacture_Object<scgms::CTime_Segment>(&cloned) != S_OK)
		return {};

	size_t count;
	scgms::ISignal* target;

	std::vector<double> tmpTimes, tmpLevels;

	// clone each signal
	for (auto const& signal : mSignals)
	{
		// create signal in cloned object
		if (cloned->Get_Signal(&signal.first, &target) != S_OK)
			continue;

		// retrieve discrete bounds (this will fail for calculated signals, which we are not fancy copying)
		if (signal.second->Get_Discrete_Bounds(nullptr, nullptr, &count) != S_OK)
			continue;

		if (count == 0)
			continue;

		// resize to fit
		tmpTimes.resize(count);
		tmpLevels.resize(count);

		size_t filled = count;

		// retrieve all values
		if (signal.second->Get_Discrete_Levels(tmpTimes.data(), tmpLevels.data(), count, &filled) != S_OK)
			continue;

		// update cloned signal
		target->Update_Levels(tmpTimes.data(), tmpLevels.data(), filled);
	}

	return refcnt::make_shared_reference_ext<scgms::STime_Segment, scgms::ITime_Segment>(cloned, false);
}
