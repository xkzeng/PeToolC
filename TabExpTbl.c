//TabExpTbl.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabExpTbl.h"

#define CNT_LIST_COLS 4

static HWND _hWnd = NULL;      //当前页面窗口的句柄;
static HWND _hListCtrl = NULL; //当前页面窗口上面的列表控件句柄;

static struct SListHead _stLvHdr[CNT_LIST_COLS] =
{
  {TEXT("Ordinal"),  60, LVCFMT_CENTER}, //0
  {TEXT("Hint"),     60, LVCFMT_CENTER}, //1
  {TEXT("Function"), 96, LVCFMT_LEFT},   //2
  {TEXT("Address"),  60, LVCFMT_CENTER}  //3
};

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hListCtrl = GetDlgItem(_hWnd, IDC_LIST_EXPORT_TABLE);
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_EXPORT_DIRECTORY lpIED = NULL;
  LPDWORD adwRvaAddrs = NULL;
  LPDWORD adwRvaNames = NULL;
  LPWORD awOrdinals = NULL;
  WORD i = 0, wOrdinal = 0;
  DWORD dwFoa = 0, dwAddr = 0;
  LPCSTR strName = NULL;
  LVITEM stRow;
  INT32 iRow = 0;
  TCHAR strMsg[TEMP_BUFF_SIZE];

  SendMessage(_hListCtrl, LVM_DELETEALLITEMS, 0, 0);

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取ExportTable时,文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取ExportTable时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpExpTbl) || (NULL == lpFmtPe->lpExpBlk))
  {
    MessageBox(_hWnd, "读取的ExportTable无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;
  
  if(0 == lpFmtPe->lpExpBlk->Size)
  {
    //insert one row;
    stRow.iItem = 0;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hListCtrl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //fill the first column;
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%s", NO_PE_DATA);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
    return FALSE;
  }

  lpIED = lpFmtPe->lpExpTbl;

  //Characteristics:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->Characteristics);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_CHARACTERISTICS), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //TimeDateStamp:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->TimeDateStamp);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_TIMEDATESTAMP), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //MajorVersion:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpIED->MajorVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_MAJORVERSION), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //MinorVersion:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpIED->MinorVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_MINORVERSION), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //Name:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->Name);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_NAME), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //Base:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->Base);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_BASE), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //NumberOfFunctions:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->NumberOfFunctions);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_NUMBEROFFUNCTIONS), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //NumberOfNames:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->NumberOfNames);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_NUMBEROFNAMES), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //AddressOfFunctions:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->AddressOfFunctions);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_ADDRESSOFFUNCTIONS), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //AddressOfNames:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->AddressOfNames);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_ADDRESSOFNAMES), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //AddressOfNameOrdinals:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpIED->AddressOfNameOrdinals);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_ADDRESSOFNAMEORDINALS), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //DllName:
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->Name);
  strName = (LPCSTR)strFileBase + dwFoa;
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %s", strName);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_DLLNAME), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //MkTime:
  ZeroMemory(strMsg, sizeof(strMsg));
  strftime(strMsg, sizeof(strMsg), " %Y-%m-%d %H:%M:%S ", localtime((const time_t*)&(lpIED->TimeDateStamp)));
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_EXP_TBL_MKTIME), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //函数地址表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfFunctions);
  adwRvaAddrs = (LPDWORD)(strFileBase + dwFoa); //RVA Array,so,adwRvaAddrs[i] is a RVA;

  //函数名称表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfNames);
  adwRvaNames = (LPDWORD)(strFileBase + dwFoa); //RVA Array,so,adwRvaNames[i] is a RVA;

  //函数序数表
  dwFoa = g_lpPeFile->Rva2Foa(lpIED->AddressOfNameOrdinals);
  awOrdinals = (LPWORD)(strFileBase + dwFoa);
  
  //导出函数列表视图
  for(i = 0; i < lpIED->NumberOfNames; i++)
  {
    //Name:
    dwFoa = g_lpPeFile->Rva2Foa(adwRvaNames[i]);
    strName = (LPCSTR)strFileBase + dwFoa;

    //Ordinal:
    wOrdinal = awOrdinals[i];

    //Address:
    dwAddr = adwRvaAddrs[wOrdinal];

    //STEP1: insert new row
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hListCtrl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //STEP2: set the text for every column
    ////Oridinal:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%04X", wOrdinal + lpIED->Base);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Hint:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%04X", wOrdinal);
    stRow.iSubItem = 1;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Function:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%s", strName);
    stRow.iSubItem = 2;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Address:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwAddr);
    stRow.iSubItem = 3;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
  }
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

//实现应用程序对话框过程;
INT_PTR CALLBACK ExpTblProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  
  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      InitListView(_hListCtrl, _stLvHdr, CNT_LIST_COLS);
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
