//global.h: Define global resource;
#pragma once

#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <windows.h>
#include "Resource.h"
#include "Define.h"

extern HINSTANCE g_hAppMain; //应用程序实例句柄,只有一个;
extern HWND      g_hWndMain; //应用程序主窗口的句柄,只有一个;
extern HICON     g_hIcoMain; //应用程序主窗口的图标句柄,只有一个;
extern HWND      g_hTabCtrl; //Tab控件的句柄,只有一个;
extern struct SPeFile* g_lpPeFile; //PE文件对象,只有一个;
extern struct SPePage  g_aTabPages[NUMBER_OF_TABS];

#endif
