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
#include "monitorui.h"
#include "log.h"

// static 
PMONITORUI CMonitorUI::Initialize()
{
	static MONITORUI ui = { sizeof(ui), 0 };
	ui.pfnAddPortUI = CMonitorUI::AddPortUI;
	ui.pfnConfigurePortUI = CMonitorUI::ConfigurePortUI;
	ui.pfnDeletePortUI = CMonitorUI::DeletePortUI;

	return &ui;
}
