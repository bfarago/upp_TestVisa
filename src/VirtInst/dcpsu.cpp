#include "VirtInst.h"
#include "eng.h"
DcPsu::DcPsu()
:m_Channel(0)
{
}
DcPsu::~DcPsu()
{
}
bool DcPsu::open(){
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
		cmd("SYSTem:REMote", 0);
	}
	return true;
}
bool DcPsu::close(){
	clear();
	if (session) {
		cmd("*CLS",NULL);
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
			cmd("SYSTem:LOCal", 0);
		}
		statusCode=viClose(session);
	}
	session=0;
	m_IsOpen=false;
	return true;
}
void DcPsu::selectChannel(int channel)
{
	char cmmand[MAX_MESSAGE_LENGTH];
	sprintf(cmmand, "INST OUTP%d\n",channel+1);
	if(cmd(cmmand, 0)){
		m_Channel=channel;
	}else{
		m_Channel=0;
		 handleError(cmmand);
	}
}
bool DcPsu::getOutputChannelState(int channel)
{
	char cmmand[MAX_MESSAGE_LENGTH];
	char buffer[MAX_RESULT_LENGTH];
	if (m_Channel!=channel){
		selectChannel(channel);
	}
	
	sprintf(cmmand, "OUTP?\n");
	if(cmd(cmmand, buffer)){
		return buffer[0]=='1';
	}else{
		 handleError("getOutputChannelState");
	}
	return false;
}
void DcPsu::setOutputChannelState(int channel, bool state){
	char cmmand[MAX_MESSAGE_LENGTH];
	if (m_Channel!=channel){
		selectChannel(channel);
	}
	sprintf(cmmand, "OUTP %d\n", state?1:0);
	if(cmd(cmmand, 0)){
	}else{
		 handleError("setOutputChannelState");
	}
}
void DcPsu::setApplyString(int channel, const char* volt, const char* amp){
	double dv= eng_2d(volt);
	double da= eng_2d(amp);
	setApply(channel, dv, da);
}
void DcPsu::setApply(int channel, double volt, double amp){
	char cmmand[MAX_MESSAGE_LENGTH];
	//char buffer[MAX_RESULT_LENGTH];
	if (m_Channel!=channel){
		selectChannel(channel);
	}
	sprintf(cmmand, "APPLY %f,%f\n", volt, amp);
	if(cmd(cmmand, 0)){
		m_Voltage[channel]=volt;
		m_Ampere[channel]=amp;
	}else{
		 handleError("setApply");
	}
	
}
void DcPsu::getOutputMeasurement(int channel){
	char cmmand[MAX_MESSAGE_LENGTH];
	char buffer[MAX_RESULT_LENGTH];
	if (m_Channel!=channel){
		selectChannel(channel);
	}
	/*
	sprintf(cmmand, "volt?\n");
	if(cmd(cmmand, buffer)){
		m_Voltage[channel]= atof(buffer);
	}else{
		 handleError("getOutputMeasurement");
	}
	*/
	sprintf(cmmand, "APPLy?\n");
	if(cmd(cmmand, buffer)){
		String s=buffer;
		int sep=s.Find(',');
		if (sep>0){
			buffer[sep]=0;
			m_Voltage[channel]= atof(buffer);
			m_Ampere[channel]= atof(&buffer[sep+1]);
		}
	}else{
		 handleError("getOutputMeasurement");
	}
	sprintf(cmmand, "MEASure:VOLTage?\n");
	if(cmd(cmmand, buffer)){
		m_MeasuredVoltage[channel]= atof(buffer);
	}else{
		 handleError("getOutputMeasurement");
	}
	sprintf(cmmand, "MEASure:CURRent?\n");
	if(cmd(cmmand, buffer)){
		m_MeasuredAmpere[channel]= atof(buffer);
	}else{
		 handleError("getOutputMeasurement");
	}
}
const char* DcPsu::getOutputVoltageString(int channel)
{
	double& d=m_Voltage[channel];
	String& s=m_sVoltage[channel];
	const char* sUnit="V";
	if (d>99e30){
		s="Overload (";
		s<<sUnit;
		s<<")";
	}
	else{
		s=eng(d, 6, 0);
		s<<sUnit;
	}
	return s.Begin();
}
const char* DcPsu::getOutputAmpereString(int channel)
{
	double& d=m_Ampere[channel];
	String& s=m_sAmpere[channel];
	const char* sUnit="A";
	if (d>99e30){
		s="Overload (";
		s<<sUnit;
		s<<")";
	}
	else{
		s=eng(d, 3, 0);
		s<<sUnit;
	}
	return s.Begin();
}
const char* DcPsu::getMeasuredVoltageString(int channel)
{
	double& d=m_MeasuredVoltage[channel];
	String& s=m_sMeasuredVoltage[channel];
	const char* sUnit="V";
	if (d>99e30){
		s="Overload (";
		s<<sUnit;
		s<<")";
	}
	else{
		s=eng(d, 6, 0);
		s<<sUnit;
	}
	return s.Begin();
}
const char* DcPsu::getMeasuredAmpereString(int channel)
{
	double& d=m_MeasuredAmpere[channel];
	String& s=m_sMeasuredAmpere[channel];
	const char* sUnit="A";
	if (d>99e30){
		s="Overload (";
		s<<sUnit;
		s<<")";
	}
	else{
		s=eng(d, 6, 0);
		s<<sUnit;
	}
	return s.Begin();
}