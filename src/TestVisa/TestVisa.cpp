/*
TestVisa small u++ application main source file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#include "TestVisa.h"

testVisa* testVisa::m_Singleton=NULL;
testVisa::testVisa()
{
	m_Singleton=this;
	CtrlLayout(*this, "Keysight DMM SCPI test Visa api");
	bnInit.WhenPush=THISBACK(onBnInit);
	bnDeInit.WhenPush=THISBACK(onBnDeinit);
	
	bnOpen.WhenPush=THISBACK(onBnOpen);
	bnClose.WhenPush=THISBACK(onBnClose);
	
	bnTrigger.WhenPush=THISBACK(onBnTrigger);
	bnRead.WhenPush=THISBACK(onBnRead);
	
	ividmm.WhenInstrumentFind=THISBACK(onInstrumentFind);
	ividmm.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
	
	dlPreset.WhenAction=THISBACK(onDlPreset);
	bnOpen.Disable();
	bnClose.Disable();
	bnDeInit.Disable();
	bnTrigger.Disable();
	bnRead.Disable();
	dllogicalName.Disable();
	
	mLog.AddColumn("Cmd");
	mLog.AddColumn("Message");
}

GUI_APP_MAIN
{
	testVisa().Run();
}
