#pragma once

#ifndef __DEFINE_H__
#define __DEFINE_H__
#include <windows.h>

#define UDM_FILLFORM         WM_USER+1000
#define UDM_SETTITLETEXT     WM_USER+1001
#define UDM_PARSECOMMANDLINE WM_USER+1002
#define UDM_CR_UPDATE        WM_USER+1003

#define PAGE_TITLE_LEN 32
#define TEMP_BUFF_SIZE 32

enum TabId
{
  TAB_PE_FILE = 0,  //Home Page
  TAB_DOS_HDR = 1,  //Dos Header
  TAB_FIL_HDR = 2,  //File Header
  TAB_OPT_HDR = 3,  //Optional Header
  TAB_DAT_DIR = 4,  //Data Directory
  TAB_BLK_TBL = 5,  //Section Table
  TAB_EXP_TBL = 6,  //Export Table
  TAB_IMP_TBL = 7,  //Import Table
  TAB_RLC_TBL = 8,  //Base Relocation Table
  TAB_RES_TBL = 9,  //Resource Table
  TAB_USR_OPR = 10, //User Operation
  NUMBER_OF_TABS
};

typedef BOOL (*CBKINIT)(HWND hWnd);

struct SPePage
{
  UINT    uiIDD;
  HWND    hTabWnd;
  DLGPROC ProcWnd;
  LPSTR   strTitle;
};

struct SListHead
{
  LPTSTR strTitle;
  INT32  iWidth;
  INT32  iAlign;
};

#endif
