#include "oled_filter.h"
#include "descriptor.h"

#include <oled_ssd1306.h>

#include <uart_print.h>
#include "rtl/rattime.h"

void COLED_Filter::drawMainScreen()
{
	sDisplay_SSD1306.Clear();

	sDisplay_SSD1306.Put_HLine(13);
	sDisplay_SSD1306.Put_HLine(26);
	sDisplay_SSD1306.Put_HLine(39);

	sDisplay_SSD1306.Put_VLine(64);
	sDisplay_SSD1306.Fill_Rectangle(0, 40, 128, 24, false);

	sDisplay_SSD1306.Put_String(0, 2, "--- mg/dL");
	sDisplay_SSD1306.Put_String(0, 15, "> -.-- U/h");
	sDisplay_SSD1306.Put_String(0, 28, "  -.- U");

	sDisplay_SSD1306.Put_String(65, 2, "  --:--:--");
	sDisplay_SSD1306.Put_String(65, 15, "   [#####]");
	sDisplay_SSD1306.Put_String(65, 28, "    --.- C");

	sDisplay_SSD1306.Put_String(40, 42, "STARTING");
	sDisplay_SSD1306.Put_String(7, 52, "Waiting for values.");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateBG(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(0, 0, 64, 13, false);

	char buffer[11];
	itoa(static_cast<int>(level), buffer, 10);
	if (level < 100.0)
	{
		sDisplay_SSD1306.Put_String(6, 2, buffer);
	}
	else
	{
		sDisplay_SSD1306.Put_String(0, 2, buffer);
	}

	sDisplay_SSD1306.Put_String(18, 2, " mg/dL");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateTime(double time)
{
	sDisplay_SSD1306.Fill_Rectangle(65, 0, 64, 13, false);

	std::string time_str = Rat_Time_To_Local_Time_Str(time, "  %H:%M:%S");
	sDisplay_SSD1306.Put_String(65, 2, time_str.c_str());

	sDisplay_SSD1306.Flip();
}

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

	drawMainScreen();

	return S_OK;
}

HRESULT IfaceCalling COLED_Filter::Do_Execute(scgms::UDevice_Event event)
{
	if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		if (event.signal_id() == scgms::signal_BG)
		{
			updateBG(event.level());
			updateTime(event.device_time());
		}
	}

	return mOutput.Send(event);
};