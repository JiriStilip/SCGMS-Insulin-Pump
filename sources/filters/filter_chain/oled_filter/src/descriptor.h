
#include <rtl/FilterLib.h> // GENERATED
#pragma once

#include <rtl/guid.h>

namespace oled_filter {

	constexpr GUID id = { 0xc0683887, 0x6871, 0x4c87, { 0x80, 0xaa, 0x5d, 0x21, 0xc6, 0x8f, 0x16, 0x89 } }; // {C0683887-6871-4C87-80AA-5D21C68F1689}
}

extern "C" HRESULT IfaceCalling do_create_filter_oled_filter(const GUID *id, scgms::IFilter *output, scgms::IFilter **filter) ;
extern "C" HRESULT IfaceCalling do_get_filter_descriptors_oled_filter(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end) ;