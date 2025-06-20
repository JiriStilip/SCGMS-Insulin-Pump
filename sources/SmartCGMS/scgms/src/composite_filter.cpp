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

#include "composite_filter.h"
#include "device_event.h"
#if defined(EMBEDDED)
#include <filters/generated/filters.h>
#elif
#include "filters.h"
#endif

#include <scgms/lang/dstrings.h>
#include <scgms/utils/string_utils.h>
//#include <scgms/rtl/UILib.h>
#include <scgms/rtl/hresult.h>

#include <map>
#include <stdexcept>

#if defined(FREERTOS) || defined (WASM)
CComposite_Filter::CComposite_Filter() noexcept {
	//
}
#elif defined (ESP32)
CComposite_Filter::CComposite_Filter(std::recursive_mutex &communication_guard) noexcept : mCommunication_Guard(communication_guard) {
	//
}
#endif

HRESULT CComposite_Filter::Build_Filter_Chain(scgms::IFilter_Chain_Configuration *configuration, scgms::IFilter *next_filter, scgms::TOn_Filter_Created on_filter_created, const void* on_filter_created_data, refcnt::Swstr_list& error_description) noexcept {
	mRefuse_Execute = true;
	if (!mExecutors.empty())
		return E_ILLEGAL_METHOD_CALL;	//so far, we are able to configure the chain just once

	if (!configuration || !next_filter) return E_INVALIDARG;

#if defined(ESP32)
	std::lock_guard<std::recursive_mutex> guard{ mCommunication_Guard };
#endif
	scgms::IFilter *last_filter = next_filter;
		
	scgms::IFilter_Configuration_Link **link_begin, **link_end;
	HRESULT rc = configuration->get(&link_begin, &link_end);
	if (rc != S_OK) {
		error_description.push(dsCannot_read_configuration);
		return rc;
	}

	//we have to create the filter executors from the last one
	{
		auto send_shut_down = [this]() {
			if (mExecutors.empty())
				return;

			scgms::IDevice_Event* shutdown_event = allocate_device_event( scgms::NDevice_Event_Code::Shut_Down );
			if (shutdown_event)
				mExecutors[0]->Execute(shutdown_event);
		};

		size_t link_position = std::distance(link_begin, link_end);

		//1st round - create the filters
		do {
			link_position--;

			//scgms::IFilter_Configuration_Link* &link = *(link_end-1);	-- let's increase its ref count safely, because we are working with it
			scgms::SFilter_Configuration_Link link = refcnt::make_shared_reference_ext< scgms::SFilter_Configuration_Link, scgms::IFilter_Configuration_Link>(*(link_end - 1), true);

			GUID filter_id;
			rc = link->Get_Filter_Id(&filter_id);
			if (rc != S_OK) {
				error_description.push(dsCannot_read_filter_id);
				send_shut_down();
				mExecutors.clear();
				return rc;
			}
#if defined(ESP32)
			std::unique_ptr<CFilter_Executor> new_executor = std::make_unique<CFilter_Executor>(filter_id, mCommunication_Guard, last_filter, on_filter_created, on_filter_created_data);
#elif defined(FREERTOS) || defined(WASM)
			std::unique_ptr<CFilter_Executor> new_executor = std::make_unique<CFilter_Executor>(filter_id, last_filter, on_filter_created, on_filter_created_data);
#endif
			//try to configure the filter 
			if (!new_executor) {
				send_shut_down();
				mExecutors.clear();
				return E_OUTOFMEMORY;
			}


			rc = new_executor->Configure(link.get(), error_description.get());
			if (!Succeeded(rc)) {
				//if failed, we need to delete this, newly constructed filter first,
				//i.e., before clearing mExecutors because it is tied to resources,
				//which must be released AFTER destroying this filter
				new_executor.reset(nullptr);

				//describe such an event anyway just in the case the filter would not do so - hence we would at least know the configuration-failing filter
				std::wstring err_str{dsFailed_to_configure_filter};
				err_str += GUID_To_WString(filter_id);
				err_str += L"; filter zero-indexed position: ";
				err_str += std::to_wstring(link_position);
				
				bool failed_to_resolve_descriptor = false;
				{//try to obtain filter's name
					scgms::TFilter_Descriptor desc = scgms::Null_Filter_Descriptor;
					if (scgms::get_filter_descriptor_by_id(filter_id, desc) ) {
						err_str += L" \"";
						err_str += desc.description;
						err_str += L'"';
					}
					else
						failed_to_resolve_descriptor = true;
				}
				error_description.push(err_str.c_str());

				if (failed_to_resolve_descriptor)
					describe_loaded_filters(error_description);

				err_str = dsLast_RC + std::wstring{ Describe_Error(rc) };
				error_description.push(err_str.c_str());

				send_shut_down();
				
				mExecutors.clear();

				return rc;
			}

			//filter is configured, insert it into the chain
			last_filter = new_executor.get();
			mExecutors.insert(mExecutors.begin(), std::move(new_executor));
			
			link_end--;
		} while (link_end != link_begin);

		//2nd round - gather information about the feedback receivers
		std::map<std::wstring, scgms::SFilter_Feedback_Receiver> feedback_map;
		for (auto &possible_receiver : mExecutors) {
			scgms::SFilter_Feedback_Receiver feedback_receiver;
			refcnt::Query_Interface<scgms::IFilter, scgms::IFilter_Feedback_Receiver>(possible_receiver.get(), scgms::IID_Filter_Feedback_Receiver, feedback_receiver);
			if (feedback_receiver) {
				wchar_t *name;
				if (feedback_receiver->Name(&name) == S_OK) {
					feedback_map[name] = feedback_receiver;
				}
			}
		}

		//3nd round - set the receivers to the senders
		//multiple senders can connect to a single receiver (so that we can have a single feedback filter)
		if (!feedback_map.empty())
			for (auto &possible_sender : mExecutors) {
				refcnt::SReferenced<scgms::IFilter_Feedback_Sender> feedback_sender;
				refcnt::Query_Interface<scgms::IFilter, scgms::IFilter_Feedback_Sender>(possible_sender.get(), scgms::IID_Filter_Feedback_Sender, feedback_sender);
				if (feedback_sender) {
					wchar_t *name;
					if (feedback_sender->Name(&name) == S_OK) {

						auto feedback_receiver = feedback_map.find(name);
						if (feedback_receiver != feedback_map.end()) {
							feedback_sender->Sink(feedback_receiver->second.get());
						}
						else {
							std::wstring err_str{ dsFeedback_sender_not_connected };
							err_str += name;
							error_description.push(err_str.c_str());
							send_shut_down();
							mExecutors.clear();
							return E_FAIL;	//this is very likely severe error in the configuration, hence we stop it
						}
					}
				}
			}
	}

	mRefuse_Execute = false;
	return S_OK;
}

HRESULT CComposite_Filter::Execute(scgms::IDevice_Event *event) noexcept {
	if (!event) return E_INVALIDARG;
	if (mExecutors.empty()) {
		event->Release();
		return S_FALSE;
	}

#if defined(ESP32)
	std::lock_guard<std::recursive_mutex> lock_guard{ mCommunication_Guard };
#endif
	if (mRefuse_Execute) {
		event->Release();
		return E_ILLEGAL_METHOD_CALL;
	}

	return mExecutors[0]->Execute(event);	//and by this, we delegate event's release to the filters
}

HRESULT CComposite_Filter::Clear() noexcept {
	//obtain the communication guard/lock to ensure that no new communication will be accepted
	//via the execute method
	
	{
#if defined(ESP32)
		std::lock_guard<std::recursive_mutex> guard{ mCommunication_Guard };
#endif
		mRefuse_Execute = true;
	}

	//once we refuse any communication from the Execute method, we can safely release the filters
	//assuming that they terminate any threads they have spawned
	for (size_t i = 0; i < mExecutors.size(); i++)
		mExecutors[i]->Release_Filter();
	mExecutors.clear();	//calls reset on all contained unique ptr's	
	

	return S_OK;
}

bool CComposite_Filter::Empty() const noexcept {
	return mExecutors.empty();
}
