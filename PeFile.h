#pragma once

#ifndef __PE_FILE_H__
#define __PE_FILE_H__
#include <windows.h>

#define FILE_PATH_LEN  256
#define SIGN_BUF_LEN   32
#define SKIP_LENGTH    1
#define KEY_ENCRPTY    0xA
#define NO_PE_DATA     "None"

#pragma pack(1)
struct SSoftSign
{
  BYTE bLength;   //��¼����ǩ����ص���Ϣ�ܳ���;[0,255]
  WORD wVersion1; //[0,65535]
  BYTE bVersion2; //[0,255]
  BYTE bVersion3; //[0,255]
  //DWORD dwSoftName;
  //DWORD dwAuthor;
  //DWORD dwEmail;
};
#pragma pack()

struct SString
{
  BYTE  bLength;
  TCHAR strBuffer[SIGN_BUF_LEN];
};

struct SPeFormat
{
  PIMAGE_DOS_HEADER         lpDosHdr; //DOS Header
  PIMAGE_NT_HEADERS         lpNtHdr;  //NT Header
  PIMAGE_FILE_HEADER        lpFilHdr; //File Header
  PIMAGE_OPTIONAL_HEADER    lpOptHdr; //Optional Header
  PIMAGE_DATA_DIRECTORY     lpDatDir; //Data Directory
  PIMAGE_SECTION_HEADER     lpBlkTbl; //Section Table

  PIMAGE_DATA_DIRECTORY     lpExpBlk; //Export Block(VirtualAddress && Size)
  PIMAGE_EXPORT_DIRECTORY   lpExpTbl; //Export Directory(FOA)

  PIMAGE_DATA_DIRECTORY     lpImpBlk; //Import Block(VirtualAddress && Size)
  PIMAGE_IMPORT_DESCRIPTOR  lpImpTbl; //Import Directory(FOA)

  PIMAGE_DATA_DIRECTORY     lpResBlk; //Resource Directory(VirtualAddress && Size)
  PIMAGE_RESOURCE_DIRECTORY lpResTbl; //Resource Directory(FOA)

  PIMAGE_DATA_DIRECTORY     lpRlcBlk; //Base Relocation Block(VirtualAddress && Size)
  PIMAGE_BASE_RELOCATION    lpRlcTbl; //Base Relocation Table(FOA)

  PIMAGE_DATA_DIRECTORY     lpDbgBlk; //Debug Block(VirtualAddress && Size)
  PIMAGE_DEBUG_DIRECTORY    lpDbgTbl; //Debug Directory(FOA)

  PIMAGE_DATA_DIRECTORY     lpTlsBlk; //TLS Block(VirtualAddress && Size)
  PIMAGE_TLS_DIRECTORY      lpTlsTbl; //TLS Directory(FOA)

  PIMAGE_DATA_DIRECTORY     lpIatBlk; //Import Address Table Block(VirtualAddress && Size)
  PIMAGE_THUNK_DATA         lpIatTbl; //Import Address Table(FOA)
};
typedef struct SPeFormat  FORMAT_PE;
typedef struct SPeFormat* PFORMAT_PE;

struct SPeFile
{
  TCHAR     strFileName[FILE_PATH_LEN];
  HANDLE    hFile;
  HANDLE    hMapFile;
  LPVOID    lpvBase;
  LPBYTE    strBase;
  FORMAT_PE stFmtPe;
  
  /***
  ����: ��һ��PE�ļ�;
  ����: strFileName: PE�ļ���·����;
  dwDesiredAccess: ��Ҫʹ�õķ���ģʽ,��ѡֵ��GENERIC_READ/GENERIC_WRITE,Ĭ��ΪGENERIC_READ;
  DWORD dwShareMode: ��Ҫʹ�õĹ���ģʽ,��ѡֵ��FILE_SHARE_READ/FILE_SHARE_WRITE,Ĭ��ΪFILE_SHARE_READ;
  ���: ����ļ��򿪳ɹ�,�򷵻�TRUE;����,����FALSE;
  ***/
  BOOL (*Open)(LPCTSTR strFileName, DWORD dwDesiredAccess/* = GENERIC_READ*/, DWORD dwShareMode/* = FILE_SHARE_READ*/);
  
  /***
  ����: �����ļ����ڴ�ӳ�����;
  ����: dwMapProtect: ����CreateFileMapping()ʱʹ�õ�ӳ�䱣��ģʽ,��ѡֵ��PAGE_READONLY/PAGE_READWRITE/PAGE_WRITECOPY,Ĭ��ΪPAGE_READONLY;
  lpObjectName: ӳ��������ַ���,Ĭ��ΪNULL;
  ���: ��������ɹ�,�򷵻�TRUE;����,����FALSE;
  ***/
  BOOL (*MapFile)(DWORD dwMapProtect, LPCSTR lpObjectName/* = NULL*/);
  
  /***
  ����: ���ļ���������ͼ�ķ�ʽӳ�䵽�ļ�ӳ�������;
  ����: dwDesiredViewAccess: ����MapViewOfFile()ʱ��Ҫʹ�õ�ӳ����ͼ����ģʽ,��ѡֵ��FILE_MAP_READ/FILE_MAP_WRITE/FILE_MAP_ALL_ACCESS,Ĭ��ΪFILE_MAP_READ;
  dwNumberOfBytesToMap: ӳ����ֽ���,Ĭ��Ϊ0,��ʾȫ�����ݶ�ӳ�䵽�ڴ���;
  ���: ���ӳ��ɹ�,�򷵻�TRUE;����,����FALSE;
  ***/
  BOOL (*MapFileView)(DWORD dwDesiredViewAccess, SIZE_T dwNumberOfBytesToMap/* = 0*/);
  
  /***
  ����: ���ļ�����ӳ����ͼ���ڴ�ӳ������г���;
  ����: void
  ���: ���ӳ����ͼ�����ɹ�,�򷵻�TRUE;����,����FALSE;
  ***/
  BOOL (*UnMapFileView)(VOID);
  
  /***
  ����: ��һ��PE�ļ�;
  ����: strFileName: PE�ļ���·����;
  lpResult: ����ļ��򿪹����еĴ�����;
  dwDesiredAccess: ��Ҫʹ�õķ���ģʽ,��ѡֵ��GENERIC_READ/GENERIC_WRITE,Ĭ��ΪGENERIC_READ;
  dwShareMode: ��Ҫʹ�õĹ���ģʽ,��ѡֵ��FILE_SHARE_READ/FILE_SHARE_WRITE,Ĭ��ΪFILE_SHARE_READ;
  dwMapProtect: ����CreateFileMapping()ʱʹ�õ�ӳ�䱣��ģʽ,��ѡֵ��PAGE_READONLY/PAGE_READWRITE/PAGE_WRITECOPY,Ĭ��ΪPAGE_READONLY;
  dwDesiredViewAccess: ����MapViewOfFile()ʱ��Ҫʹ�õ�ӳ����ͼ����ģʽ,��ѡֵ��FILE_MAP_READ/FILE_MAP_WRITE/FILE_MAP_ALL_ACCESS,Ĭ��ΪFILE_MAP_READ;
  ���: ����ļ��򿪳ɹ�,�򷵻�TRUE,lpResultָ����ڴ��ֵΪ0;����,����FALSE,lpResultָ����ڴ��ֵΪ������;
  ***/
  BOOL (*OpenEx)(LPCTSTR strFileName, LPDWORD lpResult, DWORD dwDesiredAccess/* = GENERIC_READ*/, DWORD dwShareMode/* = FILE_SHARE_READ*/, DWORD dwMapProtect/* = PAGE_READONLY*/, DWORD dwDesiredViewAccess/* = FILE_MAP_READ*/);
  VOID (*Close)(VOID);
  
  DWORD (*Rva2Foa)(DWORD dwRva);
  DWORD (*Foa2Rva)(DWORD dwFoa);
  LPSTR (*GetMachineString)(WORD machine);
  LPSTR (*GetSubsystemString)(WORD subsystem, WORD magic);
  DWORD (*GetSectionProperty)(LPSTR strBuf, WORD wBufSize, DWORD dwCharacteristic);
  DWORD (*Encrpty)(LPBYTE lpStart, WORD wLength, CONST BYTE bKey);
  DWORD (*Decrpty)(LPBYTE lpStart, WORD wLength, CONST BYTE bKey);
  
  /***
  return:
  TRUE  -> (struct SPeFormat*)(*lpdwResult),lpdwResult��һ��ָ��struct SPeFormat�ṹ��ָ��;
  FALSE -> (*lpResult) is reason for failed;
  ***/
  BOOL (*Parse)(LPDWORD lpdwResult);
};

extern struct SPeFile* g_lpPeFile;

struct SPeFile* AcquirePeFile(void);
void ReleasePeFile(void);

#endif
