// FakeAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

void ShowMessageBox(char* buf)
{
	if (OpenClipboard(NULL))
	{
		int len = strlen(buf) + 1;

		HGLOBAL hmem = GlobalAlloc(GHND, len);
		char *pmem = (char*)GlobalLock(hmem);

		EmptyClipboard();
		strcpy(pmem, buf);
		SetClipboardData(CF_TEXT, hmem);
		CloseClipboard();
		GlobalFree(hmem);
	}

	MessageBoxA(nullptr, buf, "", MB_OK);
}



