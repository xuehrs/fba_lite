#include "burner.h"

// Set the current directory to be the application's directory
int AppDirectory()
{
    TCHAR szPath[MAX_PATH] = _T("");
    int nLen = 0;
    TCHAR *pc1, *pc2;
    TCHAR *szCmd = GetCommandLine();

    // Find the end of the "c:\directory\program.exe" bit
    if (szCmd[0] == _T('\"'))  						// Filename is enclosed in quotes
    {
        szCmd++;
        for (pc1 = szCmd; *pc1; pc1++)
        {
            if (*pc1 == _T('\"')) break;			// Find the last "
        }
    }
    else
    {
        for (pc1 = szCmd; *pc1; pc1++)
        {
            if (*pc1 == _T(' ')) break;				// Find the first space
        }
    }
    // Find the last \ or /
    for (pc2 = pc1; pc2 >= szCmd; pc2--)
    {
        if (*pc2 == _T('\\')) break;
        if (*pc2 == _T('/')) break;
    }

    // Copy the name of the executable into a variable
    nLen = pc1 - pc2 - 1;
    if (nLen > EXE_NAME_SIZE)
    {
        nLen = EXE_NAME_SIZE;
    }
    _tcsncpy(szAppExeName, pc2 + 1, nLen);
    szAppExeName[nLen] = 0;

    // strip .exe
    if ((pc1 = _tcschr(szAppExeName, _T('.'))) != 0)
    {
        *pc1 = 0;
    }

    nLen = pc2 - szCmd;
    if (nLen <= 0) return 1;			// No path

    // Now copy the path into a new buffer
    _tcsncpy(szPath, szCmd, nLen);
    SetCurrentDirectory(szPath);		// Finally set the current directory to be the application's directory

    dprintf(szPath);
    dprintf(_T("\n"));

    return 0;
}

void UpdatePath(TCHAR *path)
{
    int pathlen = _tcslen(path);
    if (pathlen)
    {
        DWORD attrib = INVALID_FILE_ATTRIBUTES;
        TCHAR curdir[MAX_PATH] = _T("");
        int curlen = 0;

        attrib = GetFileAttributes(path);
        if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) && path[pathlen - 1] != _T('\\'))
        {
            path[pathlen] = _T('\\');
            path[pathlen + 1] = _T('\0');

            pathlen++;
        }

        GetCurrentDirectory(sizeof(curdir), curdir);
        curlen = _tcslen(curdir);

        if (_tcsnicmp(curdir, path, curlen) == 0 && path[curlen] == _T('\\'))
        {
            TCHAR newpath[MAX_PATH];

            _tcscpy(newpath, path + curlen + 1);
            _tcscpy(path, newpath);
        }
    }
}

// ---------------------------------------------------------------------------

// Get the position of the client area of a window on the screen
int GetClientScreenRect(HWND hWnd, RECT *pRect)
{
    GetClientRect(hWnd, pRect);
	MapWindowRect(hWnd, NULL, pRect);
    return 0;
}

// Put a window in the middle of another window
int WndInMid(HWND hMid, HWND hBase)
{
    RECT MidRect = {0, 0, 0, 0};
    int mw = 0, mh = 0;
    RECT BaseRect = {0, 0, 0, 0};
    int bx = 0, by = 0;

    // Find the height and width of the Mid window
    GetWindowRect(hMid, &MidRect);
    mw = MidRect.right - MidRect.left;
    mh = MidRect.bottom - MidRect.top;

    // Find the center of the Base window
    if (hBase && IsWindowVisible(hBase))
    {
        GetWindowRect(hBase, &BaseRect);
        if (hBase == hMainWnd)
        {
            // For the main window, center in the client area.
            BaseRect.left += GetSystemMetrics(SM_CXSIZEFRAME);
            BaseRect.right -= GetSystemMetrics(SM_CXSIZEFRAME);
            BaseRect.top += GetSystemMetrics(SM_CYSIZEFRAME);
            if (bMenuEnabled)
            {
                BaseRect.top += GetSystemMetrics(SM_CYCAPTION) + nMenuHeight;
            }
            BaseRect.bottom -= GetSystemMetrics(SM_CYSIZEFRAME);
        }
    }
    else
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &BaseRect, 0);
    }

    bx = BaseRect.left + BaseRect.right;
    bx = (bx - mw) >> 1;
    by = BaseRect.top + BaseRect.bottom;
    by = (by - mh) >> 1;

    if (hBase)
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &SystemWorkArea, 0);

        if (bx + mw > SystemWorkArea.right)
        {
            bx = SystemWorkArea.right - mw;
        }
        if (by + mh > SystemWorkArea.bottom)
        {
            by = SystemWorkArea.bottom - mh;
        }
        if (bx < SystemWorkArea.left)
        {
            bx = SystemWorkArea.left;
        }
        if (by < SystemWorkArea.top)
        {
            by = SystemWorkArea.top;
        }
    }

    // Center the window
    SetWindowPos(hMid, NULL, bx, by, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    return 0;
}

