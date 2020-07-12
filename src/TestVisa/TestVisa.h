/*
TestVisa small u++ application main header file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#ifndef _TestVisa_TestVisa_h
#define _TestVisa_TestVisa_h

#include <CtrlLib/CtrlLib.h>
#include <ExpandFrame/ExpandFrame.h>

using namespace Upp;

#define LAYOUTFILE <TestVisa/TestVisa.lay>
#include <CtrlCore/lay.h>

#include <VirtInst/VirtInst.h>
#include "InstrumentConnect.h"

class testVisa : public WithTestVisaLayout<TopWindow> {
public:
	typedef testVisa CLASSNAME;
	testVisa();
	/*
	void onBnInit(){
		tabDmmConnect.OnInit();
		tabDcPsuConnect.OnInit();
		ividmm.init();
		dcpsu.init();
		bnDeInit.Enable();
		bnInit.Disable();
		dlPreset.Disable();
		dlRange.Disable();
		onInstrumentCmdStatus("Init",0,"Ok.");
	}
	void onBnDeinit(){
		tabDmmConnect.OnDeinit();
		tabDcPsuConnect.OnDeinit();
		ividmm.deInit();
		dcpsu.deInit();
		bnTrigger.Disable();
		bnRead.Disable();
		bnInit.Enable();
		bnDeInit.Disable();
		dlPreset.Disable();
		dlRange.Disable();
		onInstrumentCmdStatus("Deinit",0,"Ok.");
	}
	*/
	void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
	void onInstrumentCmdStatus(char* cmd, int scode, char* stext);
	
	IVIDmm ividmm;
	DcPsu  dcpsu;
	ExpanderCtrl expander;
	DmmPanel dmmPanel;
	DcPsuPanel dcPsuPanel;
	

	InstrumentConfigPanel instrumentConfigPanel;
	InstrumentMonitor instrumentMonitorPanel;


	public:
	static testVisa* m_Singleton;
	static void fnTimer1(){
		if (m_Singleton){
			m_Singleton->onTimer1();
		}
	}
	void onTimer1(){
		if (ividmm.isOpen()){
			dmmPanel.OnTimer();
		}
		if (dcpsu.isOpen()){
			dcPsuPanel.OnTimer();
		}
		// Very provisoric solution, later move to separate task, timestamping, etc.
		SetTimeCallback(500, &fnTimer1);
	}

};

#endif
