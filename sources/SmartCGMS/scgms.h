#pragma once
#include <stdint.h>
#include <scgms/rtl/hresult.h>
#include <scgms/utils/winapi_mapping.h>
typedef struct _SCGMSConcept_Event_Data {
	uint8_t event_code;

	double device_time;				//signal with multiple values are aggregated by device_time with the same signal_id and device_id
	int64_t logical_time;

	uint64_t segment_id;			// segment identifier or Invalid_Segment_Id

	//the following members are not in a union to make it easier for certain languages
	double level;					//level event

	double *parameters;				//parameters event
	size_t count;

	wchar_t *str;					//info event
} SCGMSConcept_Event_Data;

#ifdef __cplusplus
extern "C" {
#endif
const char * get_config_data();
int build_filter_chain(const char* configuration); 
void create_level_event(double level_input);
void create_shutdown_event();
bool create_event(const SCGMSConcept_Event_Data *simple_event);
#ifdef __cplusplus
}
#endif
