//TabUsrOpr.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#include "Global.h"
#include "PeFile.h"
#include "TabUsrOpr.h"

static HWND _hWnd = NULL; //当前页面窗口的句柄;

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  return TRUE;
}

static VOID UpdateCheckBox(VOID)
{
  INT32 iState = -1;
  
  iState = SendMessage(GetDlgItem(_hWnd, IDC_CHECK_ADDRESS_CONVERT), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
  if(BST_UNCHECKED == iState)
  {
    SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SRC), WM_SETTEXT, (WPARAM)0, (LPARAM)"RVA");
    SendMessage(GetDlgItem(_hWnd, IDC_STATIC_DST), WM_SETTEXT, (WPARAM)0, (LPARAM)"FOA");
  }
  else if(BST_CHECKED == iState)
  {
    SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SRC), WM_SETTEXT, (WPARAM)0, (LPARAM)"FOA");
    SendMessage(GetDlgItem(_hWnd, IDC_STATIC_DST), WM_SETTEXT, (WPARAM)0, (LPARAM)"RVA");
  }
}

static VOID EnableSignControl(BOOL bEnable/* = TRUE*/)
{
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER1), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER2), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER3), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_SOFT_NAME), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_AUTHOR), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_EMAIL), EM_SETREADONLY, (WPARAM)(!bEnable), (LPARAM)0);
  EnableWindow(GetDlgItem(_hWnd, IDC_BTN_SOFT_SIGN), bEnable);
}

static BOOL UseDefaultSign(VOID)
{
  SYSTEMTIME stSysTime;
  TCHAR strMsg[TEMP_BUFF_SIZE];

  //SoftName:
  //SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_SOFT_NAME), WM_SETTEXT, 0, (LPARAM)(AfxGetAppName()));
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_SOFT_NAME), WM_SETTEXT, (WPARAM)0, (LPARAM)(TEXT("PeToolC")));

  //Author:
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_AUTHOR), WM_SETTEXT, (WPARAM)0, (LPARAM)(TEXT("曾现奎")));

  //Version
  ZeroMemory(&stSysTime, sizeof(SYSTEMTIME));
  GetSystemTime(&stSysTime);

  //Version1:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%04d", stSysTime.wYear);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER1), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //Version2:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%02d", stSysTime.wMonth);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER2), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //Version3:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%02d", stSysTime.wDay);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER3), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //Email:
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_EMAIL), WM_SETTEXT, (WPARAM)0, (LPARAM)(TEXT("zengxiankui@qq.com")));
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LPBYTE strFileBase = NULL;
  WORD i = 0, wCount = 0;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_SECTION_HEADER aSections = NULL;
  PIMAGE_SECTION_HEADER lpSection = NULL;
  LPBYTE strSignBase = NULL, pos = NULL;
  LPBYTE strBackUp = NULL;
  struct SSoftSign* lpSign = NULL;
  struct SString* lpStr = NULL;
  TCHAR strMsg[TEMP_BUFF_SIZE];

  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "读取签名时,探测到PE文件可能已经失效", "错误信息", MB_OK | MB_ICONERROR);
    UseDefaultSign();
    return FALSE;
  }
  
  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取签名时,探测到文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    UseDefaultSign();
    return FALSE;
  }

  //取得PE文件对象;
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取签名时,找不到PE结构", "错误信息", MB_OK | MB_ICONERROR);
    UseDefaultSign();
    return FALSE;
  }
  
  if((NULL == lpFmtPe->lpBlkTbl) || (NULL == lpFmtPe->lpFilHdr))
  {
    MessageBox(_hWnd, "读取签名时,节表无效", "错误信息", MB_OK | MB_ICONERROR);
    UseDefaultSign();
    return FALSE;
  }

  //Enable the sign control
  EnableSignControl(TRUE);
  
  //查找可执行的代码段,并返回代码段的起始地址(FOA:PointerToRawData);
  aSections = lpFmtPe->lpBlkTbl;
  wCount = (WORD)(lpFmtPe->lpFilHdr->NumberOfSections);
  for(i = 0; i < wCount; i++)
  {
    //get section data
    lpSection = NULL;
    lpSection = aSections + i;
    
    //该节具有可读(R)、可执行(E)、包含代码(C)的属性,则该节就是代码段(.text/CODE);
    if((lpSection->Characteristics & IMAGE_SCN_MEM_READ) && (lpSection->Characteristics & IMAGE_SCN_MEM_EXECUTE) && (lpSection->Characteristics & IMAGE_SCN_CNT_CODE))
    {
      break;
    }
  }
  
  if(i == wCount)
  {
    MessageBox(_hWnd, "读取签名信息时,找不到存放签名信息的地方", "错误信息", MB_OK | MB_ICONWARNING);
    UseDefaultSign();
    return FALSE;
  }
  
  if(NULL == lpSection)
  {
    MessageBox(_hWnd, "读取签名时,找到的位置是无效的", "错误信息", MB_OK | MB_ICONWARNING);
    UseDefaultSign();
    return FALSE;
  }
  
  //找到符合条件的代码段之后,取得该段在磁盘文件中的起始地址;
  strSignBase = strFileBase + lpSection->PointerToRawData + lpSection->Misc.VirtualSize + SKIP_LENGTH;
  pos = strSignBase;
  
  lpSign = (struct SSoftSign*)pos;
  if(0 == lpSign->bLength)
  {
    MessageBox(_hWnd, "没有找到签名信息,该PE文件有可能没有签名", "警告", MB_OK | MB_ICONWARNING);
    UseDefaultSign();
    return FALSE;
  }

  strBackUp = NULL;
  strBackUp = (LPBYTE)malloc(lpSign->bLength);
  if(NULL == strBackUp)
  {
    MessageBox(_hWnd, "解析软件签名时,分配内存失败", "错误信息", MB_OK | MB_ICONERROR);
    UseDefaultSign();
    return FALSE;
  }

  ZeroMemory(strBackUp, lpSign->bLength);
  CopyMemory(strBackUp, pos, lpSign->bLength);
  g_lpPeFile->Decrpty(strBackUp + sizeof(lpSign->bLength), (WORD)(lpSign->bLength - sizeof(lpSign->bLength)), KEY_ENCRPTY);
  pos = strBackUp;

  //Version;
  lpSign = (struct SSoftSign*)pos;
  //if((lpSign->wVersion1 == 0) && (lpSign->wVersion2 == 0) && (lpSign->wVersion3 == 0))
  //{
  //  free(strBackUp);
  //  MessageBox(_hWnd, "没有找到签名信息,该PE文件有可能没有签名", "警告", MB_OK | MB_ICONWARNING);
  //  UseDefaultSign();
  //  return FALSE;
  //}

  //Version1:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%04d", lpSign->wVersion1);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER1), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //Version2:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%02d", lpSign->bVersion2);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER2), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //Version3:
  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%02d", lpSign->bVersion3);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER3), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  pos += sizeof(struct SSoftSign);
  
  //SoftName:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_SOFT_NAME), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //Author:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_AUTHOR), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //Email:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_EMAIL), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //free the buffer
  free(strBackUp);

  //Disable the sign control
  EnableSignControl(FALSE);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

static VOID OnCheckAddressConvert() 
{
	UpdateCheckBox();
}

static VOID OnBtnAddressConvert() 
{
  LRESULT lResult = 0;
  int iState = -1;
  TCHAR strMsg[TEMP_BUFF_SIZE];
  DWORD dwAddr = 0;

  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "PE文件参数无效", "ERROR", MB_OK|MB_ICONERROR);
    return;
  }
  
  ZeroMemory(strMsg, sizeof(strMsg));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_ADDR_SRC), WM_GETTEXT, (WPARAM)sizeof(strMsg), (LPARAM)strMsg);
  if(8 != lResult)
  {
    MessageBox(_hWnd, "源地址必须是8个字符长的十六进制数", "Prompt", MB_OK|MB_ICONINFORMATION);
    return;
  }
  
  sscanf(strMsg, "%08X", &dwAddr);
  
  iState = SendMessage(GetDlgItem(_hWnd, IDC_CHECK_ADDRESS_CONVERT), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
  if(BST_UNCHECKED == iState)     //RVA to FOA
  {
    dwAddr = g_lpPeFile->Rva2Foa(dwAddr);
  }
  else if(BST_CHECKED == iState)  //FOA to RVA
  {
    dwAddr = g_lpPeFile->Foa2Rva(dwAddr);
  }
  else
  {
    dwAddr = 0;
  }

  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", dwAddr);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_ADDR_DST), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
}

static VOID OnBtnSearchImport() 
{
	LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_IMPORT_DESCRIPTOR lpIID = NULL;
  LPDWORD dwaINT = NULL, dwaIAT = NULL;
  DWORD dwRva = 0, dwFoa = 0;
  DWORD dwThkINT = 0, dwThkIAT = 0;
  PIMAGE_IMPORT_BY_NAME lpByName = NULL;
  WORD i = 0, wOrdinal = 0;
  LPCSTR strDllName = NULL;
  LRESULT lResult = 0;
  BOOL bFound = FALSE;
  CHAR strKey[TEMP_BUFF_SIZE];
  CHAR strMsg[TEMP_BUFF_SIZE+128];

  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_IMP_KEY), WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
  if(((sizeof(strKey) - 1) < lResult) || (0 >= lResult))
  {
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "没有数据查找依据,或者输入的信息太长,最大信息长度是%d", (sizeof(strKey) - 1));
    MessageBox(_hWnd, strMsg, "提示", MB_OK | MB_ICONERROR);
    return;
  }

  ZeroMemory(strKey, sizeof(strKey));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_IMP_KEY), WM_GETTEXT, (WPARAM)sizeof(strKey), (LPARAM)strKey);

  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "PE文件可能没有被打开", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "查询时探测到文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "查询时探测到打开的文件无效,可能没有PE文件被打开", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  if((NULL == lpFmtPe->lpImpBlk) || (NULL == lpFmtPe->lpImpTbl))
  {
    MessageBox(_hWnd, "查询时探测到打开的PE文件格式无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }

  if(0 == lpFmtPe->lpImpBlk->Size)
  {
    MessageBox(_hWnd, "查询时探测到该PE文件没有导入表", "错误信息", MB_OK | MB_ICONWARNING);
    return;
  }

  lpIID = lpFmtPe->lpImpTbl;

  //begin to search
  bFound = FALSE;
  for(; lpIID->Name; lpIID++)
  {
    //get dll name
    dwFoa = g_lpPeFile->Rva2Foa(lpIID->Name);
    strDllName = (LPCSTR)(strFileBase + dwFoa);
    
    //start address of the Original First Thunk table,named Import Name Table(INT);
    dwFoa = g_lpPeFile->Rva2Foa(lpIID->OriginalFirstThunk);
    dwaINT = (LPDWORD)(strFileBase + dwFoa);
    
    //start address of the First Thunk table,named Import Address Table(IAT);
    dwFoa = g_lpPeFile->Rva2Foa(lpIID->FirstThunk);
    dwaIAT = (LPDWORD)(strFileBase + dwFoa);
    
    //Iterate the Import Name Table(INT) and Import Address Table(IAT);
    for(i = 0; (dwThkINT = dwaINT[i]) && (dwThkIAT = dwaIAT[i]); i++)
    {
      //Import By Ordinal,no operation
      if(IMAGE_SNAP_BY_ORDINAL32(dwThkINT))
      {
        wOrdinal = (WORD)IMAGE_ORDINAL32(dwThkINT);
        continue;
      }

      //Import By Name
      dwFoa = g_lpPeFile->Rva2Foa(dwThkINT);
      lpByName = (PIMAGE_IMPORT_BY_NAME)(strFileBase + dwFoa);
      if((0 == StrCmpNI((LPCTSTR)(lpByName->Name), (LPCTSTR)strKey, lResult)) || StrStrI((LPCTSTR)(lpByName->Name), (LPCTSTR)strKey))
      {
        bFound = TRUE; //Found the function
        ZeroMemory(strMsg, sizeof(strMsg));
        _snprintf(strMsg, sizeof(strMsg), "%s: %s\r\n%-4s: %s\r\n%-4s: %08X\r\n%-4s: %08X\r\n%-4s: %08X\r\n%-4s: %04X",
                                          "Func", lpByName->Name,
                                          "DLL", strDllName,
                                          "RVA", dwThkINT,
                                          "FOA", dwFoa,
                                          "IAT", dwThkIAT,
                                          "Hint", lpByName->Hint);
        SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_IMP_RESULT), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
        break;
      }
    }
    
    if(TRUE == bFound)
    {
      break;
    }
  }
  
  if(FALSE == bFound)
  {
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "'%s' is not found!!!", strKey);
    SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_IMP_RESULT), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  }
}

static VOID OnBtnSearchExport() 
{
  LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_EXPORT_DIRECTORY lpIED = NULL;
  LPDWORD adwRvaAddrs = NULL;
  LPDWORD adwRvaNames = NULL;
  LPWORD  awOrdinals = NULL;
  WORD i = 0, wOrdinal = 0;
  DWORD dwFoa = 0, dwAddr = 0;
  LRESULT lResult = 0;
  BOOL bFound = FALSE;
  LPCSTR strFuncName = NULL;
  CHAR strKey[TEMP_BUFF_SIZE];
  CHAR strMsg[TEMP_BUFF_SIZE+128];
  
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_EXP_KEY), WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
  if(((sizeof(strKey) - 1) < lResult) || (0 >= lResult))
  {
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "没有数据查找依据,或者输入的信息太长,最大信息长度是%d", (sizeof(strKey) - 1));
    MessageBox(_hWnd, strMsg, "提示", MB_OK | MB_ICONERROR);
    return;
  }
  
  ZeroMemory(strKey, sizeof(strKey));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_EXP_KEY), WM_GETTEXT, (WPARAM)sizeof(strKey), (LPARAM)strKey);
  
  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "PE文件可能没有被打开", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "查询时探测到文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "查询时探测到打开的文件无效,可能没有PE文件被打开", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  if((NULL == lpFmtPe->lpExpBlk) || (NULL == lpFmtPe->lpExpTbl))
  {
    MessageBox(_hWnd, "查询时探测到打开的PE文件格式无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  if(0 == lpFmtPe->lpExpBlk->Size)
  {
    MessageBox(_hWnd, "查询时探测到该PE文件没有导出表", "错误信息", MB_OK | MB_ICONWARNING);
    return;
  }
  
  lpIED = lpFmtPe->lpExpTbl;
  
  //函数地址表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfFunctions);
  adwRvaAddrs = (LPDWORD)(strFileBase + dwFoa); //RVA Array,so,adwRvaAddrs[i] is a RVA;
  
  //函数名称表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfNames);
  adwRvaNames = (LPDWORD)(strFileBase + dwFoa); //RVA Array,so,adwRvaNames[i] is a RVA;
  
  //函数序数表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfNameOrdinals);
  awOrdinals = (LPWORD)(strFileBase + dwFoa);
  
  //begin to search
  bFound = FALSE;
  for(i = 0; i < lpIED->NumberOfNames; i++)
  {
    //Name:
    dwFoa = g_lpPeFile->Rva2Foa(adwRvaNames[i]);
    strFuncName = (LPCSTR)strFileBase + dwFoa;
    if((0 == StrCmpNI((LPCTSTR)strFuncName, (LPCTSTR)strKey, lResult)) || StrStrI((LPCTSTR)strFuncName, (LPCTSTR)strKey))
    {
      bFound = TRUE; //Found the function

      //Ordinal:
      wOrdinal = awOrdinals[i];
      
      //Address:
      dwAddr = adwRvaAddrs[wOrdinal];

      ZeroMemory(strMsg, sizeof(strMsg));
      _snprintf(strMsg, sizeof(strMsg), "%s: %s\r\n%-4s: %04X\r\n%-4s: %04X\r\n%-4s: %08X",
                                        "Func", strFuncName,
                                        "Ordi", wOrdinal + lpIED->Base,
                                        "Hint", wOrdinal,
                                        "Addr", dwAddr);
      SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_EXP_RESULT), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
      break;
    }
  }

  if(FALSE == bFound)
  {
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "'%s' is not found!!!", strKey);
    SendMessage(GetDlgItem(_hWnd, IDC_EDT_SRCH_EXP_RESULT), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  }
}

static VOID OnBtnSoftSign() 
{
  LPBYTE strFileBase = NULL;
  WORD i = 0, wCount = 0;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_SECTION_HEADER aSections = NULL;
  PIMAGE_SECTION_HEADER lpSection = NULL;
  LPBYTE strSignBase = NULL, pos = NULL;
	struct SSoftSign stSign;
  struct SSoftSign* lpSign = NULL;
  struct SString stSoftName;
  struct SString stAuthor;
  struct SString stEmail;
  struct SString* lpStr = NULL;
  DWORD dwSignLength = 0;
  LRESULT lResult = 0;
  TCHAR strBuf[TEMP_BUFF_SIZE];

  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "签名时,探测PE文件可能已经失效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "签名时,探测到文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }

  ZeroMemory(&stSign, sizeof(struct SSoftSign));
  ZeroMemory(&stSoftName, sizeof(struct SString));
  ZeroMemory(&stAuthor, sizeof(struct SString));
  ZeroMemory(&stEmail, sizeof(struct SString));

  //Version1:
  ZeroMemory(strBuf, sizeof(strBuf));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER1), WM_GETTEXT, (WPARAM)sizeof(strBuf), (LPARAM)strBuf);
  stSign.wVersion1 = StrToInt(strBuf);

  //Version2:
  ZeroMemory(strBuf, sizeof(strBuf));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER2), WM_GETTEXT, (WPARAM)sizeof(strBuf), (LPARAM)strBuf);
  stSign.bVersion2 = StrToInt(strBuf);

  //Version3:
  ZeroMemory(strBuf, sizeof(strBuf));
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_VER3), WM_GETTEXT, (WPARAM)sizeof(strBuf), (LPARAM)strBuf);
  stSign.bVersion3 = StrToInt(strBuf);

  //SoftName:
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_SOFT_NAME), WM_GETTEXT, (WPARAM)sizeof(stSoftName.strBuffer), (LPARAM)(stSoftName.strBuffer));
  stSoftName.bLength = (BYTE)lResult;

  //Author:
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_AUTHOR), WM_GETTEXT, (WPARAM)sizeof(stAuthor.strBuffer), (LPARAM)(stAuthor.strBuffer));
  stAuthor.bLength = (BYTE)lResult;

  //Email:
  lResult = SendMessage(GetDlgItem(_hWnd, IDC_EDT_SIGN_EMAIL), WM_GETTEXT, (WPARAM)sizeof(stEmail.strBuffer), (LPARAM)(stEmail.strBuffer));
  stEmail.bLength = (BYTE)lResult;

  //计算签名的总长度dwSignLength:
  dwSignLength = sizeof(struct SSoftSign);
  dwSignLength += (sizeof(stSoftName.bLength) + stSoftName.bLength); //SoftName的长度;
  dwSignLength += (sizeof(stAuthor.bLength) + stAuthor.bLength);     //Author的长度;
  dwSignLength += (sizeof(stEmail.bLength) + stEmail.bLength);       //EMail的长度;
  dwSignLength += SKIP_LENGTH; //距离代码段尾部SKIP_LENGTH个字节的位置处开始存放签名信息;
  
  //取得PE文件对象;
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "签名时,找不到PE结构", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  if((NULL == lpFmtPe->lpBlkTbl) || (NULL == lpFmtPe->lpFilHdr))
  {
    MessageBox(_hWnd, "签名时,节表无效", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  //根据签名的总长度,查找可执行的代码段,并返回代码段的起始地址(FOA:PointerToRawData);
  aSections = lpFmtPe->lpBlkTbl;
  wCount = (WORD)(lpFmtPe->lpFilHdr->NumberOfSections);
  for(i = 0; i < wCount; i++)
  {
    //get section data
    lpSection = NULL;
    lpSection = aSections + i;
    
    //该节具有可读(R)、可执行(E)、包含代码(C)的属性,则该节就是代码段(.text/CODE);如果该节后面的剩余空间长度大于签名信息总长度,则该节适合存放签名信息;
    if((lpSection->Characteristics & IMAGE_SCN_MEM_READ) && (lpSection->Characteristics & IMAGE_SCN_MEM_EXECUTE) && (lpSection->Characteristics & IMAGE_SCN_CNT_CODE) && ((lpSection->SizeOfRawData - lpSection->Misc.VirtualSize) > dwSignLength))
    {
      break;
    }
  }

  if(i == wCount)
  {
    MessageBox(_hWnd, "签名信息可能太长,找不到合适的地方存放签名信息", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }

  if(NULL == lpSection)
  {
    MessageBox(_hWnd, "签名时,找到的位置是无效的", "错误信息", MB_OK | MB_ICONERROR);
    return;
  }
  
  //Enable the sign control
  EnableSignControl(TRUE);

  //找到符合条件的代码段之后,取得该段在磁盘文件中的起始地址;
  strSignBase = strFileBase + lpSection->PointerToRawData + lpSection->Misc.VirtualSize + SKIP_LENGTH;
  pos = strSignBase;
  
  //写入签名信息;
  //Version;
  lpSign = (struct SSoftSign*)pos;
  lpSign->bLength = (BYTE)(dwSignLength - SKIP_LENGTH);
  lpSign->wVersion1 = stSign.wVersion1;
  lpSign->bVersion2 = stSign.bVersion2;
  lpSign->bVersion3 = stSign.bVersion3;
  pos += sizeof(struct SSoftSign);

  //SoftName:
  lpStr = (struct SString*)pos;
  lpStr->bLength = stSoftName.bLength;
  CopyMemory(lpStr->strBuffer, stSoftName.strBuffer, stSoftName.bLength);
  //lpSign->dwSoftName = pos - strFileBase - lpSection->PointerToRawData;
  pos += (sizeof(stSoftName.bLength) + stSoftName.bLength);

  //Author:
  lpStr = (struct SString*)pos;
  lpStr->bLength = stAuthor.bLength;
  CopyMemory(lpStr->strBuffer, stAuthor.strBuffer, stAuthor.bLength);
  //lpSign->dwAuthor = pos - strFileBase - lpSection->PointerToRawData;
  pos += (sizeof(stAuthor.bLength) + stAuthor.bLength);

  //Email:
  lpStr = (struct SString*)pos;
  lpStr->bLength = stEmail.bLength;
  CopyMemory(lpStr->strBuffer, stEmail.strBuffer, stEmail.bLength);
  //lpSign->dwEmail = pos - strFileBase - lpSection->PointerToRawData;
  pos += (sizeof(stEmail.bLength) + stEmail.bLength);
  
  //Encrpty Sign
  g_lpPeFile->Encrpty(strSignBase + sizeof(lpSign->bLength), (WORD)(lpSign->bLength - sizeof(lpSign->bLength)), KEY_ENCRPTY);
  
  //Disable the sign control
  EnableSignControl(FALSE);

  //通知第1个页面更新版权信息;
  if(g_aTabPages[TAB_PE_FILE].hTabWnd)
  {
    PostMessage(g_aTabPages[TAB_PE_FILE].hTabWnd, UDM_CR_UPDATE, (WPARAM)0, (LPARAM)0);
  }

  MessageBox(_hWnd, "Signed OK", "Tip", MB_OK | MB_ICONINFORMATION);
}

static VOID OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT uiID = 0;
  
  uiID = LOWORD(wParam);
  switch(uiID)
  {
    case IDC_CHECK_ADDRESS_CONVERT:
    {
      OnCheckAddressConvert();
    }
    break;
    
    case IDC_BTN_ADDRESS_CONVERT:
    {
      OnBtnAddressConvert();
    }
    break;
    
    case IDC_BTN_SEARCH_IMPORT:
    {
      OnBtnSearchImport();
    }
    break;
    
    case IDC_BTN_SEARCH_EXPORT:
    {
      OnBtnSearchExport();
    }
    break;
    
    case IDC_BTN_SOFT_SIGN:
    {
      OnBtnSoftSign();
    }
    break;
    
    default:
    {}
  }
}

//Window Procedure
INT_PTR CALLBACK UsrOprProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  UINT uiID = 0;

  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      SendMessage(GetDlgItem(_hWnd, IDC_CHECK_ADDRESS_CONVERT), BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
      UpdateCheckBox();
      result = (INT_PTR)TRUE;
    }
    break;
    
    case UDM_FILLFORM:
    {
      OnFillForm(wParam, lParam);
      result = (INT_PTR)TRUE;
    }
    break;
    
    /*
    case UDM_PARSECOMMANDLINE:
    {
      OnParseCommandLine(wParam, lParam);
      result = (INT_PTR)TRUE;
      SetWindowLong(hWnd, DWL_MSGRESULT, result); //设置消息处理结果的返回值为result,这个返回值会覆盖并作为SendMessage()的返回值被返回给消息的发送者;
    }
    break;

    case UDM_CR_UPDATE:
    {
      OnUpdateCopyRight(wParam, lParam);
      result = (INT_PTR)TRUE;
      SetWindowLong(hWnd, DWL_MSGRESULT, result);
    }
    break;
    */

    case WM_COMMAND:
    {
      OnCommand(wParam, lParam);
      result = (INT_PTR)TRUE;
    }
    break;

    default:
    {
      result = (INT_PTR)FALSE;  
    }
  }
  return result;
}
