/*
TestVisa small u++ application main source file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#include "TestVisa.h"

InstrumentConnectPanel::InstrumentConnectPanel(InstrumentBase* instrument)
: mInstrument(instrument)
{
	bnOpen.WhenPush= THISBACK(onBnOpen);
	bnClose.WhenPush=THISBACK(onBnClose);
	if (mInstrument){
		instrument->WhenInstrumentFind=THISBACK(onInstrumentFind);
	}
	bnOpen.Disable();
	bnClose.Disable();
	oRelease.SetData("1");
	dllogicalName.Disable();
}
void InstrumentConnectPanel::OnInit()
{
	dllogicalName.Clear();
	dllogicalName.Enable();
	bnOpen.Enable();
	bnClose.Disable();
	mIdn.SetData("Disconnected");
}
void InstrumentConnectPanel::OnDeinit()
{
	dllogicalName.Disable();
	bnOpen.Disable();
	bnClose.Disable();
	mIdn.SetData("Disconnected");
}
void InstrumentConnectPanel::onBnOpen(){
	if (!mInstrument) return;
	if (!mInstrument->isOpen()){
		mInstrument->logicalName=~dllogicalName;
		if (mInstrument->open()){
			mIdn.SetData(mInstrument->getIdn());
			dllogicalName.Disable();
			bnOpen.Disable();
			bnClose.Enable();
			WhenInstrumentConnect(mInstrument, true, true);
		}else{
			bnOpen.Enable();
			bnClose.Disable();
			WhenInstrumentConnect(mInstrument, true, false);
		}
	}else{
		WhenInstrumentConnect(mInstrument, true, false);
	}
}

void InstrumentConnectPanel::onBnClose(){
	if (!mInstrument) return;
	if (mInstrument->isOpen()){
		mInstrument->SetGoToLocalBack(oRelease.Get() == 1);
		mInstrument->close();
		bnOpen.Enable();
		bnClose.Disable();
		dllogicalName.Enable();
		mIdn.SetData("Disconnected");
		WhenInstrumentConnect(mInstrument, false, true);
	}else{
		WhenInstrumentConnect(mInstrument, false, false);
	}
}
void testVisa::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull)
{
	if (connected){
		if (successfull){
			bnTrigger.Enable();
			bnRead.Enable();
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
			onInstrumentCmdStatus("Open",0,"Error");
			//onInstrumentCmdStatus("Open",0,"It was already opened before.");
		}
	}else{ //close
		if (successfull){
			bnTrigger.Disable();
			bnRead.Disable();
			dlPreset.Disable();
			dlRange.Disable();
			onInstrumentCmdStatus("Close",0,"Ok");
		}else{
			onInstrumentCmdStatus("Close",0,"It was not opened before.");
		}
	}
}
testVisa* testVisa::m_Singleton=NULL;
testVisa::testVisa()
:
	tabDmmConnect(&ividmm)
	,tabDcPsuConnect(&dcpsu)
{
	m_Singleton=this;
	CtrlLayout(*this, "DMM and PSU SCPI test Visa api");
	CtrlLayout(tabDmmConnect);
	CtrlLayout(tabDcPsuConnect);
	CtrlLayout(tabLog);
	CtrlLayout(tabStatus);
	tabDmmConnect.WhenInstrumentConnect=THISBACK(OnInstrumentConnect);
	tabDcPsuConnect.WhenInstrumentConnect=THISBACK(OnInstrumentConnect);
	
	bnInit.WhenPush=THISBACK(onBnInit);
	bnDeInit.WhenPush=THISBACK(onBnDeinit);
	
	bnTrigger.WhenPush=THISBACK(onBnTrigger);
	bnRead.WhenPush=THISBACK(onBnRead);
	
	ividmm.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
	dcpsu.WhenInstrumentCmdStatus=THISBACK(onInstrumentCmdStatus);
	
	oOutput1.WhenAction=THISBACK(onOutput1);
	oOutput2.WhenAction=THISBACK(onOutput2);
	
	dlPreset.WhenAction=THISBACK(onDlPreset);
	dlRange.WhenAction=THISBACK(onDlRange);

	bnDeInit.Disable();
	bnTrigger.Disable();
	bnRead.Disable();
	
	dlRange.Disable();

	tabsTop.Add(tabDmmConnect.SizePos(), "DMM");
	tabsTop.Add(tabDcPsuConnect.SizePos(), "DC PSU");
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

void testVisa::onOutput1(){
	bool b= oOutput1.Get();
	dcpsu.setOutputChannelState(1, b);
}
void testVisa::onOutput2(){
	bool b= oOutput2.Get();
	dcpsu.setOutputChannelState(2, b);
}
GUI_APP_MAIN
{
	testVisa().Run();
}
