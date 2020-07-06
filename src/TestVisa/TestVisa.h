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
		mIdn.SetData("Disconnected");
		onInstrumentCmdStatus("Deinit",0,"Ok.");
	}
	void onBnOpen(){
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
				dlPreset.Clear();
				for (int i=0; i< ividmm.getMaxConfigPreset(); i++){
					dlPreset.Add(i, ividmm.getConfigPresetName(i));
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
	void onBnClose(){
		if (ividmm.isOpen()){
			ividmm.close();
			bnTrigger.Disable();
			bnRead.Disable();
			bnOpen.Enable();
			bnClose.Disable();
			dllogicalName.Enable();
			dlPreset.Disable();
			mIdn.SetData("Disconnected");
			onInstrumentCmdStatus("Close",0,"Ok");
		}else{
			onInstrumentCmdStatus("Close",0,"It was not opened before.");
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
	void onInstrumentCmdStatus(char* cmd, int scode, char* stext){
		mCmd.SetData(cmd);
		mStatusCode.SetData(scode);
		mStatus.SetData(stext);
		mLog.Add(cmd, stext);
		LOGF("%s, %s\n",cmd, stext);
		mLog.SetCursor( mLog.GetCount()-1);
	}

	IVIDmm ividmm;
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
