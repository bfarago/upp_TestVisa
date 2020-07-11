#include "VirtInst.h"
/*
Visa/Instrument base class implementation. Source file.
Author: Barna Farago MYND-ideal. (C)2020

Prereq:
Agilent IO Library Suite that can downloaded at : www.agilent.com/find/iosuite/
VISA Header Path:         $(VXIPNPPATH)winnt\include
VISA Library 32 Path:     $(VXIPNPPATH)winnt\lib\msc
VISA Library 64 Path:     $(VXIPNPPATH)winnt\Lib_x64\msc
*/

#define LLOGF(...)
#define SEARCH_USB_INSTR ("?*INSTR")
//#define SEARCH_USB_INSTR ("USB?*INSTR")
//#define SEARCH_TCP_INSTR ("TCP?*INSTR")



InstrumentBase::InstrumentBase()
:
 m_IsInit(false), m_IsOpen(false), session(0)
 ,m_GoToLocalBack(true)	   //it works
 ,m_GoToRemoteFirst(false) //doesn't work on 34465A, therefore turned off
{
}
InstrumentBase::~InstrumentBase()
{
	// deInit();
}

void InstrumentBase::init(){
	ViStatus s;
	s=viOpenDefaultRM(&defaultRM);
	if (s < VI_SUCCESS)
	{
		LLOGF("Could not open a session to the VISA Resource Manager!\n");
		return;
	}

	s = viFindRsrc (defaultRM, SEARCH_USB_INSTR, &findList, &numInstrs, instrResourceString);
	if (s < VI_SUCCESS)
	{
		//viOpen(defaultRM, )
		LLOGF("Could not open a session to the VISA Resource Manager!\n");
		return;
	}
	for(unsigned int i=0; i<numInstrs; i++){
		if (i){
			s=viFindNext(findList, instrResourceString);
		}
		if (s >=VI_SUCCESS){
			WhenInstrumentFind(instrResourceString);
		}
	}
	m_IsInit=true;
}
void InstrumentBase::deInit(){
	if (m_IsOpen) close();
	statusCode=viClose(defaultRM);
	m_IsInit=false;
}

bool InstrumentBase::cmd(char * cmmand, char* buffer){
	if (!session) return false;
	statusCode = viPrintf (session, cmmand);
	if (statusCode < VI_SUCCESS)
	{
		//LLOGF ("Error writing to the device %d.\n", i+1);
		handleError(cmmand);
		return false;
	}
	if (buffer){
		statusCode = viScanf(session, "%t", buffer);
		if (statusCode < VI_SUCCESS){
			//LLOGF ("Error reading a response from the device %d.\n", i+1);
			handleError("scanf");
			return false;
		}
	}
	return true;
}
void InstrumentBase::handleError(char* cmmand)
{
	ViStatus s;
	ViChar buffer[MAX_MESSAGE_LENGTH];
	if (statusCode<0){
		m_WasError=true;
		s=viStatusDesc(session, statusCode, buffer);
		WhenInstrumentCmdStatus(cmmand, statusCode, buffer);
		clear();
		return;
	}
	//statusCode = viClose (session);
}
void InstrumentBase::clear()
{
	if (session>0)	{
		viClear(session);
		if (!m_WasError){
			cmd("*CLS", NULL);
		}
	}
	else{
		if (defaultRM>0){
		//	viClear(defaultRM);
		}
	}
	m_WasError=false;
}