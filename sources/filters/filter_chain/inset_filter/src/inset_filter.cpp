#include "inset_filter.h"
#include "descriptor.h"

#include <gpio_irq.h>

CInSet_Filter::CInSet_Filter(scgms::IFilter *output) : CBase_Filter(output)
{
	//
}

CInSet_Filter::~CInSet_Filter()
{
	//
}

void CInSet_Filter::Create_Event(GUID signal_id, double level, bool masked)
{
	if (masked)
	{
		scgms::UDevice_Event event{scgms::NDevice_Event_Code::Masked_Level};
		event.level() = level;
		event.signal_id() = signal_id;

		mOutput.Send(event);
	}
	else
	{
		scgms::UDevice_Event event{scgms::NDevice_Event_Code::Level};
		event.level() = level;
		event.device_time() = device_time;
		event.signal_id() = signal_id;

		mOutput.Send(event);
	}
}

void inset_task(void *pvParameters)
{
	CInSet_Filter *inset = (CInSet_Filter *)pvParameters;
	uint32_t notify_value = 0;
	double basal_rate = 0.0;
	bool bolus_setting = false;
	double bolus_dose = 0.0;
	while (true)
	{
		notify_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (notify_value & INSET_PLUS)
		{
			if (bolus_setting)
			{
				bolus_dose += 0.5;

				inset->Create_Event(scgms::signal_Requested_Insulin_Bolus, bolus_dose, true);
			}
			else
			{
				basal_rate += 0.05;

				inset->Create_Event(scgms::signal_Requested_Insulin_Basal_Rate, basal_rate);
			}
		}
		else if (notify_value & INSET_MINUS)
		{
			if (bolus_setting)
			{
				bolus_dose -= 0.5;
				if (bolus_dose < 0) bolus_dose = 0;

				inset->Create_Event(scgms::signal_Requested_Insulin_Bolus, bolus_dose, true);
			}
			else 
			{
				basal_rate -= 0.05;
				if (basal_rate < 0) basal_rate = 0;

				inset->Create_Event(scgms::signal_Requested_Insulin_Basal_Rate, basal_rate);
			}
		}
		else if (notify_value & INSET_SET)
		{
			if (bolus_setting)
			{
				inset->Create_Event(scgms::signal_Requested_Insulin_Bolus, bolus_dose);
				bolus_dose = 0.0;
				bolus_setting = false;
				inset->Create_Event(scgms::signal_Requested_Insulin_Basal_Rate, basal_rate, true);
			}
			else 
			{
				bolus_setting = true;
				inset->Create_Event(scgms::signal_Requested_Insulin_Bolus, bolus_dose, true);
			}
		}
	}
}

HRESULT IfaceCalling CInSet_Filter::QueryInterface(const GUID *riid, void **ppvObj)
{
	if (Internal_Query_Interface<scgms::IFilter>(inset_filter::id, *riid, ppvObj))
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT IfaceCalling CInSet_Filter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list &error_description)
{
	device_time = 0.0;

	xTaskCreate(inset_task, "Insulin Setting Filter", 4 * 1024, this, 1, &inset_task_handle);

	return S_OK;
}

HRESULT IfaceCalling CInSet_Filter::Do_Execute(scgms::UDevice_Event event)
{
	if (event.event_code() == scgms::NDevice_Event_Code::Level)
	{
		if (event.signal_id() == scgms::signal_BG)
		{
			device_time = event.device_time();
		}
	}

	return mOutput.Send(event);
}