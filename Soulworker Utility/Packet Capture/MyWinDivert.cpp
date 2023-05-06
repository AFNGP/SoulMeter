#include "pch.h"
#include ".\Packet Capture\MyWinDivert.h"
#include ".\Packet Capture\PacketParser.h"
#include ".\Packet Capture\PacketCapture.h"
#include ".\UI\Option.h"
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iphlpapi.h>

DWORD MyWinDivert::Init(HANDLE handle, BOOL& isMudfish, BOOL reInit) {

	DWORD error = ERROR_SUCCESS;

	do {

		if (!_inited)
		{
			char filterRule[512] = WINDIVERT_FILTER_RULE;
			if (isMudfish)
			{
				do
				{
					ULONG remoteAddr;
					inet_pton(AF_INET, "127.0.0.1", &remoteAddr);
					USHORT remotePort = htons(36650);
					USHORT localPort = 36650;
					PMIB_TCPTABLE2 tcpTable = nullptr;
					DWORD tcpTableSize = 0;
					GetTcpTable2(tcpTable, &tcpTableSize, TRUE);
					tcpTable = (PMIB_TCPTABLE2)malloc(tcpTableSize);
					if (GetTcpTable2(tcpTable, &tcpTableSize, TRUE) != NO_ERROR) {
						Log::WriteLogA("Could not get TCP TABLE\n");
						isMudfish = FALSE;
						break;
					}
					bool found = false;
					for (DWORD i = 0; i < tcpTable->dwNumEntries; ++i) {
						const MIB_TCPROW2& row = tcpTable->table[i];
						if (row.dwRemoteAddr == remoteAddr && row.dwRemotePort == remotePort) {
							localPort = ntohs(row.dwLocalPort);
							found = true;
							break;
						}
					}
					if (found) {
						sprintf_s(filterRule, "(tcp.DstPort == %d or tcp.DstPort == 36650) and tcp.PayloadLength > 0 and ip and loopback", localPort);
					}
					else {
						// Log::WriteLogA("Could find inbound port.\n");
						isMudfish = FALSE;
					}
				} while (false);
			}
			
			handle = WinDivertOpen(filterRule, WINDIVERT_LAYER_NETWORK, 0, WINDIVERT_FLAG_SNIFF);

			if (handle == INVALID_HANDLE_VALUE) {
				Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertOpen: %x")), GetLastError());
				error = ERROR_INVALID_HANDLE;
				break;
			}

			if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_LEN, 16384))
			{
				Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertSetParam1: %x")), GetLastError());
				error = ERROR_INVALID_HANDLE;
				break;
			}

			if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_TIME, 8000))
			{
				Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertSetParam2: %x")), GetLastError());
				error = ERROR_INVALID_HANDLE;
				break;
			}

			if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_SIZE, 33554432))
			{
				Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertSetParam3: %x")), GetLastError());
				error = ERROR_INVALID_HANDLE;
				break;
			}

			_handle = handle;
		}
		else {
			_stop = TRUE;
			WinDivertClose(_handle);
			while (TRUE)
			{
				if (!_stop)
					break;
				Sleep(100);
			}
			if (reInit)
			{
				_inited = FALSE;
				continue;
			}
		}
		
		HANDLE h = CreateThread(NULL, 0, ReceiveCallback, this, 0, NULL);
		if (h != NULL)
			CloseHandle(h);
		break;

	} while (true);

	return error;
}

DWORD MyWinDivert::ReceiveCallback(LPVOID prc) {

	DWORD error = ERROR_SUCCESS;

	MyWinDivert* _this = (MyWinDivert*)prc;
	if (_this == nullptr)
		return ERROR_NOT_FOUND;

	do {
		WINDIVERT_ADDRESS addr;
		UINT addrlen = sizeof(WINDIVERT_ADDRESS);
		UINT recvlen = 0;
		BYTE* pkt_data = new BYTE[WINDIVERT_MTU_MAX];

		PIP_INTERFACE_INFO pInfo = NULL;
		ULONG ulOutBufLen = 0;
		DWORD dwRetVal = 0;
		const CHAR* ifName = UIOPTION.GetUseInterface();
		ULONG IfIdx = 0;

		dwRetVal = GetInterfaceInfo(NULL, &ulOutBufLen);
		if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
			pInfo = (IP_INTERFACE_INFO*)malloc(ulOutBufLen);
			if (pInfo != NULL) {
				dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
				if (dwRetVal == NO_ERROR) {
					CHAR interfaceName[MAX_PATH] = { 0 };
					for (INT32 i = 0; i < pInfo->NumAdapters; i++) {
						UTF16toUTF8(pInfo->Adapter[i].Name + 14, interfaceName, MAX_PATH);
						if (strcmp(interfaceName, ifName) == 0)
						{
							IfIdx = pInfo->Adapter[i].Index;
							break;
						}
					}
				}
				free(pInfo);
			}
		}

		if (!_this->_inited)
			_this->_inited = TRUE;

		while (TRUE) {

			if (_this->_stop)
			{
				_this->_stop = FALSE;
				break;
			}

			// Windivert 1.4.2
			if (!WinDivertRecvEx(_this->_handle, pkt_data, WINDIVERT_MTU_MAX, 0, &addr, &recvlen, NULL)) {
				// WinDivert 2.2
				//if (!WinDivertRecvEx(_this->_handle, pkt_data, packetlen, &recvlen, 0, &addr, &addrlen, NULL)) {
				if(!_this->_stop)
					Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertRecv : %x")), GetLastError());
				continue;
			}

			if (IfIdx != 0 && IfIdx != addr.IfIdx)
				continue;

			IPv4Packet packet;
			PACKETCAPTURE.ParseWinDivertStruct(&packet, pkt_data);

			PACKETPARSER.Parse(&packet, packet._isRecv);
		}

	} while (false);

	return error;
}
