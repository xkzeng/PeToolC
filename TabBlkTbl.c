//TabBlkTbl.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabBlkTbl.h"

#define CNT_LIST_COLS 7

static HWND _hWnd = NULL;      //当前页面窗口的句柄;
static HWND _hListCtrl = NULL; //当前页面窗口上面的列表控件句柄;

static struct SListHead _stLvHdr[CNT_LIST_COLS] =
{
  {TEXT("Name"),             60,  LVCFMT_CENTER}, //0
  {TEXT("VirtualAddress"),   96,  LVCFMT_CENTER}, //1
  {TEXT("VirtualSize"),      80,  LVCFMT_CENTER}, //2
  {TEXT("PointerToRawData"), 110, LVCFMT_CENTER}, //3
  {TEXT("SizeOfRawData"),    96,  LVCFMT_CENTER}, //4
  {TEXT("Characteristics"),  102, LVCFMT_CENTER}, //5
  {TEXT("Description"),      78,  LVCFMT_LEFT}    //6
};

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hListCtrl = GetDlgItem(_hWnd, IDC_LIST_SECTION_TABLE);
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LVITEM stRow;
  INT32 iRow = 0;
  WORD i = 0, wCount = 0;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_SECTION_HEADER aSections = NULL;
  PIMAGE_SECTION_HEADER lpSection = NULL;
  TCHAR strMsg[TEMP_BUFF_SIZE];
  
  SendMessage(_hListCtrl, LVM_DELETEALLITEMS, 0, 0);

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取SectionTable时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpBlkTbl) || (NULL == lpFmtPe->lpFilHdr))
  {
    MessageBox(_hWnd, "读取的SectionTable无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;
  
  aSections = lpFmtPe->lpBlkTbl;
  wCount = (WORD)(lpFmtPe->lpFilHdr->NumberOfSections);

  for(i = 0; i < wCount; i++)
  {
    //get data
    lpSection = aSections + i;

    //STEP1: insert new row
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hListCtrl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //STEP2: set the text for every column
    ////Name:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%s", lpSection->Name);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////VirtualAddress:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpSection->VirtualAddress);
    stRow.iSubItem = 1;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////VirtualSize:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpSection->Misc.VirtualSize);
    stRow.iSubItem = 2;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////PointerToRawData:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpSection->PointerToRawData);
    stRow.iSubItem = 3;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////SizeOfRawData:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpSection->SizeOfRawData);
    stRow.iSubItem = 4;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Characteristics:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpSection->Characteristics);
    stRow.iSubItem = 5;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    //Description
    g_lpPeFile->GetSectionProperty(strMsg, sizeof(strMsg), lpSection->Characteristics);
    stRow.iSubItem = 6;
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
INT_PTR CALLBACK BlkTblProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
