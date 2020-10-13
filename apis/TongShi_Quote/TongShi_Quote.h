// TongShi_Quote.h : main header file for the TongShi_Quote DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CTongShi_QuoteApp
// See TongShi_Quote.cpp for the implementation of this class
//

class CTongShi_QuoteApp : public CWinApp
{
public:
	CTongShi_QuoteApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
