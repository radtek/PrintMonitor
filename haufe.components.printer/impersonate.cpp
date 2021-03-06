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
#include "impersonate.h"
#include "log.h"

Impersonate::Impersonate()
{
	if (::OpenThreadToken(::GetCurrentThread(), TOKEN_IMPERSONATE, TRUE, &_hToken))
	{
		if (!::RevertToSelf())
		{
			Log.Error(__FUNCTION__ " (RevertToSelf %d)", ::GetLastError());
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (OpenThreadToken %d)", ::GetLastError());
	}
}

Impersonate::~Impersonate()
{
	if (_hToken)
	{
		if (!::SetThreadToken(NULL, _hToken))
		{
			Log.Error(__FUNCTION__ " (SetThreadToken %d)", ::GetLastError());
		}

		::CloseHandle(_hToken);
	}
}

ImpersonateUser::ImpersonateUser(HANDLE hToken) :
	_hToken(hToken)
{
	if (_hToken)
	{
		if (!::ImpersonateLoggedOnUser(_hToken))
		{
			Log.Error(__FUNCTION__ " (ImpersonateLoggedOnUser %d)", ::GetLastError());
		}
	}
}

ImpersonateUser::~ImpersonateUser()
{
	if (_hToken)
	{
		if (!::RevertToSelf())
		{
			Log.Error(__FUNCTION__ " (RevertToSelf %d)", ::GetLastError());
		}
	}
}
