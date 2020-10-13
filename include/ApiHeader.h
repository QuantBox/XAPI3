#ifndef _API_HEADER_H_
#define _API_HEADER_H_

#include "CrossPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif
	//用于分隔输入的合列表，与前置机地址列表，所以不能出现“:”一类的
#define _QUANTBOX_SEPS_ ";"

	DLL_PUBLIC void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif

// 长度不够时，后面会填0，但长度超出会写出界
#define SAFE_STRNCPY(x,y) if(y) strncpy_s(x, y, sizeof(x));x[sizeof(x)-1]=0;
#define SAFE_STRNCAT(x,y) if(y) strncat_s(x, y, sizeof(x));x[sizeof(x)-1]=0;
// 有循环问题，不能乱用，比如abc%s,结果将变成abcabc
#define SAFE_SNPRINTF(x, ...) _snprintf_s(x,sizeof(x),##__VA_ARGS__);x[sizeof(x)-1]=0;

#endif//end of _API_HEADER_H_
