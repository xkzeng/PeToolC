//TabPeFile.c: Implementation file
//
#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include "Global.h"
#include "PeFile.h"
#include "TabPeFile.h"

static HWND _hWnd = NULL; //��ǰҳ�洰�ڵľ��;

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
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "��ͨ");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "Ŀ¼");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_OFFLINE)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "����");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "ϵͳ");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "��ʱ");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_READONLY)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "ֻ��");
    wTotalLen += wLen;
  }
  
  if(dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "����");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "ѹ��");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s,", "����");
    wTotalLen += wLen;
  }

  if(dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s", "�鵵");
    wTotalLen += wLen;
  }

  if(0 == wTotalLen)
  {
    wLen = _snprintf(pos + wTotalLen, wBufSize - wTotalLen, "%s", "δ֪");
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
    MessageBox(_hWnd, "û�д���PE�ļ������,��PE�ļ��������Ч", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if((NULL == strFileName) || (strlen((LPCSTR)strFileName) < 5))
  {
    MessageBox(_hWnd, "��ѡ���ļ�", "��ʾ", MB_OK | MB_ICONINFORMATION);
    return FALSE;
  }
  
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_PE_FILE), WM_SETTEXT, 0, (LPARAM)strFileName);
  
  //Get the attribute of the file
  ZeroMemory(&wfad, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
  bRet = GetFileAttributesEx(strFileName, GetFileExInfoStandard, &wfad);
  if(FALSE == bRet)
  {
    MessageBox(_hWnd, "��ȡ�ļ�����ʧ��", "������Ϣ", MB_OK | MB_ICONERROR);
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
    MessageBox(_hWnd, "���ļ�ʧ��", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  bRet = g_lpPeFile->Parse(&dwResult);
  if(FALSE == bRet)
  {
    MessageBox(_hWnd, "�����ļ�ʧ��", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  //֪ͨ��ǰ���ڶ�ȡ��Ȩ��Ϣ;
  PostMessage(_hWnd, UDM_CR_UPDATE, 0, 0);
  
  //֪ͨ������,�ø�����֪ͨ�����Ӵ���,����������;
  PostMessage(g_hWndMain, UDM_FILLFORM, 0, 0);
  
  //֪ͨ������,������������������ı�Ϊ��ǰ�����ļ��������ڽ�����PE�ļ���·����;
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
  SendMessage(GetDlgItem(_hWnd, IDC_STATIC_SOFT_AUTHOR), WM_SETTEXT, 0, (LPARAM)TEXT("���ֿ�"));
  
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

  //ȡ��PE�ļ�����;
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

  //���ҿ�ִ�еĴ����,�����ش���ε���ʼ��ַ(FOA:PointerToRawData);
  aSections = lpFmtPe->lpBlkTbl;
  wCount = (WORD)(lpFmtPe->lpFilHdr->NumberOfSections);
  for(i = 0; i < wCount; i++)
  {
    //get section data
    lpSection = NULL;
    lpSection = aSections + i;
    
    //�ýھ��пɶ�(R)����ִ��(E)����������(C)������,��ýھ��Ǵ����(.text/CODE);
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
  
  //�ҵ����������Ĵ����֮��,ȡ�øö��ڴ����ļ��е���ʼ��ַ;
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
  ofn.lpstrFilter = TEXT("EXE�ļ�(*.exe)\0*.exe\0DLL�ļ�(*.dll)\0*.dll\0OCX�ļ�(*.ocx)\0*.ocx\0COM�ļ�(*.com)\0*.com\0SYS�ļ�(*.sys)\0*.sys\0DRV�ļ�(*.drv)\0*.drv\0");
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
      SetWindowLong(hWnd, DWL_MSGRESULT, result); //������Ϣ�������ķ���ֵΪresult,�������ֵ�Ḳ�ǲ���ΪSendMessage()�ķ���ֵ�����ظ���Ϣ�ķ�����;
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
