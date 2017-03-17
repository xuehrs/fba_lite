/*
	================================================================================================
	FB Alpha MDI (Multiple Document Interface) (by CaptainCPS-X / Jezer Andino)
	================================================================================================
	------------------------------------------------------------------------------------------------
	Notes:
	------------------------------------------------------------------------------------------------

		At this point MDI basic features are working fine, since there is no need to minimize
		the main Video Window (child window), I left it to be maximized always. When switching
		to FullScreen mode DirectX will then use hScrnWnd instead of hVideoWindow (for now,
		until a fullscreen child window is properly implemented).

	------------------------------------------------------------------------------------------------
	Technical Notes:
	------------------------------------------------------------------------------------------------

		Based on Microsoft Developer Network documentation, most of the MDI functions and features
		will require a minimum operative system of Windows 2000. Maybe it will work on previous
		versions of Windows but that's what is documented on MSDN.

	------------------------------------------------------------------------------------------------
	TODO:
	------------------------------------------------------------------------------------------------

		* ...

	================================================================================================
*/

#include "burner.h"

const TCHAR	szMDIFrameClass[]	= _T("MDICLIENT");		// <- (don't change this)
const TCHAR szVidWndClass[]		= _T("VidWndClass");
const TCHAR szVidWndTitle[]		= _T("\0");

HWND hWndChildFrame				= NULL;
HWND hVideoWindow				= NULL;

// Prototypes
LRESULT CALLBACK	VideoWndProc		(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL				RegNewMDIChild		();
HWND				CreateNewMDIChild	(HWND hMDIClient);
void				DestroyBurnerMDI	(int nAction);

static void OnPaint         (HWND);
static int OnLButtonDblClk	(HWND, BOOL, int, int, UINT);
static int OnSysCommand		(HWND, UINT, int, int);

int InitBurnerMDI (HWND hParentWnd)
{
    CLIENTCREATESTRUCT ccs;
    ccs.hWindowMenu		= NULL;
    ccs.idFirstChild	= ID_MDI_START_CHILD;
    UINT nWindowStyle	= WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE;
    UINT nWindowStyleEx	= 0;

    hWndChildFrame		= CreateWindowEx( nWindowStyleEx, szMDIFrameClass, NULL, nWindowStyle, 0, 0, 0, 0, hParentWnd, NULL, hAppInst, (LPVOID)&ccs );

    //if(!hWndChildFrame) MessageBox(hWnd, _T("Could not create MDI client."), _T("Error"), MB_OK | MB_ICONERROR);

    hVideoWindow		= nVidFullscreen ? NULL : CreateNewMDIChild(hWndChildFrame);

    return 0;
}

LRESULT CALLBACK VideoWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        HANDLE_MSGB(hWnd, WM_PAINT,			OnPaint);
        HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK,	OnLButtonDblClk);
	    // We can't use the macro from windowsx.h macro for this one
	    case WM_SYSCOMMAND:
	    {
	        if (OnSysCommand(hWnd, (UINT)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)))
	        {
	            return 0;
	        }
	        break;
	    }

	    default:
	        return DefMDIChildProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

BOOL RegNewMDIChild()
{
    if(nVidFullscreen)
    {
        return FALSE;
    }

    WNDCLASSEX wcex;
    wcex.cbSize				= sizeof(WNDCLASSEX);
    wcex.style				= CS_DBLCLKS;										// handle double click
    wcex.lpfnWndProc		= VideoWndProc;										// video window process callback
    wcex.cbClsExtra			= 0;
    wcex.cbWndExtra			= 0;
    wcex.hInstance			= hAppInst;												// fba instance handle
    wcex.hIcon				= LoadIcon(hAppInst, MAKEINTRESOURCE(IDI_APP));			// fba icon
    wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);							// regular cursor
    wcex.hbrBackground		= static_cast<HBRUSH>( GetStockObject( BLACK_BRUSH ));	// black video window background
    wcex.lpszMenuName		= NULL;													// no menu
    wcex.lpszClassName		= szVidWndClass;										// video window class (MDI child)
    wcex.hIconSm			= LoadIcon(hAppInst, MAKEINTRESOURCE(IDI_APP));

    if(!RegisterClassEx(&wcex))
    {
        //MessageBox(NULL, _T("There was a problem registering the window class"), _T("Error"), MB_OK);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

HWND CreateNewMDIChild(HWND hMDIClient)
{
    HWND hNewWnd = NULL;
    hNewWnd = CreateMDIWindow(szVidWndClass, szVidWndTitle, MDIS_ALLCHILDSTYLES | CS_DBLCLKS | WS_DLGFRAME | WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMDIClient, hAppInst, 0);
    //if(!hNewWnd) MessageBox(NULL, _T("Error creating child window"), _T("Creation Error"), MB_OK);
    return hNewWnd;
}

void DestroyBurnerMDI(int nAction)
{
    switch(nAction)
    {
    // OnDestroy
    case 0:
    {
        hVideoWindow	= NULL;
        hWndChildFrame	= NULL;
        break;
    }

    // ScrnExit()
    case 1:
    {
        if(hVideoWindow)
        {
            SendMessage(hWndChildFrame, WM_MDIDESTROY, (INT_PTR) hVideoWindow, 0L);
            hVideoWindow = NULL;
        }
        if(hWndChildFrame)
        {
            DestroyWindow(hWndChildFrame);
            hWndChildFrame = NULL;
        }
        UnregisterClass(szVidWndClass, hAppInst);
        break;
    }
    }
}

static void OnPaint(HWND hwnd)
{
    if(!nVidFullscreen)
    {
        PAINTSTRUCT ps;
        HDC hdc = NULL;
        hdc = BeginPaint (hwnd, &ps);
        EndPaint (hwnd, &ps);
		if (bDrvOkay)
        {
        	VidPaint(1);
		}
    }
}

// MOUSE LEFT BUTTON (DOUBLE CLICK)
static int OnLButtonDblClk(HWND /*hwnd*/, BOOL, int, int, UINT)
{
    if (bDrvOkay)
    {
        nVidFullscreen = !nVidFullscreen;
        POST_INITIALISE_MESSAGE;
        return 0;
    }
    return 1;
}

// Block screensaver and windows menu if needed
static int OnSysCommand(HWND, UINT sysCommand, int, int)
{
    switch (sysCommand)
    {
    case SC_MONITORPOWER:
    case SC_SCREENSAVE:
    {
        if (!bRunPause && bDrvOkay)
        {
            return 1;
        }
        break;
    }
    case SC_KEYMENU:
    case SC_MOUSEMENU:
    {
        if (kNetGame)
        {
            return 1;
        }
        break;
    }
    }
    return 0;
}
