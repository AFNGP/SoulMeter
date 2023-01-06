#pragma once
#include <iostream>
#include <codecvt>
#include <locale>
#include <string>
using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

typedef std::chrono::system_clock::time_point timePoint;

#define FLOOR(x) (FLOAT)((INT)x)

inline BOOL UTF16toUTF8(_In_ WCHAR* src, _Out_ CHAR* dest, _In_ SIZE_T destLen) {

	if (src == nullptr || dest == nullptr)
		return FALSE;

	wstring_convert<codecvt_utf8_utf16<WCHAR>> wchar_to_utf8;
	wstring wstr(src);
	string utf8str = wchar_to_utf8.to_bytes(wstr);

	if (destLen < utf8str.size())
		return FALSE;

	strcpy_s(dest, destLen, utf8str.c_str());

	return TRUE;
}

inline BOOL ANSItoUTF8(_In_ CHAR* src, _Out_ CHAR* dest, _In_ INT32 destLen) {

	if (src == nullptr || dest == nullptr)
		return FALSE;

	BSTR bstr;
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, NULL);
	bstr = SysAllocStringLen(NULL, len);

	MultiByteToWideChar(CP_UTF8, 0, src, -1, bstr, len);

	len = WideCharToMultiByte(CP_ACP, 0, bstr, -1, NULL, 0, NULL, NULL);

	if (len < 1)
		return FALSE;

	if (len < destLen) {
		WideCharToMultiByte(CP_ACP, 0, bstr, -1, dest, destLen, NULL, NULL);
	}
	else {
		SysFreeString(bstr);
		return FALSE;
	}

	return TRUE;
}

inline BOOL TextCommma(_In_ CHAR* src, _Out_ CHAR* dest) {

	if (src == nullptr || dest == nullptr) {
		return FALSE;
	}

	size_t len = strlen(src);

	while (*src) {
		*dest++ = *src++;

		if (--len && (len % 3) == 0)
			*dest++ = ',';
	}
	*dest++ = 0;

	return TRUE;
}

inline BOOL TextCommmaIncludeDecimal(_In_ DOUBLE src, _In_ size_t destLen, _Out_ CHAR* dest) 
{
	if (dest == nullptr) {
		return FALSE;
	}

	char tmp[128] = { 0 };
	char comma[128] = { 0 };
	DOUBLE whole = floor(src);
	DOUBLE decimal = (src - whole) * 10;

	sprintf_s(tmp, "%.0f", whole);
	TextCommma(tmp, comma);
	sprintf_s(dest, destLen, "%s.%.0f", comma, decimal);

	return TRUE;
}

inline ULONG64 GetCurrentTimeStamp() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static BOOL file_contents(const std::filesystem::path& path, std::string* str)
{
	if (!std::filesystem::is_regular_file(path))
		return FALSE;

	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return FALSE;

	std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

	file.close();

	*str = content;

	return TRUE;
}

#define MAX_BUFFER_LENGTH 1024

#pragma pack(push, 1)

typedef struct _ETHERNETHEADER {
	UCHAR DesMac[6];
	UCHAR SrcMac[6];
	USHORT Type;
}ETHERNETHEADER;

typedef struct _IPHEADER {
	//little endian begin
	USHORT len : 4;					//
	USHORT version : 4;				//
	// little endian end
	USHORT tos : 8;					// 
	USHORT length;					// 
	USHORT id;						// 16 
	//little endian begin
	USHORT fragment_offset1 : 5;	// 
	USHORT flags : 3;				// flag
	USHORT fragment_offset2 : 8;	// 
	//little endian end
	USHORT ttl : 8;					// ttl
	USHORT protocol : 8;			// protocol tcp==06
	USHORT checksum;				// 
	ULONG src_ip;					//  IP
	ULONG dest_ip;					//  IP
}IPHEADER;

typedef struct _TCPHEADER {
	USHORT src_port;				//  port
	USHORT dest_port;				//  port
	ULONG seq_number;				//  
	ULONG ack_number;				// ack 
	//little endian begin 18
	USHORT reserved1 : 4;			// reserved
	USHORT length : 4;				// 
	USHORT fin : 1;					// FIN
	USHORT syn : 1;					// SYN
	USHORT rst : 1;					// RST
	USHORT psh : 1;					// PSH
	USHORT ack : 1;					// ACK
	USHORT urg : 1;					// URG
	USHORT reserved2 : 2;			// reserved
	//little endian end
	USHORT window_size;				// 
	USHORT tcp_checksum;			// TCP checksum
	USHORT urg_point;				//
}TCPHEADER;

typedef struct _IPV4PACKET {
	ETHERNETHEADER* _ethernetHeader; // options
	IPHEADER* _ipHeader; // options
	TCPHEADER* _tcpHeader; // options
	const uint8_t* _pkt; // save packet origin ptr
	uint8_t* _data;
	size_t _datalength;
	BOOL _isRecv;
	UINT64 _ts;
}IPv4Packet;

#pragma pack(pop)