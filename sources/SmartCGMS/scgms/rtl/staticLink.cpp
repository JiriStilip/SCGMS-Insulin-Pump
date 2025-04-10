#include "staticLink.h"
#include <scgms/src/device_event.h>
#include <scgms/src/persistent_chain_configuration.h>
#include <scgms/src/filter_parameter.h>
#include <scgms/src/filter_configuration_executor.h>
#include <scgms/src/configuration_link.h>
#include <generated/filters.h>

void* resolve_symbol_static(const char *symbol_name) noexcept
{
	if (strcmp(symbol_name, "create_device_event") == 0) 
    {
        return reinterpret_cast<void*>(create_device_event);
    }
    if (strcmp(symbol_name, "get_filter_descriptors") == 0) 
    {
        return reinterpret_cast<void*>(get_all_descriptors);
    }
	if (strcmp(symbol_name, "create_persistent_filter_chain_configuration") == 0) 
    {
        return reinterpret_cast<void*>(create_persistent_filter_chain_configuration);
    }	
	if (strcmp(symbol_name, "execute_filter_configuration") == 0) 
    {
        return reinterpret_cast<void*>(execute_filter_configuration);
    }
	if (strcmp(symbol_name, "create_filter_parameter") == 0) 
    {
        return reinterpret_cast<void*>(create_filter_parameter);
    }
	if (strcmp(symbol_name, "create_filter_configuration_link") == 0) 
    {
        return reinterpret_cast<void*>(create_filter_configuration_link);
    }

    return nullptr;
}