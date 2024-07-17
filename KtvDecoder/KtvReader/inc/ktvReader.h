#pragma once
#ifndef _KTVREADER_H_
#define _KTVREADER_H_

//#include <windows.h>
#include <wtypes.h>
#include <stdint.h>
#include <WinUSB.h>
#include <WinBase.h>
#include <tchar.h>
#include <stdbool.h>

static HANDLE SCardStartedEvent = NULL;
#define READER_NAME  _T("KTV_READER ")

/*
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef signed int		int32_t;
*/

#define EM28XX_REG_I2C_CLK		0x06
#define EM2874_REG_TS_ENABLE	0x5f
#define EM28XX_REG_GPIO			0x80

#define EM2874_REG_CAS_STATUS	0x70
#define EM2874_REG_CAS_DATALEN	0x71
#define EM2874_REG_CAS_MODE1	0x72
#define EM2874_REG_CAS_RESET	0x73
#define EM2874_REG_CAS_MODE2	0x75

#define EM2874_EP_TS1		0x84

#define DEMOD_ADDR	0x20
#define EEPROM_ADDR	0xa0
#define TUNER_ADDR	0xc0

/* EM2874 TS Enable Register (0x5f) */
#define EM2874_TS1_CAPTURE_ENABLE 0x01
#define EM2874_TS1_FILTER_ENABLE  0x02
#define EM2874_TS1_NULL_DISCARD   0x04

#define EM2874_TS

extern LONG KtvCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext);
extern LONG KtvCardListReaders(SCARDCONTEXT hContext, LPCTSTR mszGroups, LPTSTR mszReaders, LPDWORD pcchReaders);
extern LONG KtvCardConnect(SCARDCONTEXT hContext, LPCTSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol);
extern LONG KtvCardTransmit(SCARDHANDLE hCard, LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, const DWORD cbSendLength, LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength);
extern LONG KtvCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition);
extern LONG KtvCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem);
extern LONG KtvCardReleaseContext(SCARDCONTEXT hContext);


#endif