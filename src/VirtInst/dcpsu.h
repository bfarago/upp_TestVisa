#ifndef _VirtInst_dcpsu_h_
#define _VirtInst_dcpsu_h_
#include "VirtInst.h"

#define DCPSU_MAXCHANNEL (4)

class DcPsu : public InstrumentBase {
	public:
		DcPsu();
		~DcPsu();
		bool open();
		bool close();
		void selectChannel(int channel);
		bool getOutputChannelState(int channel);
		void setOutputChannelState(int channel, bool state);
		void getOutputMeasurement(int channel);
		void setApplyString(int channel, const char* volt, const char* amp);
		void setApply(int channel, double volt, double amp);
		double getOutputVoltageDouble(int channel){return m_Voltage[channel];}
		double getMeasuredVoltageDouble(int channel){return m_MeasuredVoltage[channel];}
		double getMeasuredAmpereDouble(int channel){return m_MeasuredAmpere[channel];}
		const char* getOutputVoltageString(int channel);
		const char* getOutputAmpereString(int channel);
		const char* getMeasuredVoltageString(int channel);
		const char* getMeasuredAmpereString(int channel);
		
	protected:
		double m_Voltage[DCPSU_MAXCHANNEL];
		double m_Ampere[DCPSU_MAXCHANNEL];
		double m_MeasuredVoltage[DCPSU_MAXCHANNEL];
		double m_MeasuredAmpere[DCPSU_MAXCHANNEL];
		String m_sVoltage[DCPSU_MAXCHANNEL];
		String m_sAmpere[DCPSU_MAXCHANNEL];
		String m_sMeasuredVoltage[DCPSU_MAXCHANNEL];
		String m_sMeasuredAmpere[DCPSU_MAXCHANNEL];
		int m_Channel;
};
#endif
