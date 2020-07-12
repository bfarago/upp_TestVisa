/*
TestVisa small u++ application main source file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#include "TestVisa.h"

testVisa* testVisa::m_Singleton=NULL;
testVisa::testVisa()
:
	dmmPanel(ividmm)
	,dcPsuPanel(dcpsu)
	,instrumentConfigPanel(ividmm, dcpsu)
{
	Size sz = Size(555, 200);
	m_Singleton=this;
	CtrlLayout(*this, "DMM and PSU SCPI test Visa api");
	Sizeable();
	SetMinSize(sz);
	SetRect(sz);
	
	Add(expander.SizePos());
	
	CtrlLayout(instrumentConfigPanel);
	expander.AddExpander(instrumentConfigPanel, true).SetTitle("Connection Config");
	instrumentConfigPanel.WhenInstrumentConnect=THISBACK(OnInstrumentConnect);
	
	CtrlLayout(dmmPanel);
	dmmPanel.SetExpandFrame( expander.AddExpander(dmmPanel, false).SetTitle("DMM") );
	
	CtrlLayout(dcPsuPanel);
	dcPsuPanel.SetExpandFrame( expander.AddExpander(dcPsuPanel, false).SetTitle("DC PSU") );
	
	CtrlLayout(instrumentMonitorPanel);
	expander.AddExpander(instrumentMonitorPanel, true).SetTitle("Monitor");
	
	ividmm.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
	dcpsu.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
}

void testVisa::onInstrumentCmdStatus(char* cmd, int scode, char* stext){
	instrumentMonitorPanel.onInstrumentCmdStatus(cmd, scode, stext);
}

void testVisa::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull)
{
	if (instr == &ividmm){
		dmmPanel.OnInstrumentConnect(instr, connected, successfull);
	}else if (instr == &dcpsu){
		dcPsuPanel.OnInstrumentConnect(instr, connected, successfull);
	}
	instrumentMonitorPanel.OnInstrumentConnect(instr, connected, successfull);
	if (connected){
		if (successfull){
			onTimer1();
		}else{
			onInstrumentCmdStatus("Open",0,"Error");
			//onInstrumentCmdStatus("Open",0,"It was already opened before.");
		}
	}else{ //close
		if (successfull){
			onInstrumentCmdStatus("Close",0,"Ok");
		}else{
			onInstrumentCmdStatus("Close",0,"It was not opened before.");
		}
	}
}

GUI_APP_MAIN
{
	testVisa().Run();
}
