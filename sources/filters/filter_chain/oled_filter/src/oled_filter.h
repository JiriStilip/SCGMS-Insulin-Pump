#pragma once

#include <rtl/FilterLib.h>
#include <rtl/referencedImpl.h>

#include <oled_ssd1306.h>

#pragma warning( push )
#pragma warning( disable : 4250 ) // C4250 - 'class1' : inherits 'class2::member' via dominance

class COLED_Filter : public scgms::CBase_Filter {	
	protected:
		virtual HRESULT Do_Execute(scgms::UDevice_Event event) override final;
		virtual HRESULT Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list& error_description) override final;
	public:
		void drawMainScreen();
		void updateBG(double level);
		void updateTime(double time);
		COLED_Filter(scgms::IFilter *output);
		virtual ~COLED_Filter();
	
		virtual HRESULT IfaceCalling QueryInterface(const GUID*  riid, void ** ppvObj) override final;
};

#pragma warning( pop )
