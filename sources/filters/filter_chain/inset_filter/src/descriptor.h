
#include <rtl/FilterLib.h> // GENERATED
#pragma once

#include <rtl/guid.h>

namespace inset_filter {

	constexpr GUID id = { 0xb3b796c4, 0x46f8, 0x4f6c, { 0xa6, 0xc1, 0x37, 0xc0, 0xc1, 0x74, 0xbf, 0x90 } }; // {B3B796C4-46F8-4F6C-A6C1-37C0C174BF90}
}

extern "C" HRESULT IfaceCalling do_create_filter_inset_filter(const GUID *id, scgms::IFilter *output, scgms::IFilter **filter) ;
extern "C" HRESULT IfaceCalling do_get_filter_descriptors_inset_filter(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end) ;