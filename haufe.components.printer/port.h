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

#pragma once
class Job;

class Port
{
public:
	Port();
	~Port();

	Job *_pJob = NULL;

public:
	enum LogLevel : DWORD { None, Error, All };
	struct Configuration
	{
		DWORD dwLogLevel;
	};

	static void GetConfiguration(Configuration *pConfiguration) { if (pConfiguration) *pConfiguration = _cfg; }
	static void SetConfiguration(Configuration *pConfiguration) { if (pConfiguration) _cfg = *pConfiguration; }

	static DWORD GetLogLevel() { return _cfg.dwLogLevel; }
	static void SetLogLevel(DWORD dwLogLevel) { _cfg.dwLogLevel = dwLogLevel; }

	static BOOL GetConfiguration(Configuration &cfg);
	static BOOL SetConfiguration(Configuration &cfg);

private:
	static Configuration _cfg;

	static DWORD GetPortSize(DWORD dwLevel, bool bStrings = false);
	static BOOL CopyPort(DWORD dwLevel, PBYTE pBytes, DWORD dwBytes);

public:
	static BOOL CopyPortToBuffer(DWORD dwLevel, LPBYTE pBytes, DWORD dwBytes, PDWORD pdwNeeded, PDWORD pdwPorts);
};

__declspec(selectany) Port::Configuration Port::_cfg = { 
#ifdef _DEBUG
	Port::All
#else
	Port::Error
#endif
};
