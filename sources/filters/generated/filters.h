#pragma once
#include <filters/filter_chain/indel_filter/src/descriptor.h>
#include <filters/filter_chain/print_filter/src/descriptor.h>
#include <filters/filter_chain/inset_filter/src/descriptor.h>
#include <filters/filter_chain/cgm_filter/src/descriptor.h>
#include <filters/filter_chain/auxin_filter/src/descriptor.h>
#include <filters/filter_chain/supervisor_filter/src/descriptor.h>
#include <filters/filter_chain/oled_filter/src/descriptor.h>
#include <scgms/iface/UIIface.h>

class CLoaded_Filters
{
protected:
    std::vector<scgms::TFilter_Descriptor> mFilter_Descriptors;

public:
    CLoaded_Filters();
    void load_descriptors();
    HRESULT get_filter_descriptors_body(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end);
};

HRESULT get_all_descriptors(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end);
scgms::SFilter create_filter_body(const GUID &id, scgms::IFilter *next_filter);
void describe_loaded_filters(refcnt::Swstr_list error_description);

void* resolve_generated_symbol(const char *symbol_name) noexcept;