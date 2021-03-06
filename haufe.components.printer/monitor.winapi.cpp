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
BOOL WINAPI Monitor::EnumPorts(HANDLE hMonitor, LPWSTR pName, DWORD Level, LPBYTE pPorts, DWORD cbBuf, LPDWORD pcbNeeded, LPDWORD pcReturned)
{
	hMonitor, pName;
	Log.Info(__FUNCTION__ " (%d, %p, %d)", Level, pPorts, cbBuf);
	return Port::CopyPortToBuffer(Level, pPorts, cbBuf, pcbNeeded, pcReturned);
}

// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/winsplp/nf-winsplp-openport
// static 
BOOL WINAPI Monitor::OpenPort(HANDLE hMonitor, LPWSTR pName, PHANDLE pHandle)
{
	hMonitor;
	if (pName && pHandle)
	{
		if (::wcscmp(pName, Printer::_szPortName))
		{
			Log.Warning(__FUNCTION__ "(%S)", pName);
		}

		Log.Info(__FUNCTION__ "(%S)", pName);
		*pHandle = new Port();
		return TRUE;
	}

	Log.Error(__FUNCTION__ "(%S) ERROR_INVALID_PARAMETER", pName);
	::SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
}

// static 
BOOL WINAPI Monitor::StartDocPort(HANDLE hPort, LPWSTR pPrinterName, DWORD JobId, DWORD Level, LPBYTE pDocInfo)
{
	Level;
	if (Port *pPort = (Port*)hPort)
	{
		if (DOC_INFO_1W *pdi = (DOC_INFO_1W*)pDocInfo)
		{
			Job *pJob = new Job();

			if (pPort->_pJob)
			{
				Log.Error(__FUNCTION__ " (pJob)");
			}

			pPort->_pJob = pJob;
			if (pJob->Start(JobId, pdi->pDocName, pPrinterName))
			{
				Log.Info(__FUNCTION__ " (%d, %S, %S, %S)", JobId, pPrinterName, pdi->pOutputFile, pdi->pDocName);
				return TRUE;
			}
		}
		else
		{
			Log.Error(__FUNCTION__ " (pdi)");
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (pPort)");
	}

	CPrinterHandle printer(pPrinterName);
	if (printer)
	{
		::SetJob(printer, JobId, 0, NULL, JOB_CONTROL_DELETE);
	}

	::SetLastError(ERROR_CAN_NOT_COMPLETE);
	return FALSE;
}

// static 
BOOL WINAPI Monitor::WritePort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten)
{
	if (Port* pPort = (Port*)hPort)
	{
		if (Job *pJob = pPort->_pJob)
		{
			if (pJob->Write(pBuffer, cbBuf, pcbWritten))
			{
			//	Log.Info(__FUNCTION__);
				return TRUE;
			}

			Log.Error(__FUNCTION__ " (%d, %S)", pJob->GetJobId(), pJob->GetPrinterName());

			CPrinterHandle printer(pJob->GetPrinterName());
			if (printer)
			{
				::SetJob(printer, pJob->GetJobId(), 0, NULL, JOB_CONTROL_DELETE);
			}
		}
		else
		{
			Log.Error(__FUNCTION__ " (pJob)");
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (pPort)");
	}

	::SetLastError(ERROR_CAN_NOT_COMPLETE);
	return FALSE;
}

// static 
BOOL WINAPI Monitor::ReadPort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pcbRead)
{
	hPort, pBuffer, cbBuffer, pcbRead;
	Log.Error(__FUNCTION__ " failed (ERROR_INVALID_HANDLE)");
	::SetLastError(ERROR_INVALID_HANDLE);
	return FALSE;
}

// static 
BOOL WINAPI Monitor::EndDocPort(HANDLE hPort)
{
	if (Port* pPort = (Port*)hPort)
	{
		if (Job *pJob = pPort->_pJob)
		{
			CPrinterHandle printer(pJob->GetPrinterName());										// 'An Drucker gesendet'
			if (printer)
			{
				Log.Info(__FUNCTION__ " (%d)", pJob->GetJobId());
				::SetJob(printer, pJob->GetJobId(), 0, NULL, JOB_CONTROL_DELETE);
			}

			pJob->End();
			delete pJob;
			pPort->_pJob = NULL;
			return TRUE;
		}
		else
		{
			Log.Error(__FUNCTION__ " (pJob)");
		}
	}
	
	Log.Error(__FUNCTION__ " (pPort)");
	::SetLastError(ERROR_CAN_NOT_COMPLETE);
	return FALSE;
}

// static 
BOOL WINAPI Monitor::ClosePort(HANDLE hPort)
{
	if (Port *pPort = (Port*)hPort)
	{
		Log.Info(__FUNCTION__);
		delete pPort;
		return TRUE;
	}

	Log.Error(__FUNCTION__);
	return TRUE;
}

// static 
VOID WINAPI Monitor::Shutdown(HANDLE hMonitor)
{
	hMonitor;
	Log.Info(__FUNCTION__);
}