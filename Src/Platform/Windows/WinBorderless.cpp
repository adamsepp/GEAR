#ifdef _WIN32
#include "Platform/Windows/WinBorderless.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <GLFW/glfw3native.h>
#pragma comment(lib,"dwmapi.lib")

// Globals for our hook (simplified: one window supported)
static WNDPROC g_OldProc = nullptr;
static TitleHitTestFn g_HitTest = nullptr;
static void* g_User = nullptr;

// Our custom WndProc: make window borderless, but still draggable via titlebar
static LRESULT CALLBACK BorderlessProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_NCCALCSIZE:
		if (w) return 0; // remove default non-client area (caption/borders)
		break;

	case WM_NCHITTEST:
	{
		// Ask user callback if this point is inside custom titlebar
		POINT pt{ GET_X_LPARAM(l), GET_Y_LPARAM(l) };
		ScreenToClient(h, &pt);
		if (g_HitTest && g_HitTest((float)pt.x, (float)pt.y, g_User))
			return HTCAPTION; // native move -> Fancy Zones works
		return HTCLIENT;      // everything else handled by app (e.g. custom resize)
	}

	case WM_SYSCOMMAND:
		if ((w & 0xFFF0u) == SC_SIZE) return 0; // block native resize loop
		break;
	}
	return CallWindowProcW(g_OldProc, h, m, w, l);
}

void HookBorderlessForGLFW(GLFWwindow* win, TitleHitTestFn cb, void* user)
{
	HWND hwnd = glfwGetWin32Window(win);
	g_HitTest = cb; g_User = user;

	// Borderless style: keep THICKFRAME so Windows sees it resizable (needed for Fancy Zones),
	// remove caption/border so we draw our own titlebar.
	LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
	style |= (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	style &= ~(WS_CAPTION | WS_BORDER);
	SetWindowLongPtrW(hwnd, GWL_STYLE, style);
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// Subclass the WndProc
	g_OldProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)BorderlessProc);

	// Optional: keep shadow even without frame
	BOOL enable = TRUE;
	DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &enable, sizeof(enable));
}
#endif
