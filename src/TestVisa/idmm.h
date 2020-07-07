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
	int getMaxConfigPreset()const { return C_MAXPreset;}
	int getMaxRange()const { return C_MAXRange;}
	const char* getConfigPresetName(int id)const;
	const char* IVIDmm::getConfigRange(int id)const;
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
	void SetGoToLocalBack(bool b){m_GoToLocalBack=b;}
	void setRange(int id){ m_RangeId= (tConfigRange_en)id;}
	void setPreset(int id){ configPreset= (tConfigPreset_en)id;}
	typedef enum{
		C_VoltageDcAuto,
		C_VoltageAcAuto,
		C_CurrentDcAuto,
		C_CurrentAcAuto,
		C_CapacitanceAuto,
		C_ResistanceAuto,
		C_MAXPreset
	}tConfigPreset_en;
	typedef enum{
		C_RangeAuto,
		C_Range1k,
		C_Range10,
		C_Range1,
		C_Range1m,
		C_Range1u,
		C_MAXRange
	}tConfigRange_en;
	tConfigPreset_en configPreset;
	
	private:
	bool m_GoToLocalBack;
	bool m_GoToRemoteFirst; // this one looks like not so usable
	bool m_IsInit;
	bool m_IsOpen;
	bool m_WasError;
	ViSession defaultRM;
	ViSession session;
	ViStatus statusCode;
	ViUInt32 numInstrs;
	ViFindList findList;
	tConfigRange_en m_RangeId;
	double m_RangeMax;
	double m_RangeResolution;
	double m_Value;
	String m_sValue;
	String m_sIdn;
	char* m_sUnit;
	char instrResourceString[VI_FIND_BUFLEN];
	void getRange(char* buf);
};

#endif
