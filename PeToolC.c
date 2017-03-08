//PeToolC.c: Defines the class behaviors for the application.
//
#include <windows.h>
#include <commctrl.h>
#include "PeFile.h"
#include "PeToolC.h"
#include "MainWnd.h"

HINSTANCE g_hAppMain = NULL;

BOOL InitInstance(VOID)
{
  BOOL bResult = FALSE;

  AcquirePeFile();                //SEQ:111111
  if(NULL == g_lpPeFile)
  {
    MessageBox(NULL, "Failed to alloc memory for PeFile object", "ERROR", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  //initialize the common controller
  InitCommonControls();

  //initialize the main window
  bResult = InitMainWnd(SW_SHOW); //SEQ:222222
  if(FALSE == bResult)
  {
    return FALSE;
  }

  bResult = InitTabPage();        //SEQ:333333
  if(FALSE == bResult)
  {
    return FALSE;
  }
	return TRUE;
}

BOOL ExitInstance(VOID)
{
  FreeTabCtrl();                  //SEQ:333333
  FreeMainWnd();                  //SEQ:222222
  ReleasePeFile();                //SEQ:111111
  return FALSE;
}

//Win32程序的主函数;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
  static TCHAR strAppName[] = TEXT("PeToolC");
  MSG msg;
  BOOL bRet = FALSE;
  LPSTR strFileName = NULL;
  HWND hWndPeFile = NULL;
  LRESULT lResult = -1;

  //parse the command line parameters; BEGIN
  //......
  
  strFileName = lpCmdLine;
  //parse the command line parameters; END

  g_hAppMain = hInstance;
  
  bRet = InitInstance();
  if(FALSE == bRet)
  {
    MessageBox(g_hWndMain, "应用程序实例初始化失败", "错误", MB_OK | MB_ICONERROR);
    ExitProcess(1);
  }

  if(strFileName && strFileName[0])
  {
    hWndPeFile = g_aTabPages[TAB_PE_FILE].hTabWnd;
    if(hWndPeFile)
    {
      lResult = SendMessage(hWndPeFile, UDM_PARSECOMMANDLINE, 0, (LPARAM)strFileName);
      if(lResult <= 0)
      {
        MessageBox(g_hWndMain, "命令行参数解析失败", "错误", MB_OK | MB_ICONERROR);
        ExitProcess(2);
      }

      lResult = SendMessage(hWndPeFile, UDM_CR_UPDATE, 0, 0);
      if(lResult <= 0)
      {
        MessageBox(g_hWndMain, "更新文件页面上的版权信息失败", "错误", MB_OK | MB_ICONERROR);
        ExitProcess(3);
      }
    }
  }

  //Win32程序处理消息的消息循环;
  while(TRUE)
  {
    ///GetMessage()是同步消息读取,无消息时原地等待;PeekMessage是异步消息读取,无消息时直接返回0;
    ///有消息时,这两个函数都是读取消息并返回;如果要充分利用CPU资源,可考虑使用PeekMessage()函数; 
    bRet = GetMessage(&msg, NULL, 0, 0);

    //GetMessage() = -1, failed;
    if(bRet < 0)
    {
      break;
    }

    //GetMessage() == 0, return message WM_QUIT;
    if(0 == bRet)
    {
      break;
    }

    //GetMessage() != 0, return a message;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  ExitInstance();
  return msg.wParam;
}

VOID InitListView(HWND hList, struct SListHead lstLvHdrs[], CONST WORD wCount)
{
  LONG  lStyle = 0;
  DWORD dwStyle = 0;
  struct SListHead* lpLstHdr = NULL;
  LVCOLUMN stColumn;
  WORD i = 0;

  //set Style
  lStyle = GetWindowLong(hList, GWL_STYLE);
  lStyle &= ~LVS_TYPEMASK;
  lStyle |= LVS_REPORT;
  SetWindowLong(hList, GWL_STYLE, lStyle);

  //set Extended Style
  dwStyle = SendMessage(hList, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
  dwStyle |= LVS_EX_FULLROWSELECT;
  dwStyle |= LVS_EX_GRIDLINES;
  SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)dwStyle);

  //insert the table header
  ZeroMemory(&stColumn, sizeof(LVCOLUMN));
  stColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
  stColumn.iSubItem = 0;

  for(i = 0; i < wCount; i++)
  {
    //column i:
    lpLstHdr = lstLvHdrs + i;
    stColumn.fmt     = lpLstHdr->iAlign;
    stColumn.cx      = lpLstHdr->iWidth;
    stColumn.pszText = lpLstHdr->strTitle;
    SendMessage(hList, LVM_INSERTCOLUMN, (WPARAM)i, (LPARAM)&stColumn);
  }
}