//==============================================================================
//Copyright(c)  Meilhaus Electronic GmbH
//              http://www.meilhaus.de
//              support@meilhaus.de
//
//Source File : metypes.h                                              
//              typedefs of callback functions and structures used in ME-iDS
//==============================================================================

#ifndef _METYPES_H_
#define _METYPES_H_


typedef int (__stdcall * meErrorCB_t)(char *pcFunctionName, int iErrorCode);

typedef int (__stdcall * meIOStreamCB_t)(
		int iDevice,
		int iSubdevice,
		int iCount,
	   	void *pvContext,
	   	int iErrorCode);

typedef int  (__stdcall * meIOIrqCB_t)(
		int iDevice,
	   	int iSubdevice,
	   	int iChannel,
		int iIrqCount,
	   	int iValue,
	   	void *pvContext,
	   	int iErrorCode);


typedef struct meIOSingle {
	int iDevice;
	int iSubdevice;
	int iChannel;
	int iDir;
	int iValue;
	int iTimeOut;
	int iFlags;
	int iErrno;
} meIOSingle_t;


typedef struct meIOStreamConfig {
	int iChannel;
	int iStreamConfig;
	int iRef;
	int iFlags;
} meIOStreamConfig_t;


typedef struct meIOStreamTrigger {
	int iAcqStartTrigType;
	int iAcqStartTrigEdge;
	int iAcqStartTrigChan;
	int iAcqStartTicksLow;
	int iAcqStartTicksHigh;
	int iAcqStartArgs[10];
	int iScanStartTrigType;
	int iScanStartTicksLow;
	int iScanStartTicksHigh;
	int iScanStartArgs[10];
	int iConvStartTrigType;
	int iConvStartTicksLow;
	int iConvStartTicksHigh;
	int iConvStartArgs[10];
	int iScanStopTrigType;
	int iScanStopCount;
	int iScanStopArgs[10];
	int iAcqStopTrigType;
	int iAcqStopCount;
	int iAcqStopArgs[10];
	int iFlags;
} meIOStreamTrigger_t;


typedef struct meIOStreamStart {
	int iDevice;
	int iSubdevice;
	int iStartMode;
	int iTimeOut;
	int iFlags;
	int iErrno;
} meIOStreamStart_t;


typedef struct meIOStreamStop {
	int iDevice;
	int iSubdevice;
	int iStopMode;
	int iFlags;
	int iErrno;
} meIOStreamStop_t;


#endif
