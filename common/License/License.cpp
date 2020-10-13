#include "stdafx.h"
#include "License.h"
#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi.lib")

#ifdef ENABLE_LICENSE

#include <regex>
#include <time.h>



#include "../../include/inirw.h"

#define APP_NAME_USER		"User"
#define KEY_NAME_ACCOUNT	"Account"
#define KEY_NAME_USERNAME	"UserName"

#define KEY_NAME_PRODUCT	"Product"
#define APP_NAME_LICENSE	"License"
#define KEY_NAME_COUNT		"Count"
#define KEY_NAME_MAC		"MachineID"
#define KEY_NAME_TRIAL		"Trial"
#define KEY_NAME_EXPIRE_DATE	"ExpireDate"
#define KEY_NAME_SERIAL_NUMBER	"SerialNumber"
#define KEY_NAME_INFO		"Info"

using namespace std;

//// Prints the MAC address stored in a 6 byte array to stdout
//static void PrintMACaddress(unsigned char MACData[])
//{
//
//#ifdef PRINTING_TO_CONSOLE_ALLOWED
//
//	printf("\nMAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
//		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
//
//#endif
//
//	char string[256];
//	sprintf(string, "%02X-%02X-%02X-%02X-%02X-%02X\n", MACData[0], MACData[1],
//		MACData[2], MACData[3], MACData[4], MACData[5]);
//	//WriteConstantString("MACaddress", string);
//	printf(string);
//}

// Fetches the MAC address and prints it
//DWORD GetMACaddress()
//{
//	DWORD MACaddress = 0;
//	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
//	// for up to 16 NICs
//	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
//
//	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
//		AdapterInfo,                 // [out] buffer to receive data
//		&dwBufLen);                  // [in] size of receive data buffer
//	//assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
//	// valid, no buffer overflow
//
//	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
//	// current adapter info
//	do {
//		if (MACaddress == 0)
//			MACaddress = pAdapterInfo->Address[5] + pAdapterInfo->Address[4] * 256 +
//			pAdapterInfo->Address[3] * 256 * 256 +
//			pAdapterInfo->Address[2] * 256 * 256 * 256;
//		//PrintMACaddress(pAdapterInfo->Address); // Print MAC address
//		//memcpy(pAddress1, pAdapterInfo->Address, sizeof(pAdapterInfo->Address));
//		//break;
//		pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
//	} while (pAdapterInfo);                    // Terminate if last adapter
//
//	return MACaddress;
//}

// Prints the MAC address stored in a 6 byte array to stdout
static void PrintMACaddress(unsigned char MACData[],char* string)
{

#ifdef PRINTING_TO_CONSOLE_ALLOWED

	printf("\nMAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);

#endif

	//char string[256];
	sprintf(string, "%02X%02X%02X%02X%02X%02X", MACData[0], MACData[1],
		MACData[2], MACData[3], MACData[4], MACData[5]);
	//WriteConstantString("MACaddress", string);
	//printf(string);
}

DWORD GetMACaddress(char* string)
{
	DWORD MACaddress = 0;
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
	// for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer
	//assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
	// valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
	// current adapter info
	do {
		if (MACaddress == 0)
			MACaddress = pAdapterInfo->Address[5] + pAdapterInfo->Address[4] * 256 +
			pAdapterInfo->Address[3] * 256 * 256 +
			pAdapterInfo->Address[2] * 256 * 256 * 256;
		PrintMACaddress(pAdapterInfo->Address, string); // Print MAC address
		//memcpy(pAddress1, pAdapterInfo->Address, sizeof(pAdapterInfo->Address));
		break;
		pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
	} while (pAdapterInfo);                    // Terminate if last adapter

	return MACaddress;
}


CLicense::CLicense()
{
	memset(m_LicensePath, 0, sizeof(m_LicensePath));
	memset(m_PublicKeyPath, 0, sizeof(m_PublicKeyPath));
	memset(m_PrivateKeyPath, 0, sizeof(m_PrivateKeyPath));
	memset(m_SignaturePath, 0, sizeof(m_SignaturePath));
	memset(m_PublicKeyString, 0, sizeof(m_PublicKeyString));
	memset(m_SignatureString, 0, sizeof(m_SignatureString));
	memset(m_RealMAC, 0, sizeof(m_RealMAC));

	m_bHasSaved = false;
	m_bLoaded = false;
	m_bSendOrderFlag = false;

	//sprintf_s(m_RealMAC, "%ld", GetMACaddress());
	GetMACaddress(m_RealMAC);

	CreateDefault();
}


CLicense::~CLicense()
{
}

int CLicense::Today(int day)
{
	time_t now = time(0);
	now += day * 86400;
	struct tm *ptmNow = localtime(&now);

	return (ptmNow->tm_year + 1900) * 10000
		+ (ptmNow->tm_mon + 1) * 100
		+ ptmNow->tm_mday;
}

int CLicense::Date(int date, int day)
{
	time_t now = time(0);
	struct tm *ptmNow = localtime(&now);

	ptmNow->tm_year = date / 10000 - 1900;
	ptmNow->tm_mon = date % 10000 / 100 - 1;
	ptmNow->tm_mday = date % 100;

	now = mktime(ptmNow);

	now += day * 86400;
	ptmNow = localtime(&now);

	return (ptmNow->tm_year + 1900) * 10000
		+ (ptmNow->tm_mon + 1) * 100
		+ ptmNow->tm_mday;
}

void CLicense::GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, szFunctionName, &hModule);

	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void CLicense::SetLicensePath(const char* licPath)
{
	// 授权文件的路径是生成签名和验证都需要，所以最核心
	char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = {0};
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };
	_splitpath(licPath, drive, dir, fname, ext);
	_makepath(m_LicensePath, drive, dir, fname, FILE_EXT_LIC);
	_makepath(m_PublicKeyPath, drive, dir, fname, FILE_EXT_PUB);
	_makepath(m_PrivateKeyPath, drive, dir, fname, FILE_EXT_PRIV);
	_makepath(m_SignaturePath, drive, dir, fname, FILE_EXT_SIGN);
}

void CLicense::SetPublicKeyString(const char* pubKey)
{
	strncpy(m_PublicKeyString, pubKey, sizeof(m_PublicKeyString));
}

void CLicense::SetSignatureString(const char* signature)
{
	strncpy(m_SignatureString, signature, sizeof(m_SignatureString));
}

bool CLicense::IsTrial()
{
	return (m_Trial == 0 || strlen(m_PublicKeyString) <= 0);
}

bool CLicense::HasSignature()
{
	return (strlen(m_SignatureString) > 0);
}

bool CLicense::HasSaved()
{
	return m_bHasSaved;
}

void CLicense::SetSendOrderFlag(bool flag)
{
	m_bSendOrderFlag = flag;
}

bool CLicense::GetSendOrderFlag()
{
	return m_bSendOrderFlag;
}

int CLicense::LoadIni()
{
	m_bLoaded = iniFileLoad(m_LicensePath);

	// 无所谓是否加载成功，反正有默认
	m_ExpireDate = iniGetInt(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, -1);
	m_Trial = iniGetInt(APP_NAME_LICENSE, KEY_NAME_TRIAL, 5);

	iniGetString(APP_NAME_USER, KEY_NAME_ACCOUNT, m_Account, sizeof(m_Account), "");
	iniGetString(APP_NAME_USER, KEY_NAME_USERNAME, m_UserName, sizeof(m_UserName), "");
	iniGetString(APP_NAME_LICENSE, KEY_NAME_MAC, m_MAC, sizeof(m_MAC), "");

	iniFileFree();

	if (m_bLoaded != 0)
		return 0;

	sprintf(m_ErrorInfo, ERROR_CODE_1, m_RealMAC);
	m_ErrorCode = 1;
	return m_ErrorCode;
}

void CLicense::AddUser(const char* account, const char* name)
{
	m_UserMap[string(account)] = string(name);
}

int CLicense::SaveIni()
{
	iniFileLoad(m_LicensePath);

	int bRet = iniSetInt(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, m_ExpireDate, Today(7));
	iniSetInt(APP_NAME_LICENSE, KEY_NAME_TRIAL, m_Trial, 5);
	iniSetString(APP_NAME_LICENSE, KEY_NAME_MAC, m_MAC);

	iniSetString(APP_NAME_USER, KEY_NAME_ACCOUNT, m_Account);
	iniSetString(APP_NAME_USER, KEY_NAME_USERNAME, m_UserName);

	char szInfo[2048] = { 0 };
	strcat(szInfo, m_RealMAC);
	strcat(szInfo, ":");
	for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		strcat(szInfo, it->first.c_str());
		strcat(szInfo, "|");
		strcat(szInfo, it->second.c_str());
		strcat(szInfo, "|");
	}
	iniSetString(APP_NAME_USER, KEY_NAME_INFO, szInfo);
	iniFileFree();

	if (bRet != 0)
	{
		m_bHasSaved = true;
		return 0;
	}

	sprintf(m_ErrorInfo, ERROR_CODE_11, m_Trial, m_RealMAC);
	m_ErrorCode = 11;
	return m_ErrorCode;
}


void CLicense::CreateDefault()
{
	m_Trial = 5;	// 默认5次
	m_ExpireDate = Today(7);	// 7天
	m_UserMap.clear();
	m_nCurrentTrial = 0;

	strncpy(m_Account, ".*", sizeof(m_Account));
	strncpy(m_UserName, "", sizeof(m_UserName));
	strncpy(m_MAC, ".*", sizeof(m_MAC));
}

int CLicense::GetErrorCodeForSign()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		// 由于在前面已经有GetErrorCodeForSign()做了签名信息是否存在的检查
		if (!HasSignature())
		{
			// 所以这里是以最小授权进行通过
			// 只有试用次数可以检查了
			m_Trial = min(m_Trial, 5);
			// 由于在这之前就已经检查过了过期时间了，所以这地方没有必要
			//m_ExpireDate = min(m_ExpireDate, Today(7));

			m_ErrorCode = 12;
			sprintf_s(m_ErrorInfo, ERROR_CODE_12, m_Trial, m_RealMAC);

			break;
		}

		string s = LoadStringFromFile(m_LicensePath);

		if (Verify(s.c_str(), m_PublicKeyString, m_SignatureString))
		{

		}
		else
		{
			m_ErrorCode = -2;
			strcpy(m_ErrorInfo, ERROR_CODE_2);
			break;
		}

	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeForMachineID()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		if (strlen(m_MAC) <= 0)
		{
			break;
		}
		// 检查机器码
		regex pattern(m_MAC);
		if (!regex_search(m_RealMAC, pattern))
		{
			strcpy(m_ErrorInfo, ERROR_CODE_5);
			m_ErrorCode = -5;
			break;
		}

	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeForExpireDate()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		// 是否过期了
		if (m_ExpireDate < Today(0))
		{
			m_ErrorCode = -3;
			sprintf(m_ErrorInfo, ERROR_CODE_3, m_ExpireDate);
			break;
		}

		// 看授权时间是否合要求
		int metion = Date(m_ExpireDate, -15);
		if (metion <= Today(0))
		{
			m_ErrorCode = 13;
			sprintf_s(m_ErrorInfo, ERROR_CODE_13, m_ExpireDate, metion);
			break;
		}
	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeByAccount(const char* account)
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		if (strlen(m_Account) <= 0)
		{
			break;
		}
		regex pattern(m_Account);
		if (!regex_search(account, pattern))
		{
			m_ErrorCode = -4;
			sprintf(m_ErrorInfo, ERROR_CODE_4, account);
			break;
		}

	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeByNameThenAccount(const char* name, const char* account)
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		// 汉字使用正则太复杂，还是改用查找
		if (strlen(m_UserName) <= 0)
		{
			break;
		}
		if (nullptr == strstr(m_UserName, name))
		{
			m_ErrorCode = -9;
			sprintf(m_ErrorInfo, ERROR_CODE_9, name);
			break;
		}
	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeForSendOrder()
{
	do
	{
		if (!m_bSendOrderFlag)
		{
			sprintf(m_ErrorInfo, ERROR_CODE_14);
			m_ErrorCode = -14;
			break;
		}

		// 检查次数
		if (m_Trial > 0)
		{
			if (m_nCurrentTrial >= m_Trial)
			{
				sprintf(m_ErrorInfo, ERROR_CODE_6, m_Trial);
				m_ErrorCode = -6;
				break;
			}
		}

		// 认证通过就加1
		++m_nCurrentTrial;

		// 认证通过
		m_ErrorCode = 0;
	} while (false);

	return m_ErrorCode;
}

const char* CLicense::GetErrorInfo()
{
	if (m_ErrorCode == 0)
		return nullptr;
	return m_ErrorInfo;
}

string CLicense::LoadStringFromFile(const char *filename)
{
	try
	{
		string result;
		FileSource(filename, true, new StringSink(result));
		return result;
	}
	catch (...)
	{
		return "";
	}
}

void CLicense::Sign(const char* message)
{
	if (strlen(message) <= 0)
	{
		return;
	}
	if (strlen(m_PrivateKeyPath) <= 0)
	{
		return;
	}
	RSASignFileStringFile(m_PrivateKeyPath, message, m_SignaturePath);
}

bool CLicense::Verify(const char* message, const char* pubKey, const char* signature)
{
	if (strlen(message) <= 0)
	{
		return false;
	}
	if (strlen(pubKey) <= 0)
	{
		return false;
	}

	return RSAVerifyStringStringString(pubKey, message, signature);
}

//------------------------
// 定义全局的随机数池
//------------------------
RandomPool & GlobalRNG()
{
	static RandomPool randomPool;
	return randomPool;
}

void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed)
{
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privFile(new FileSink(privFilename));
	priv.DEREncode(privFile);
	privFile.MessageEnd();

	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubFile(new FileSink(pubFilename));
	pub.DEREncode(pubFile);
	pubFile.MessageEnd();
}

string RSAEncryptString(const char *pubFilename, const char *seed, const char *message)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubFile);

	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

string RSADecryptString(const char *privFilename, const char *ciphertext)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privFile);

	string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}

void RSASignFileStringFile(const char *privFilename, const char *messageFilename, const char *signatureFilename)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Signer priv(privFile);
	StringSource f(messageFilename, true, new SignerFilter(GlobalRNG(), priv, new HexEncoder(new FileSink(signatureFilename))));
}

bool RSAVerifyStringStringString(const char *pubFilename, const char *messageFilename, const char *signatureFilename)
{
	StringSource pubFile(pubFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Verifier pub(pubFile);

	StringSource signatureFile(signatureFilename, true, new HexDecoder);
	if (signatureFile.MaxRetrievable() != pub.SignatureLength())
		return false;
	SecByteBlock signature(pub.SignatureLength());
	signatureFile.Get(signature, signature.size());

	VerifierFilter *verifierFilter = new VerifierFilter(pub);
	verifierFilter->Put(signature, pub.SignatureLength());
	StringSource f(messageFilename, true, verifierFilter);

	return verifierFilter->GetLastResult();
}

#endif
