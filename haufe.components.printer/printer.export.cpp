#include "stdafx.h"
#include "printer.h"
#include "monitor.h"
#include "port.h"
#include "job.h"
#include "monitorui.h"
#include "shell.h"
#include "log.h"

// export, called from Windows
LPMONITOR2 WINAPI InitializePrintMonitor2(PMONITORINIT pMonitorInit, PHANDLE phMonitor)
{
	return Monitor::Initialize(pMonitorInit, phMonitor);
}

// export, called from Windows
PMONITORUI WINAPI InitializePrintMonitorUI()
{
	return CMonitorUI::Initialize();
}

// export, rundll32.exe "$(TargetPath)",Install d:\tfs\ThirdParty\Ghostscript\lib\ghostpdf.inf
void WINAPI InstallW(HWND hwnd, HINSTANCE hinst, PWSTR szCmdLine, int nCmdShow)
{
	hwnd, hinst, nCmdShow;
	CLog::Register();
	Printer::Register(szCmdLine);

	WCHAR szCommand[0x400];
	WCHAR szDirectory[200];
	Job::JOBOPTIONS jo;
	Monitor::GetCommandLine(jo, szCommand, szDirectory, L"<path>");									// generate log entry
}

// export
void WINAPI UninstallW(HWND hwnd, HINSTANCE hinst, PWSTR szCmdLine, int nCmdShow)
{
	hwnd, hinst, szCmdLine, nCmdShow;
	CLog::Unregister();
	Printer::Unregister(true);
}

// use the already existing Amyuni settings
void GetUserOptions(Job::JOBOPTIONS &jo)
{
	ATL::CRegKey key;
	if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, L"SOFTWARE\\" COMPANY_NAME "\\Amyuni", KEY_READ))
	{
		key.QueryDWORDValue(L"PdfA", jo.dwPdfA);
	//	key.QueryDWORDValue(L"ImageOption", jo.dwImageOption);
		key.QueryDWORDValue(L"JpegOption", jo.dwJpegOption);
	}
}

// export, called from PrintMonitor.h
BOOL WINAPI SetJobOptionsA(DWORD dwJobID, DWORD dwOptions, PCSTR szPath)
{
	Job::JOBOPTIONS jo;

	jo.dwProcessID = ::GetCurrentProcessId();
	jo.dwJobID = dwJobID;
	jo.dwOptions = dwOptions;
	::wcscpy_s(jo.szOutputPath, ATL::CA2W(szPath));
	::GetUserOptions(jo);

	return Job::SetJobOptions(jo);
}

// export
BOOL WINAPI SetJobOptionsW(DWORD dwJobID, DWORD dwOptions, PCWSTR szPath)
{
	Job::JOBOPTIONS jo;

	jo.dwProcessID = ::GetCurrentProcessId();
	jo.dwJobID = dwJobID;
	jo.dwOptions = dwOptions;
	::wcscpy_s(jo.szOutputPath, szPath);
	::GetUserOptions(jo);

	return Job::SetJobOptions(jo);
}

// export
BOOL WINAPI WaitForJob(DWORD dwJobID, DWORD dwTimeout)
{
	bool bWaitForJob = false;
	WCHAR szEvent[MAX_PATH] = { 0 };
	Job::GetEventNameForJobId(szEvent, dwJobID);

	if (HANDLE hEvent = ::OpenEvent(SYNCHRONIZE, FALSE, szEvent))
	{
		switch (::WaitForSingleObject(hEvent, dwTimeout))
		{
		case WAIT_OBJECT_0:			
			Log.Info(__FUNCTION__ " (1, %S)", szEvent);
			bWaitForJob = true;						// done
			break;	
		case WAIT_FAILED:
			Log.Error(__FUNCTION__ " (WAIT_FAILED: %d)", ::GetLastError());
			break;
		case WAIT_ABANDONED:
			Log.Error(__FUNCTION__ " (WAIT_ABANDONED)");
			break;
		default:
			Log.Error(__FUNCTION__ " (WaitForSingleObject)");
			break;
		}

		::CloseHandle(hEvent);
	}
	else
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND)
		{
			Log.Info(__FUNCTION__ " (2, %S)", szEvent);
			bWaitForJob = true;							// printer job already done
		}
		else
		{
			Log.Error(__FUNCTION__ " (OpenEvent %d)", ::GetLastError());
		}
	}

	return bWaitForJob;
}

// export
void WINAPI QueryOutputPathW(HWND hwnd, HINSTANCE hinst, PWSTR szCmdLine, int nCmdShow)
{
	hwnd, hinst, nCmdShow;
	Shell::QueryOutputPath(szCmdLine);
}

