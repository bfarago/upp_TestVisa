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
	CtrlLayout(tabLog);
	CtrlLayout(tabStatus);
	
	bnInit.WhenPush=THISBACK(onBnInit);
	bnDeInit.WhenPush=THISBACK(onBnDeinit);
	
	bnOpen.WhenPush=THISBACK(onBnOpen);
	bnClose.WhenPush=THISBACK(onBnClose);
	
	bnTrigger.WhenPush=THISBACK(onBnTrigger);
	bnRead.WhenPush=THISBACK(onBnRead);
	
	ividmm.WhenInstrumentFind=THISBACK(onInstrumentFind);
	ividmm.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
	
	dlPreset.WhenAction=THISBACK(onDlPreset);
	dlRange.WhenAction=THISBACK(onDlRange);
	bnOpen.Disable();
	bnClose.Disable();
	bnDeInit.Disable();
	bnTrigger.Disable();
	bnRead.Disable();
	dllogicalName.Disable();
	dlRange.Disable();
	oRelease.SetData("1");
	
	tabsBottom.Add(tabLog.SizePos(), "Log");
	tabsBottom.Add(tabStatus.SizePos(), "Status");
	tabLog.mLog.AddColumn("Cmd");
	tabLog.mLog.AddColumn("Message");
}
void testVisa::onInstrumentCmdStatus(char* cmd, int scode, char* stext){
	tabStatus.mCmd.SetData(cmd);
	tabStatus.mStatusCode.SetData(scode);
	tabStatus.mStatus.SetData(stext);
	tabLog.mLog.Add(cmd, stext);
	LOGF("%s, %s\n",cmd, stext);
	tabLog.mLog.SetCursor( tabLog.mLog.GetCount()-1);
}
void testVisa::onBnOpen(){
	if (!ividmm.isOpen()){
		ividmm.logicalName=~dllogicalName;
		if (ividmm.open()){
			mIdn.SetData(ividmm.getIdn());
			dllogicalName.Disable();
			bnTrigger.Enable();
			bnRead.Enable();
			bnOpen.Disable();
			bnClose.Enable();
			dlPreset.Enable();
			dlRange.Enable();
			dlPreset.Clear();
			dlRange.Clear();
			for (int i=0; i< ividmm.getMaxConfigPreset(); i++){
				dlPreset.Add(i, ividmm.getConfigPresetName(i));
			}
			for (int i=0; i< ividmm.getMaxRange(); i++){
				dlRange.Add(i, ividmm.getConfigRange(i));
			}
			onTimer1();
		}else{
			bnOpen.Enable();
			bnClose.Disable();
			//onInstrumentCmdStatus("Open",0,"Error");
		}
	}else{
		onInstrumentCmdStatus("Open",0,"It was already opened before.");
	}
}
void testVisa::onBnClose(){
	if (ividmm.isOpen()){
		ividmm.SetGoToLocalBack(oRelease.Get() == 1);
		ividmm.close();
		bnTrigger.Disable();
		bnRead.Disable();
		bnOpen.Enable();
		bnClose.Disable();
		dllogicalName.Enable();
		dlPreset.Disable();
		dlRange.Disable();
		mIdn.SetData("Disconnected");
		onInstrumentCmdStatus("Close",0,"Ok");
	}else{
		onInstrumentCmdStatus("Close",0,"It was not opened before.");
	}
}
GUI_APP_MAIN
{
	testVisa().Run();
}
