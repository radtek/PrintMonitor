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
#include "handle.h"
#include "log.h"

CPrinterHandle::CPrinterHandle(PWSTR szPrinterName, ACCESS_MASK dwDesiredAccess)
{
	PRINTER_DEFAULTS pd = { 0 };
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;
	pd.DesiredAccess = dwDesiredAccess;

	if (!::OpenPrinter(szPrinterName, &_hHandle, &pd))
	{
		Log.Error(__FUNCTION__ " (OpenPrinter %d)", ::GetLastError());
		_hHandle = NULL;
	}
}

CPrinterHandle::CPrinterHandle(PWSTR szPrinterName, PPRINTER_DEFAULTS pDefaults)
{
	if (!::OpenPrinter(szPrinterName, &_hHandle, pDefaults))
	{
		Log.Error(__FUNCTION__ " (OpenPrinter (%d)", ::GetLastError());
		_hHandle = NULL;
	}
}

CPrinterHandle::~CPrinterHandle()
{
	if (_hHandle)
	{
		DWORD dwLastError = ::GetLastError();
		::ClosePrinter(_hHandle);
		::SetLastError(dwLastError);
	}
}
