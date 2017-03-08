// MainWnd.c: Implement the main window of application;
//
#include "Global.h"
#include <commctrl.h>
#include <stdio.h>
#include "Define.h"
#include "MainWnd.h"
#include "TabPeFile.h"
#include "TabDosHdr.h"
#include "TabFilHdr.h"
#include "TabOptHdr.h"
#include "TabDatDir.h"
#include "TabBlkTbl.h"
#include "TabExpTbl.h"
#include "TabImpTbl.h"
#include "TabRlcTbl.h"
#include "TabResTbl.h"
#include "TabUsrOpr.h"

HWND  g_hWndMain = NULL; //应用程序主窗口的句柄,只有一个;
HICON g_hIcoMain = NULL; //应用程序主窗口的图标句柄,只有一个;
HWND  g_hTabCtrl = NULL; //Tab控件的句柄,只有一个;

struct SPePage g_aTabPages[NUMBER_OF_TABS] = 
{
 //uiIDD,           hTabWnd, ProcWnd,    strTitle
  {IDD_DLG_PE_FILE, NULL,    PeFileProc, TEXT("文  件")},
  {IDD_DLG_DOS_HDR, NULL,    DosHdrProc, TEXT("DOS 头")},
  {IDD_DLG_FIL_HDR, NULL,    FilHdrProc, TEXT("文件头")},
  {IDD_DLG_OPT_HDR, NULL,    OptHdrProc, TEXT("可选头")},
  {IDD_DLG_DAT_DIR, NULL,    DatDirProc, TEXT("数据目录")},
  {IDD_DLG_BLK_TBL, NULL,    BlkTblProc, TEXT("节  表")},
  {IDD_DLG_EXP_TBL, NULL,    ExpTblProc, TEXT("导出表")},
  {IDD_DLG_IMP_TBL, NULL,    ImpTblProc, TEXT("导入表")},
  {IDD_DLG_RLC_TBL, NULL,    RlcTblProc, TEXT("重定位表")},
  {IDD_DLG_RES_TBL, NULL,    ResTblProc, TEXT("资源表")},
  {IDD_DLG_USR_OPR, NULL,    UsrOprProc, TEXT("操  作")}
};

BOOL InitMainWnd(CONST int iCmdShow)
{
  DWORD dwRet = 0;

  //Load the icon of the application main window;
  g_hIcoMain = NULL;
  g_hIcoMain = LoadIcon(g_hAppMain, MAKEINTRESOURCE(IDI_MAIN_WND));
  if(NULL == g_hIcoMain)
  {
    MessageBox(NULL, TEXT("failed to load the icon of the application main window"), "ERROR", MB_OKCANCEL | MB_ICONERROR);
    return FALSE;
  }
  
  //Create the main window from resource template;
  g_hWndMain = NULL;
  g_hWndMain = CreateDialog(g_hAppMain, MAKEINTRESOURCE(IDD_PE_MAIN_WND), GetDesktopWindow(), (DLGPROC)MainWndProc);
  if(NULL == g_hWndMain)
  {
    dwRet = GetLastError();
    MessageBox(NULL, TEXT("failed to create main dialog"), "ERROR", MB_OKCANCEL | MB_ICONERROR);
    return FALSE;
  }

  //显示主对话框窗口,如果在资源编辑器中为对话框设置了WS_VISIBLE属性,那么这里就不必再调用ShowWindow函数了;
  ShowWindow(g_hWndMain, iCmdShow); //显示对话框;

  //此处不必刷新对话框窗口;
  UpdateWindow(g_hWndMain);         //更新对话框;
  return TRUE;
}

VOID FreeMainWnd(VOID)
{
  if(g_hWndMain)
  {
    DestroyWindow(g_hWndMain);
  }
  g_hWndMain = NULL;
}

BOOL InitTabPage(VOID)
{
  WORD i = 0;
  struct SPePage* lpTabPage = NULL;
  TCITEM item;
  DWORD dwSize = 0;
  RECT rs;

  g_hTabCtrl = GetDlgItem(g_hWndMain, IDC_TAB_PETOOL);
  ZeroMemory(&rs, sizeof(RECT));

  GetClientRect(g_hTabCtrl, &rs);
  rs.top += 20;
  rs.bottom -= 2;
  rs.left += 2;
  rs.right -= 2;

  dwSize = sizeof(g_aTabPages);

  //ZeroMemory(g_aTabPages, sizeof(g_aTabPages));
  ZeroMemory(&item, sizeof(TCITEM));
  
  item.mask = TCIF_TEXT | TCIF_PARAM | TCIF_STATE;

  for(i = 0; i < NUMBER_OF_TABS; i++)
  {
    lpTabPage = g_aTabPages + i;
    lpTabPage->hTabWnd = CreateDialog(g_hAppMain, MAKEINTRESOURCE(lpTabPage->uiIDD), g_hTabCtrl, (DLGPROC)(lpTabPage->ProcWnd));
    
    MoveWindow(lpTabPage->hTabWnd, rs.left, rs.top, rs.right - rs.left, rs.bottom - rs.top, TRUE);
    ShowWindow(lpTabPage->hTabWnd, (TAB_PE_FILE == i));

    //item.cchTextMax = 5;
    item.pszText = lpTabPage->strTitle;
    SendMessage(GetDlgItem(g_hWndMain, IDC_TAB_PETOOL), TCM_INSERTITEM, (WPARAM)i, (LPARAM)&item);
  }

  //SendMessage(g_hTabCtrl, TCM_SETCURSEL, (WPARAM)TAB_OPT_HDR, 0);
  //ShowWindow(g_aTabPages[TAB_OPT_HDR].hTabWnd, TRUE);
  return TRUE;
}

VOID FreeTabCtrl(VOID)
{
  WORD i = 0;
  struct SPePage* lpTabPage = NULL;
  HWND lpTabWnd = NULL;
  
  for(i = 0; i < NUMBER_OF_TABS; i++)
  {
    lpTabPage = g_aTabPages + i;
    lpTabWnd = lpTabPage->hTabWnd;
    if(lpTabWnd)
    {
      DestroyWindow(lpTabWnd);
    }
    lpTabPage->hTabWnd = NULL;
  }
}

static BOOL OnTabSelChange(LPNMHDR lpNMHDR)
{
  LONG i = 0;
  struct SPePage* lpPePage = NULL;
  HWND hTabWnd = NULL;
  LRESULT lCurSel = -1;
  
  lCurSel = SendMessage(g_hTabCtrl, TCM_GETCURSEL, 0, 0);
  if(lCurSel < 0) //no tab is selected;
  {
    return FALSE;
  }
  
  for(i = 0; i < NUMBER_OF_TABS; i++)
  {
    lpPePage = g_aTabPages + i;
    hTabWnd = lpPePage->hTabWnd;
    if(hTabWnd)
    {
      ShowWindow(hTabWnd, (i == lCurSel));
    }
  }
  return TRUE;
}

static VOID OnNotify(WPARAM wParam, LPARAM lParam)
{
  LPNMHDR lpNMHDR = NULL;
  
  lpNMHDR = (LPNMHDR)lParam;
  if(NULL == lpNMHDR)
  {
    return;
  }
  
  if(TCN_SELCHANGE == lpNMHDR->code)
  {
    OnTabSelChange(lpNMHDR);
  }
}

static VOID OnSetTitleText(WPARAM wParam, LPARAM lParam)
{
  DWORD i = 0, dwLen = 0;
  LRESULT lLen = 0;
  LPSTR pos = NULL;
  CHAR strTitle[TEMP_BUFF_SIZE+88];
  
  ZeroMemory(strTitle, sizeof(strTitle));
  
  dwLen = GetModuleFileName(NULL, strTitle, sizeof(strTitle));
  if(0 == dwLen)
  {
    ZeroMemory(strTitle, sizeof(strTitle));
    lLen = SendMessage(g_hWndMain, WM_GETTEXT, (WPARAM)sizeof(strTitle), (LPARAM)strTitle);
    dwLen = (DWORD)lLen;
  }
  
  pos = strTitle + dwLen;
  while(('\\' != (*pos)) && (pos != strTitle))
  {
    pos--;
  }
  
  if('\\' == (*pos)) //找到'\'
  {
    //skip '\'
    pos++;
    
    //copy file name
    i = 0;
    for(i = 0; 0 != (*pos); pos++, i++)
    {
      strTitle[i] = *pos;
    }
  }
  else //没有找到'\'
  {
    i = dwLen;
  }
  
  ZeroMemory(strTitle + i, sizeof(strTitle) - i);
  _snprintf(strTitle + i, sizeof(strTitle) - i, " -> %s", (PSTR)lParam);
  SendMessage(g_hWndMain, WM_SETTEXT, 0, (LPARAM)strTitle);
}

//收到通知消息之后,再通知其它子窗口,进行填充界面;
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  WORD i = 0;
  struct SPePage* lpPePage = NULL;
  HWND lpTabWnd = NULL;
  
  for(i = 1; i < NUMBER_OF_TABS; i++)
  {
    lpTabWnd = g_aTabPages[i].hTabWnd;
    if(lpTabWnd)
    {
      PostMessage(lpTabWnd, UDM_FILLFORM, 0, 0);
    }
  }
}

//实现应用程序对话框过程;
INT_PTR CALLBACK MainWndProc(HWND hMainWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;

  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      SendMessage(hMainWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcoMain);
      result = (INT_PTR)TRUE;
    }
    break;

    case WM_NOTIFY:
    {
      if(IDC_TAB_PETOOL == wParam)
      {
        OnNotify(wParam, lParam);
        result = (INT_PTR)TRUE;
      }
    }
    break;

    case UDM_SETTITLETEXT:
    {
      OnSetTitleText(wParam, lParam);
      result = (INT_PTR)TRUE;
    }
    break;

    case UDM_FILLFORM:
    {
      OnFillForm(wParam, lParam);
      result = (INT_PTR)TRUE;
    }
    break;

    case WM_COMMAND:
    {
      if((SC_CLOSE == wParam) || (IDOK == wParam) || (IDCANCEL == wParam))
      {
        PostQuitMessage(0);
      }
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
