//TabRlcTbl.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabRlcTbl.h"

#define CNT_LIST_COLS 5

static HWND _hWnd = NULL;      //��ǰҳ�洰�ڵľ��;
static HWND _hListCtrl = NULL; //��ǰҳ�洰��������б�ؼ����;

static struct SListHead _stLvHdr[CNT_LIST_COLS] =
{
  {TEXT("TypeOffset"),  80, LVCFMT_CENTER}, //0
  {TEXT("CodeOffset"),  80, LVCFMT_CENTER}, //1
  {TEXT("CodeRVA"),     60, LVCFMT_CENTER}, //2
  {TEXT("CodeVA"),      96, LVCFMT_CENTER}, //3
  {TEXT("MachineCode"), 80, LVCFMT_CENTER}  //4
};

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hListCtrl = GetDlgItem(_hWnd, IDC_LIST_BASE_RELOCATION);
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  INT32 iRow = 0;
  WORD i = 0;
  LVITEM stRow;
  LPBYTE strFileBase = NULL;
  LPBYTE strRlcBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  PIMAGE_BASE_RELOCATION lpRlcTbl = NULL;
  LPWORD awTypeOffsets = NULL;
  LPBYTE strMachineCodes = NULL;
  DWORD dwFoa = 0, dwBytesOfRlc = 0, dwNumberOfRlc = 0;
  WORD wTypeOffset = 0, wCodeOffset = 0;
  DWORD dwCodeRVA = 0, dwCodeVA = 0, dwMachineCode = 0;
  TCHAR strMsg[TEMP_BUFF_SIZE];

  SendMessage(_hListCtrl, LVM_DELETEALLITEMS, 0, 0);

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "��ȡ�ض�λ��ʱ,�ļ���ַ��Ч", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "��ȡ�ض�λ��ʱ,PE�ṹ��Ч", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  if((NULL == lpFmtPe->lpRlcTbl) || (NULL == lpFmtPe->lpRlcBlk) || (NULL == lpFmtPe->lpOptHdr))
  {
    MessageBox(_hWnd, "��ȡ���ض�λ����Ч", "������Ϣ", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  ZeroMemory(&stRow, sizeof(LVITEM));
  stRow.mask = LVIF_TEXT;
  stRow.cchTextMax = TEMP_BUFF_SIZE;

  if(0 == lpFmtPe->lpRlcBlk->Size)
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

  lpRlcTbl = lpFmtPe->lpRlcTbl;
  strRlcBase = (LPBYTE)lpRlcTbl;

  //VirtualAddress:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpRlcTbl->VirtualAddress);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_RLC_TBL_VIRTUALADDRESS), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);
  
  //FOA:
  dwFoa = g_lpPeFile->Rva2Foa(lpRlcTbl->VirtualAddress);
  strMachineCodes = (LPBYTE)(strFileBase + dwFoa);
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", dwFoa);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_RLC_TBL_FOA), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //SizeOfBlock:
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpRlcTbl->SizeOfBlock);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_RLC_TBL_SIZEOFBLOCK), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //BytesOfReloc:
  dwBytesOfRlc = lpRlcTbl->SizeOfBlock - 8;
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", dwBytesOfRlc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_RLC_TBL_BYTESOFRELOC), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  //NumberOfReloc:
  dwNumberOfRlc = dwBytesOfRlc / 2;
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", dwNumberOfRlc);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_RLC_TBL_NUMBEROFRELOC), WM_SETTEXT, (WPARAM)0, (LPARAM)strMsg);

  awTypeOffsets = (LPWORD)(strRlcBase + 8);
  for(i = 0; i < dwNumberOfRlc; i++)
  {
    wTypeOffset   = awTypeOffsets[i];
    wCodeOffset   = wTypeOffset & 0x0FFF;                        //��Ҫ�����ض�λ�����������������е�ƫ�Ƶ�ַ;
    dwCodeRVA     = wCodeOffset + lpRlcTbl->VirtualAddress;      //��Ҫ�����ض�λ�����������������е�ƫ�Ƶ�ַ��RVA;
    dwCodeVA      = dwCodeRVA + lpFmtPe->lpOptHdr->ImageBase;    //��Ҫ�����ض�λ�����������������е�ƫ�Ƶ�ַ��VA;
    dwMachineCode = *((LPDWORD)(strMachineCodes + wCodeOffset)); //ֱ��Ѱַָ����,��Ҫ�����ض�λ�ĵ�ַ����(������ָ������);

    //STEP1: insert new row
    stRow.iItem = i;
    stRow.iSubItem = 0;
    iRow = SendMessage(_hListCtrl, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&stRow);
    
    //STEP2: set the text for every column
    ////TypeOffset:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%04X", wTypeOffset);
    stRow.iSubItem = 0;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////CodeOffset:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%04X", wCodeOffset);
    stRow.iSubItem = 1;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////CodeRVA:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwCodeRVA);
    stRow.iSubItem = 2;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    ////CodeVA:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwCodeVA);
    stRow.iSubItem = 3;
    stRow.pszText = strMsg;
    SendMessage(_hListCtrl, LVM_SETITEM, (WPARAM)iRow, (LPARAM)&stRow);

    //MachineCode:
    ZeroMemory(strMsg, sizeof(strMsg));
    wsprintf(strMsg, "%08X", dwMachineCode);
    stRow.iSubItem = 4;
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

//ʵ��Ӧ�ó���Ի������;
INT_PTR CALLBACK RlcTblProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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