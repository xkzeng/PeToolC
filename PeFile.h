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
  BYTE bLength;   //记录所有签名相关的信息总长度;[0,255]
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
  功能: 打开一个PE文件;
  参数: strFileName: PE文件的路径名;
  dwDesiredAccess: 需要使用的访问模式,可选值有GENERIC_READ/GENERIC_WRITE,默认为GENERIC_READ;
  DWORD dwShareMode: 需要使用的共享模式,可选值有FILE_SHARE_READ/FILE_SHARE_WRITE,默认为FILE_SHARE_READ;
  结果: 如果文件打开成功,则返回TRUE;否则,返回FALSE;
  ***/
  BOOL (*Open)(LPCTSTR strFileName, DWORD dwDesiredAccess/* = GENERIC_READ*/, DWORD dwShareMode/* = FILE_SHARE_READ*/);
  
  /***
  功能: 创建文件的内存映射对象;
  参数: dwMapProtect: 调用CreateFileMapping()时使用的映射保护模式,可选值有PAGE_READONLY/PAGE_READWRITE/PAGE_WRITECOPY,默认为PAGE_READONLY;
  lpObjectName: 映射对象名字符串,默认为NULL;
  结果: 如果创建成功,则返回TRUE;否则,返回FALSE;
  ***/
  BOOL (*MapFile)(DWORD dwMapProtect, LPCSTR lpObjectName/* = NULL*/);
  
  /***
  功能: 把文件内容以视图的方式映射到文件映射对象中;
  参数: dwDesiredViewAccess: 调用MapViewOfFile()时需要使用的映射视图访问模式,可选值有FILE_MAP_READ/FILE_MAP_WRITE/FILE_MAP_ALL_ACCESS,默认为FILE_MAP_READ;
  dwNumberOfBytesToMap: 映射的字节数,默认为0,表示全部内容都映射到内存中;
  结果: 如果映射成功,则返回TRUE;否则,返回FALSE;
  ***/
  BOOL (*MapFileView)(DWORD dwDesiredViewAccess, SIZE_T dwNumberOfBytesToMap/* = 0*/);
  
  /***
  功能: 把文件内容映射视图从内存映射对象中撤销;
  参数: void
  结果: 如果映射视图撤销成功,则返回TRUE;否则,返回FALSE;
  ***/
  BOOL (*UnMapFileView)(VOID);
  
  /***
  功能: 打开一个PE文件;
  参数: strFileName: PE文件的路径名;
  lpResult: 存放文件打开过程中的错误码;
  dwDesiredAccess: 需要使用的访问模式,可选值有GENERIC_READ/GENERIC_WRITE,默认为GENERIC_READ;
  dwShareMode: 需要使用的共享模式,可选值有FILE_SHARE_READ/FILE_SHARE_WRITE,默认为FILE_SHARE_READ;
  dwMapProtect: 调用CreateFileMapping()时使用的映射保护模式,可选值有PAGE_READONLY/PAGE_READWRITE/PAGE_WRITECOPY,默认为PAGE_READONLY;
  dwDesiredViewAccess: 调用MapViewOfFile()时需要使用的映射视图访问模式,可选值有FILE_MAP_READ/FILE_MAP_WRITE/FILE_MAP_ALL_ACCESS,默认为FILE_MAP_READ;
  结果: 如果文件打开成功,则返回TRUE,lpResult指向的内存的值为0;否则,返回FALSE,lpResult指向的内存的值为错误码;
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
  TRUE  -> (struct SPeFormat*)(*lpdwResult),lpdwResult是一个指向struct SPeFormat结构的指针;
  FALSE -> (*lpResult) is reason for failed;
  ***/
  BOOL (*Parse)(LPDWORD lpdwResult);
};

extern struct SPeFile* g_lpPeFile;

struct SPeFile* AcquirePeFile(void);
void ReleasePeFile(void);

#endif
