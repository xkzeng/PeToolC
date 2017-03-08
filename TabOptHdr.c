//TabOptHdr.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include "Global.h"
#include "PeFile.h"
#include "TabOptHdr.h"

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
  PIMAGE_OPTIONAL_HEADER lpHdr = NULL;
  CHAR strMsg[TEMP_BUFF_SIZE];
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取OptionalHeader时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if(NULL == lpFmtPe->lpOptHdr)
  {
    MessageBox(_hWnd, "读取的OptionalHeader无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  lpHdr = lpFmtPe->lpOptHdr;
  
  //Magic
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->Magic);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MAGIC), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MajorLinkerVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %02X", lpHdr->MajorLinkerVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MAJORLINKERVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MinorLinkerVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %02X", lpHdr->MinorLinkerVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MINORLINKERVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfCode
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfCode);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFCODE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfInitializedData
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfInitializedData);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFINITIALIZEDDATA), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfUninitializedData
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfUninitializedData);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFUNINITIALIZEDDATA), WM_SETTEXT, 0, (LPARAM)strMsg);

  //AddressOfEntryPoint
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->AddressOfEntryPoint);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_ADDRESSOFENTRYPOINT), WM_SETTEXT, 0, (LPARAM)strMsg);

  //BaseOfCode
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->BaseOfCode);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_BASEOFCODE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //BaseOfData
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->BaseOfData);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_BASEOFDATA), WM_SETTEXT, 0, (LPARAM)strMsg);

  //ImageBase
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->ImageBase);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_IMAGEBASE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SectionAlignment
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SectionAlignment);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SECTIONALIGNMENT), WM_SETTEXT, 0, (LPARAM)strMsg);

  //FileAlignment
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->FileAlignment);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_FILEALIGNMENT), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MajorOperatingSystemVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MajorOperatingSystemVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MAJOROPERATINGSYSTEMVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MinorOperatingSystemVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MinorOperatingSystemVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MINOROPERATINGSYSTEMVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MajorImageVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MajorImageVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MAJORIMAGEVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MinorImageVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MinorImageVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MINORIMAGEVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MajorSubsystemVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MajorSubsystemVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MAJORSUBSYSTEMVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //MinorSubsystemVersion
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->MinorSubsystemVersion);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_MINORSUBSYSTEMVERSION), WM_SETTEXT, 0, (LPARAM)strMsg);

  //Win32VersionValue
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->Win32VersionValue);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_WIN32VERSIONVALUE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfImage
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfImage);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFIMAGE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfHeaders
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfHeaders);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFHEADERS), WM_SETTEXT, 0, (LPARAM)strMsg);

  //CheckSum
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->CheckSum);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_CHECKSUM), WM_SETTEXT, 0, (LPARAM)strMsg);

  //Subsystem
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X -> %s", lpHdr->Subsystem, g_lpPeFile->GetSubsystemString(lpHdr->Subsystem, lpHdr->Magic));
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SUBSYSTEM), WM_SETTEXT, 0, (LPARAM)strMsg);

  //DllCharacteristics
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %04X", lpHdr->DllCharacteristics);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_DLLCHARACTERISTICS), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfStackReserve
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfStackReserve);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFSTACKRESERVE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfStackCommit
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfStackCommit);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFSTACKCOMMIT), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfHeapReserve
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfHeapReserve);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFHEAPRESERVE), WM_SETTEXT, 0, (LPARAM)strMsg);

  //SizeOfHeapCommit
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->SizeOfHeapCommit);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_SIZEOFHEAPCOMMIT), WM_SETTEXT, 0, (LPARAM)strMsg);

  //LoaderFlags
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->LoaderFlags);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_LOADERFLAGS), WM_SETTEXT, 0, (LPARAM)strMsg);

  //NumberOfRvaAndSizes
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %08X", lpHdr->NumberOfRvaAndSizes);
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_NUMBEROFRVAANDSIZES), WM_SETTEXT, 0, (LPARAM)strMsg);

  //DataDirectory
  ZeroMemory(strMsg, sizeof(strMsg));
  _snprintf(strMsg, sizeof(strMsg), " %s", "NEXT");
  SendMessage(GetDlgItem(_hWnd, IDC_EDT_OPT_DATA_DIR), WM_SETTEXT, 0, (LPARAM)strMsg);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

//实现应用程序对话框过程;
INT_PTR CALLBACK OptHdrProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
