#ifndef _VirtInst_dcpsu_h_
#define _VirtInst_dcpsu_h_
#include "VirtInst.h"

#define DCPSU_MAXCHANNEL (8)

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
		double getOutputVoltageDouble(int channel){return m_Voltage[channel];}
		const char* getOutputVoltageString(int channel);
		
	protected:
		double m_Voltage[DCPSU_MAXCHANNEL];
		String m_sVoltage[DCPSU_MAXCHANNEL];
		int m_Channel;
};
#endif
