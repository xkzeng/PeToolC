//TabFilHdr.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Global.h"
#include "PeFile.h"
#include "TabFilHdr.h"

static HWND _hWnd = NULL; //当前页面窗口的句柄;

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_NT_HEADERS lpNtHdr = NULL;
  PIMAGE_FILE_HEADER lpHdr = NULL;
  LPBYTE tmp = NULL;
  LPSTR strFileType = NULL;
  CHAR strTime[32];
  CHAR strMsg[TEMP_BUFF_SIZE+2];
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取FileHeader时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpNtHdr) || (NULL == lpFmtPe->lpFilHdr))
  {
    MessageBox(_hWnd, "读取的FileHeader无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  lpNtHdr = lpFmtPe->lpNtHdr;
  lpHdr   = lpFmtPe->lpFilHdr;

  //Signature
  tmp = (LPBYTE)&(lpNtHdr->Signature);
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X -> %c%c%c%c", lpNtHdr->Signature, tmp[0], tmp[1], tmp[2], tmp[3]);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_SIGNATURE), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //Machine
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X -> %s", lpHdr->Machine, g_lpPeFile->GetMachineString(lpHdr->Machine));
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_MACHINE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //NumberOfSections
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->NumberOfSections);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_NUMBEROFSECTION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //TimeDateStamp
  ZeroMemory(strTime, sizeof(strTime));
  strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", localtime((const time_t*)&(lpHdr->TimeDateStamp)));
  
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X -> %s", lpHdr->TimeDateStamp, strTime);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_TIMEDATESTAMP), WM_SETTEXT, 0, (LPARAM)strMsg);

  //PointerToSymbolTable
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->PointerToSymbolTable);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_POINTERTOSYMBOLTABLE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //NumberOfSymbols
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->NumberOfSymbols);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_NUMBEROFSYMBOLS), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfOptionalHeader
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X -> %d", lpHdr->SizeOfOptionalHeader, lpHdr->SizeOfOptionalHeader);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_SIZEOFOPTIONALHEADER), WM_SETTEXT, 0, (LPARAM)strMsg);

  //Characteristics
  ZeroMemory(strMsg, sizeof(strMsg));
  //if(lpHdr->Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
  if(lpHdr->Characteristics & IMAGE_FILE_DLL)
  {
    strFileType = "DLL";
  }
  else if(lpHdr->Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
  {
    strFileType = "EXE";
  }
  else if(lpHdr->Characteristics & IMAGE_FILE_SYSTEM)
  {
    strFileType = "SYS";
  }
  else
  {
    strFileType = "UNK";
  }

  _snprintf(strMsg, sizeof(strMsg), " %04X -> %s", lpHdr->Characteristics, strFileType);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_FIL_CHARACTERISTICS), WM_SETTEXT, 0, (LPARAM)strMsg);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

//实现应用程序对话框过程;
INT_PTR CALLBACK FilHdrProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  
  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      result = (INT_PTR)TRUE;
    }
    break;
    
    case UDM_FILLFORM:
    {
      OnFillForm(wParam, lParam);
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
