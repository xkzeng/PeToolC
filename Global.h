//global.h: Define global resource;
#pragma once

#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <windows.h>
#include "Resource.h"
#include "Define.h"

extern HINSTANCE g_hAppMain; //Ӧ�ó���ʵ�����,ֻ��һ��;
extern HWND      g_hWndMain; //Ӧ�ó��������ڵľ��,ֻ��һ��;
extern HICON     g_hIcoMain; //Ӧ�ó��������ڵ�ͼ����,ֻ��һ��;
extern HWND      g_hTabCtrl; //Tab�ؼ��ľ��,ֻ��һ��;
extern struct SPeFile* g_lpPeFile; //PE�ļ�����,ֻ��һ��;
extern struct SPePage  g_aTabPages[NUMBER_OF_TABS];

#endif
