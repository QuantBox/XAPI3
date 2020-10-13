#pragma once

#ifdef ENABLE_LICENSE

#include <list>
#include <map>
#include <set>
#include <iostream>
#include <vector>

#include "randpool.h"
#include "rsa.h"
#include "hex.h"
#include "files.h"

// 1
#define ERROR_CODE_1	"没有读取到授权文件,将自动生成试用版授权到dll同目录下，机器码为[%s]"
#define ERROR_CODE_2	"授权认证失败"
#define ERROR_CODE_3	"授权过期，过期时间为[%d]"
#define ERROR_CODE_4	"没有[用户号:%s]的授权"
#define ERROR_CODE_5	"机器码验证失败"
#define ERROR_CODE_6	"试用超次数，限制[%d]次"
#define ERROR_CODE_7	"授权产品名不对应"
#define ERROR_CODE_8	"[用户号:%s]授权的[姓名:%s]，现获得的为[%s]，下单将被拒绝"
#define ERROR_CODE_9	"没有[姓名:%s]的授权"
#define ERROR_CODE_10	"授权范围太广时，到期时间不能过长"
#define ERROR_CODE_11	"写入授权文件失败，请检查是否有权限写入或以管理员方式再次启动。试用次数为[%d]，机器码为[%s]"
#define ERROR_CODE_14	"授权失败，无法下单"
// 12
#define ERROR_CODE_12	"未读取到签名信息，使用最小授权，试用次数为[%d]，机器码为[%s]"
#define ERROR_CODE_13	"授权快要过期，请提前申请新的授权。过期时间为[%d]，开始提醒日为[%d]"

#define FILE_EXT_LIC "License"
#define FILE_EXT_PUB "PublicKey"
#define FILE_EXT_PRIV "PrivateKey"
#define FILE_EXT_SIGN "Signature"

using namespace std;
using namespace CryptoPP;

#ifdef _DEBUG
#pragma comment(lib, "../../lib/Debug/cryptlib.lib")
#else
#pragma comment(lib, "../../lib/Release/cryptlib.lib")
#endif // DEBUG



void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed);
string RSAEncryptString(const char *pubFilename, const char *seed, const char *message);
string RSADecryptString(const char *privFilename, const char *ciphertext);
void RSASignFileStringFile(const char *privFilename, const char *messageFilename, const char *signatureFilename);
bool RSAVerifyStringStringString(const char *pubFilename, const char *messageFilename, const char *signatureFilename);

RandomPool & GlobalRNG();

/*
功能说明：
1.没有证书文件时要重新生成，生成的要能直接通过临时认证
2.有证书文件时，要判断是不是绑定了机器
3.还有试用检查
4.没有绑定的时间不能过长

应当检查一下，如果是试用版，就可以一直添加新用户和机器码，而正式用户就不可以了，这样就可以记录很多账户了

决定多加一些东西
1.记录一些权限信息.License
2.记录公钥.PublicKey
3.记录签名文件.Signature

4.记录私钥.PrivateKey

如何防止用户自己生成公私钥后再签名
1.公钥不容易改，建议添加到资源中
2.授权文件可再加一次密后再来签名

授权检查的逻辑
1.所有相关文件都没有，使用临时授权，生成临时授权文件，不生成签名
2.有授权文件，但没有签名，临时授权与本地授权文件取最小
3.有授权文件，但签名是错的，直接拒绝
4.有授权文件，签名也是正确的，按授权文件来
*/
class CLicense
{
public:
	CLicense();
	~CLicense();

	// 通过导出函数得到当前dll的路径
	void GetDllPathByFunctionName(const char* szFunctionName, char* szPath);
	// 设置证书路径，会同时再生成其它路径，这个函数会忽视后缀，生成自己的后缀
	void SetLicensePath(const char* licPath);



	int LoadIni();
	int SaveIni();

	const char* GetErrorInfo();
	int GetErrorCodeForMachineID();
	int GetErrorCodeByAccount(const char* account);
	int GetErrorCodeForSign();
	int GetErrorCodeForExpireDate();
	int GetErrorCodeByNameThenAccount(const char* name, const char* account);
	int GetErrorCodeForSendOrder();

	void SetSendOrderFlag(bool flag);
	bool GetSendOrderFlag();

	
	int Today(int day);
	int Date(int date, int day);

	void AddUser(const char* account, const char* name);

	void CreateDefault();


	// 只是简单的加载文件
	string LoadStringFromFile(const char *filename);

	// 对某个字符串进行签名，可以先对此字符串预处理
	void Sign(const char* message);
	// 对某个字符串进行验证，可以先对此字符串预处理
	// 公钥可以保存在dll中，不便修改的模式，这样用户
	bool Verify(const char* message, const char* pubKey, const char* signature);
	
	
	// 在进行GetErrorCode时，需要先设置公钥，这样好检查是否正确
	void SetPublicKeyString(const char* pubKey);
	void SetSignatureString(const char* signature);

	bool IsTrial();
	bool HasSignature();
	bool HasSaved();

public:
	char m_LicensePath[260];
	char m_PublicKeyPath[260];
	char m_PrivateKeyPath[260];
	char m_SignaturePath[260];

	char m_PublicKeyString[1024];
	char m_SignatureString[1024];

private:
	bool m_bHasSaved;
	int m_ExpireDate;
	int m_Trial;
	int m_nCurrentTrial;
	
	char m_RealMAC[32];

	char m_ErrorInfo[256];
	int m_ErrorCode;

	map<string, string> m_UserMap;
	//set<string> m_MacSet;
	char m_Account[1024];
	char m_UserName[1024];
	char m_MAC[1024];
	bool m_bLoaded;

	bool m_bHasStarAccount;
	bool m_bHasStarUserName;
	bool m_bHasStarMAC;

	bool m_bSendOrderFlag;
};

#endif

