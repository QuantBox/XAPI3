#include "stdafx.h"
#include "../../../include/XApiCpp.h"

#include "XApiImpl.h"

CXApi::CXApi()
{
}

CXApi::~CXApi()
{
}

CXApi* CXApi::CreateApi(const char* libPath)
{
	return new CXApiImpl(libPath);
}
