#ifndef _TestVisa_InstrumentConnect_h_
#define _TestVisa_InstrumentConnect_h_

class ExpandInstrument{
	public:
		ExpandFrame* frame;
		void SetExpandFrame(ExpandFrame& rFrame){ frame=&rFrame;}
		void SetExpandTitle(const String& rString){ if (frame) frame->SetTitle(rString.Begin());}
};

class InstrumentConnectPanel: public WithInstrumentConnectLayout<ParentCtrl>{
	typedef InstrumentConnectPanel CLASSNAME;
	public:
		InstrumentConnectPanel(InstrumentBase& instrument);
		void OnInit();
		void OnDeinit();
		void onInstrumentFind(char* instr){
			dllogicalName.Add(instr);
		}
		void onBnOpen();
		void onBnClose();
		InstrumentBase& mInstrument;
		Event<InstrumentBase*, bool, bool>  WhenInstrumentConnect;
};


class InstrumentConfigPanel: public WithInstrumentConfigLayout<ParentCtrl>{
	typedef InstrumentConfigPanel CLASSNAME;
	public:
		InstrumentConnectPanel tabDmmConnect;
		InstrumentConnectPanel tabDcPsuConnect;
		
		InstrumentConfigPanel(IVIDmm& rDmm, DcPsu& rDcPsu);
		
		void onBnInit();
		void onBnDeinit();
		
		void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
		Event<InstrumentBase*, bool, bool>  WhenInstrumentConnect;
};

class InstrumentMonitor: public WithMonitorLayout<ParentCtrl>{
	typedef InstrumentMonitor CLASSNAME;
	public:
	InstrumentMonitor();
		
	WithLogLayout<ParentCtrl> tabLog;
	WithStatusLayout<ParentCtrl> tabStatus;
	void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
	void onInstrumentCmdStatus(char* cmd, int scode, char* stext);
};

class DmmPanel: public WithDmmLayout<ParentCtrl>, public ExpandInstrument{
	typedef DmmPanel CLASSNAME;
	public:
		DmmPanel(IVIDmm& rIviDmm);
		void OnTimer();
		void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
		void onBnTrigger();
		void onBnRead();
		void onDlRange();
		void onDlPreset();
		IVIDmm& ividmm;
};

class DcPsuPanel: public WithDcPsuLayout<ParentCtrl>, public ExpandInstrument{
	typedef DcPsuPanel CLASSNAME;
	public:
		DcPsuPanel(DcPsu& rDcPsu);
		void OnTimer();
		void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
		void onOutput1();
		void onOutput2();
		DcPsu& dcpsu;
};
#endif
