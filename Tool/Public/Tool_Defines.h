#pragma once

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

//#include "Engine_Defines.h"
//#include "Engine_Struct.h"
#include <process.h>

namespace Tool
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

#include "ImGui_Manager_Tool.h"

using namespace Tool;