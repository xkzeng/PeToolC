// MainWnd.h: Define the main window of application;
//
#pragma once

#ifndef __MAINWND_H__
#define __MAINWND_H__
#include "Global.h"

BOOL InitMainWnd(CONST int iCmdShow);
VOID FreeMainWnd(VOID);
BOOL InitTabPage(VOID);
VOID FreeTabCtrl(VOID);
INT_PTR CALLBACK MainWndProc(HWND hMainWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
