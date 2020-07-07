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

#include "idmm.h"


class testVisa : public WithTestVisaLayout<TopWindow> {
public:
	typedef testVisa CLASSNAME;
	testVisa();
	void onBnInit(){
		dllogicalName.Clear();
		ividmm.init();
		dllogicalName.Enable();
		bnDeInit.Enable();
		bnInit.Disable();
		bnOpen.Enable();
		bnClose.Disable();
		dlPreset.Disable();
		dlRange.Disable();
		mIdn.SetData("Disconnected");
		onInstrumentCmdStatus("Init",0,"Ok.");
	}
	void onBnDeinit(){
		ividmm.deInit();
		dllogicalName.Disable();
		bnTrigger.Disable();
		bnRead.Disable();
		bnInit.Enable();
		bnDeInit.Disable();
		bnOpen.Disable();
		bnClose.Disable();
		dlPreset.Disable();
		dlRange.Disable();
		mIdn.SetData("Disconnected");
		onInstrumentCmdStatus("Deinit",0,"Ok.");
	}
	void onBnOpen();
	void onBnClose();
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
	void onInstrumentFind(char* instr){
		dllogicalName.Add(instr);
	}
	void onInstrumentCmdStatus(char* cmd, int scode, char* stext);

	IVIDmm ividmm;
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
		// Very provisoric solution, later move to separate task, timestamping, etc.
		SetTimeCallback(500, &fnTimer1);
	}
};

#endif
