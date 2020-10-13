// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
// BOOLEAN与asn.h中的冲突，不得不将asn.h中的改了，希望有人帮忙解决这个问题
// 已经解决，移动到这就可以了
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
