// iconv.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <stdlib.h>

#define MYLIBAPI extern "C" __declspec(dllexport)

MYLIBAPI int URLEncodeGB(char *,char *, int);
MYLIBAPI int URLEncodeUTF(char *,char *, int);


typedef unsigned char uchar;

static void UTF_8ToUnicode(WCHAR* pOut,char *pText);
static void UnicodeToGB2312(char* pOut,unsigned short uData);
static void _urlencode(uchar *in,uchar *out);
static int _urlencode2(uchar *in,uchar *out, int len);

inline uchar toHex(uchar x)

{
    return x>9?x+55:x+48;
}

void _urlencode(uchar *in,uchar *out)
{
 if(out)
 {
  while (*in)
  {
   if(isalnum(*in))
    *out++ = *in;
   else
    if(isspace(*in))
     *out++ = '+';
    else
    {
     *out++ = '%';
     *out++ = toHex(*in>>4);
     *out++ = toHex(*in%16);
    }
    in++;
  }
  *out = '\0';
 }
}

int _urlencode2(uchar *in,uchar *out, int len)
{
	int olen = 0;
	int k = 0;
	if(out)
	{
		while ( k < len)  {
			if(isalnum(*in)) {
				*out++ = *in;
				olen ++;
			}
			else {
				if(isspace(*in)) {
					*out++ = '+';
					olen ++;
				}
				else
				{
					*out++ = '%';
					*out++ = toHex(*in>>4);
					*out++ = toHex(*in%16);
					olen += 3;
				}
			}
			in++;
			k ++;
		}
	}
	return olen;
}

void UTF_8ToUnicode(WCHAR* pOut,char *pText)
{
    char* uchar = (char *)pOut;
    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
    return;
}


void UnicodeToGB2312(char* pOut,unsigned short uData)
{
    ::WideCharToMultiByte(CP_ACP,NULL,(LPCWSTR)&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
}

int  URLEncodeGB (char *pText, char *newBuf, int pLen)
{
    // char * newBuf = (char *)malloc( (pLen + 1) * sizeof(char));
    char Ctemp[4];
    memset(Ctemp,0,4);
    memset(newBuf,0, pLen);

    int i =0;
    int j = 0;

    while (i < pLen)
    {
        if (pText[i] > 0)
        {
            newBuf[j++] = pText[i++];
        }
        else
        {
            WCHAR Wtemp;
            UTF_8ToUnicode(&Wtemp,pText + i);

            UnicodeToGB2312(Ctemp,Wtemp);

            newBuf[j] = Ctemp[0];
            newBuf[j + 1] = Ctemp[1];
            
            i += 3;
            j += 2;
        }
    }
    newBuf[j] = '\0';

	char *buf = (char *)malloc( strlen(newBuf) * 3 + 1);
	_urlencode( (unsigned char *) newBuf, (unsigned char *)buf);

	memcpy( newBuf, buf, strlen(buf));

	free(buf);

    return strlen(newBuf);
}

int  URLEncodeUTF (char *pText, char *newBuf, int pLen)
{
	return  _urlencode2( (unsigned char *) pText, (unsigned char *)newBuf, pLen);
}