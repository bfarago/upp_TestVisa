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
#include "VirtInst.h"

class IVIDmm: public InstrumentBase {
	public:
	IVIDmm();
	~IVIDmm();
	
	bool open();
	bool close();
	
	int getMaxConfigPreset()const { return C_MAXPreset;}
	int getMaxRange()const { return C_MAXRange;}
	
	const char* getConfigPresetName(int id)const;
	const char* getConfigRange(int id)const;
	void read();
	void reset();
	void sendTrigger();
	double getValueDouble()const;
	const char* getValueString();
	
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

	void getRange(char* buf);
	tConfigRange_en m_RangeId;
	double m_RangeMax;
	double m_RangeResolution;
	
	double m_Value;
	String m_sValue;
	char* m_sUnit;
};

#endif
