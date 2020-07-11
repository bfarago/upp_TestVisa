/*
Visa/IviDmm small DMM class implementation. Source file.
Author: Barna Farago MYND-ideal. (C)2020

Prereq:
Agilent IO Library Suite that can downloaded at : www.agilent.com/find/iosuite/
VISA Header Path:         $(VXIPNPPATH)winnt\include
VISA Library 32 Path:     $(VXIPNPPATH)winnt\lib\msc
VISA Library 64 Path:     $(VXIPNPPATH)winnt\Lib_x64\msc
*/

#include "TestVisa.h"
//#include "iviDmm.h"
#include "eng.h"

#define LLOGF(...)
#define SEARCH_USB_INSTR ("?*INSTR")
//#define SEARCH_USB_INSTR ("USB?*INSTR")
#define SEARCH_TCP_INSTR ("TCP?*INSTR")

#define MAX_MESSAGE_LENGTH (1024) // status or error response
#define MAX_RESULT_LENGTH (512)   // simple value result

IVIDmm::IVIDmm()
:
 m_IsInit(false), m_IsOpen(false), session(0),
 m_GoToLocalBack(true),	  //it works
 m_GoToRemoteFirst(false) //doesn't work on 34465A, therefore turned off
{
}
IVIDmm::~IVIDmm(){
	deInit();
}
void IVIDmm::init(){
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
bool IVIDmm::open(){
	char* cstr=(char*)logicalName.Begin();
	
	statusCode = viOpen (defaultRM, cstr, VI_NULL, VI_NULL, &session);
	if (statusCode < VI_SUCCESS)
	{
		handleError(cstr);
		return false;
	}
	char * cmmand ="*IDN?\n";
	char buffer[MAX_RESULT_LENGTH];
	if(cmd("*IDN?\n", buffer)){
		m_sIdn=buffer;
	}else{
		return false;
	}
	m_IsOpen=true;
	WhenInstrumentCmdStatus("Open", 0, "Ok");
	if (m_GoToRemoteFirst){
		statusCode=viGpibControlREN(session, VI_GPIB_REN_ASSERT_ADDRESS_LLO);
		if (statusCode<0) handleError("VI_GPIB_REN_ASSERT_ADDRESS_LLO");
	}
	return true;
}
bool IVIDmm::close(){
	clear();
	if (session) {
		cmd("*CLS",NULL);
		// cmd("SYST:LOCAL", NULL); //doesn't work
		/*
		if (mLocked){
			statusCode=viUnlock(session);
			if (statusCode<0) {
				handleError("Unlock");
			}
		}
		*/
		if (m_GoToLocalBack){
			//Send instrument back to local mode from Remote mode.
			statusCode=viGpibControlREN(session, VI_GPIB_REN_ADDRESS_GTL);
			if (statusCode<0) handleError("GPIB_GTL");
		}
		statusCode=viClose(session);
	}
	session=0;
	m_IsOpen=false;
	return true;
}
void IVIDmm::deInit(){
	if (m_IsOpen) close();
	statusCode=viClose(defaultRM);
	m_IsInit=false;
}

bool IVIDmm::cmd(char * cmmand, char* buffer){
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
const char* IVIDmm::getConfigPresetName(int id)const
{
	const char* p;
	switch (id){
		case C_VoltageDcAuto: p="Voltage DC"; break;
		case C_VoltageAcAuto: p="Voltage AC"; break;
		case C_CurrentDcAuto: p="Current DC"; break;
		case C_CurrentAcAuto: p="Current AC"; break;
		case C_CapacitanceAuto: p="Capacitance"; break;
		case C_ResistanceAuto: p="Resistance"; break;
		default:
			p="Invalid preset";
			break;
	}
	return p;
}
const char* IVIDmm::getConfigRange(int id)const
{
	const char* p;
	switch (id){
		case C_RangeAuto: p="Auto"; break;
		case C_Range1k: p="1k"; break;
		case C_Range10: p="10"; break;
		case C_Range1: p="1"; break;
		case C_Range1m: p="1 mili"; break;
		case C_Range1u: p="1 micro"; break;
		default:
			p="Invalid range";
			break;
	}
	return p;
}
void IVIDmm::getRange(char* buf){
	switch(m_RangeId){
		case 0: sprintf(buf, ""); break;
		case 1: sprintf(buf, " 1000,0.001"); break;
		case 2: sprintf(buf, " 10,0.001"); break;
		case 3: sprintf(buf, " 1,0.001"); break;
		case 4: sprintf(buf, " 0.001,0.000001"); break;
		case 5: sprintf(buf, " 0.000001,0.000000001"); break;
	}
}
void IVIDmm::read()
{
	bool ok;
	char* cfg;
	char range[MAX_RESULT_LENGTH];
	unsigned char buffer[MAX_RESULT_LENGTH];
	char*p=(char*)buffer;
	switch (configPreset){
		case C_VoltageDcAuto: m_sUnit="Vdc"; cfg="CONF:VOLT:DC"; break;
		case C_VoltageAcAuto: m_sUnit="Vac"; cfg="CONF:VOLT:AC"; break;
		case C_CurrentDcAuto: m_sUnit="Adc"; cfg="CONF:CURR:DC"; break;
		case C_CurrentAcAuto: m_sUnit="Aac"; cfg="CONF:CURR:AC"; break;
		case C_CapacitanceAuto: m_sUnit="F"; cfg="CONF:CAP"; break;
		case C_ResistanceAuto: m_sUnit="Ohm"; cfg="CONF:RES"; break;
		default:
			configPreset=C_VoltageDcAuto;
			m_sUnit="Vdc"; cfg="CONF:VOLT:DC";
			break;
	}
	
	getRange(&range[0]);
	sprintf(p, "%s%s\n", cfg, range);
	ok=cmd(p, NULL);
	if (!ok) {
		return;
	}
	ok=cmd("READ?\n", p);
	if (!ok) {
		return;
	}
	m_Value= atof(p);
	WhenInstrumentCmdStatus("Read", 0, "Ok");
}
void IVIDmm::reset()
{
//		statusCode=IviDmm_reset(session);
}
void IVIDmm::clear()
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
void IVIDmm::sendTrigger()
{
	// Does not work
	statusCode=viAssertTrigger(session,0 );
	if (statusCode<0) handleError("Trigger");
	//statusCode=IviDmm_SendSoftwareTrigger(session);
}
void IVIDmm::handleError(char* cmmand)
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
double IVIDmm::getValueDouble()const
{
	return m_Value;
}

const char* IVIDmm::getValueString()
{
	if (m_Value>99e30){
		m_sValue="Overload (";
		m_sValue<<m_sUnit;
		m_sValue<<")";
	}/*else if (m_Value<1e-30){ //infinit small
		m_sValue="Range Error (";
		m_sValue<<m_sUnit;
		m_sValue<<")";
	}*/
	else{
		m_sValue=eng(m_Value, 9, 0);
		m_sValue<<m_sUnit;
	}
	return m_sValue.Begin();
}