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
//	if (m_GoToRemoteFirst){
		//was not working? retest!
		//statusCode=viGpibControlREN(session, VI_GPIB_REN_ASSERT_ADDRESS_LLO);
		//if (statusCode<0) handleError("VI_GPIB_REN_ASSERT_ADDRESS_LLO");
//	}
	return true;
}
bool DcPsu::close(){
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
//		if (m_GoToLocalBack){
			//Was not working
			//Send instrument back to local mode from Remote mode.
			//statusCode=viGpibControlREN(session, VI_GPIB_REN_ADDRESS_GTL);
			//if (statusCode<0) handleError("GPIB_GTL");
//		}
		statusCode=viClose(session);
	}
	session=0;
	m_IsOpen=false;
	return true;
}
void DcPsu::selectChannel(int channel)
{
	char cmmand[MAX_MESSAGE_LENGTH];
	sprintf(cmmand, "INST OUTP%d\n",channel);
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
void DcPsu::getOutputMeasurement(int channel){
	char cmmand[MAX_MESSAGE_LENGTH];
	char buffer[MAX_RESULT_LENGTH];
	if (m_Channel!=channel){
		selectChannel(channel);
	}
	
	sprintf(cmmand, "volt?\n");
	if(cmd(cmmand, buffer)){
		m_Voltage[channel]= atof(buffer);
	}else{
		 handleError("getOutputMeasurement");
	}
}
const char* DcPsu::getOutputVoltageString(int channel)
{
	const char* sUnit="V";
	if (m_Voltage[channel]>99e30){
		m_sVoltage[channel]="Overload (";
		m_sVoltage[channel]<<sUnit;
		m_sVoltage[channel]<<")";
	}
	else{
		m_sVoltage[channel]=eng(m_Voltage[channel], 9, 0);
		m_sVoltage[channel]<<sUnit;
	}
	return m_sVoltage[channel].Begin();
}