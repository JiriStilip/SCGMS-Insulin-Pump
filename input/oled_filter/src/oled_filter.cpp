#include "oled_filter.h"
#include "descriptor.h"

#include <oled_ssd1306.h>

#include <uart_print.h>

COLED_Filter::COLED_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

COLED_Filter::~COLED_Filter()
{
	//
}

HRESULT IfaceCalling COLED_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(oled_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling COLED_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
    sDisplay_SSD1306.Init();
	sDisplay_SSD1306.Set_Pixel(128, 59, false);
	sDisplay_SSD1306.Set_Pixel(128, 60, false);

	sDisplay_SSD1306.Put_String(25, 28, "Hello, world!");

	sDisplay_SSD1306.Flip();

	return S_OK;
}

HRESULT IfaceCalling COLED_Filter::Do_Execute(scgms::UDevice_Event event)
{
	return mOutput.Send(event);
};