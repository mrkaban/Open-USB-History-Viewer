/*
	usb_main

	14:41:22 25-03-2013

	Generated by VISG v.0.951

	VISG: visual and smart GUI builder
	Get the latest version of VISG at
	http://mrhx.clan.su/
	http://mrhx.ucoz.com/

	Tested on Visual C++ 6.0 and devcpp
*/

#include <windows.h>
#include <commctrl.h>		// INCLUDE LIBRARY comctl32.lib INTO YOUR PROJECT!
//#include "resource.h"		// UNCOMMENT THIS IF YOU USE RESOURCES.

// names of window classes.
#define WND_CLASS_NAME0 "my_wnd_class_name_0"

// identifiers.
#define IDC_TREEVIEW0 2000
#define IDC_BUTTON0   2001
#define IDC_BUTTON1   2002
#define BTN_GETFFile  2003
#define BTN_GETINFO   2004
#define IDC_GROUP1    2005
#define HistoryGrid   2006
#define IDC_GROUP0    2007

// function prototypes.
void register_classes();
int message_loop();
HWND create_wnd0();
void create_wnd_content0(HWND parent);
LRESULT CALLBACK wnd_proc0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// global data.
HINSTANCE instance;
HFONT h_font;

// main function.
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinst, LPSTR cmdline, int show)
{
	instance = hinstance;
	InitCommonControls();
	register_classes();
	h_font = CreateFont(-13, 0, 0, 0, FW_NORMAL, 0,
				0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");
	create_wnd0();
	return message_loop();
}

// window procedure #0 [Small Usb History Viewer by Cynic].
LRESULT CALLBACK wnd_proc0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case BTN_GETINFO:	// button "Get info"
			MessageBox(hwnd, "the button BTN_GETINFO is clicked.", "Event", MB_OK|MB_ICONINFORMATION);
			break;

		case BTN_GETFFile:	// button "Get info from file"
			MessageBox(hwnd, "the button BTN_GETFFile is clicked.", "Event", MB_OK|MB_ICONINFORMATION);
			break;

		case IDC_BUTTON1:	// button "Get info by LAN"
			MessageBox(hwnd, "the button IDC_BUTTON1 is clicked.", "Event", MB_OK|MB_ICONINFORMATION);
			break;

		case IDC_BUTTON0:	// button "Export"
			MessageBox(hwnd, "the button IDC_BUTTON0 is clicked.", "Event", MB_OK|MB_ICONINFORMATION);
			break;

		}
		break;

	case WM_CREATE:
		create_wnd_content0(hwnd);
		break;

	case WM_CLOSE:
		if (IDYES == MessageBox(hwnd, "Quit?", "Event", MB_YESNO|MB_ICONQUESTION)) DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);	// PostQuitMessage(return_code) quits the message loop.
		break;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return FALSE;
}

// create window #0 [Open Usb History Viewer �� ����������������.��].
HWND create_wnd0()
{
	HWND wnd;
	wnd = CreateWindowEx(0x00040180, WND_CLASS_NAME0, "Open Usb History Viewer �� ����������������.��", 0x14CF0000, CW_USEDEFAULT, CW_USEDEFAULT, 700, 414, NULL, NULL, instance, NULL);
	ShowWindow(wnd, SW_SHOWNORMAL);
	UpdateWindow(wnd);
	return wnd;
}

// create window content #0 [Open Usb History Viewer �� ����������������.��].
void create_wnd_content0(HWND parent)
{
	HWND wnd;
	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	wnd = CreateWindowEx(0x00000000, "Button", "History", 0x50020007, 0, 0, 680, 328, parent, (HMENU) IDC_GROUP0, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000200, "SysListView32", "", 0x50010001, 192, 16, 480, 304, parent, (HMENU) HistoryGrid, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	col.cx = 84;
	col.pszText = "ID ����������";
	SendMessage(wnd, LVM_INSERTCOLUMN, 0, (LPARAM) &col);
	col.cx = 98;
	col.pszText = "Parent ID prefix";
	SendMessage(wnd, LVM_INSERTCOLUMN, 1, (LPARAM) &col);
	col.cx = 84;
	col.pszText = "Driver";
	SendMessage(wnd, LVM_INSERTCOLUMN, 2, (LPARAM) &col);
	col.cx = 108;
	col.pszText = "Disk Stamp";
	SendMessage(wnd, LVM_INSERTCOLUMN, 3, (LPARAM) &col);
	col.cx = 102;
	col.pszText = "Volume Stamp";
	SendMessage(wnd, LVM_INSERTCOLUMN, 4, (LPARAM) &col);
	wnd = CreateWindowEx(0x00000000, "Button", "Controls", 0x50020007, 0, 320, 416, 56, parent, (HMENU) IDC_GROUP1, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000000, "Button", "Get info", 0x50012F00, 8, 336, 96, 32, parent, (HMENU) BTN_GETINFO, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000000, "Button", "Get info from file", 0x50012F00, 104, 336, 96, 32, parent, (HMENU) BTN_GETFFile, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000000, "Button", "Get info by LAN", 0x50012F00, 200, 336, 96, 32, parent, (HMENU) IDC_BUTTON1, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000000, "Button", "Export", 0x50012F00, 296, 336, 104, 32, parent, (HMENU) IDC_BUTTON0, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(0x00000200, "SysTreeView32", "", 0x50010000, 8, 16, 184, 304, parent, (HMENU) IDC_TREEVIEW0, instance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
}

// register all the window classes.
void register_classes()
{
	WNDCLASS wc;

	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance     = instance;
	wc.lpszMenuName  = NULL;
	wc.style         = CS_PARENTDC | CS_DBLCLKS;

	wc.lpfnWndProc   = wnd_proc0;
	wc.lpszClassName = WND_CLASS_NAME0;

	RegisterClass(&wc);
}

// message loop.
int message_loop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(h_font);
	return msg.wParam;
}

/* End of file */