#include <scgms/rtl/FilterLib.h>
#include <scgms/rtl/referencedImpl.h>
#include <scgms/utils/winapi_mapping.h>
#include <scgms/utils/string_utils.h>
#include <filters/config.h>
#include "scgms.h"

#if defined(FREERTOS)
#include <uart_print.h>
#endif

#if defined(WASM) || defined (ESP32)
void print(const char * str)
{
	printf("%s\n", str);
}
#endif

// HOTFIX undefined getentropy in newlib 3.30
extern "C" int getentropy(void*, size_t) {
	return 0;
}

scgms::SFilter_Executor Global_Filter_Executor;

const char * get_config_data()
{
	return config_data;
}

bool create_event(const SCGMSConcept_Event_Data *simple_event)
{
	if (simple_event->event_code >= static_cast<std::underlying_type_t<scgms::NDevice_Event_Code>>(scgms::NDevice_Event_Code::count))
		return false;

	scgms::UDevice_Event event_to_send{ static_cast<scgms::NDevice_Event_Code>(simple_event->event_code) };

	event_to_send.device_time() = simple_event->device_time;
	event_to_send.segment_id() = simple_event->segment_id;

	switch (scgms::UDevice_Event_internal::major_type(event_to_send.event_code())) {
		case scgms::UDevice_Event_internal::NDevice_Event_Major_Type::level:
			event_to_send.level() = simple_event->level;
			break;

		case scgms::UDevice_Event_internal::NDevice_Event_Major_Type::parameters:
			event_to_send.parameters->add(simple_event->parameters, simple_event->parameters + simple_event->count);
			break;

		case scgms::UDevice_Event_internal::NDevice_Event_Major_Type::info:
			event_to_send.info.set(simple_event->str);	
			break;

		default:
			break;
	}

	if(Global_Filter_Executor)
	{
		Global_Filter_Executor.Execute(std::move(event_to_send));
		return true;
	}

	return false;
}

int build_filter_chain(const char*  configuration_input)
{
	const char* config;
	if(configuration_input == NULL)
	{
		configuration_input = config_data;
		print("Creating SCGMS filter chain from config.h");	
	}
	else
	{
		print("Creating SCGMS filter chain from input");	
	}
	print("------------------------------------------");
	print("Configuration:");
	print("------------------------------------------");
	refcnt::Swstr_list errors = refcnt::Swstr_list{};
	errors.get();
	scgms::SPersistent_Filter_Chain_Configuration configuration{};
	if (configuration == NULL)
	{
		print("Failed to construct SPersistent_Filter_Chain_Configuration");
		return -1;
	}

	print(configuration_input);
	print("------------------------------------------");

	print("Config errors:");
	configuration->Load_From_Memory(configuration_input, strlen(configuration_input), errors.get());
	print("------------------------------------------");

	print("Filter executor construction:");
	Global_Filter_Executor = scgms::SFilter_Executor{ configuration.get(), nullptr, nullptr, errors };
	bool success = true;
	errors.for_each([&success](auto str) {print("error:");auto newstr = Narrow_WString(str);print(newstr.c_str());success = false;});
	print("------------------------------------------");

	if(Global_Filter_Executor && success)
	{
		print("Filter chain is ready to execute:");
		print("------------------------------------------");
	}
	else
	{
		print("Error constructing filter chain");
		print("------------------------------------------");
		return -1;
	}

	return 0;
}