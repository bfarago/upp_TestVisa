#ifndef _VirtInst_ibase_h_
#define _VirtInst_ibase_h_

#include "visa.h"
#include <CtrlLib/CtrlLib.h>
#include <Core/Core.h>

#define MAX_MESSAGE_LENGTH (1024) // status or error response
#define MAX_RESULT_LENGTH (512)   // simple value result

using namespace Upp;

class InstrumentBase{
	public:
		InstrumentBase();
		virtual ~InstrumentBase();
		void init();
		void deInit();
		bool isOpen(){return m_IsOpen;}
		bool wasError(){return m_WasError;}
		const char* getIdn()const { return m_sIdn.Begin();}
		void SetGoToLocalBack(bool b){m_GoToLocalBack=b;}
		bool cmd(char * cmmand, char* buffer);
		void handleError(char* cmmand);
		void clear();

		Event<char*>  WhenInstrumentFind;
		Event<char*, int, char*>  WhenInstrumentCmdStatus;
		String logicalName;
		virtual bool open()=0;
		virtual bool close()=0;
	protected:
		ViSession defaultRM;
		ViSession session;
		ViStatus statusCode;
		ViUInt32 numInstrs;
		ViFindList findList;
		char instrResourceString[VI_FIND_BUFLEN];
		bool m_IsInit;
		bool m_IsOpen;
		bool m_WasError;
		String m_sIdn;
		bool m_GoToLocalBack;
		bool m_GoToRemoteFirst; // this one looks like not so usable
};

#endif
