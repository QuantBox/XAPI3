#include "stdafx.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inirw.h"

#define SIZE_LINE		1024	//每行最大长度
#define SIZE_FILENAME	256		//文件名最大长度

#define min(x, y)		(x <= y) ? x : y

typedef enum _ELineType_ {
    LINE_IDLE,		//未处理行
	LINE_ERROR,		//错误行
	LINE_EMPTY,		//空白行或注释行
	LINE_SECTION,	//节定义行
	LINE_VALUE		//值定义行
} ELineType ;


static char gFilename[SIZE_FILENAME];
static char *gBuffer;
static int gBuflen;

const char* GetBuffer()
{
	return gBuffer;
}
int GetBufferLen()
{
	return gBuflen;
}


//去除串首尾空格，原串被改写
static char *StrStrip(char *s)
{
	size_t size;
	char *p1, *p2;

	size = strlen(s);
	if (!size)
		return s;

	p2 = s + size - 1;

	while ((p2 >= s) && isspace((unsigned char)*p2))
		p2 --;
	*(p2 + 1) = '\0';

	p1 = s;
	while (*p1 && isspace((unsigned char)*p1))
		p1 ++;
	if (s != p1)
		memmove(s, p1, p2 - p1 + 2);
	return s;
}


//不区分大小写比较字符串
static int StriCmp(const char *s1, const char *s2)
{
	int ch1, ch2;
	do
	{
		ch1 = (unsigned char)*(s1++);
		if ((ch1 >= 'A') && (ch1 <= 'Z'))
			ch1 += 0x20;

		ch2 = (unsigned char)*(s2++);
		if ((ch2 >= 'A') && (ch2 <= 'Z'))
			ch2 += 0x20;
	} while ( ch1 && (ch1 == ch2) );
	return(ch1 - ch2);
}


//取一行
//输入：数据区(指针及长度)
//输出：行类型、有效内容串(去首尾空格)、注释首、注释尾、下一行首(行尾与下一行首间为换行符)
//      有效内容位置为[buf, rem1)
static int GetLine(char *buf, int buflen, char *content, char **rem1, char **rem2, char **nextline)
{
	char *cont1, *cont2;
	int cntblank, cntCR, cntLF;		//连续空格、换行符数量
	char isQuot1, isQuot2;			//引号
	int i;
	char *p;

	//首先断行断注释，支持如下换行符：\r、\n、\r\n、\n\r
	cntblank = 0;
	cntCR = cntLF = 0;
	isQuot1 = isQuot2 = 0;
	cont1 = *rem1 = 0;
	content[0] = 0;
	for (i = 0, p = buf; i < buflen; i ++, p ++)
	{
		if (*p == 0) {
			p ++;
			break;
		}
		//2个CR或LF，行结束
		if (cntCR == 2 || cntLF == 2) {
			// 这个地方会导致两个LF时前一句读出来的"少一个
			//p --;	//回溯1
			break;
		}
		//CR或LF各1个之后任意字符，行结束
		if (cntCR + cntLF >= 2) {
			break;
		}
		//CR或LF之后出现其它字符，行结束
		if ((cntCR || cntLF) && *p != '\r' && *p != '\n')
			break;

		switch (*p) {
		case '\r':
			cntCR ++;
			break;
		case '\n':
			cntLF ++;
			break;
		case '\'':
			if (!isQuot2)
				isQuot1 = 1 - isQuot1;
			break;
		case '\"':
			if (!isQuot1)
				isQuot2 = 1 - isQuot2;
			break;
		case ';':
		case '#':
			if (isQuot1 || isQuot2)
				break;
			if (*rem1 == NULL)
				*rem1 = p - cntblank;
			break;
		default:
			if (isspace((unsigned char)*p)) {
				cntblank ++;
			} else {
				cntblank = 0;
				if ((*rem1 == NULL) && (cont1 == NULL))
					cont1 = p;
			}
			break;
		}
	}

	*nextline = p;
	*rem2 = p - cntCR - cntLF;
	if (*rem1 == NULL)
		*rem1 = *rem2;
	cont2 = *rem1 - cntblank;

	if (cont1 == NULL) {
		cont1 = cont2;
		return LINE_EMPTY;
	}

	i = (int)(cont2 - cont1);
	if (i >= SIZE_LINE)
		return LINE_ERROR;

	//内容头尾已无空格
	memcpy(content, cont1, i);
	content[i] = 0;

	if (content[0] == '[' && content[i - 1] == ']')
		return LINE_SECTION;
	if (strchr(content, '=') != NULL)
		return LINE_VALUE;
	
	return LINE_ERROR;
}


//取一节section
//输入：节名称
//输出：成功与否、节名称首、节名称尾、节内容首、节内容尾(含换行)、下一节首(节尾与下一节首间为空行或注释行)
static int FindSection(const char *section, char **sect1, char **sect2, char **cont1, char **cont2, char **nextsect)
{
	int type;
	char content[SIZE_LINE];
	char *rem1, *rem2, *nextline = nullptr;

	char *p = nullptr;
	char *empty = nullptr;
	int uselen = 0;
	char found = 0;

	if (gBuffer == NULL) {
		return 0;
	}

	while (gBuflen - uselen > 0) {
		p = gBuffer + uselen;
		type = GetLine(p, gBuflen - uselen, content, &rem1, &rem2, &nextline);
		uselen += (int)(nextline - p);

		if (LINE_SECTION == type) {
			if (found || section == NULL) break;		//发现另一section
			content[strlen(content) - 1] = 0;			//去尾部]
			StrStrip(content + 1);						//去首尾空格
			if (StriCmp(content + 1, section) == 0) {
				found = 1;
				*sect1 = p;
				*sect2 = rem1;
				*cont1 = nextline;
			}
			empty = nextline;
		} else
		if (LINE_VALUE == type) {
			if (!found && section == NULL) {
				found = 1;
				*sect1 = p;
				*sect2 = p;
				*cont1 = p;
			}
			empty = nextline;
		}
	}
	
	if (!found) return 0;

	*cont2 = empty;
	*nextsect = nextline;
	return 1;
}


//从一行取键、值
//输入：内容串(将被改写)
//输出：键串、值串
static void GetKeyValue(char *content, char **key, char **value)
{
	char *p;

	p = strchr(content, '=');
	*p = 0;
	StrStrip(content);
	StrStrip(p + 1);
	*key = content;
	*value = p + 1;
}


//释放ini文件所占资源
void iniFileFree()
{
	if (gBuffer != NULL) {
		free(gBuffer);
		gBuffer = 0;
		gBuflen = 0;
	}
}


//加载ini文件至内存
int iniFileLoad(const char *filename)
{
	FILE *file;
	int len;

	iniFileFree();
	if (strlen(filename) >= sizeof(gFilename))
		return 0;
	strcpy(gFilename, filename);

	file = fopen(gFilename, "rb");
	if (file == NULL) 
		return 0;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	gBuffer = (char*)malloc(len);
	if (gBuffer == NULL) {
		fclose(file);
		return 0;
	}

	fseek(file, 0, SEEK_SET);
	len = fread(gBuffer, 1, len, file);
	fclose(file);
	gBuflen = len;
	return 1;
}


//读取值原始串
static int iniGetValue(const char *section, const char *key, char *value, int maxlen, const char *defvalue)
{
	int type;
	char content[SIZE_LINE];
	char *rem1, *rem2, *nextline;
	char *key0, *value0;

	char *p;
	int uselen = 0;
	char found = 0;

	int len;

	if (gBuffer == NULL || key == NULL) {
		if (value != NULL)
			value[0] = 0;
		return 0;
	}

	while (gBuflen - uselen > 0) {
		p = gBuffer + uselen;
		type = GetLine(p, gBuflen - uselen, content, &rem1, &rem2, &nextline);
		uselen += (int)(nextline - p);

		if (LINE_SECTION == type) {
			if (found || section == NULL) break;		//发现另一section
			content[strlen(content) - 1] = 0;			//去尾部]
			StrStrip(content + 1);						//去首尾空格
			if (StriCmp(content + 1, section) == 0) {
				found = 1;
			}
		} else
		if (LINE_VALUE == type) {
			if (!found && section == NULL) {
				found = 1;
			}
			if (!found)
				continue;
			GetKeyValue(content, &key0, &value0);
			if (StriCmp(key0, key) == 0) {
				len = strlen(value0);
				if (len == 0) break;		//空值视为无效
				if (value != NULL) {
					len = min(len, maxlen - 1);
					strncpy(value, value0, len);
					value[len] = 0;
				}
				return 1;
			}
		}
	}
	
	//未发现键值取缺省
	if (value != NULL) {
		if (defvalue != NULL) {
			len = min(strlen(defvalue), maxlen - 1);
			strncpy(value, defvalue, len);
			value[len] = 0;
		} else {
			value[0] = 0;
		}
	}
	return 0;
}


//获取字符串，不带引号
int iniGetString(const char *section, const char *key, char *value, int maxlen, const char *defvalue)
{
	int ret;
	int len;

	ret = iniGetValue(section, key, value, maxlen, defvalue);
	if (!ret)
		return ret;

	//去首尾空格
	len = strlen(value);
	if (value[0] == '\'' && value[len - 1] == '\'') {
		value[len - 1] = 0;
		memmove(value, value + 1, len - 1);
	} else
	if (value[0] == '\"' && value[len - 1] == '\"') {
		value[len - 1] = 0;
		memmove(value, value + 1, len - 1);
	}
	return ret;
}


//获取整数值
int iniGetInt(const char *section, const char *key, int defvalue)
{
	char valstr[64];

	if (iniGetString(section, key, valstr, sizeof(valstr), ""))
	    return (int)strtol(valstr, NULL, 0);
	return defvalue;
}


//获取浮点数
double iniGetDouble(const char *section, const char *key, double defvalue)
{
	char valstr[64];

	if (iniGetString(section, key, valstr, sizeof(valstr), ""))
	    return (int)atof(valstr);
	return defvalue;
}


//设置字符串：若value为NULL，则删除该key所在行，包括注释
int iniSetString(const char *section, const char *key, const char *value, bool quote)
{
	FILE *file;
	char *sect1, *sect2, *cont1, *cont2, *nextsect;
	char *p;
	int len, type;
	char content[SIZE_LINE];
	char *key0, *value0;
	char *rem1, *rem2, *nextline;

	// 注了就可以创建文件了
	//if (gBuffer == NULL) {
	//	return 0;
	//}

	if (FindSection(section, &sect1, &sect2, &cont1, &cont2, &nextsect) == 0)
	{
		//未找到节

		//value无效则返回
		if (value == NULL) 
			return 0;

		//在文件尾部添加
		file = fopen(gFilename, "ab");
		if (file == NULL) 
			return 0;
		if (quote)
		{
			fprintf(file, "\n[%s]\n%s=\"%s\"\n", section, key, value);
		}
		else
		{
			fprintf(file, "\n[%s]\n%s=%s\n", section, key, value);
		}
		fclose(file);
		iniFileLoad(gFilename);
		return 1;
	}

	//找到节，则节内查找key
	p = cont1;
	len = (int)(cont2 - cont1);
	while (len > 0) {
		type = GetLine(p, len, content, &rem1, &rem2, &nextline);

		if (LINE_VALUE == type) {
			GetKeyValue(content, &key0, &value0);
			if (StriCmp(key0, key) == 0) {
				//找到key
				file = fopen(gFilename, "wb");
				if (file == NULL) 
					return 0;
				len = (int)(p - gBuffer);
				fwrite(gBuffer, 1, len, file);					//写入key之前部分
				if (value == NULL) {
					//value无效，删除
					len = (int)(nextline - gBuffer);			//整行连同注释一并删除
				} else {
					//value有效，改写
					if (quote)
					{
						fprintf(file, "%s=\"%s\"", key, value);
					}
					else
					{
						fprintf(file, "%s=%s", key, value);
					}
					len = (int)(rem1 - gBuffer);				//保留尾部原注释!
				}
				fwrite(gBuffer + len, 1, gBuflen - len, file);	//写入key所在行含注释之后部分
				fclose(file);
				iniFileLoad(gFilename);
				return 1;
			}	
		}

		len -= (int)(nextline - p);
		p = nextline;
	}

	//未找到key

	//value无效则返回
	if (value == NULL) 
		return 0;

	//在文件尾部添加
	file = fopen(gFilename, "wb");
	if (file == NULL) 
		return 0;
	len = (int)(cont2 - gBuffer);
	fwrite(gBuffer, 1, len, file);					//写入key之前部分
	if (quote)
	{
		fprintf(file, "%s=\"%s\"\n", key, value);
	}
	else
	{
		fprintf(file, "%s=%s\n", key, value);
	}
	fwrite(gBuffer + len, 1, gBuflen - len, file);	//写入key之后部分
	fclose(file);
	iniFileLoad(gFilename);
	return 1;
}


//设置整数值：base取值10、16、8，分别表示10、16、8进制，缺省为10进制
int iniSetInt(const char *section, const char *key, int value, int base)
{
	char valstr[64];

	switch (base) {
	case 16:
		sprintf(valstr, "0x%x", value);
		return iniSetString(section, key, valstr, false);
	case 8:
		sprintf(valstr, "0%o", value);
		return iniSetString(section, key, valstr, false);
	default:	//10
		sprintf(valstr, "%d", value);
		return iniSetString(section, key, valstr, false);
	}
}
