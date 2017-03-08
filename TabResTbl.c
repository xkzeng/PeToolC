//TabResTbl.c: Implementation File
//
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <commctrl.h>
#include "Global.h"
#include "PeFile.h"
#include "PeToolC.h"
#include "TabResTbl.h"

#define BYTES_OF_PER_LINE 60
#define EXTRA_BYTES       128
#define NODE_PARAM_VALUE  0xFFFFFFF0
#define NODE_PARAM_EMPTY  0x00000000

static HWND _hWnd = NULL;        //当前页面窗口的句柄;
static HWND _hTreeCtrl = NULL;   //当前页面窗口上面的树型控件句柄;
static HWND _hEdtResData = NULL; //当前页面窗口上面用于显示资源数据的编辑框句柄;
static LPBYTE _strResTbl = NULL;

/////////////////////////////////////////////////////////////////////////////
//Common Function
static BOOL InitTabWnd(HWND hWnd)
{
  _hWnd = hWnd;
  _hTreeCtrl = GetDlgItem(_hWnd, IDC_TREE_RESOURCE_TABLE);
  _hEdtResData = GetDlgItem(_hWnd, IDC_EDT_RES_TBL_DATA);
  _strResTbl = NULL;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Common Function
static VOID CreateTreeView(HTREEITEM hParent, WORD wDepth, LPSTR strLvlStr, WORD wCount)
{
  WORD i = 0;
  TVINSERTSTRUCT tvi;
  HTREEITEM hNode = NULL;
  TCHAR strCurLvlStr[TEMP_BUFF_SIZE];
  TCHAR strNodeText[TEMP_BUFF_SIZE];
  
  ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
  tvi.item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
  tvi.item.cchTextMax = sizeof(strNodeText);

  for(i = 1; i <= wCount; i++)
  {
    ZeroMemory(strCurLvlStr, sizeof(strCurLvlStr));
    wsprintf(strCurLvlStr, "%s%d", strLvlStr, i);
    ZeroMemory(strNodeText, sizeof(strNodeText));
    if(3 <= wDepth) //总共wDepth层,即,树的高度是wDepth层;如果已经达到最大深度wDepth,则第wDepth层是叶子节点;
    {
      wsprintf(strNodeText, "Left%s", strCurLvlStr);
      tvi.item.cChildren = 0;
      tvi.item.pszText = TEXT(strNodeText);
      tvi.hParent = hParent;
      tvi.hInsertAfter = TVI_LAST;
      tvi.item.lParam = (LPARAM)NODE_PARAM_EMPTY;
      hNode = (HTREEITEM)SendMessage(_hTreeCtrl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvi);
    }
    else
    {
      wsprintf(strNodeText, "%s%s", ((1 == wDepth) ? "Root" : "Sub"), strCurLvlStr);
      tvi.item.cChildren = 1;
      tvi.item.pszText = TEXT(strNodeText);
      tvi.hParent = hParent;
      tvi.hInsertAfter = TVI_LAST;
      tvi.item.lParam = (LPARAM)NODE_PARAM_VALUE;
      hNode = (HTREEITEM)SendMessage(_hTreeCtrl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvi);
      CreateTreeView(hNode, (WORD)(wDepth + 1), strCurLvlStr, wCount);
    }
  }
}

static VOID InitTreeView(VOID)
{
  DWORD dwStyle = 0;
  
  //set Style
  dwStyle = GetWindowLong(_hTreeCtrl, GWL_STYLE);
  dwStyle |= TVS_HASBUTTONS;
  dwStyle |= TVS_HASLINES;
  dwStyle |= TVS_LINESATROOT;
  SetWindowLong(_hTreeCtrl, GWL_STYLE, dwStyle);

  CreateTreeView(TVI_ROOT, 1, "", 4);
}

//资源类型ID定义在头文件WinUser.h中;
static LPCTSTR Type2Name(WORD wTypeId)
{
  static LPCTSTR strResType = NULL;
  switch(wTypeId)
  {
  case RT_CURSOR: //Hardware-dependent cursor resource
    strResType = "Cursor";
    break;

  case RT_BITMAP: //Bitmap resource
    strResType = "BitMap";
    break;

  case RT_ICON: //Hardware-dependent icon resource
    strResType = "Icon";
    break;

  case RT_MENU: //Menu resource
    strResType = "Menu";
    break;

  case RT_DIALOG: //Dialog box
    strResType = "Dialog";
    break;

  case RT_STRING: //String-table entry
    strResType = "String";
    break;

  case RT_FONTDIR: //Font directory resource
    strResType = "FontDir";
    break;

  case RT_FONT: //Font resource
    strResType = "Font";
    break;

  case RT_ACCELERATOR: //Accelerator table
    strResType = "Accelerator";
    break;

  case RT_RCDATA: //Application-defined resource (raw data)
    strResType = "RCdata";
    break;

  case RT_MESSAGETABLE: //Message-table entry
    strResType = "MessageTable";
    break;

  case RT_GROUP_CURSOR: //Hardware-independent cursor resource
    strResType = "GroupCursor";
    break;

  case RT_GROUP_ICON: //Hardware-independent icon resource
    strResType = "GroupIcon";
    break;

  case RT_VERSION: //Version resource
    strResType = "Version";
    break;

  case RT_DLGINCLUDE:
    strResType = "DlgInclude";
    break;

  case RT_PLUGPLAY: //Plug and Play resource
    strResType = "PlugPlay";
    break;

  case RT_VXD: //VxD
    strResType = "VxD";
    break;

  case RT_ANICURSOR: //Animated cursor
    strResType = "AniCursor";
    break;

  case RT_ANIICON: //Animated icon
    strResType = "AniIcon";
    break;

  case RT_HTML: //HTML
    strResType = "Html";
    break;

  default:
    strResType = "UserDefined";
  }
  return strResType;
}

static BOOL ParseResourceDirectory(PIMAGE_RESOURCE_DIRECTORY lpResDir, HTREEITEM hParent, WORD wDepth)
{
  PIMAGE_RESOURCE_DIRECTORY lpResSubDir = NULL;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY apResDirEntry = NULL;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY lpResDirEntry = NULL;
  PIMAGE_RESOURCE_DIR_STRING_U lpResName = NULL;
  PIMAGE_RESOURCE_DATA_ENTRY lpResDataEntry = NULL;
  TVINSERTSTRUCT tvi;
  HTREEITEM hCurNode = NULL, hLeaf = NULL;
  WORD wLoop = 0, wNumberOfDirEntries = 0;
  LPCTSTR strResType = NULL;
  TCHAR strNodeText[TEMP_BUFF_SIZE];

  ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
  tvi.item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
  tvi.item.cchTextMax = sizeof(strNodeText);

  //the number of IMAGE_RESOURCE_DIRECTORY_ENTRY
  wNumberOfDirEntries = lpResDir->NumberOfNamedEntries + lpResDir->NumberOfIdEntries;
  apResDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((LPBYTE)lpResDir + sizeof(IMAGE_RESOURCE_DIRECTORY));
  for(wLoop = 0; wLoop < wNumberOfDirEntries; wLoop++)
  {
    lpResDirEntry = apResDirEntry + wLoop;
    
    //Name
    ZeroMemory(strNodeText, sizeof(strNodeText));
    //if(lpResDirEntry->Name & 0x80000000) //bit[31] = 1, Name is a String point to IMAGE_RESOURCE_DIR_STRING_U;
    if(lpResDirEntry->NameIsString)        //this way is OK,too: lpResDirEntry->NameIsString <==> (lpResDirEntry->Name & 0x80000000)
    {
      //lpResName = (PIMAGE_RESOURCE_DIR_STRING_U)(_strResTbl+ (lpResDirEntry->Name & (~0x80000000)));
      lpResName = (PIMAGE_RESOURCE_DIR_STRING_U)(_strResTbl + lpResDirEntry->NameOffset); //this way is OK,too: lpResDirEntry->NameOffset <==> (lpResDirEntry->Name & (~0x80000000))
      wsprintf(strNodeText, "%s", lpResName->NameString);
    }
    else                            //bit[31] = 0, Name is Id;
    {
      if(0 == wDepth)
      {
        strResType = Type2Name(lpResDirEntry->Id);
        wsprintf(strNodeText, "%s", strResType);
      }
      else
      {
        wsprintf(strNodeText, "%d", lpResDirEntry->Id);
      }
    }

    //create a sub node
    tvi.item.cChildren = 1;
    tvi.item.pszText = TEXT(strNodeText);
    tvi.hParent = hParent;
    tvi.hInsertAfter = TVI_LAST;
    tvi.item.lParam = (LPARAM)NODE_PARAM_VALUE;
    hCurNode = (HTREEITEM)SendMessage(_hTreeCtrl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvi);
    
    //OffsetToData
    //if(lpResDirEntry->OffsetToData & 0x80000000) //bit[31] = 1, bit[30:0] is a pointer that point to the address of the next level directory(IMAGE_RESOURCE_DIRECTORY);
    if(lpResDirEntry->DataIsDirectory)             //this way is OK,too: lpResDirEntry->DataIsDirectory <==> (lpResDirEntry->OffsetToData & 0x80000000)
    { 
      //lpResSubDir = (PIMAGE_RESOURCE_DIRECTORY)(_strResTbl+ (lpResDirEntry->OffsetToData & (~0x80000000)));
      lpResSubDir = (PIMAGE_RESOURCE_DIRECTORY)(_strResTbl+ lpResDirEntry->OffsetToDirectory); //this way is OK,too: lpResDirEntry->OffsetToDirectory <==> (lpResDirEntry->OffsetToData & (~0x80000000))
      ParseResourceDirectory(lpResSubDir, hCurNode, (WORD)(wDepth + 1)); //递归调用,解析下一层目录;
    }
    else                                  //bit[31] = 0, bit[30:0] is a pointer that point to IMAGE_RESOURCE_DATA_ENTRY
    {
      lpResDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(_strResTbl+ lpResDirEntry->OffsetToData);
      ZeroMemory(strNodeText, sizeof(strNodeText));
      wsprintf(strNodeText, "%X - %X", lpResDataEntry->OffsetToData, lpResDataEntry->Size);
      //Leaf Node
      tvi.item.cChildren = 0;
      tvi.item.pszText = TEXT(strNodeText);
      tvi.hParent = hCurNode;
      tvi.hInsertAfter = TVI_LAST;
      tvi.item.lParam = (LPARAM)lpResDataEntry; //资源数据块的起始地址可以保存在当前叶子节点hLeaf中或当前叶子节点hLeaf的父节点hCurNode里面;此实现是保存在当前叶子节点hLeaf中;
      hLeaf = (HTREEITEM)SendMessage(_hTreeCtrl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvi);
      //break; //exit the current function
    }
  }
  return TRUE;
}

static BOOL Fill(LPVOID lpParam/* = NULL*/)
{
  LPBYTE strFileBase = NULL;
  PFORMAT_PE lpFmtPe = NULL;
  LPBYTE strResTbl = NULL;
  PIMAGE_RESOURCE_DIRECTORY lpResRootDir = NULL;
  TVINSERTSTRUCT tvi;
  TCHAR strNodeText[TEMP_BUFF_SIZE];
  
  //TreeView_DeleteAllItems();
  SendMessage(_hTreeCtrl, TVM_DELETEITEM, (WPARAM)0, (LPARAM)TVI_ROOT);

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取资源表时,文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  lpFmtPe = (PFORMAT_PE)(&(g_lpPeFile->stFmtPe));
  if(NULL == lpFmtPe)
  {
    MessageBox(_hWnd, "读取资源表时,PE结构无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if((NULL == lpFmtPe->lpResTbl) || (NULL == lpFmtPe->lpResBlk))
  {
    MessageBox(_hWnd, "读取的资源表无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }
  
  if(0 == lpFmtPe->lpResBlk->Size)
  {
    //insert one item;
    ZeroMemory(strNodeText, sizeof(strNodeText));
    wsprintf(strNodeText, "%s", NO_PE_DATA);
    ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
    tvi.item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
    tvi.hParent = TVI_ROOT;
    tvi.hInsertAfter = TVI_LAST;
    tvi.item.cchTextMax = sizeof(strNodeText);
    tvi.item.pszText = TEXT(strNodeText);
    tvi.item.lParam = (LPARAM)NODE_PARAM_EMPTY;
    SendMessage(_hTreeCtrl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvi);
    return FALSE;
  }
  
  lpResRootDir = lpFmtPe->lpResTbl;
  _strResTbl = (LPBYTE)lpResRootDir;

  ParseResourceDirectory(lpResRootDir, TVI_ROOT, 0);
  return TRUE;
}

static BOOL ReadResourceData(PIMAGE_RESOURCE_DATA_ENTRY lpResDataEntry)
{
  LPBYTE strFileBase = NULL, strResDataBase = NULL;
  DWORD dwNumberOfLines = 0, dwTotalBytes = 0;
  LPBYTE pos = NULL, posHead = NULL, posTail = NULL;
  DWORD dwFoa = 0, i = 0, j = 0;
  int iLen = 0, k = -1;
  LPTSTR strData = NULL;

  if(NULL == lpResDataEntry)
  {
    MessageBox(_hWnd, "读取资源数据时,数据参数无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  strFileBase = g_lpPeFile->strBase;
  if(NULL == strFileBase)
  {
    MessageBox(_hWnd, "读取资源数据时,文件基址无效", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  dwNumberOfLines = (lpResDataEntry->Size / 16);
  if(lpResDataEntry->Size % 16)
  {
    dwNumberOfLines += 1; //总行数;
  }

  //总字节数;
  dwTotalBytes = (BYTES_OF_PER_LINE * dwNumberOfLines) + EXTRA_BYTES;
  strData = NULL;
  strData = (LPTSTR)malloc(dwTotalBytes);
  if(NULL == strData)
  {
    MessageBox(_hWnd, "读取资源数据时,分配内存失败", "错误信息", MB_OK | MB_ICONERROR);
    return FALSE;
  }

  dwFoa = g_lpPeFile->Rva2Foa(lpResDataEntry->OffsetToData);
  strResDataBase = strFileBase + dwFoa;
  posHead = strResDataBase;
  posTail = posHead + lpResDataEntry->Size;

  ZeroMemory(strData, dwTotalBytes);

  //format the header;
  i = _snprintf(strData, dwTotalBytes, "%-10s", "FOA");
  //i = wsprintf(strData, "%-10s", "FOA");
  for(j = 0; j < 16; j++)
  {
    iLen = _snprintf(strData + i, dwTotalBytes - i, ((j == 0xF) ? "%02X\r\n" : "%02X "), j);
    //iLen = wsprintf(strData + i, ((j == 0xF) ? "%02X\r\n" : "%02X "), j);
    i += iLen;
  }

  iLen = _snprintf(strData + i, dwTotalBytes - i, "%s", "----------------------------------------------------------\r\n");
  //iLen = wsprintf(strData + i, "%s", "----------------------------------------------------------\r\n");
  i += iLen;

  //format the resource data;
  j = 0;
  for(pos = posHead; pos != posTail; pos++)
  {
    //format one line;
    //FOA:
    if(0 == j) //new line
    {
      k++;
      iLen = _snprintf(strData + i, dwTotalBytes - i, "%08X: ", dwFoa + 16*k);
      //iLen = wsprintf(strData + i, "%08X: ", dwFoa + 16*k);
      i += iLen;
    }

    //byte:
    iLen = _snprintf(strData + i, dwTotalBytes - i, ((j == 0xF) ? "%02X\r\n" : "%02X "), (*pos));
    //iLen = wsprintf(strData + i, ((j == 0xF) ? "%02X\r\n" : "%02X "), (*pos));
    i += iLen;
    
    j++;
    if(16 == j)
    {
      j = 0;
    }
  }
  
  if(' ' == strData[i - 1])
  {
    strData[i - 1] = 0;
  }
  
  SetDlgItemText(_hWnd, IDC_EDT_RES_TBL_DATA, strData);

  if(strData)
  {
    free(strData);
  }
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Message Handlers
static VOID OnFillForm(WPARAM wParam, LPARAM lParam)
{
  Fill(NULL);
}

static VOID OnSelchangedTreeResourceTable(NMHDR* pNMHDR) 
{
	LPNMTREEVIEW pNMTreeView = NULL;
  HTREEITEM hLeaf = NULL;
  TVITEM tvNode;
  BOOL bRet = FALSE, bHasSubNode = FALSE;
  PIMAGE_RESOURCE_DATA_ENTRY lpResDataEntry = NULL;
  
  if(NULL == pNMHDR)
  {
    return;
  }

  pNMTreeView = (LPNMTREEVIEW)pNMHDR;
  
  hLeaf = pNMTreeView->itemNew.hItem;
  if(NULL == hLeaf)
  {
    return;
  }

  ZeroMemory(&tvNode, sizeof(TVITEM));

  //设置GetItem时使用的查询条件;
  tvNode.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
  tvNode.state = TVIS_SELECTED;
  tvNode.hItem = hLeaf;

  //发送查询消息;
  bRet = SendMessage(_hTreeCtrl, TVM_GETITEM, (WPARAM)0, (LPARAM)&tvNode);
  if(FALSE == bRet)
  {
    return; //查询失败;
  }
  
  //如果所选择的节点没有子节点(即,该节点是叶子节点),则读取叶子节点上保存的资源数据的起始地址;
  bHasSubNode = tvNode.cChildren;
  if(TRUE == bHasSubNode)
  {
    return; //选择的不是叶子节点;
  }

  if((NODE_PARAM_VALUE == tvNode.lParam) || (NODE_PARAM_EMPTY == tvNode.lParam))
  {
    return; //叶子节点上保存的资源数据起始地址无效;
  }

  //依据叶子节点上保存的资源数据的起始地址,读取资源数据;
  lpResDataEntry = NULL;
  lpResDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(tvNode.lParam);
  ReadResourceData(lpResDataEntry);
}

static VOID OnNotify(WPARAM wParam, LPARAM lParam)
{
  LPNMHDR lpNMHDR = NULL;
  
  lpNMHDR = (LPNMHDR)lParam;
  if(NULL == lpNMHDR)
  {
    return;
  }
  
  if(TVN_SELCHANGED == lpNMHDR->code)
  {
    OnSelchangedTreeResourceTable(lpNMHDR);
  }
}

//实现应用程序对话框过程;
INT_PTR CALLBACK ResTblProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR result = 0;
  
  switch(uMsg)
  {
    case WM_INITDIALOG:
    {
      InitTabWnd(hWnd);
      InitTreeView();
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
      if(IDC_TREE_RESOURCE_TABLE == wParam)
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