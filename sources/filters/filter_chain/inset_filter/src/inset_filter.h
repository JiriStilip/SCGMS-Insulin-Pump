#pragma once

#include <rtl/FilterLib.h>
#include <rtl/referencedImpl.h>

#pragma warning( push )
#pragma warning( disable : 4250 ) // C4250 - 'class1' : inherits 'class2::member' via dominance

class CInSet_Filter : public scgms::CBase_Filter {	
	private:
		double device_time;
	protected:
		virtual HRESULT Do_Execute(scgms::UDevice_Event event) override final;
		virtual HRESULT Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list& error_description) override final;
	public:
		CInSet_Filter(scgms::IFilter *output);
		virtual ~CInSet_Filter();
		virtual HRESULT IfaceCalling QueryInterface(const GUID*  riid, void ** ppvObj) override final;
		void Create_Event(GUID signal_id, double level, bool masked = false);
};

#pragma warning( pop )
