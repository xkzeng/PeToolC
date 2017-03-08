//TabPeFile.c: Implementation file
//
#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include "Global.h"
#include "PeFile.h"
#include "TabPeFile.h"

static HWND _hWnd = NULL; //当前页面窗口的句柄;

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  return TRUE;
}

static PSYSTEMTIME FileTime2SystemTime(PFILETIME lpFileTime)
{
  FILETIME   ft;
  static SYSTEMTIME st;

  FileTimeToLocalFileTime(lpFileTime, &ft);
  FileTimeToSystemTime(&ft, &st);
  return (&st);
}

static WORD FormatFileTime(LPSTR strBuf, WORD wBufSize, PFILETIME lpFileTime)
{
  WORD wLen = 0;
  FILETIME   ft;
  SYSTEMTIME st;

  FileTimeToLocalFileTime(lpFileTime, &ft);
  FileTimeToSystemTime(&ft, &st);

  ZeroMemory(strBuf, wBufSize);
  wLen = _snprintf(strBuf, wBufSize, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
  return wLen;
}

static WORD FormatFileAttr(LPSTR strBuf, WORD wBufSize, DWORD dwFileAttributes)
{
  WORD wLen = 0, wTotalLen = 0;
  LPSTR pos = NULL;

  ZeroMemory(strBuf, wBufSize);
  pos = strBuf;

  if(dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "普通");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "目录");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_OFFLINE)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "离线");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "系统");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "临时");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_READONLY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "只读");
    wTotalLen += wLen;
  }
  
  if(dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "隐藏");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "压缩");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "加密");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s", "归档");
    wTotalLen += wLen;
  }

  if(0 == wTotalLen)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s", "未知");
    wTotalLen += wLen;
    return wTotalLen;
  }

  if(',' == strBuf[wTotalLen - 1])
  {
    wTotalLen--;
    strBuf[wTotalLen] = 0;
  }
  return wTotalLen;
}

static BOOL ParseFeFile(LPTSTR strFileName)
{
  BOOL bRet = FALSE;
  DWORD dwResult = 0;
  ULONGLONG ullFileSize = 0;
  WIN32_FILE_ATTRIBUTE_DATA wfad;
  CHAR strMsg[TEMP_BUFF_SIZE];
  
  if(NULL == g_lpPeFile)
  {
    MessageBox(_hWnd, "没有创建PE文件类对象,或PE文件类对象无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if((NULL == strFileName) || (strlen((LPCSTR)strFileName) < 5))
  {
    MessageBox(_hWnd, "请选择文件", "提示", MB_OK | MB_ICONINFORMATION);
    return FALSE;
  }
  
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_PE_FILE), WM_SETTEXT, 0, (LPARAM)strFileName);
  
  //Get the attribute of the file
  ZeroMemory(&wfad, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
  bRet = GetFileAttributesEx(strFileName, GetFileExInfoStandard, &wfad);
  if(FALSE == bRet)
  {
    MessageBox(_hWnd, "提取文件属性失败", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  //File Size
  ullFileSize = (wfad.nFileSizeHigh << (sizeof(DWORD) * 8)) + wfad.nFileSizeLow;
  ZeroMemory(strMsg, TEMP_BUFF_SIZE);
  _snprintf(strMsg, TEMP_BUFF_SIZE, "%I64u bytes", ullFileSize);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_FILE_SIZE), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Create Time
  FormatFileTime(strMsg, TEMP_BUFF_SIZE, &wfad.ftCreationTime);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_CTIME), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Modified Time
  FormatFileTime(strMsg, TEMP_BUFF_SIZE, &wfad.ftLastWriteTime);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_MTIME), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Access Time
  FormatFileTime(strMsg, TEMP_BUFF_SIZE, &wfad.ftLastAccessTime);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_ATIME), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //All Attributes
  FormatFileAttr(strMsg, TEMP_BUFF_SIZE, wfad.dwFileAttributes);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_ATTRS), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  g_lpPeFile->Close();
  
  bRet = g_lpPeFile->OpenEx(strFileName, &dwResult, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, PAGE_READWRITE, FILE_MAP_READ | FILE_MAP_WRITE);
  if(bRet == FALSE)
  {
    MessageBox(_hWnd, "打开文件失败", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  bRet = g_lpPeFile->Parse(&dwResult);
  if(FALSE == bRet)
  {
    MessageBox(_hWnd, "解析文件失败", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  //通知当前窗口读取版权信息;
  PostMessage(_hWnd, UDM_CR_UPDATE, 0, 0);
  
  //通知父窗口,让父窗口通知其它子窗口,进行填充界面;
  PostMessage(g_hWndMain, UDM_FILLFORM, 0, 0);
  
  //通知父窗口,让其设置其标题栏的文本为当前程序文件名和正在解析的PE文件的路径名;
  SendMessage(g_hWndMain, UDM_SETTITLETEXT, 0, (LPARAM)strFileName);
  return TRUE;
}

static VOID ShowDefaultCopyRight(VOID)
{
  //SoftName:
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_NAME), WM_SETTEXT, 0, (LPARAM)TEXT("PeToolC"));
  
  //Version:
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_VERSION), WM_SETTEXT, 0, (LPARAM)TEXT("2015.10.28 -- Now"));
  
  //Author:
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_AUTHOR), WM_SETTEXT, 0, (LPARAM)TEXT("曾现奎"));
  
  //Email:
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_EMAIL), WM_SETTEXT, 0, (LPARAM)TEXT("zengxiankui@qq.com"));
}

static BOOL ShowCopyRight(VOID)
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
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }
  
  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }

  //取得PE文件对象;
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }
  
  if((NULL == lpFmtPe->lpBlkTbl) || (NULL == lpFmtPe->lpFilHdr))
  {
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }

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
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }
  
  if(NULL == lpSection)
  {
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }
  
  //找到符合条件的代码段之后,取得该段在磁盘文件中的起始地址;
  strSignBase = strFileBase + lpSection->PointerToRawData + lpSection->Misc.VirtualSize + SKIP_LENGTH;
  pos = strSignBase;
  
  lpSign = (struct SSoftSign*)pos;
  if(0 == lpSign->bLength)
  {
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }

  strBackUp = (LPBYTE)malloc(lpSign->bLength);
  if(NULL == strBackUp)
  {
    ShowDefaultCopyRight();
    MessageBox(_hWnd, "failed to alloc memory when show copyright", "ERROR", MB_OK | MB_ICONERROR);
    goto ERROR_READ_COPYRIGHT;
  }

  ZeroMemory(strBackUp, lpSign->bLength);
  CopyMemory(strBackUp, pos, lpSign->bLength);
  g_lpPeFile->Decrpty(strBackUp + sizeof(lpSign->bLength), (WORD)(lpSign->bLength - sizeof(lpSign->bLength)), KEY_ENCRPTY);
  pos = strBackUp;

  //Version;
  lpSign = (struct SSoftSign*)pos;
  if((lpSign->wVersion1 == 0) && (lpSign->bVersion2 == 0) && (lpSign->bVersion3 == 0))
  {
    free(strBackUp);
    ShowDefaultCopyRight();
    goto ERROR_READ_COPYRIGHT;
  }

  ZeroMemory(strMsg, sizeof(strMsg));
  wsprintf(strMsg, "%04d.%02d.%02d", lpSign->wVersion1, lpSign->bVersion2, lpSign->bVersion3);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_VERSION), WM_SETTEXT, 0, (LPARAM)strMsg);
  pos += sizeof(struct SSoftSign);
  
  //SoftName:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_NAME), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Author:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_AUTHOR), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Email:
  lpStr = (struct SString*)pos;
  ZeroMemory(strMsg, sizeof(strMsg));
  CopyMemory(strMsg, lpStr->strBuffer, lpStr->bLength);
  pos += (sizeof(lpStr->bLength) + lpStr->bLength);
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_EMAIL), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  free(strBackUp);
  return TRUE;
  
ERROR_READ_COPYRIGHT:
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Message Handlers
static VOID OnBtnOpenFile() 
{
  // TODO: Add your control notification handler code here
  BOOL bRet = FALSE;
  OPENFILENAME ofn;
  TCHAR strFileName[FILE_PATH_LEN];
  
  ZeroMemory((PVOID)strFileName, (SIZE_T)FILE_PATH_LEN);
  ZeroMemory((PVOID)&ofn, (SIZE_T)sizeof(OPENFILENAME));
  
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.lpstrFilter = TEXT("EXE文件(*.exe)\0*.exe\0DLL文件(*.dll)\0*.dll\0OCX文件(*.ocx)\0*.ocx\0COM文件(*.com)\0*.com\0SYS文件(*.sys)\0*.sys\0DRV文件(*.drv)\0*.drv\0");
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = strFileName;
  ofn.nMaxFile = FILE_PATH_LEN;
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY | OFN_HIDEREADONLY;
  
  bRet = GetOpenFileName(&ofn);
  if(FALSE == bRet)
  {
    return;
  }
  
  bRet = ParseFeFile(strFileName);
  //if(bRet)
  //{
  //  ShowCopyRight();
  //}
  //else
  //{
  //  ShowDefaultCopyRight();
  //}
}

static VOID OnParseCommandLine(WPARAM wParam, LPARAM lParam)
{
  ParseFeFile((LPTSTR)lParam);
}

static VOID OnUpdateCopyRight(WPARAM wParam, LPARAM lParam)
{
  ShowCopyRight();
}

static VOID OnDropFiles(HDROP hDropFile)
{
  UINT iFileNum = 0;
  TCHAR strFileName[FILE_PATH_LEN];
  
  if(NULL == hDropFile)
  {
    return;
  }
  
  iFileNum = DragQueryFile(hDropFile, 0xFFFFFFFF, NULL, 0);
  if(iFileNum < 1)
  {
    goto EXIT_OnDropFiles;
    return;
  }
  
  ZeroMemory(strFileName, sizeof(strFileName));
  iFileNum = DragQueryFile(hDropFile, 0, strFileName, sizeof(strFileName));
  if(iFileNum <= 0)
  {
    goto EXIT_OnDropFiles;
    return;
  }
  
  ParseFeFile(strFileName);

EXIT_OnDropFiles:
  DragFinish(hDropFile);
}

//Window Procedure
INT_PTR CALLBACK PeFileProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  UINT uiID = 0;

  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      DragAcceptFiles(hWnd, TRUE); //enable the file dragging and dropping function;
      result = (INT_PTR)TRUE;
    }
    break;

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

    case WM_COMMAND:
    {
      uiID = LOWORD(wParam);
      if(IDC_BTN_OPEN_FILE == uiID)
      {
        OnBtnOpenFile();
      }
      result = (INT_PTR)TRUE;
    }
    break;
    
    case WM_DROPFILES:
    {
      OnDropFiles((HDROP)wParam);
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
