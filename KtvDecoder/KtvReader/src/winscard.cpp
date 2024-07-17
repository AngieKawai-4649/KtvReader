
#include <Windows.h>
//#include <winscard.h>
#include <stdint.h>

/*
#ifdef _WIN64
#pragma comment(linker,"/EXPORT:SCardEstablishContext=SCardEstablishContext_")
#pragma comment(linker,"/EXPORT:SCardListReadersA=SCardListReadersA_")
#pragma comment(linker,"/EXPORT:SCardListReadersW=SCardListReadersW_")
#pragma comment(linker,"/EXPORT:SCardConnectA=SCardConnectA_")
#pragma comment(linker,"/EXPORT:SCardConnectW=SCardConnectW_")
#pragma comment(linker,"/EXPORT:SCardTransmit=SCardTransmit_")
#pragma comment(linker,"/EXPORT:SCardDisconnect=SCardDisconnect_")
#pragma comment(linker,"/EXPORT:SCardFreeMemory=SCardFreeMemory_")
#pragma comment(linker,"/EXPORT:SCardGetStatusChangeA=SCardGetStatusChangeA_")
#pragma comment(linker,"/EXPORT:SCardGetStatusChangeW=SCardGetStatusChangeW_")
#pragma comment(linker,"/EXPORT:SCardReleaseContext=SCardReleaseContext_")
#pragma comment(linker,"/EXPORT:SCardReconnect=SCardReconnect_")
#pragma comment(linker,"/EXPORT:SCardAccessStartedEvent=SCardAccessStartedEvent_")
#pragma comment(linker,"/EXPORT:SCardReleaseStartedEvent=SCardReleaseStartedEvent_")
#pragma comment(linker,"/EXPORT:SCardCancel=SCardCancel_")
#pragma comment(linker,"/EXPORT:g_rgSCardT1Pci=g_rgSCardT1Pci_")
#pragma comment(linker,"/EXPORT:SCardIsValidContext=SCardIsValidContext_")
#pragma comment(linker,"/EXPORT:SCardStatusA=SCardStatusA_")
#pragma comment(linker,"/EXPORT:SCardStatusW=SCardStatusW_")
#else
#pragma comment(linker,"/EXPORT:SCardEstablishContext=_SCardEstablishContext_@16")
#pragma comment(linker,"/EXPORT:SCardListReadersA=_SCardListReadersA_@16")
#pragma comment(linker,"/EXPORT:SCardListReadersW=_SCardListReadersW_@16")
#pragma comment(linker,"/EXPORT:SCardConnectA=_SCardConnectA_@24")
#pragma comment(linker,"/EXPORT:SCardConnectW=_SCardConnectW_@24")
#pragma comment(linker,"/EXPORT:SCardTransmit=_SCardTransmit_@28")
#pragma comment(linker,"/EXPORT:SCardDisconnect=_SCardDisconnect_@8")
#pragma comment(linker,"/EXPORT:SCardFreeMemory=_SCardFreeMemory_@8")
#pragma comment(linker,"/EXPORT:SCardGetStatusChangeA=_SCardGetStatusChangeA_@16")
#pragma comment(linker,"/EXPORT:SCardGetStatusChangeW=_SCardGetStatusChangeW_@16")
#pragma comment(linker,"/EXPORT:SCardReleaseContext=_SCardReleaseContext_@4")
#pragma comment(linker,"/EXPORT:SCardReconnect=_SCardReconnect_@20")
#pragma comment(linker,"/EXPORT:SCardAccessStartedEvent=_SCardAccessStartedEvent_@0")
#pragma comment(linker,"/EXPORT:SCardReleaseStartedEvent=_SCardReleaseStartedEvent_@0")
#pragma comment(linker,"/EXPORT:SCardCancel=_SCardCancel_@4")
#pragma comment(linker,"/EXPORT:g_rgSCardT1Pci=_g_rgSCardT1Pci_")
#pragma comment(linker,"/EXPORT:SCardIsValidContext=_SCardIsValidContext_@4")
#pragma comment(linker,"/EXPORT:SCardStatusA=_SCardStatusA_@28")
#pragma comment(linker,"/EXPORT:SCardStatusW=_SCardStatusW_@28")
#endif
*/

HANDLE usbHandle;

static HANDLE SCardStartedEvent = NULL;
#define READER_NAME_A   "KTV_READER"
#define READER_NAME_W  L"KTV_READER"

extern int KtvCardTransmit(HANDLE handle, const void* pSend, const DWORD nSendLen, void* pRecv, DWORD* nRecvLen);
extern int KtvCardReset(HANDLE handle);


//extern"C" {

	LONG WINAPI SCardEstablishContext(_In_ DWORD, _Reserved_ LPCVOID, _Reserved_ LPCVOID, _Out_ LPSCARDCONTEXT phContext)
	{
		*phContext = (SCARDCONTEXT)SCardStartedEvent;
		return SCARD_S_SUCCESS;
	}

	_Success_(return == SCARD_S_SUCCESS)
		LONG WINAPI	SCardListReadersA(
			_In_     SCARDCONTEXT hContext,
			_In_opt_ LPCSTR mszGroups,
			_When_(_Old_(*pcchReaders) == SCARD_AUTOALLOCATE, _At_((LPSTR*)mszReaders, _Outptr_result_buffer_maybenull_(*pcchReaders) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
			_When_(_Old_(*pcchReaders) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaders) _Post_ _NullNull_terminated_)
			LPSTR mszReaders,
			_Inout_  LPDWORD pcchReaders)
	{
		if (mszReaders) {
			if (*pcchReaders == SCARD_AUTOALLOCATE) {
				mszReaders = _strdup(READER_NAME_A);
				memset(mszReaders + strlen(READER_NAME_A), '\0', 2);
			}
			else {
				strcpy_s(mszReaders, *pcchReaders, READER_NAME_A);
				memset(mszReaders + strlen(READER_NAME_A), '\0', 2);
			}
			*pcchReaders = (DWORD)strlen(READER_NAME_A) + 2;
		}
		else {
			*pcchReaders = (DWORD)strlen(READER_NAME_A)+2;
		}

		return SCARD_S_SUCCESS;
	}

	_Success_(return == SCARD_S_SUCCESS)
	LONG WINAPI SCardListReadersW(_In_ SCARDCONTEXT hContext, _In_opt_ LPCTSTR mszGroups,
		_When_(_Old_(*pcchReaders) == SCARD_AUTOALLOCATE, _At_((LPWSTR*)mszReaders, _Outptr_result_buffer_maybenull_(*pcchReaders) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
		_When_(_Old_(*pcchReaders) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaders) _Post_ _NullNull_terminated_)
		LPWSTR mszReaders, _Inout_ LPDWORD pcchReaders)
	{
		if (mszReaders) {
			if (*pcchReaders == SCARD_AUTOALLOCATE) {
				mszReaders = _wcsdup(READER_NAME_W);
				memset(mszReaders+ wcslen(READER_NAME_W), L'\0', 2);
			}
			else {
				wcscpy_s(mszReaders, *pcchReaders, READER_NAME_W);
				memset(mszReaders + wcslen(READER_NAME_W), L'\0', 2);
			}
			*pcchReaders = sizeof(READER_NAME_W) / sizeof(WCHAR) + 2;
		}
		else {
			*pcchReaders = sizeof(READER_NAME_W) / sizeof(WCHAR) + 2;
		}
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardConnectA(
			_In_    SCARDCONTEXT hContext,
			_In_    LPCSTR szReader,
			_In_    DWORD dwShareMode,
			_In_    DWORD dwPreferredProtocols,
			_Out_   LPSCARDHANDLE phCard,
			_Out_   LPDWORD pdwActiveProtocol)

	{
		*phCard = (SCARDHANDLE)usbHandle;
		*pdwActiveProtocol = SCARD_PROTOCOL_T1;
		KtvCardReset(usbHandle);
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardConnectW(
			_In_    SCARDCONTEXT hContext,
			_In_    LPCWSTR szReader,
			_In_    DWORD dwShareMode,
			_In_    DWORD dwPreferredProtocols,
			_Out_   LPSCARDHANDLE phCard,
			_Out_   LPDWORD pdwActiveProtocol)
	{
		*phCard = (SCARDHANDLE)usbHandle;
		*pdwActiveProtocol = SCARD_PROTOCOL_T1;
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardTransmit(
			_In_        SCARDHANDLE hCard,
			_In_        LPCSCARD_IO_REQUEST pioSendPci,
			_In_reads_bytes_(cbSendLength) LPCBYTE pbSendBuffer,
			_In_        DWORD cbSendLength,
			_Inout_opt_ LPSCARD_IO_REQUEST pioRecvPci,
			_Out_writes_bytes_(*pcbRecvLength) LPBYTE pbRecvBuffer,
			_Inout_     LPDWORD pcbRecvLength)
	{
		int ret = KtvCardTransmit((HANDLE)hCard, pbSendBuffer, cbSendLength, pbRecvBuffer, pcbRecvLength);
		return((ret == 0) ? SCARD_S_SUCCESS : SCARD_E_INVALID_HANDLE);
	}

	LONG WINAPI
		SCardDisconnect(
			_In_    SCARDHANDLE hCard,
			_In_    DWORD dwDisposition)
	{
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardFreeMemory(
			_In_ SCARDCONTEXT hContext,
			_In_ LPCVOID pvMem)
	{
		free((void *)pvMem);
		return SCARD_S_SUCCESS;
	}


	LONG WINAPI
		SCardGetStatusChangeA(
			_In_    SCARDCONTEXT hContext,
			_In_    DWORD dwTimeout,
			_Inout_ LPSCARD_READERSTATEA rgReaderStates,
			_In_    DWORD cReaders)
	{
		if (rgReaderStates->dwCurrentState == SCARD_STATE_UNAWARE)
		{
			rgReaderStates->dwEventState = SCARD_STATE_PRESENT;
			return SCARD_S_SUCCESS;
		}
		else if (dwTimeout == INFINITE)
		{
			return SCARD_E_INVALID_PARAMETER;
		}
		Sleep(dwTimeout);
		return SCARD_E_TIMEOUT;
	}


	LONG WINAPI
		SCardGetStatusChangeW(
			_In_    SCARDCONTEXT hContext,
			_In_    DWORD dwTimeout,
			_Inout_ LPSCARD_READERSTATEW rgReaderStates,
			_In_    DWORD cReaders)
	{
		if (rgReaderStates->dwCurrentState == SCARD_STATE_UNAWARE)
		{
			rgReaderStates->dwEventState = SCARD_STATE_PRESENT;
			return SCARD_S_SUCCESS;
		}
		else if (dwTimeout == INFINITE)
		{
			return SCARD_E_INVALID_PARAMETER;
		}
		Sleep(dwTimeout);
		return SCARD_E_TIMEOUT;
	}

	LONG WINAPI
		SCardReleaseContext(
			_In_      SCARDCONTEXT hContext)
	{
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardReconnect(
			_In_      SCARDHANDLE hCard,
			_In_      DWORD dwShareMode,
			_In_      DWORD dwPreferredProtocols,
			_In_      DWORD dwInitialization,
			_Out_opt_ LPDWORD pdwActiveProtocol)
	{
		if (pdwActiveProtocol) {
			*pdwActiveProtocol = SCARD_PROTOCOL_T1;
		}
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardIsValidContext(
			_In_      SCARDCONTEXT hContext)
	{
		return hContext ? SCARD_S_SUCCESS : ERROR_INVALID_HANDLE;
	}

	LONG WINAPI
		SCardStatusA(
			_In_        SCARDHANDLE hCard,
			_When_(_Old_(*pcchReaderLen) == SCARD_AUTOALLOCATE, _At_((LPSTR*)mszReaderNames, _Outptr_result_buffer_maybenull_(*pcchReaderLen) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
			_When_(_Old_(*pcchReaderLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaderLen) _Post_ _NullNull_terminated_)
			LPSTR mszReaderNames,
			_Inout_opt_ LPDWORD pcchReaderLen,
			_Out_opt_   LPDWORD pdwState,
			_Out_opt_   LPDWORD pdwProtocol,
			_When_(_Old_(*pcbAtrLen) == SCARD_AUTOALLOCATE, _At_((LPBYTE*)pbAtr, _Outptr_result_buffer_maybenull_(*pcbAtrLen) _At_(*_Curr_, _Post_ _NullNull_terminated_)))
			_When_(_Old_(*pcbAtrLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcbAtrLen) _Post_ _NullNull_terminated_)
			LPBYTE pbAtr,
			_Inout_opt_ LPDWORD pcbAtrLen)
	{
		if (pcchReaderLen)
		{
			if (!mszReaderNames || *pcchReaderLen != SCARD_AUTOALLOCATE)
				return SCARD_E_INVALID_PARAMETER;
			for (*pcchReaderLen = 2;
				READER_NAME_A[*pcchReaderLen - 2]
				|| READER_NAME_A[*pcchReaderLen - 1]; ++*pcchReaderLen);
			memcpy(mszReaderNames, READER_NAME_A, sizeof(READER_NAME_A));
		}
		if (pdwState)
			*pdwState = SCARD_PRESENT;
		if (pdwProtocol)
			*pdwProtocol = SCARD_PROTOCOL_T1;
		if (pcbAtrLen)
		{
			if (*pcbAtrLen == SCARD_AUTOALLOCATE)
				return SCARD_E_INVALID_PARAMETER;
			*pcbAtrLen = 0;
		}
		return SCARD_S_SUCCESS;
	}

	LONG WINAPI
		SCardStatusW(
			_In_        SCARDHANDLE hCard,
			_When_(_Old_(*pcchReaderLen) == SCARD_AUTOALLOCATE, _At_((LPWSTR*)mszReaderNames, _Outptr_result_buffer_maybenull_(*pcchReaderLen) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
			_When_(_Old_(*pcchReaderLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaderLen) _Post_ _NullNull_terminated_)
			LPWSTR mszReaderNames,
			_Inout_opt_ LPDWORD pcchReaderLen,
			_Out_opt_   LPDWORD pdwState,
			_Out_opt_   LPDWORD pdwProtocol,
			_When_(_Old_(*pcbAtrLen) == SCARD_AUTOALLOCATE, _At_((LPBYTE*)pbAtr, _Outptr_result_buffer_maybenull_(*pcbAtrLen) _At_(*_Curr_, _Post_ _NullNull_terminated_)))
			_When_(_Old_(*pcbAtrLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcbAtrLen) _Post_ _NullNull_terminated_)
			LPBYTE pbAtr,
			_Inout_opt_ LPDWORD pcbAtrLen)
	{
		if (pcchReaderLen)
		{
			if (!mszReaderNames || *pcchReaderLen != SCARD_AUTOALLOCATE)
				return SCARD_E_INVALID_PARAMETER;
			for (*pcchReaderLen = 2;
				READER_NAME_W[*pcchReaderLen - 2]
				|| READER_NAME_W[*pcchReaderLen - 1]; ++*pcchReaderLen);
			memcpy(mszReaderNames, READER_NAME_W, sizeof(READER_NAME_W));
		}
		if (pdwState)
			*pdwState = SCARD_PRESENT;
		if (pdwProtocol)
			*pdwProtocol = SCARD_PROTOCOL_T1;
		if (pcbAtrLen)
		{
			if (*pcbAtrLen == SCARD_AUTOALLOCATE)
				return SCARD_E_INVALID_PARAMETER;
			*pcbAtrLen = 0;
		}
		return SCARD_S_SUCCESS;
	}


	HANDLE WINAPI SCardAccessStartedEvent_(void)
	{
		return SCardStartedEvent;
	}
	void WINAPI SCardReleaseStartedEvent_(void)
	{
	}
	LONG WINAPI SCardCancel_(SCARDCONTEXT)
	{
		return SCARD_S_SUCCESS;
	}
	//const SCARD_IO_REQUEST g_rgSCardT1Pci;
	//SCARD_IO_REQUEST __imp_g_rgSCardT1Pci;
	const SCARD_IO_REQUEST g_rgSCardT1Pci = { SCARD_PROTOCOL_T1, sizeof(SCARD_IO_REQUEST) };



//}