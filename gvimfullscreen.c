/*
   cl /LD gvimfullscreen.c user32.lib gdi32.lib
   ------------------------------
   :call libcallnr("gvimfullscreen.dll", "ToggleFullScreen", 1)
   */
#include <windows.h>

//#pragma commen(lib, "User32.lib")
//#pragma commen(lib, "Gdi32.lib")

#ifndef LWA_ALPHA
#define LWA_ALPHA 2
#endif

#ifndef MONITOR_DEFAULTTONEAREST
#define MONITOR_DEFAULTTONEAREST    0x00000002
#endif

/*#ifndef WS_EX_LAYERD*/
/*#define WS_EX_LAYERED 0x00080000*/
/*#endif*/

int g_x, g_y, g_dx, g_dy;

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam) {
	HWND* pphWnd = (HWND*)lParam;

	if (GetParent(hwnd)) {
		*pphWnd = NULL;
		return TRUE;
	}
	*pphWnd = hwnd;
	return FALSE;
}

LONG _declspec(dllexport) ToggleFullScreen() {
	HWND hTop = NULL;
	HWND hTextArea = NULL;

	EnumThreadWindows(GetCurrentThreadId(), FindWindowProc, (LPARAM)&hTop);
	hTextArea = FindWindowEx(hTop, NULL, "VimTextArea", "Vim text area");

	if (hTop != NULL && hTextArea != NULL) {
		/* Determine the current state of the window */
		HDC dc = GetDC(hTextArea);

		if (dc != NULL) {
			COLORREF rgb = GetPixel(dc, 80, 3);
			if(rgb != CLR_INVALID) {
				SetDCBrushColor(dc, rgb);
			}
			ReleaseDC(hTextArea, dc);
		}

		if ( GetWindowLong(hTop, GWL_STYLE) & WS_CAPTION ) {
			/* Has a caption, so isn't maximised */

			MONITORINFO mi;
			RECT rc;
			HMONITOR hMonitor;
			long unsigned int z;
			char p[MAX_PATH];

			z = (long unsigned int)IsZoomed(hTop)?1:0;
			if(z) {
				SendMessage(hTop, WM_SYSCOMMAND, SC_RESTORE, 0);
			}

			GetWindowRect(hTop, &rc);
			sprintf(p, "gVim_Position=%ld\t%ld\t%ld\t%ld\t%d", rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, z);
			putenv(p);

			hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

			//
			// get the work area or entire monitor rect.
			//
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMonitor, &mi);

			g_x = mi.rcMonitor.left;
			g_y = mi.rcMonitor.top;
			g_dx = mi.rcMonitor.right - g_x;
			g_dy = mi.rcMonitor.bottom - g_y;

			/* Remove border, caption, and edges */
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_BORDER);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_CAPTION);
			SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
			SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_EX_WINDOWEDGE);

			SetWindowPos(hTop, HWND_TOP, g_x, g_y, g_dx, g_dy, SWP_SHOWWINDOW);

			SetWindowLong(hTextArea, GWL_EXSTYLE, GetWindowLong(hTextArea, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE); 
			SetWindowPos(hTextArea, HWND_TOP, 0, 0, g_dx, g_dy, SWP_SHOWWINDOW);
		}else{
			long unsigned int L, R, W, H, Z;
			char *p;

			/* Already full screen, so restore all the previous styles */
			SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) | WS_BORDER);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_CAPTION);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MINIMIZEBOX);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MAXIMIZEBOX);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_CLIENTEDGE);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_WINDOWEDGE);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_THICKFRAME);
			SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_DLGFRAME);

			/*SetWindowLong(hTextArea, GWL_EXSTYLE, GetWindowLong(hTextArea, GWL_EXSTYLE) | WS_EX_CLIENTEDGE); 修改过的VIM源码不需要这设置*/

			if((p = getenv("gVim_Position")) != NULL) {
				sscanf(p, "%ld\t%ld\t%ld\t%ld\t%d", &L, &R, &W, &H, &Z);
				SetWindowPos(hTop, HWND_TOP, L, R, W, H, SWP_SHOWWINDOW);
				if(Z) {
					SendMessage(hTop, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				}
			}
		};

#ifdef _WIN64
		SetClassLongPtr(hTextArea, GCLP_HBRBACKGROUND, (LONG)GetStockObject(DC_BRUSH));
#else
		SetClassLong(hTextArea, GCL_HBRBACKGROUND, (LONG)GetStockObject(DC_BRUSH));
#endif
	}
	return GetLastError();
}

LONG _declspec(dllexport) SetAlpha(LONG nTrans) {
	HWND hTop = NULL;

	EnumThreadWindows(GetCurrentThreadId(), FindWindowProc, (LPARAM)&hTop);

	if(hTop != NULL) {
		if(nTrans == 255) {
			SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_EX_LAYERED); 
		}else{
			SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hTop, 0, (BYTE)nTrans, LWA_ALPHA);
		}
	}
	return GetLastError();
}

LONG _declspec(dllexport) EnableTopMost(LONG bEnable) {
	HWND hTop = NULL;
	DWORD dwThreadID;

	dwThreadID = GetCurrentThreadId();
	EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

	if(hTop) {
		if (bEnable == 0) {
			SetWindowPos(hTop, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}else{
			SetWindowPos(hTop, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
	}
	return GetLastError();
}
