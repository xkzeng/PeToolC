//TabImpTbl.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabImpTbl.h"

#define CNT_IMP_TBL_COLS 7
#define CNT_IMP_DTL_COLS 5

static HWND _hWnd = NULL;      //当前页面窗口的句柄;
static HWND _hLvImpTbl = NULL; //当前页面窗口上面的Import Table列表控件句柄;
static HWND _hLvImpDtl = NULL; //当前页面窗口上面的Import Name Table列表控件句柄;
static INT32 _iRowSelectedLast = -10000;
static BOOL _bImpTblIsEmpty = FALSE;

//The header of Import Name Table List View
static struct SListHead _stLvHdrImpTbl[CNT_IMP_TBL_COLS] =
{
  {TEXT("OriginalFirstThunk"), 120, LVCFMT_CENTER}, //0
  {TEXT("TimeDateStamp"),      90,  LVCFMT_CENTER}, //1
  {TEXT("ForwarderChain"),     96,  LVCFMT_CENTER}, //2
  {TEXT("NameRVA"),            60,  LVCFMT_CENTER}, //3
  {TEXT("NameFOA"),            60,  LVCFMT_CENTER}, //4
  {TEXT("Name"),               90,  LVCFMT_LEFT},   //5
  {TEXT("FirstThunk"),         75,  LVCFMT_CENTER}  //6
};

//The header of Import Address Table List View
static struct SListHead _stLvHdrImpDtl[CNT_IMP_DTL_COLS] =
{
  {TEXT("RVA"),      60,  LVCFMT_CENTER}, //0
  {TEXT("FOA"),      60,  LVCFMT_CENTER}, //1
  {TEXT("IAT"),      60,  LVCFMT_CENTER}, //2
  {TEXT("Hint"),     60,  LVCFMT_CENTER}, //3
  {TEXT("Function"), 463, LVCFMT_LEFT}    //4
};

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hLvImpTbl = GetDlgItem(_hWnd, IDC_LIST_IMPORT_TABLE);
  _hLvImpDtl = GetDlgItem(_hWnd, IDC_LIST_IMPORT_DETAIL);
  _iRowSelectedLast = -10000;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LVITEM stRow;
  INT32 iRow = 0;
  WORD i = 0;
  DWORD dwFoa = 0;
  LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_IMPORT_DESCRIPTOR lpIID = NULL;
  TCHAR strMsg[TEMP_BUFF_SIZE];

  SendMessage(_hLvImpTbl, LVM_DELETEALLITEMS, 0, 0);
  SendMessage(_hLvImpDtl, LVM_DELETEALLITEMS, 0, 0);

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取ImportTable时,文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取ImportTable时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpImpTbl) || (NULL == lpFmtPe->lpImpBlk))
  {
    MessageBox(_hWnd, "读取的ImportTable无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;

  if(0 == lpFmtPe->lpImpBlk->Size)
  {
    //insert one row;
    stRow.iItem = 0;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hLvImpTbl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //fill the first column;
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%s", NO_PE_DATA);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
    _bImpTblIsEmpty = TRUE;
    return FALSE;
  }

  lpIID = lpFmtPe->lpImpTbl;

  for(i = 0; lpIID->Name; lpIID++, i++)
  {
    //STEP1: insert new row
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hLvImpTbl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);

    //STEP2: set the text for every column
    ////OriginalFirstThunk:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpIID->OriginalFirstThunk);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////TimeDateStamp:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpIID->TimeDateStamp);
    stRow.iSubItem = 1;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////ForwarderChain:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpIID->ForwarderChain);
    stRow.iSubItem = 2;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////NameRVA:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpIID->Name);
    stRow.iSubItem = 3;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////NameFOA:
    dwFoa = g_lpPeFile->Rva2Foa(lpIID->Name);
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwFoa);
    stRow.iSubItem = 4;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////DllName String:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%s", strFileBase + dwFoa);
    stRow.iSubItem = 5;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////FirstThunk:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", lpIID->FirstThunk);
    stRow.iSubItem = 6;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpTbl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
  }
  _bImpTblIsEmpty = FALSE;
  return TRUE;
}

static BOOL FillDetail(LPNM_LISTVIEW lpLv)
{
  LVITEM stRow, stRowSel;
  LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  DWORD dwRva = 0, dwFoa = 0;
  LPDWORD dwaINT = NULL, dwaIAT = NULL;
  DWORD dwThkINT = 0, dwThkIAT = 0;
  PIMAGE_IMPORT_BY_NAME lpByName = NULL;
  WORD i = 0, wOrdinal = 0;
  INT32 iRow = 0, iLen = 0;
  TCHAR strMsg[TEMP_BUFF_SIZE];
  
  SendMessage(_hLvImpDtl, LVM_DELETEALLITEMS, 0, 0);
  
  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取ImportTable详情时,文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取ImportTable详情时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if((NULL == lpFmtPe->lpImpTbl) || (NULL == lpFmtPe->lpImpBlk))
  {
    MessageBox(_hWnd, "读取的ImportTable详情无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;
  
  //读取导入表视图第0列OriginalFirstThunk的值,并定位Original First Thunk Table,用于填充Import Name Table(INT)列表视图;
  ZeroMemory(strMsg, sizeof(strMsg));
  ZeroMemory(&stRowSel, sizeof(LVITEM));
  stRowSel.iSubItem = 0;
  stRowSel.pszText = strMsg;
  stRowSel.cchTextMax = sizeof(strMsg);
  iLen = SendMessage(_hLvImpTbl, LVM_GETITEMTEXT, (WPARAM)(lpLv->iItem), (LPARAM)&stRowSel);
  iLen = sscanf(strMsg, "%08X", &dwRva);
  dwFoa = g_lpPeFile->Rva2Foa(dwRva);
  dwaINT = (LPDWORD)(strFileBase + dwFoa);
  
  //读取导入表视图第6列FirstThunk的值,并定位First Thunk Table,用于填充Import Address Table(IAT)列表视图;
  ZeroMemory(strMsg, sizeof(strMsg));
  stRowSel.iSubItem = 6;
  iLen = SendMessage(_hLvImpTbl, LVM_GETITEMTEXT, (WPARAM)(lpLv->iItem), (LPARAM)&stRowSel);
  iLen = sscanf(strMsg, "%08X", &dwRva);
  dwFoa = g_lpPeFile->Rva2Foa(dwRva);
  dwaIAT = (LPDWORD)(strFileBase + dwFoa);
  
  //Iterate the Import Name Table(INT) and Import Address Table(IAT),then,Fill the Import Name Table List View and Import Address Table List View;
  for(i = 0; (dwThkINT = dwaINT[i]) && (dwThkIAT = dwaIAT[i]); i++)
  {
    //STEP1: insert new row
    //iRow = this->m_oLvImpINT.InsertItem(i, "11");
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hLvImpDtl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //STEP2: set the text for every column
    ////RVA:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwThkINT);
    //this->m_oLvImpINT.SetItemText(iRow, 0, strMsg);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
    
    if(IMAGE_SNAP_BY_ORDINAL32(dwThkINT)) //Import By Ordinal
    {
      wOrdinal = (WORD)IMAGE_ORDINAL32(dwThkINT);
      
      ////FOA:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%08X", 0);
      //this->m_oLvImpINT.SetItemText(iRow, 1, strMsg);
      stRow.iSubItem = 1;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////IAT:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%08X", dwThkIAT);
      //this->m_oLvImpINT.SetItemText(iRow, 2, strMsg);
      stRow.iSubItem = 2;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////Hint:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%04X", wOrdinal);
      //this->m_oLvImpINT.SetItemText(iRow, 3, strMsg);
      stRow.iSubItem = 3;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////Function:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%s", "by ordinal");
      //this->m_oLvImpINT.SetItemText(iRow, 4, strMsg);
      stRow.iSubItem = 4;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
    }
    else                                  //Import By Name
    {
      ////FOA:
      dwFoa = g_lpPeFile->Rva2Foa(dwThkINT);
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%08X", dwFoa);
      //this->m_oLvImpINT.SetItemText(iRow, 1, strMsg);
      stRow.iSubItem = 1;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////IAT:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%08X", dwThkIAT);
      //this->m_oLvImpINT.SetItemText(iRow, 2, strMsg);
      stRow.iSubItem = 2;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////Hint:
      lpByName = (PIMAGE_IMPORT_BY_NAME)(strFileBase + dwFoa);
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%04X", lpByName->Hint);
      //this->m_oLvImpINT.SetItemText(iRow, 3, strMsg);
      stRow.iSubItem = 3;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
      
      ////Function:
      ZeroMemory(strMsg, sizeof(strMsg));
      wsprintf(strMsg, "%s", lpByName->Name);
      //this->m_oLvImpINT.SetItemText(iRow, 4, strMsg);
      stRow.iSubItem = 4;
      stRow.pszText = strMsg;
      SendMessage(_hLvImpDtl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);
    }
  }
  return TRUE;
}

static VOID OnItemchangedListImportTable(NMHDR* pNMHDR) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(TRUE == _bImpTblIsEmpty)
  {
    return;
  }
	
  if(pNMListView->uChanged & LVIF_STATE)  //状态发生改变;
  {
    //新状态为:有焦点,切选中;
    if((pNMListView->uNewState & LVIS_FOCUSED) && (pNMListView->uNewState & LVIS_SELECTED))
    {
      if((-1 != pNMListView->iItem) && (pNMListView->iItem != _iRowSelectedLast))
      {
        FillDetail(pNMListView);
        _iRowSelectedLast = pNMListView->iItem;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  _iRowSelectedLast = -10000;
  Fill(NULL);
}

static VOID OnNotify(WPARAM wParam, LPARAM lParam)
{
  LPNMHDR lpNMHDR = NULL;
  
  lpNMHDR = (LPNMHDR)lParam;
  if(NULL == lpNMHDR)
  {
    return;
  }
  
  if(LVN_ITEMCHANGED == lpNMHDR->code)
  {
    OnItemchangedListImportTable(lpNMHDR);
  }
}

//实现应用程序对话框过程;
INT_PTR CALLBACK ImpTblProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  
  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      InitListView(_hLvImpTbl, _stLvHdrImpTbl, CNT_IMP_TBL_COLS);
      InitListView(_hLvImpDtl, _stLvHdrImpDtl, CNT_IMP_DTL_COLS);
      result = (INT_PTR)TRUE;
    }
    break;
    
    case UDM_FILLFORM:
    {
      OnFillForm(wParam, lParam);
      result = (INT_PTR)TRUE;
    }
    break;
    
    case WM_NOTIFY:
    {
      if(IDC_LIST_IMPORT_TABLE == wParam)
      {
        OnNotify(wParam, lParam);
        result = (INT_PTR)TRUE;
      }
    }
    break;

    default:
    {
      result = (INT_PTR)FALSE;  
    }
  }
  return result;
}
