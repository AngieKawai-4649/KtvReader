
//#include <stdio.h>

#include "ktvReader.h"

HANDLE usbHandle;
UINT cardResponseTime;


static uint8_t cardPCB = 0;
static bool isCardReady = false;

static uint8_t ICC_checkSum(const uint8_t* data, int len)
{
	uint8_t sum = 0;
	for (; len > 0; len--) {
		sum ^= *data++;
	}
	return sum;
}

static void milliWait(DWORD s)
{
	Sleep(s);
}


static bool readReg(WINUSB_INTERFACE_HANDLE usbHandle, const uint8_t idx, uint8_t* val)
{
	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0xc0;
	spkt.Index = idx;
	spkt.Length = 1;

	ULONG ret;
	BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)val, 1, &ret, NULL);
	if (!bRet) {
	}
	return (ret == 1) && bRet;
}

static uint8_t readRegB(WINUSB_INTERFACE_HANDLE usbHandle, const uint8_t idx)
{
	uint8_t val;
	readReg(usbHandle, idx, &val);
	return val;
}

static int getCardStatus(WINUSB_INTERFACE_HANDLE usbHandle)
{
	uint8_t val;
	if (readReg(usbHandle, EM2874_REG_CAS_STATUS, &val)) {
		/*
		if (val == 0x03) {
			val = 0x05;
		}
		*/

		return val;
	}
	return -1;
}


static bool writeReg(WINUSB_INTERFACE_HANDLE usbHandle, const uint8_t idx, const uint8_t val)
{
	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0x40;
	spkt.Index = idx;
	spkt.Length = 1;

	ULONG ret;
	BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)&val, 1, &ret, NULL);
	if (!bRet) {
	}
	return (ret == 1) && bRet;
}

static bool readI2C(WINUSB_INTERFACE_HANDLE usbHandle, const uint8_t addr, const uint16_t size, uint8_t* data, const bool isStop)
{
	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0xc0;
	spkt.Request = isStop ? 2 : 3;
	spkt.Index = addr;
	spkt.Length = size;

	ULONG ret;
	BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)data, spkt.Length, &ret, NULL);
	if (!bRet) {
		return false;
	}
	readReg(usbHandle, 0x05, (uint8_t*)&ret);
	if ((uint8_t)ret) {
		return false;
	}
	return true;
}

static bool writeI2C(WINUSB_INTERFACE_HANDLE usbHandle, const uint8_t addr, const uint16_t size, uint8_t* data, const bool isStop)
{
	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0x40;
	spkt.Request = isStop ? 2 : 3;
	spkt.Index = addr;
	spkt.Length = size;

	ULONG ret;
	BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)data, spkt.Length, &ret, NULL);
	if (!bRet) {
		return false;
	}
	readReg(usbHandle, 0x05, (uint8_t*)&ret);
	if ((uint8_t)ret) {
		return false;
	}
	return true;
}



static bool writeICC(WINUSB_INTERFACE_HANDLE usbHandle, const size_t size, const void* data)
{
	uint8_t val;
	if (getCardStatus(usbHandle) != 0x5)
		return false;
	writeReg(usbHandle, EM2874_REG_CAS_MODE1, 0x01);

	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0x40;
	spkt.Request = 0x14;
	spkt.Index = 0x200;

	ULONG ret;
	uint8_t cardBuf[256] = {0,};
	cardBuf[0] = 0;
	cardBuf[1] = cardPCB;
	cardBuf[2] = (uint8_t)size;
	memcpy(cardBuf + 3, data, size);
	cardBuf[size + 3] = ICC_checkSum(cardBuf + 1, (int)size + 2);
	val = (uint8_t)size + 4;
	for (int i = 0; i < val; i += 64) {
		spkt.Index = 0x200 + i;
		spkt.Length = (val - i) > 64 ? 64 : (val - i);

		BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)(cardBuf + i)
			, spkt.Length, &ret, NULL);
		if (!bRet) {
			return false;
		}
	}

	cardPCB ^= 0x40;
	writeReg(usbHandle, EM2874_REG_CAS_MODE2, 0);
	readReg(usbHandle, EM2874_REG_CAS_STATUS, &val);
	writeReg(usbHandle, EM2874_REG_CAS_DATALEN, (uint8_t)size + 4);
	milliWait(1);
	return true;
}

static bool readICC(WINUSB_INTERFACE_HANDLE usbHandle, size_t* size, void* data)
{
	uint8_t val;
	val = readRegB(usbHandle, EM2874_REG_CAS_DATALEN);
	if (val > *size + 4 || val < 5)
		return false;
	*size = (size_t)val - 4;

	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0xc0;
	spkt.Request = 0x14;

	ULONG ret;
	uint8_t cardBuf[256]={0,};
	for (uint8_t i = 0; i < val; i += 64) {
		spkt.Index = i;
		spkt.Length = (val - i) > 64 ? 64 : (val - i);

		BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, (PUCHAR)(cardBuf + i)
			, spkt.Length, &ret, NULL);
		if (!bRet) {
			return false;
		}
	}
	memcpy(data, cardBuf + 3, (size_t)val - 4);
	return true;
}

static int waitICC(WINUSB_INTERFACE_HANDLE usbHandle)
{
	uint8_t val;
	int i;
	
	for (i = 0; i < 40; i++) {
		milliWait(8);
		if (!readReg(usbHandle, EM2874_REG_CAS_STATUS, &val))
			continue;
		if (val == 5) {
			return i;
		}
		if (val == 0) return -1;	// card error
	}
	return -2;	// timeout
}

static bool resetICC_1(WINUSB_INTERFACE_HANDLE usbHandle)
{
	//writeReg(usbHandle, EM28XX_REG_GPIO, 0xFF);
	//writeReg(usbHandle, EM28XX_REG_GPIO, 0xFE);
	if (getCardStatus(usbHandle) > 0 &&
		writeReg(usbHandle, EM2874_REG_CAS_MODE1, 0x1) &&
		writeReg(usbHandle, EM2874_REG_CAS_RESET, 0x1)) {
		return true;
	}

	return (false);
}
static bool resetICC_2(WINUSB_INTERFACE_HANDLE usbHandle)
{
	uint8_t rbuff[32];
	size_t rlen = sizeof(rbuff);

	readICC(usbHandle, &rlen, rbuff);
	milliWait(1);

	WINUSB_SETUP_PACKET spkt;
	memset(&spkt, '\0', sizeof(spkt));
	spkt.RequestType = 0x40;
	spkt.Request = 0x14;
	spkt.Index = 0x200;

	static UCHAR cmd[] = { 0x00, 0xc1, 0x01, 0xfe, 0x3e };
	ULONG ret;
	spkt.Length = sizeof(cmd);
	BOOL bRet = WinUsb_ControlTransfer(usbHandle, spkt, cmd, sizeof(cmd), &ret, NULL);
	if (!bRet) {
		return false;
	}

	writeReg(usbHandle, EM2874_REG_CAS_MODE2, 0);
	writeReg(usbHandle, EM2874_REG_CAS_STATUS, 0x80);

	milliWait(100);
	writeReg(usbHandle, EM2874_REG_CAS_DATALEN, 5);
	if (waitICC(usbHandle) < 0) {
		return false;
	}

	spkt.RequestType = 0xc0;
	spkt.Index = 0;
	bRet = WinUsb_ControlTransfer(usbHandle, spkt, rbuff, 4, &ret, NULL);
	if (!bRet || rbuff[1] != 0xe1 || rbuff[3] != 0xfe) {
		return false;
	}
	cardPCB = 0;
	isCardReady = true;

	return true;
}

static bool resetICC(WINUSB_INTERFACE_HANDLE usbHandle)
{
	if (!resetICC_1(usbHandle)) {
		writeReg(usbHandle, EM2874_REG_CAS_MODE1, 0x0);
		return false;
	}

	if (waitICC(usbHandle) < 0) {
		writeReg(usbHandle, EM2874_REG_CAS_MODE1, 0x0);
		return false;
	}
	return resetICC_2(usbHandle);
}

LONG KtvCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	*phContext = (SCARDCONTEXT)SCardStartedEvent;
	return SCARD_S_SUCCESS;
}


LONG KtvCardListReaders(SCARDCONTEXT hContext, LPCTSTR mszGroups, LPTSTR mszReaders, LPDWORD pcchReaders)
{
	if (mszReaders) {
		if (*pcchReaders == SCARD_AUTOALLOCATE) {
			*(LPTSTR*)mszReaders = _tcsdup(READER_NAME);
			LPTSTR p = _tcschr(*(LPTSTR*)mszReaders, _T(' '));
			if (p) {
				*p = _T('\0');
			}
		}else{
			_tcscpy_s(mszReaders, sizeof(READER_NAME) / sizeof(TCHAR) + 1, READER_NAME);
			LPTSTR p = _tcschr(mszReaders, _T(' '));
			if (p) {
				*p = _T('\0');
			}
		}
		*pcchReaders = sizeof(READER_NAME) / sizeof(TCHAR) + 1;
	}
	return SCARD_S_SUCCESS;
}

LONG KtvCardConnect(SCARDCONTEXT hContext, LPCTSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	*phCard = (SCARDHANDLE)usbHandle;
	*pdwActiveProtocol = SCARD_PROTOCOL_T1;
	if (isCardReady) {
		return(SCARD_S_SUCCESS);
	}
	bool bRet = resetICC(usbHandle);
	return bRet ? SCARD_S_SUCCESS : SCARD_E_INVALID_HANDLE;
}

LONG KtvCardTransmit(SCARDHANDLE hCard, LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, const DWORD cbSendLength, LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	bool bRet;
	int ret;

	bRet = writeICC((WINUSB_INTERFACE_HANDLE)hCard, cbSendLength, pbSendBuffer);
	if (!bRet) {
		return SCARD_E_INVALID_HANDLE;
	}

	milliWait(cardResponseTime);

	ret = waitICC((WINUSB_INTERFACE_HANDLE)hCard);
	if (ret < 0) {
		return SCARD_E_INVALID_HANDLE;
	}
	size_t rlen = (*pcbRecvLength > 254) ? 254 : *pcbRecvLength;
	bRet = readICC((WINUSB_INTERFACE_HANDLE)hCard, &rlen, pbRecvBuffer);
	if (!bRet) {
		*pcbRecvLength = 0;
		return SCARD_E_INVALID_HANDLE;
	}
	else {
		*pcbRecvLength = (DWORD)rlen;
		return (SCARD_S_SUCCESS);
	}
}

LONG KtvCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
	//writeReg((WINUSB_INTERFACE_HANDLE)hCard, EM28XX_REG_GPIO, 0xFF);
	writeReg((WINUSB_INTERFACE_HANDLE)hCard, EM2874_REG_CAS_MODE1, 0x00);
	isCardReady = false;
	return SCARD_S_SUCCESS;
}

LONG KtvCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem)
{
	free((void*)pvMem);
	return SCARD_S_SUCCESS;
}

LONG KtvCardReleaseContext(SCARDCONTEXT hContext)
{
	return SCARD_S_SUCCESS;
}

#pragma warning( push )
#pragma warning( disable : 4273 )
const SCARD_IO_REQUEST g_rgSCardT1Pci = { SCARD_PROTOCOL_T1, sizeof(SCARD_IO_REQUEST) };
#pragma warning( pop )

