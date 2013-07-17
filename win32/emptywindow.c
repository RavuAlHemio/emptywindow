#include <windows.h>
#include <wchar.h>

#include "resources.h"

#define W_IDC_ARROW MAKEINTRESOURCEW(0x7F00)

/**
 * Processes events in the main window.
 * @param win Window which triggered the event.
 * @param msg The message which was sent.
 * @param wpar One parameter of the message.
 * @param lpar Another parameter of the message.
 */
static LRESULT CALLBACK MainWinProc(HWND win, UINT msg, WPARAM wpar, LPARAM lpar)
{
	switch (msg)
	{
	case WM_DESTROY:
		/* the window has been closed; quit */
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(win, msg, wpar, lpar);
}

/**
 * Loads a string from the EXE/DLL/etc.'s string table. If that fails, copies
 * the default string.
 * @param res The ID of the string resource to read.
 * @param def The default string.
 * @param buf Buffer into which to store the string.
 * @param bufsize Size of buf (in units of wchar_t).
 */
static void xlate(unsigned int res, const wchar_t *def, wchar_t *buf, size_t bufsize)
{
	HMODULE inst;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, NULL, &inst);

	if (LoadStringW(inst, res, buf, bufsize) == 0)
	{
		wcsncpy(buf, def, bufsize);
		buf[bufsize-1] = '\0';
	}
}

enum
{
	/** The style of the main and only window. */
	MY_WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
};

/**
 * The entry point of a Win32 application. Hi, Windows!
 */
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
)
{
	HWND mainWin;
	WNDCLASSEXW winCl;
	ATOM winClAtom;
	BOOL bRet;
	MSG msg;
	wchar_t winTitleBuf[256];

	winCl.cbSize        = sizeof(winCl);
	winCl.style         = 0;
	winCl.lpfnWndProc   = MainWinProc;
	winCl.cbClsExtra    = 0;
	winCl.cbWndExtra    = 0;
	winCl.hInstance     = hInstance;
	winCl.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APPICON));
	winCl.hCursor       = LoadCursorW(NULL, W_IDC_ARROW);
	winCl.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	winCl.lpszMenuName  = NULL;
	winCl.lpszClassName = L"EmptyWindow";
	winCl.hIconSm       = NULL;

	winClAtom = RegisterClassExW(&winCl);
	if (winClAtom == (ATOM)0)
	{
		return 1;
	}

	xlate(IDS_WINDOWCAPTION, L"Empty Window", winTitleBuf, sizeof(winTitleBuf)/sizeof(winTitleBuf[0]));

	mainWin = CreateWindowExW(
		0,                            /* extended style */
		(LPCWSTR)(intptr_t)winClAtom, /* window class */
		winTitleBuf,                  /* window title */
		MY_WINDOW_STYLE,              /* window style */
		CW_USEDEFAULT,                /* x coord */
		0,                            /* y coord (ignored if x == CW_USEDEFAULT) */
		CW_USEDEFAULT,                /* width */
		0,                            /* height (ignored if width == CW_USEDEFAULT) */
		NULL,                         /* parent window (none here) */
		NULL,                         /* menu (none here) */
		hInstance,                    /* owning instance */
		NULL                          /* parameter passed to the create message */
	);

	if (mainWin == NULL)
	{
		return 2;
	}

	ShowWindow(mainWin, SW_SHOWDEFAULT);
	UpdateWindow(mainWin);

	/* msg loop */
	while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			/* crud. */
			return 3;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	/* exit with what the quit message told us */
	return msg.wParam;
}
