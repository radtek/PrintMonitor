//	PrintMonitor for the Ghostscript Printer Driver
//	Copyright (C) 2019  Haufe-Lexware
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Affero General Public License as
//	published by the Free Software Foundation, either version 3 of the
//	License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Affero General Public License for more details.
//
//	You should have received a copy of the GNU Affero General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "printer.h"
#include "port.h"
#include "job.h"
#include "log.h"

Port::Port()
{
}

Port::~Port()
{
	if (_pJob)
	{
		Log.Error(__FUNCTION__ "(pJob)");
		delete _pJob;
	}
}

// static
BOOL Port::SetConfiguration(Configuration &cfg)
{
	return Printer::SetXcvData(cfg, Printer::_szSetConfiguration);
}

// static
BOOL Port::GetConfiguration(Configuration &cfg)
{
	return Printer::SetXcvData(cfg, Printer::_szGetConfiguration);
}

// static
DWORD Port::GetPortSize(DWORD dwLevel, bool bStrings)
{
	switch (dwLevel)
	{
	case 1:
		return (bStrings ? 0 : sizeof(PORT_INFO_1W)) + sizeof(Printer::_szPortName) + 16;
	case 2:
		return (bStrings ? 0 : sizeof(PORT_INFO_2W)) + sizeof(Printer::_szPortName) + sizeof(Printer::_szMonitorName) + sizeof(Printer::_szDescription) + 16;
	default:
		return 0;
	}
}

// static, __unaligned , "do not access a small-aligned address by using a recast pointer of larger alignment"
BOOL Port::CopyPort(DWORD dwLevel, PBYTE pBytes, DWORD dwBytes)
{
	switch (dwLevel)
	{
	case 1:
		{
			PPORT_INFO_1W pPort = (PPORT_INFO_1W)pBytes;

			DWORD dwString = GetPortSize(dwLevel, true);
			PBYTE pString = pBytes + dwBytes - dwString;

			pPort->pName = (PWSTR)pString;
			::wcscpy_s(pPort->pName, sizeof(Printer::_szPortName), Printer::_szPortName);

			Log.Info(__FUNCTION__ "(%S)", pPort->pName);
		}
		break;
	case 2:
		{
			PPORT_INFO_2W pPort = (PPORT_INFO_2W)pBytes;

			DWORD dwString = GetPortSize(dwLevel, true);
			PBYTE pString = pBytes + dwBytes - dwString;

			pPort->pPortName = (PWSTR)pString;
			::wcscpy_s(pPort->pPortName, sizeof(Printer::_szPortName), Printer::_szPortName);
			pString += sizeof(Printer::_szPortName);

			pPort->pMonitorName = (PWSTR)pString;
			::wcscpy_s(pPort->pMonitorName, sizeof(Printer::_szMonitorName), Printer::_szMonitorName);
			pString += sizeof(Printer::_szMonitorName);

			pPort->pDescription = (PWSTR)pString;
			::wcscpy_s(pPort->pDescription, sizeof(Printer::_szDescription), Printer::_szDescription);

			pPort->fPortType = 0;
			pPort->Reserved = 0;

			Log.Info(__FUNCTION__ "(%S, %S, %S)", pPort->pPortName, pPort->pMonitorName, pPort->pDescription);
		}
		break;
	}

//	static DWORD dw = 0;
//	DWORD dwWritten = 0;
//	WCHAR szPath[MAX_PATH];
//	::wsprintf(szPath, L"D:\\test (%d).bin", dw);
//	HANDLE hHandle = ::CreateFileW(szPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
//	::WriteFile(hHandle, pBytes, dwBytes, &dwWritten, NULL);
//	::CloseHandle(hHandle);

	return TRUE;
}

// static
BOOL Port::CopyPortToBuffer(DWORD dwLevel, PBYTE pBytes, DWORD dwBytes, PDWORD pdwNeeded, PDWORD pdwPorts)
{
	DWORD dwNeeded = GetPortSize(dwLevel);

	if (pdwNeeded)
	{
		*pdwNeeded = dwNeeded;
	}

	if (dwBytes < dwNeeded)
	{
		Log.Info(__FUNCTION__ " (ERROR_INSUFFICIENT_BUFFER)");
		::SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}

	if (pBytes && pdwPorts)
	{
		*pdwPorts = 1;
		switch (dwLevel)
		{
		case 1:
		//	Log.Info(__FUNCTION__ "(%d)", dwLevel);
			return CopyPort(dwLevel, pBytes, dwBytes);
		case 2:
		//	Log.Info(__FUNCTION__ "(%d)", dwLevel);
			return CopyPort(dwLevel, pBytes, dwBytes);
		default:
			Log.Error(__FUNCTION__ " failed! (ERROR_INVALID_LEVEL)");
			::SetLastError(ERROR_INVALID_LEVEL);
			return FALSE;
		}
	}

	Log.Error(__FUNCTION__ " failed! (ERROR_BAD_ARGUMENTS)");
	::SetLastError(ERROR_BAD_ARGUMENTS);														// ERROR_INVALID_PARAMETER
	return FALSE;
}
