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

#include "../iface/DeviceIface.h"
#include "referencedImpl.h"
#include "guid.h"

#include <memory>
#include <map>
#include <vector>

namespace scgms {

	class SModel_Parameter_Vector : public std::shared_ptr<IModel_Parameter_Vector> {
	public:
		bool set(const std::vector<double> &params);
		bool set(const SModel_Parameter_Vector &params);
		bool empty() const;
	};

	class STime_Segment;

	class SSignal : public std::shared_ptr<ISignal> {
	public:
		SSignal() {};	//just an empty object
		SSignal(STime_Segment segment, const GUID &signal_id);
		SSignal(STime_Segment segment, const GUID& signal_id, const GUID& approx_id);
	};

	class WSignal {
	protected:
		ISignal * mSignal;
	public:
		WSignal(ISignal *signal);

		HRESULT Get_Discrete_Levels(double* const times, double* const levels, const size_t count, size_t *filled) const;
		HRESULT Get_Discrete_Bounds(scgms::TBounds* const time_bounds, scgms::TBounds* const level_bounds, size_t *level_count) const;
	};

	class WTime_Segment { //: public std::weak_ptr<ITime_Segment> { --cannot inherit because shared-weak relationship cannot live across COM interface
						  //Perhaps, we should make STime_Segment export friendly function to WTimeSegment that will take WTime_Segment's
						  //as callback of STime_Segment. STime_Segment will call the WTime_Segment's callback from STime_Segment's dctor.
						  //That way, WTime_Segment will learn about STime_Segment's desctruction and return appropriate error codes from that moment on.
	protected:
		ITime_Segment * mSegment;
	public:
		WTime_Segment(ITime_Segment *segment);
		SSignal Get_Signal(const GUID &signal_id);
	};

	class STime_Segment : public std::shared_ptr<ITime_Segment> {
	public:
		SSignal Get_Signal(const GUID &signal_id);
	};


	//because unique_ptr::reset does not let us to specify deleter lambda, let's do it this way
	struct UDevice_Event_Deleter {
		void operator()(IDevice_Event *obj_to_release) { if (obj_to_release != nullptr) obj_to_release->Release(); };
	};

	namespace UDevice_Event_internal {
		enum class NDevice_Event_Major_Type : uint8_t {
			null = 0,
			level,
			info,
			parameters,
			control
		};

		NDevice_Event_Major_Type major_type(const scgms::NDevice_Event_Code code);
	}

	class UDevice_Event : public std::unique_ptr<IDevice_Event, UDevice_Event_Deleter> {
	protected:
		TDevice_Event * mRaw;		//mRaw must be initialized in the constructor exactly once
									//therefore, the implementation defines two helper functions,
									//which returns pointers only
		scgms::UDevice_Event_internal::NDevice_Event_Major_Type major_type() const;
	public:
		explicit UDevice_Event(const NDevice_Event_Code code = NDevice_Event_Code::Nothing)  noexcept;
		UDevice_Event(UDevice_Event&& event) noexcept;	
		UDevice_Event(IDevice_Event *event);			
		void reset(IDevice_Event *event);	

		UDevice_Event Clone();

														//this must be const, because level, parameters and info shared the same data space!!!
														//it is 100% fool proof, but programmer should still easily discover the error when overwriting e.g., info with level and then reading info
		const NDevice_Event_Code& event_code() const;
		const int64_t& logical_time() const;

		GUID& device_id() const;
		GUID& signal_id() const;
		double& device_time() const;
		uint64_t& segment_id() const;
		double& level() const;


		SModel_Parameter_Vector parameters;
		refcnt::Swstr_container info;

		bool is_level_event() const;
		bool is_parameters_event() const;
		bool is_info_event() const;
		bool is_control_event() const;
	};


#pragma warning(push)
#pragma warning( disable : 4250 ) // C4250 - 'class1' : inherits 'class2::member' via dominance

	class CTime_Segment final : public virtual ITime_Segment, public virtual refcnt::CReferenced
	{
	private:
		// managed signals; created by calling Get_Signal
		std::map<GUID, scgms::SSignal> mSignals;

	public:
		// default constructor
		CTime_Segment() = default;
		// disable copying, allow just cloning
		CTime_Segment(const CTime_Segment& b) = delete;
		virtual ~CTime_Segment();

		virtual HRESULT IfaceCalling Get_Signal(const GUID *signal_id, scgms::ISignal **signal) override;

		// clones this segment into another; calls AddRef (passes ownership to caller)
		STime_Segment Clone();
	};

#pragma warning( pop ) 

}
