#include "TestVisa.h"
InstrumentConnectPanel::InstrumentConnectPanel(InstrumentBase& instrument)
: mInstrument(instrument)
{
	bnOpen.WhenPush= THISBACK(onBnOpen);
	bnClose.WhenPush=THISBACK(onBnClose);
	mInstrument.WhenInstrumentFind=THISBACK(onInstrumentFind);
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
	mInstrument.init();
	mIdn.SetData("Disconnected");
}
void InstrumentConnectPanel::OnDeinit()
{
	dllogicalName.Disable();
	bnOpen.Disable();
	bnClose.Disable();
	mInstrument.deInit();
	mIdn.SetData("Disconnected");
}
void InstrumentConnectPanel::onBnOpen(){
	if (!mInstrument.isOpen()){
		mInstrument.logicalName=~dllogicalName;
		if (mInstrument.open()){
			mIdn.SetData(mInstrument.getIdn());
			dllogicalName.Disable();
			bnOpen.Disable();
			bnClose.Enable();
			WhenInstrumentConnect(&mInstrument, true, true);
		}else{
			bnOpen.Enable();
			bnClose.Disable();
			WhenInstrumentConnect(&mInstrument, true, false);
		}
	}else{
		WhenInstrumentConnect(&mInstrument, true, false);
	}
}

void InstrumentConnectPanel::onBnClose(){
	if (mInstrument.isOpen()){
		mInstrument.SetGoToLocalBack(oRelease.Get() == 1);
		mInstrument.close();
		bnOpen.Enable();
		bnClose.Disable();
		dllogicalName.Enable();
		mIdn.SetData("Disconnected");
		WhenInstrumentConnect(&mInstrument, false, true);
	}else{
		WhenInstrumentConnect(&mInstrument, false, false);
	}
}

InstrumentConfigPanel::InstrumentConfigPanel(IVIDmm& rDmm, DcPsu& rDcPsu)
:
	tabDmmConnect(rDmm)
	,tabDcPsuConnect(rDcPsu)
{
	CtrlLayout(tabDmmConnect);
	CtrlLayout(tabDcPsuConnect);
	tabDmmConnect.WhenInstrumentConnect=THISBACK(OnInstrumentConnect);
	tabDcPsuConnect.WhenInstrumentConnect=THISBACK(OnInstrumentConnect);
	tabsTop.Add(tabDmmConnect.SizePos(), "DMM");
	tabsTop.Add(tabDcPsuConnect.SizePos(), "DC PSU");
	bnInit.WhenPush=THISBACK(onBnInit);
	bnDeInit.WhenPush=THISBACK(onBnDeinit);
	bnDeInit.Disable();
}

void InstrumentConfigPanel::onBnInit(){
	tabDmmConnect.OnInit();
	tabDcPsuConnect.OnInit();
	bnDeInit.Enable();
	bnInit.Disable();
	//dlPreset.Disable();
	//dlRange.Disable();
	//onInstrumentCmdStatus("Init",0,"Ok.");
}
void InstrumentConfigPanel::onBnDeinit(){
	tabDmmConnect.OnDeinit();
	tabDcPsuConnect.OnDeinit();
	bnInit.Enable();
	bnDeInit.Disable();
	/*
	bnTrigger.Disable();
	bnRead.Disable();
	dlPreset.Disable();
	dlRange.Disable();
	onInstrumentCmdStatus("Deinit",0,"Ok.");
	*/
}

void InstrumentConfigPanel::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull)
{
	WhenInstrumentConnect(instr, connected, successfull);
}

InstrumentMonitor::InstrumentMonitor(){
	CtrlLayout(tabLog);
	CtrlLayout(tabStatus);
	tabsBottom.Add(tabLog.SizePos(), "Log");
	tabsBottom.Add(tabStatus.SizePos(), "Status");
	tabLog.mLog.AddColumn("Cmd");
	tabLog.mLog.AddColumn("Message");
}
void InstrumentMonitor::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull){
	if (connected && successfull){
		//tabLog.mLog.Clear();
		//tabLog.mLog.Clear();
	}
}
void InstrumentMonitor::onInstrumentCmdStatus(char* cmd, int scode, char* stext){
	static unsigned int sameCounter=0;
	String sPrevCmd= ~tabStatus.mCmd;
	String sPrevStatus= ~tabStatus.mStatus;
	if (sPrevCmd.IsEqual(cmd) && sPrevStatus.IsEqual(stext)){
		sameCounter++;
	}else{
		sameCounter=0;
		tabStatus.mCmd.SetData(cmd);
		tabStatus.mStatusCode.SetData(scode);
		tabStatus.mStatus.SetData(stext);
		tabLog.mLog.Add(cmd, stext);
		LOGF("%s, %s\n",cmd, stext);
		tabLog.mLog.SetCursor( tabLog.mLog.GetCount()-1);
	}
}

DmmPanel::DmmPanel(IVIDmm& rIviDmm)
: ividmm(rIviDmm)
{
	bnTrigger.WhenPush=THISBACK(onBnTrigger);
	bnRead.WhenPush=THISBACK(onBnRead);
	dlPreset.WhenAction=THISBACK(onDlPreset);
	dlRange.WhenAction=THISBACK(onDlRange);
	bnTrigger.Disable();
	bnRead.Disable();
	dlRange.Disable();
}
void DmmPanel::onBnTrigger(){
	ividmm.sendTrigger();
}
void DmmPanel::onBnRead(){
	//ividmm.clear();
	ividmm.read();
	//dValue.SetData(ividmm.getValueDouble());
	String s=ividmm.getValueString();
	mValue.SetData(s);
	SetExpandTitle("DMM " << s);
}
void DmmPanel::onDlRange(){
	int id= dlRange.GetData();
	if (!IsNull(id)){
		ividmm.setRange(id);
	}
}
void DmmPanel::onDlPreset(){
	int id= dlPreset.GetData();
	if (!IsNull(id)){
		ividmm.setPreset(id);
	}
}
void DmmPanel::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull)
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
		}
	}else{ //close
		if (successfull){
			mValue.SetData("-");
			bnTrigger.Disable();
			bnRead.Disable();
			dlPreset.Disable();
			dlRange.Disable();
			SetExpandTitle("DMM");
		}
	}
}
void DmmPanel::OnTimer(){
	if (mRepeat.Get()){
		onBnRead();
	}
}

DcPsuPanel::DcPsuPanel(DcPsu& rDcPsu)
: dcpsu(rDcPsu)
{
	oOutput1.WhenAction=THISBACK(onOutput1);
	oOutput2.WhenAction=THISBACK(onOutput2);
}
void DcPsuPanel::onOutput1(){
	bool b= oOutput1.Get();
	if (b){
		String sV= ~sOutpVolt1;
		String sA= ~sOutpAmp1;
		dcpsu.setApplyString(0, sV.begin(), sA.begin());
		sOutpVolt1.Enable(false);
		sOutpAmp1.Enable(false);
	}else{
		sOutpVolt1.Enable(true);
		sOutpAmp1.Enable(true);
	}
	dcpsu.setOutputChannelState(0, b);
}
void DcPsuPanel::onOutput2(){
	bool b= oOutput2.Get();
	if (b){
		String sV= ~sOutpVolt2;
		String sA= ~sOutpAmp2;
		dcpsu.setApplyString(1, sV.begin(), sA.begin());
		sOutpVolt2.Enable(false);
		sOutpAmp2.Enable(false);
	}else{
		sOutpVolt2.Enable(true);
		sOutpAmp2.Enable(true);
	}
	dcpsu.setOutputChannelState(1, b);
}
void DcPsuPanel::OnTimer(){
	#define MAXCNTR (2)
	static int cntr=MAXCNTR;
	if (cntr){
		cntr--;
	}else{
		String sTitle="DC PSU ";
		String s;
		cntr=MAXCNTR;
		dcpsu.getOutputMeasurement(0);
		bool b=dcpsu.getOutputChannelState(0);
		oOutput1.Set( b?1:0);
		if (b){
			sOutpVolt1.SetData(dcpsu.getOutputVoltageString(0));
			sOutpAmp1.SetData(dcpsu.getOutputAmpereString(0));
			sTitle+="Out1 ";
		}
		
		dcpsu.getOutputMeasurement(1);
		b=dcpsu.getOutputChannelState(1);
		oOutput2.Set(b?1:0 );
		if (b){
			sOutpVolt2.SetData(dcpsu.getOutputVoltageString(1));
			sOutpAmp2.SetData(dcpsu.getOutputAmpereString(1));
			sTitle+="Out2 ";
		}
		
		s=dcpsu.getMeasuredVoltageString(0);
		sTitle+=s+" ";
		sMeasuredVolt1.SetData(s);
		s=dcpsu.getMeasuredAmpereString(0);
		sTitle+=s+", ";
		sMeasuredAmp1.SetData(s);
		s=dcpsu.getMeasuredVoltageString(1);
		sTitle+=s+" ";
		sMeasuredVolt2.SetData(s);
		s=dcpsu.getMeasuredAmpereString(1);
		sTitle+=s;
		sMeasuredAmp2.SetData(s);
	
		SetExpandTitle(sTitle);
	}
}
void DcPsuPanel::OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull)
{
	if (connected){
		if (successfull){
			sOutpVolt1.Enable(true);
			sOutpVolt2.Enable(true);
			sOutpAmp1.Enable(true);
			sOutpAmp2.Enable(true);
			oOutput1.Enable(true);
			oOutput2.Enable(true);
		}
	}else{ //close
		if (successfull){
			//disable
			sOutpVolt1.SetData("-");
			sOutpVolt2.SetData("-");
			sOutpAmp1.SetData("-");
			sOutpAmp2.SetData("-");
			sMeasuredVolt1.SetData("-");
			sMeasuredVolt2.SetData("-");
			sMeasuredAmp1.SetData("-");
			sMeasuredAmp2.SetData("-");
			sOutpVolt1.Enable(false);
			sOutpVolt2.Enable(false);
			sOutpAmp1.Enable(false);
			sOutpAmp2.Enable(false);
			oOutput1.Enable(false);
			oOutput2.Enable(false);
			SetExpandTitle("DC PSU");
		}
	}
}