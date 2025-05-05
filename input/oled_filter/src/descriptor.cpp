#include "descriptor.h"
#include "oled_filter.h"

#include <iface/DeviceIface.h>
#include <iface/FilterIface.h>

#include <rtl/FilterLib.h>
#include <rtl/manufactory.h>
#include <utils/descriptor_utils.h>

#include <array>

/*
 * Filter descriptor block
 */

namespace oled_filter {

	constexpr size_t param_count = 0;

	const wchar_t* filter_name = L"OLED display filter";

	const scgms::TFilter_Descriptor descriptor = {
		id,
		scgms::NFilter_Flags::None,
		filter_name,
		param_count,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
}

/*
 * Array of available filter descriptors
 */

const std::array<scgms::TFilter_Descriptor, 1> filter_descriptors = { { oled_filter::descriptor } };

/*
 * Filter library interface implementations
 */

extern "C" HRESULT IfaceCalling do_get_filter_descriptors(scgms::TFilter_Descriptor **begin, scgms::TFilter_Descriptor **end) {

	return do_get_descriptors(filter_descriptors, begin, end);
}

extern "C" HRESULT IfaceCalling do_create_filter(const GUID *id, scgms::IFilter *output, scgms::IFilter **filter) {

	if (*id == oled_filter::descriptor.id) {
		return Manufacture_Object<COLED_Filter>(filter, output);
	}

	return E_NOTIMPL;
}
