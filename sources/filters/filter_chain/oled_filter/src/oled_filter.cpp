#include "oled_filter.h"
#include "descriptor.h"

#include <oled_ssd1306.h>

#include <ftoa.h>
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
	sDisplay_SSD1306.Put_String(0, 28, "  -.-  U");

	sDisplay_SSD1306.Put_String(65, 2, "  --:--:--");
	sDisplay_SSD1306.Put_String(65, 15, "   [     ]");
	sDisplay_SSD1306.Put_String(65, 28, "    --.- C");

	sDisplay_SSD1306.Put_String(40, 42, "STARTING");
	sDisplay_SSD1306.Put_String(7, 52, "Waiting for values.");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateBG(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(0, 0, 64, 13, false);

	if (level == 0)
	{
		sDisplay_SSD1306.Put_String(0, 2, "---");
	}
	else
	{
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

void COLED_Filter::updateBasal(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(0, 27, 6, 12, false);
	sDisplay_SSD1306.Put_Char(0, 15, '>');

	sDisplay_SSD1306.Fill_Rectangle(6, 14, 58, 12, false);

	char buffer[11];
	ftoa(level, buffer, 2);
	if (level < 10.0)
	{
		sDisplay_SSD1306.Put_String(12, 15, buffer);
	}
	else
	{
		sDisplay_SSD1306.Put_String(6, 15, buffer);
	}

	sDisplay_SSD1306.Put_String(36, 15, " U/h");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateBolus(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(0, 14, 6, 12, false);
	sDisplay_SSD1306.Put_Char(0, 28, '>');

	sDisplay_SSD1306.Fill_Rectangle(6, 27, 58, 12, false);

	char buffer[11];
	ftoa(level, buffer, 1);
	if (level < 10.0)
	{
		sDisplay_SSD1306.Put_String(12, 28, buffer);
	}
	else
	{
		sDisplay_SSD1306.Put_String(6, 28, buffer);
	}

	sDisplay_SSD1306.Put_String(36, 28, " U");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateReservoir(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(89, 14, 30, 12, false);

	int bars = 0;
	if (level > 80.0) bars = 5;
	else if (level > 60.0) bars = 4;
	else if (level > 40.0) bars = 3;
	else if (level > 20.0) bars = 2;
	else if (level > 0.0) bars = 1;
	else bars = 0;

	char buffer[6];
	for (int i = 0; i < 5; i++)
	{
		buffer[i] = (i < bars) ? '#' : ' '; 
	}
	buffer[5] = '\0';
	sDisplay_SSD1306.Put_String(89, 15, buffer);

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateTemperature(double level)
{
	sDisplay_SSD1306.Fill_Rectangle(65, 27, 64, 12, false);

	char buffer[11];
	ftoa(level, buffer, 1);
	if (level < 10.0)
	{
		sDisplay_SSD1306.Put_String(95, 28, buffer);
	}
	else
	{
		sDisplay_SSD1306.Put_String(89, 28, buffer);
	}

	sDisplay_SSD1306.Put_String(113, 28, " C");

	sDisplay_SSD1306.Flip();
}

void COLED_Filter::updateText(scgms::NDevice_Event_Code event_code, GUID signal_id)
{
	sDisplay_SSD1306.Fill_Rectangle(0, 40, 128, 24, false);

	if (event_code == scgms::NDevice_Event_Code::Level)
	{
		sDisplay_SSD1306.Put_String(37, 42, "NEW VALUE");
	}
	else if (event_code == scgms::NDevice_Event_Code::Error)
	{
		sDisplay_SSD1306.Put_String(49, 42, "ERROR");
	}
	else if (event_code == scgms::NDevice_Event_Code::Warning)
	{
		sDisplay_SSD1306.Put_String(43, 42, "WARNING");
	}

	if (signal_id == scgms::signal_BG)
	{
		sDisplay_SSD1306.Put_String(25, 52, "Blood Glucose");
	}
	else if (signal_id == scgms::signal_Air_Temperature)
	{
		sDisplay_SSD1306.Put_String(16, 52, "Air Temperature");
	}
	else if (signal_id == scgms::signal_Requested_Insulin_Basal_Rate)
	{
		sDisplay_SSD1306.Put_String(34, 52, "Basal Rate");
	}
	else if (signal_id == scgms::signal_Requested_Insulin_Bolus)
	{
		sDisplay_SSD1306.Put_String(49, 52, "Bolus");
	}
	else if (signal_id == scgms::signal_Remaining_Insulin)
	{
		sDisplay_SSD1306.Put_String(13, 52, "Remaining Insulin");
	}

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
		else if (event.signal_id() == scgms::signal_Air_Temperature)
		{
			updateTemperature(event.level());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Basal_Rate)
		{
			updateBasal(event.level());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Bolus)
		{
			updateBolus(event.level());
		}
		else if (event.signal_id() == scgms::signal_Remaining_Insulin)
		{
			updateReservoir(event.level());
		}
	}
	else if (event.event_code() == scgms::NDevice_Event_Code::Masked_Level)
	{
		if (event.signal_id() == scgms::signal_Requested_Insulin_Bolus)
		{
			updateBolus(event.level());
		}
		else if (event.signal_id() == scgms::signal_Requested_Insulin_Basal_Rate)
		{
			updateBasal(event.level());
		}
	}

	if (event.event_code() == scgms::NDevice_Event_Code::Error)
	{
		if (event.signal_id() == scgms::signal_BG)
		{
			updateBG(0);
		}
	}

	if (event.event_code() != scgms::NDevice_Event_Code::Masked_Level)
	{
		updateText(event.event_code(), event.signal_id());
	}

	return mOutput.Send(event);
}