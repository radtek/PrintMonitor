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
#include "monitor.h"
#include "port.h"
#include "job.h"
#include "log.h"

// static 
BOOL WINAPI Monitor::XcvOpenPort(HANDLE hMonitor, LPCWSTR pszObject, ACCESS_MASK GrantedAccess, PHANDLE phXcv)
{
	hMonitor;
	PXCVDATA pData = new XCVDATA;
	if (pszObject && !::wcscmp(pszObject, Printer::_szPortName))
	{
	//	pData->pPort = _pPort;
		Log.Info(__FUNCTION__ " (%S, %d)", pszObject, GrantedAccess);
	}
	else
	{
		Log.Error(__FUNCTION__ " (%S, %d) invalid port!", pszObject, GrantedAccess);
	}

	pData->dwGrantedAccess = GrantedAccess;
	*phXcv = (PHANDLE)pData;

	return TRUE;
}

// static 
DWORD WINAPI Monitor::XcvDataPort(HANDLE hXcv, LPCWSTR pszDataName, PBYTE pInputData, DWORD cbInputData, PBYTE pOutputData, DWORD cbOutputData, PDWORD pcbOutputNeeded)
{
	if (pszDataName)
	{
		if (!::wcscmp(pszDataName, Printer::_szSetJobOptions))
		{
			if (PXCVDATA pXCVDATA = (PXCVDATA)hXcv)
			{
				if (!(pXCVDATA->dwGrantedAccess & SERVER_ACCESS_ADMINISTER))
				{
					Log.Error(__FUNCTION__ "(%S) ERROR_ACCESS_DENIED", pszDataName);
					return ERROR_ACCESS_DENIED;
				}

				if (Job::PJOBOPTIONS pJobOptions = (Job::PJOBOPTIONS)pInputData)
				{
					Log.Info(__FUNCTION__ "(%S)", pszDataName);
					Job::SetJobOptions(pJobOptions);
					return ERROR_SUCCESS;
				}
			}
		}
		else if (!::wcscmp(pszDataName, Printer::_szGetConfiguration))
		{
			if (pcbOutputNeeded)
			{
				*pcbOutputNeeded = sizeof(Port::Configuration);
				if (cbOutputData < *pcbOutputNeeded)
				{
					Log.Error(__FUNCTION__ "(%S) ERROR_INSUFFICIENT_BUFFER", pszDataName);
					return ERROR_INSUFFICIENT_BUFFER;
				}

				if (PXCVDATA pXCVDATA = (PXCVDATA)hXcv)
				{
					if (Port::Configuration *pConfiguration = (Port::Configuration*)pOutputData)
					{
					//	if (pXCVDATA->pPort && pXCVDATA->pPort == _pPort)
						{
							Log.Info(__FUNCTION__ "(%S)", pszDataName);		
						//	_pPort->GetConfiguration(pConfiguration);
							Port::GetConfiguration(pConfiguration);
							return ERROR_SUCCESS;
						}

					//	Log.Error(__FUNCTION__ "(%S) invalid port!", pszDataName);
					//	return ERROR_BAD_ARGUMENTS;
					}
				}
			}
		}
		else if (!::wcscmp(pszDataName, Printer::_szSetConfiguration))
		{
			if (cbInputData < sizeof(Port::Configuration))
			{
				Log.Error(__FUNCTION__ "(%S) ERROR_INSUFFICIENT_BUFFER", pszDataName);
				return ERROR_INSUFFICIENT_BUFFER;
			}

			if (PXCVDATA pXCVDATA = (PXCVDATA)hXcv)
			{
				if (!(pXCVDATA->dwGrantedAccess & SERVER_ACCESS_ADMINISTER))
				{
					Log.Error(__FUNCTION__ "(%S) ERROR_ACCESS_DENIED", pszDataName);
					return ERROR_ACCESS_DENIED;
				}

				if (Port::Configuration *pConfiguration = (Port::Configuration*)pInputData)
				{
				//	if (pXCVDATA->pPort && pXCVDATA->pPort == _pPort)
					{
						Log.Info(__FUNCTION__ "(%S)", pszDataName);
					//	_pPort->SetConfiguration(pConfiguration);
						Port::SetConfiguration(pConfiguration);
						SavePort();
						return ERROR_SUCCESS;
					}

				//	Log.Error(__FUNCTION__ "(%S) invalid port!", pszDataName);
				//	return ERROR_BAD_ARGUMENTS;
				}
			}
		}
		else if (!::wcscmp(pszDataName, Printer::_szMonitorUI))
		{
			if (pcbOutputNeeded)
			{
				PCWSTR szName = Printer::GetModuleName();
				*pcbOutputNeeded = 2 * ((DWORD)::wcslen(szName) + 1);

				if (cbOutputData < *pcbOutputNeeded)
				{
					Log.Error(__FUNCTION__ "(%S) ERROR_INSUFFICIENT_BUFFER", pszDataName);
					return ERROR_INSUFFICIENT_BUFFER;
				}

				Log.Info(__FUNCTION__ "(%S, %S)", pszDataName, szName);
				::CopyMemory(pOutputData, szName, *pcbOutputNeeded);
				return ERROR_SUCCESS;
			}
		}
		else if (!::wcscmp(pszDataName, L"CleanupPort"))
		{
			Log.Info(__FUNCTION__ "(%S)", pszDataName);
			return ERROR_SUCCESS;
		}

		Log.Error(__FUNCTION__ "(%S) ERROR_CAN_NOT_COMPLETE", pszDataName);
		return ERROR_CAN_NOT_COMPLETE;
	}
	
	Log.Error(__FUNCTION__ "ERROR_CAN_NOT_COMPLETE");
	return ERROR_CAN_NOT_COMPLETE;
}

// static 
BOOL WINAPI Monitor::XcvClosePort(HANDLE hXcv)
{
	Log.Info(__FUNCTION__);

	if (PXCVDATA pData = (PXCVDATA)hXcv)
	{
		delete pData;
	}

	return TRUE;
}
