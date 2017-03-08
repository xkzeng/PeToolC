//TabDosHdr.c: Implementation file
//
#include <windows.h>
#include <stdio.h>
#include "Global.h"
#include "PeFile.h"
#include "TabDosHdr.h"

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
  PIMAGE_DOS_HEADER lpHdr = NULL;
  LPBYTE tmp = NULL;
  LPWORD w = NULL;
  CHAR strMsg[TEMP_BUFF_SIZE+20];
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取DosHeader时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if(NULL == lpFmtPe->lpDosHdr)
  {
    MessageBox(_hWnd, "读取的DosHeader无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  lpHdr = lpFmtPe->lpDosHdr;

  //e_magic
  tmp = (LPBYTE)&(lpHdr->e_magic);
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X -> %c%c", lpHdr->e_magic, tmp[0], tmp[1]);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_MAGIC), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //e_cblp
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_cblp);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CBLP), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_cp
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_cp);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CP), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_crlc
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_crlc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CRLC), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_cparhdr
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_cparhdr);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CPARHDR), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_minalloc
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_minalloc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_MINALLOC), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_maxalloc
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_maxalloc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_MAXALLOC), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_ss
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_ss);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_SS), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_sp
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_sp);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_SP), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_csum
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_csum);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CSUM), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_ip
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_ip);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_IP), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_cs
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_cs);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_CS), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //e_lfarlc
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_lfarlc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_LFARLC), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_ovno
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_ovno);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_OVNO), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_res
  w = lpHdr->e_res;
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X %04X %04X %04X", w[0], w[1], w[2], w[3]);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_RES), WM_SETTEXT, 0, (LPARAM)strMsg);
  
  //e_oemid
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_oemid);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_OEMID), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_oeminfo
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->e_oeminfo);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_OEMINFO), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_res2
  w = lpHdr->e_res2;
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X", w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], w[8], w[9]);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_RES2), WM_SETTEXT, 0, (LPARAM)strMsg);

  //e_lfanew
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->e_lfanew);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_DOS_LFANEW), WM_SETTEXT, 0, (LPARAM)strMsg);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

//实现应用程序对话框过程;
INT_PTR CALLBACK DosHdrProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
