//TabDatDir.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabDatDir.h"

#define CNT_LIST_COLS 4

static HWND _hWnd = NULL;      //当前页面窗口的句柄;
static HWND _hListCtrl = NULL; //当前页面窗口上面的列表控件句柄;

static struct SListHead _stLvHdr[CNT_LIST_COLS] =
{
  {TEXT("Type"),           40,  LVCFMT_CENTER}, //0
  {TEXT("Name"),           190, LVCFMT_LEFT},   //1
  {TEXT("VirtualAddress"), 96,  LVCFMT_CENTER}, //2
  {TEXT("Size"),           60,  LVCFMT_CENTER}  //3
};

static LPCTSTR strDatDirTypeName[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] =
{
  TEXT("Export Directory"),              //0
  TEXT("Import Directory"),              //1
  TEXT("Resource Directory"),            //2
  TEXT("Exception Directory"),           //3
  TEXT("Security Directory"),            //4
  TEXT("Base Relocation Table"),         //5
  TEXT("Debug Directory"),               //6
  TEXT("Architecture Specific Data"),    //7
  TEXT("RVA of GP"),                     //8
  TEXT("TLS Directory"),                 //9
  TEXT("Load Configuration Directory"),  //10
  TEXT("Bound Import Directory"),        //11
  TEXT("Import Address Table"),          //12
  TEXT("Delay Load Import Descriptors"), //13
  TEXT("COM Runtime descriptor"),        //14
  TEXT("UnUsed")                         //15
};

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hListCtrl = GetDlgItem(_hWnd, IDC_LIST_DATA_DIRECTORY);
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LVITEM stRow;
  INT32 iRow = 0;
  WORD i = 0, wDatDirNum = 0;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_DATA_DIRECTORY lpDatDir = NULL;
  PIMAGE_DATA_DIRECTORY lpData = NULL;
  TCHAR strMsg[TEMP_BUFF_SIZE];
  
  SendMessage(_hListCtrl, LVM_DELETEALLITEMS, 0, 0);

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取DataDirectory时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpDatDir) || (NULL == lpFmtPe->lpOptHdr))
  {
    MessageBox(_hWnd, "读取的DataDirectory无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;

  lpDatDir = lpFmtPe->lpDatDir;
  wDatDirNum = (WORD)(lpFmtPe->lpOptHdr->NumberOfRvaAndSizes);

  for(i = 0; i < wDatDirNum; i++)
  {
    //STEP1: insert new row -> i;
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hListCtrl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //STEP2: set the text for every column
    ////Type:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%02d", iRow);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Name:
    stRow.iSubItem = 1;
    stRow.pszText = (LPTSTR)strDatDirTypeName[i];
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    //get data
    lpData = lpDatDir + i;

    ////VirtualAddress:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpData->VirtualAddress);
    stRow.iSubItem = 2;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////Size:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpData->Size);
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
INT_PTR CALLBACK DatDirProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
