/*
Visa/IviDmm small DMM class implementation. Header file.
Author: Barna Farago MYND-ideal. (C)2020

Prereq:
Agilent IO Library Suite that can downloaded at : www.agilent.com/find/iosuite/
VISA Header Path:         $(VXIPNPPATH)winnt\include
VISA Library 32 Path:     $(VXIPNPPATH)winnt\lib\msc
VISA Library 64 Path:     $(VXIPNPPATH)winnt\Lib_x64\msc
*/
#ifndef _TestVisa_idmm_h_
#define _TestVisa_idmm_h_
#include "visa.h"

class IVIDmm {
	public:
	IVIDmm();
	~IVIDmm();
	
	void init();
	const char* getIdn(){
		return m_sIdn.Begin();
	}
	bool open();
	bool close();
	void deInit();
	
	bool cmd(char * cmmand, char* buffer);
	int getMaxConfigPreset()const { return C_MAX;}
	const char* getConfigPresetName(int id)const;
	void read();
	void reset();
	void clear();
	void sendTrigger();
	void handleError(char* cmmand);
	double getValueDouble()const;
	const char* getValueString();
	bool isOpen(){return m_IsOpen;}
	bool wasError(){return m_WasError;}
	String logicalName;
	bool IdQuery;
	bool ResetAtInit;
	Event<char*>  WhenInstrumentFind;
	Event<char*, int, char*>  WhenInstrumentCmdStatus;
	void setPreset(int id){ configPreset= (tConfigPreset_en)id;}
	typedef enum{
		C_VoltageDcAuto,
		C_VoltageDc1m,
		C_VoltageDc1,
		C_VoltageAcAuto,
		C_CurrentDcAuto,
		C_CurrentAcAuto,
		C_CapacitanceAuto,
		C_ResistanceAuto,
		C_MAX
	}tConfigPreset_en;
	tConfigPreset_en configPreset;
	
	private:
	bool m_GoToLocalBack;
	bool m_GoToRemoteFirst;
	bool m_IsInit;
	bool m_IsOpen;
	bool m_WasError;
	ViSession defaultRM;
	ViSession session;
	ViStatus statusCode;
	ViUInt32 numInstrs;
	ViFindList findList;
	double m_Value;
	String m_sValue;
	String m_sIdn;
	char* m_sUnit;
	char instrResourceString[VI_FIND_BUFLEN];
};

#endif
