/*
TestVisa small u++ application main header file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#ifndef _TestVisa_TestVisa_h
#define _TestVisa_TestVisa_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <TestVisa/TestVisa.lay>
#include <CtrlCore/lay.h>

#include <VirtInst/VirtInst.h>

class InstrumentConnectPanel: public WithInstrumentConnectLayout<ParentCtrl>{
	typedef InstrumentConnectPanel CLASSNAME;
	public:
		InstrumentConnectPanel(InstrumentBase* instrument);
		void OnInit();
		void OnDeinit();
		void onInstrumentFind(char* instr){
			dllogicalName.Add(instr);
		}
		void onBnOpen();
		void onBnClose();
		InstrumentBase* mInstrument;
		Event<InstrumentBase*, bool, bool>  WhenInstrumentConnect;
};

class testVisa : public WithTestVisaLayout<TopWindow> {
public:
	typedef testVisa CLASSNAME;
	testVisa();
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
	void OnInstrumentConnect(InstrumentBase* instr, bool connected, bool successfull);
	void onOutput1();
	void onOutput2();

	void onDlRange(){
		int id= dlRange.GetData();
		if (!IsNull(id)){
			ividmm.setRange(id);
		}
	}
	void onDlPreset(){
		int id= dlPreset.GetData();
		if (!IsNull(id)){
			ividmm.setPreset(id);
		}
	}
	void onBnTrigger(){
		ividmm.sendTrigger();
	}
	void onBnRead(){
		//ividmm.clear();
		ividmm.read();
		dValue.SetData(ividmm.getValueDouble());
		mValue.SetData(ividmm.getValueString());
	}
	void onInstrumentCmdStatus(char* cmd, int scode, char* stext);

	IVIDmm ividmm;
	DcPsu  dcpsu;
	InstrumentConnectPanel tabDmmConnect;
	InstrumentConnectPanel tabDcPsuConnect;
	WithLogLayout<ParentCtrl> tabLog;
	WithStatusLayout<ParentCtrl> tabStatus;
	public:
	static testVisa* m_Singleton;
	static void fnTimer1(){
		if (m_Singleton){
			m_Singleton->onTimer1();
		}
	}
	void onTimer1(){
		if (ividmm.isOpen()){
			if (mRepeat.Get()){
				onBnRead();
			}
		}
		if (dcpsu.isOpen()){
			#define MAXCNTR (4)
			static int cntr=MAXCNTR;
			if (cntr){
				cntr--;
			}else{
				cntr=MAXCNTR;
				dcpsu.getOutputMeasurement(1);
				oOutput1.Set( dcpsu.getOutputChannelState(1)?1:0);
				dcpsu.getOutputMeasurement(2);
				oOutput2.Set( dcpsu.getOutputChannelState(2)?1:0);
				sOutpVolt1.SetData(dcpsu.getOutputVoltageString(1));
				sOutpVolt2.SetData(dcpsu.getOutputVoltageString(2));
			}
		}
		// Very provisoric solution, later move to separate task, timestamping, etc.
		SetTimeCallback(500, &fnTimer1);
	}

};

#endif
