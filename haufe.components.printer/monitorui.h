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
#include "stdafx.h"

class CMonitorUI
{
private:
	CMonitorUI();

public:
	static PMONITORUI Initialize();

protected:
	static BOOL WINAPI AddPortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszMonitorNameIn, PWSTR* ppszPortNameOut);
	static BOOL WINAPI ConfigurePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName);
	static BOOL WINAPI DeletePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName);
};