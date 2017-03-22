// Screen Window
#include "burner.h"
#include <shlobj.h>

#define		HORIZONTAL_ORIENTED_RES		0
#define		VERTICAL_ORIENTED_RES		1

int nActiveGame;

static bool bLoading = 0;

int OnMenuSelect(HWND, HMENU, int, HMENU, UINT);
int OnInitMenuPopup(HWND, HMENU, UINT, BOOL);
int OnUnInitMenuPopup(HWND, HMENU, UINT, BOOL);
void DisplayPopupMenu(int nMenu);

RECT SystemWorkArea = { 0, 0, 640, 480 };				// Work area on the desktop

int  bAutoPause = 1;
bool bHasFocus = false;
int  nSavestateSlot = 1;

static TCHAR *szClass = _T("FB Alpha");					// Window class name
HWND hMainWnd = NULL;									// Handle to the screen window

static bool bMaximised;
static int nPrevWidth, nPrevHeight;
static int bBackFromHibernation = 0;

#define ID_NETCHAT 999
HWND hwndChat = NULL;
WNDPROC pOldWndProc = NULL;

static int nDragX, nDragY;

static int OnCreate(HWND, LPCREATESTRUCT);
static void OnActivateApp(HWND, BOOL, DWORD);
static void OnPaint(HWND);
static void OnClose(HWND);
static void OnDestroy(HWND);
static void OnCommand(HWND, int, HWND, UINT);
static int OnSysCommand(HWND, UINT, int, int);
static void OnSize(HWND, UINT, int, int);
static void OnEnterSizeMove(HWND);
static void OnExitSizeMove(HWND);
static void OnEnterIdle(HWND, UINT, HWND);
static int OnLButtonDblClk(HWND, BOOL, int, int, UINT);
static int OnDisplayChange(HWND, UINT, UINT, UINT);

int OnNotify(HWND, int, NMHDR *lpnmhdr);

static bool UseDialogs()
{
    if (/*!bDrvOkay ||*/ !nVidFullscreen)
    {
        return true;
    }

    return false;
}

void SetPauseMode(bool bPause)
{
    bRunPause = bPause;
    bAltPause = bPause;

    if (bPause)
    {
        AudBlankSound();
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        }
    }
    else
    {
        GameInpCheckMouse();
    }
}

static char *CreateKailleraList()
{
    unsigned int nOldDrvSelect = nBurnDrvActive;
    int nSize = 256 * 1024;
    char *pList = (char *)malloc(nSize);
    char *pName = pList;

    if (pList == NULL)
    {
        return NULL;
    }

    // Add chat option to the gamelist
    pName += sprintf(pName, "* Chat only");
    pName++;

    if (avOk)
    {
        // Add all the driver names to the list
        for (nBurnDrvActive = 0; nBurnDrvActive < nBurnDrvCount; nBurnDrvActive++)
        {

            if(BurnDrvGetFlags() & BDF_GAME_WORKING && gameAv[nBurnDrvActive])
            {
                char *szDecoratedName = DecorateGameName(nBurnDrvActive);

                if (pName + strlen(szDecoratedName) >= pList + nSize)
                {
                    char *pNewList;
                    nSize <<= 1;
                    pNewList = (char *)realloc(pList, nSize);
                    if (pNewList == NULL)
                    {
                        return NULL;
                    }
                    pName -= (INT_PTR)pList;
                    pList = pNewList;
                    pName += (INT_PTR)pList;
                }
                pName += sprintf(pName, "%s", szDecoratedName);
                pName++;
            }
        }
    }

    *pName = '\0';
    pName++;

    nBurnDrvActive = nOldDrvSelect;

    return pList;
}

void DeActivateChat()
{
    bEditActive = false;
    DestroyWindow(hwndChat);
    hwndChat = NULL;
}

int ActivateChat()
{
    RECT rect;
    GetClientRect(hMainWnd, &rect);

    DeActivateChat();

    // Create an invisible edit control
    hwndChat = CreateWindow(
                   _T("EDIT"), NULL, WS_CHILD | ES_LEFT,
                   0, rect.bottom - 32, rect.right, 32,
                   hMainWnd, (HMENU)ID_NETCHAT, hAppInst, NULL);                // pointer not needed

    EditText[0] = 0;
    bEditTextChanged = true;
    bEditActive = true;

    SendMessage(hwndChat, EM_LIMITTEXT, MAX_CHAT_SIZE, 0);			// Limit the amount of text

    SetFocus(hwndChat);

    return 0;
}

static int WINAPI gameCallback(char *game, int player, int numplayers)
{
    bool bFound = false;
    HWND hActive;

    for (nBurnDrvActive = 0; nBurnDrvActive < nBurnDrvCount; nBurnDrvActive++)
    {

        char *szDecoratedName = DecorateGameName(nBurnDrvActive);

        if (!strcmp(szDecoratedName, game))
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        //		kailleraEndGame();
        Kaillera_End_Game();
        return 1;
    }

    kNetGame = 1;
    hActive = GetActiveWindow();

    bCheatsAllowed = false;								// Disable cheats during netplay
    AudSoundStop();										// Stop while we load roms
    DrvInit(nBurnDrvActive, false);						// Init the game driver
    ScrnInit();
    AudSoundPlay();										// Restart sound
    VidInit();
    SetFocus(hMainWnd);

    //	dprintf(_T(" ** OSD startnet text sent.\n"));

    TCHAR szTemp1[256];
    TCHAR szTemp2[256];
    VidSAddChatMsg(LoadStringEx(hAppInst, IDS_NETPLAY_START, true), 0xFFFFFF, BurnDrvGetText(DRV_FULLNAME), 0xFFBFBF);
    _sntprintf(szTemp1, 256, LoadStringEx(hAppInst, IDS_NETPLAY_START_YOU, true), player);
    _sntprintf(szTemp2, 256, LoadStringEx(hAppInst, IDS_NETPLAY_START_TOTAL, true), numplayers);
    VidSAddChatMsg(szTemp1, 0xFFFFFF, szTemp2, 0xFFBFBF);

    RunMessageLoop();

    DrvExit();
    if (kNetGame)
    {
        kNetGame = 0;
        //		kailleraEndGame();
        Kaillera_End_Game();
    }
    DeActivateChat();

    bCheatsAllowed = true;								// reenable cheats netplay has ended

    SetFocus(hActive);
    return 0;
}

static void WINAPI kChatCallback(char *nick, char *text)
{
    TCHAR szTemp[128];
    _sntprintf(szTemp, 128, _T("%.32hs "), nick);
    VidSAddChatMsg(szTemp, 0xBFBFFF, ANSIToTCHAR(text, NULL, 0), 0x7F7FFF);
}

static void WINAPI kDropCallback(char *nick, int playernb)
{
    TCHAR szTemp[128];
    _sntprintf(szTemp, 128, LoadStringEx(hAppInst, IDS_NETPLAY_DROP, true), playernb, nick);
    VidSAddChatMsg(szTemp, 0xFFFFFF, NULL, 0);
}

static void DoNetGame()
{
    kailleraInfos ki;
    char tmpver[128];
    char *gameList;

    if(bDrvOkay)
    {
        DrvExit();
        ScrnTitle();
    }
    MenuEnableItems();

#ifdef _UNICODE
    _snprintf(tmpver, 128, APP_TITLE " v%.20ls", szAppBurnVer);
#else
    _snprintf(tmpver, 128, APP_TITLE " v%.20s", szAppBurnVer);
#endif

    gameList = CreateKailleraList();

    ki.appName = tmpver;
    ki.gameList = gameList;
    ki.gameCallback = &gameCallback;
    ki.chatReceivedCallback = &kChatCallback;
    ki.clientDroppedCallback = &kDropCallback;
    ki.moreInfosCallback = NULL;

    Kaillera_Set_Infos(&ki);
    //kailleraSetInfos(&ki);

    Kaillera_Select_Server_Dialog(NULL);
    //kailleraSelectServerDialog(NULL);

    if (gameList)
    {
        free(gameList);
        gameList = NULL;
    }

    End_Network();

    POST_INITIALISE_MESSAGE;
}

int CreateDatfileWindows(int bType)
{
    TCHAR szTitle[1024];
    TCHAR szFilter[1024];

    TCHAR szConsoleString[64];
    _sntprintf(szConsoleString, 64, _T(""));

    TCHAR szProgramString[25];
    _sntprintf(szProgramString, 25, _T("ClrMame Pro XML"));

    _sntprintf(szChoice, MAX_PATH, _T(APP_TITLE) _T(" v%.20s (%s%s).dat"), szAppBurnVer, szProgramString, szConsoleString);
    _sntprintf(szTitle, 256, LoadStringEx(hAppInst, IDS_DAT_GENERATE, true), szProgramString);

    _stprintf(szFilter, LoadStringEx(hAppInst, IDS_DISK_ALL_DAT, true), _T(APP_TITLE));
    memcpy(szFilter + _tcslen(szFilter), _T(" (*.dat)\0*.dat\0\0"), 16 * sizeof(TCHAR));

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = szChoice;
    ofn.nMaxFile = sizeof(szChoice) / sizeof(TCHAR);
    ofn.lpstrInitialDir = _T(".");
    ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = _T("dat");
    ofn.lpstrTitle = szTitle;
    ofn.Flags |= OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == 0)
        return -1;

    return create_datfile(szChoice, bType);
}

// Returns true if a VidInit is needed when the window is resized
static bool VidInitNeeded()
{
    // D3D blitter needs to re-initialise only when auto-size RGB effects are enabled
    if (nVidSelect == 1 && (nVidBlitterOpt[nVidSelect] & 0x00030000) == 0x00030000)
    {
        return true;
    }
    if (nVidSelect == 2)
    {
        return true;
    }

    return false;
}

// Refresh the contents of the window when re-sizing it
static void RefreshWindow(bool bInitialise)
{
    if (nVidFullscreen)
    {
        return;
    }

    if (bInitialise && VidInitNeeded())
    {
        VidInit();
        if (bVidOkay && (bRunPause || !bDrvOkay))
        {
            VidRedraw();
            VidPaint(0);
        }
    }
}

static int OnDisplayChange(HWND, UINT, UINT, UINT)
{
    if (!nVidFullscreen)
    {
        POST_INITIALISE_MESSAGE;
    }

    return 0;
}

static int OnLButtonDblClk(HWND hwnd, BOOL, int, int, UINT)
{
    //if(nVidFullscreen)
    {
        if (bDrvOkay)
        {
            nVidFullscreen = !nVidFullscreen;
            POST_INITIALISE_MESSAGE;
            return 0;
        }
    }
    return 1;
}

static int OnCreate(HWND hWnd, LPCREATESTRUCT /*lpCreateStruct*/)	// HWND hwnd, LPCREATESTRUCT lpCreateStruct
{
    return 1;
}

static void OnActivateApp(HWND hwnd, BOOL fActivate, DWORD /* dwThreadId */)
{
    bHasFocus = fActivate;
    if (!kNetGame && bAutoPause && !bAltPause && hInpdDlg == NULL && hInpCheatDlg == NULL && hInpDIPSWDlg == NULL)
    {
        bRunPause = fActivate ? 0 : 1;
    }
    if (fActivate == false && hwnd == hMainWnd)
    {
        EndMenu();
    }
    if (fActivate == false && bRunPause)
    {
        AudBlankSound();
    }

    if (fActivate)
    {
        if (hInpdDlg || hInpCheatDlg || hInpDIPSWDlg || hDbgDlg)
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        }
        else
        {
            GameInpCheckMouse();
        }
    }
}

extern HWND hSelDlg;

static void PausedRedraw(void)
{
    if (bVidOkay && bRunPause && bDrvOkay && (hSelDlg == NULL))   // Redraw the screen to show certain messages while paused. - dink
    {
        INT16 *pBtemp = pBurnSoundOut;
        pBurnSoundOut = NULL; // Mute the sound as VidRedraw() draws the frame

        VidRedraw();
        VidPaint(0);

        pBurnSoundOut = pBtemp;
    }
}

static void OnPaint(HWND hWnd)
{
    if (hWnd == hMainWnd)
    {
        VidPaint(1);

        if (bBackFromHibernation)
        {
            PausedRedraw(); // redraw game screen if paused and returning from hibernation - dink
            bBackFromHibernation = 0;
        }
		if (bDrvOkay)
        {
        	VidPaint(1);
		}
    }
}

static void OnClose(HWND)
{
    PostQuitMessage(0);					// Quit the program if the window is closed
}

static void OnDestroy(HWND)
{
	MenuDestroy();
    VidExit();							// Stop using video with the Window
    hMainWnd = NULL;					// Make sure handle is not used again
}

OPENFILENAME	bgFn;
TCHAR			szFile[MAX_PATH];

static void UpdatePreviousGameList()
{
    int nRecentIdenticalTo = -1;

    // check if this game is identical to any of the listed in the recent menu
    for(int x = 0; x < SHOW_PREV_GAMES; x++)
    {
        if(!_tcscmp(BurnDrvGetText(DRV_NAME), szPrevGames[x]))
        {
            nRecentIdenticalTo = x;
        }
    }

    // Declare temporary array
    TCHAR szTmp[SHOW_PREV_GAMES][32];

    // Backup info for later use
    for(int x = 0; x < SHOW_PREV_GAMES; x++)
    {
        _tcscpy(szTmp[x], szPrevGames[x]);
    }

    switch(nRecentIdenticalTo)
    {
    case -1:
        // Normal rotation when recent game is not identical to any of the ones listed
        // - - -
        _tcscpy(szPrevGames[9], szPrevGames[8]);			// Recent 10 = 9
        _tcscpy(szPrevGames[8], szPrevGames[7]);			// Recent 9 = 8
        _tcscpy(szPrevGames[7], szPrevGames[6]);			// Recent 8 = 7
        _tcscpy(szPrevGames[6], szPrevGames[5]);			// Recent 7 = 6
        _tcscpy(szPrevGames[5], szPrevGames[4]);			// Recent 6 = 5
        _tcscpy(szPrevGames[4], szPrevGames[3]);			// Recent 5 = 4
        _tcscpy(szPrevGames[3], szPrevGames[2]);			// Recent 4 = 3
        _tcscpy(szPrevGames[2], szPrevGames[1]);			// Recent 3 = 2
        _tcscpy(szPrevGames[1], szPrevGames[0]);			// Recent 2 = 1
        _tcscpy(szPrevGames[0], BurnDrvGetText(DRV_NAME));	// Update most recent game played (Recent 1)
        break;
    case 0:
        break;												// Nothing Change
    case 1:
        _tcscpy(szPrevGames[0], szTmp[1]);					// Update most recent game played (Recent 1 = 2)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        break;
    case 2:
        _tcscpy(szPrevGames[0], szTmp[2]);					// Update most recent game played (Recent 1 = 3)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        break;
    case 3:
        _tcscpy(szPrevGames[0], szTmp[3]);					// Update most recent game played (Recent 1 = 4)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        break;
    case 4:
        _tcscpy(szPrevGames[0], szTmp[4]);					// Update most recent game played (Recent 1 = 5)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        break;
    case 5:
        _tcscpy(szPrevGames[0], szTmp[5]);					// Update most recent game played (Recent 1 = 6)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        _tcscpy(szPrevGames[5], szTmp[4]);					// Recent 6 = 5
        break;
    case 6:
        _tcscpy(szPrevGames[0], szTmp[6]);					// Update most recent game played (Recent 1 = 7)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        _tcscpy(szPrevGames[5], szTmp[4]);					// Recent 6 = 5
        _tcscpy(szPrevGames[6], szTmp[5]);					// Recent 7 = 6
        break;
    case 7:
        _tcscpy(szPrevGames[0], szTmp[7]);					// Update most recent game played (Recent 1 = 8)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        _tcscpy(szPrevGames[5], szTmp[4]);					// Recent 6 = 5
        _tcscpy(szPrevGames[6], szTmp[5]);					// Recent 7 = 6
        _tcscpy(szPrevGames[7], szTmp[6]);					// Recent 8 = 7
        break;
    case 8:
        _tcscpy(szPrevGames[0], szTmp[8]);					// Update most recent game played (Recent 1 = 9)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        _tcscpy(szPrevGames[5], szTmp[4]);					// Recent 6 = 5
        _tcscpy(szPrevGames[6], szTmp[5]);					// Recent 7 = 6
        _tcscpy(szPrevGames[7], szTmp[6]);					// Recent 8 = 7
        _tcscpy(szPrevGames[8], szTmp[7]);					// Recent 9 = 8
        break;
    case 9:
        _tcscpy(szPrevGames[0], szTmp[9]);					// Update most recent game played (Recent 1 = 10)
        _tcscpy(szPrevGames[1], szTmp[0]);					// Recent 2 = 1
        _tcscpy(szPrevGames[2], szTmp[1]);					// Recent 3 = 2
        _tcscpy(szPrevGames[3], szTmp[2]);					// Recent 4 = 3
        _tcscpy(szPrevGames[4], szTmp[3]);					// Recent 5 = 4
        _tcscpy(szPrevGames[5], szTmp[4]);					// Recent 6 = 5
        _tcscpy(szPrevGames[6], szTmp[5]);					// Recent 7 = 6
        _tcscpy(szPrevGames[7], szTmp[6]);					// Recent 8 = 7
        _tcscpy(szPrevGames[8], szTmp[7]);					// Recent 9 = 8
        _tcscpy(szPrevGames[9], szTmp[8]);					// Recent 10 = 9
        break;
    }
}

// Compact driver loading module
int BurnerLoadDriver(TCHAR *szDriverName)
{
    unsigned int j;

    int nOldDrvSelect = nBurnDrvActive;
    DrvExit();
    bLoading = 1;

    for (j = 0; j < nBurnDrvCount; j++)
    {
        nBurnDrvActive = j;
        if (!_tcscmp(szDriverName, BurnDrvGetText(DRV_NAME)) && (!(BurnDrvGetFlags() & BDF_BOARDROM)))
        {
            nBurnDrvActive = nOldDrvSelect;
            nDialogSelect = j;
            DrvExit();
            DrvInit(j, true);	// Init the game driver
            MenuEnableItems();
            bAltPause = 0;
            AudSoundPlay();			// Restart sound
            bLoading = 0;
            UpdatePreviousGameList();
            if (bVidAutoSwitchFull)
            {
                nVidFullscreen = 1;
                POST_INITIALISE_MESSAGE;
            }
            break;
        }
    }

    return 0;
}

void scrnSSUndo() // called from the menu (shift+F8) and CheckSystemMacros() in run.cpp
{
    if (bDrvOkay)
    {
        TCHAR szString[256] = _T("state undo");
        TCHAR szStringFailed[256] = _T("state: nothing to undo");
        if (!StatedUNDO(nSavestateSlot))
        {
            VidSNewShortMsg(szString);
        }
        else
        {
            VidSNewShortMsg(szStringFailed);
        }
        PausedRedraw();
    }
}

static void OnCommand(HWND /*hDlg*/, int id, HWND /*hwndCtl*/, UINT codeNotify)
{
    if (bLoading)
    {
        return;
    }

    switch (id)
    {
    case MENU_LOAD:
    {
        int nGame;

        if(kNetGame || !UseDialogs() || bLoading)
        {
            break;
        }


        InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);

        bLoading = 1;
        AudSoundStop();						// Stop while the dialog is active or we're loading ROMs

        nGame = PopupSelectDialog(hMainWnd);		// Bring up select dialog to pick a driver
        if (nGame >= 0)
        {
            DrvExit();
            DrvInit(nGame, true);			// Init the game driver
            MenuEnableItems();
            bAltPause = 0;
            AudSoundPlay();					// Restart sound
            bLoading = 0;
            UpdatePreviousGameList();
            if (bVidAutoSwitchFull)
            {
                nVidFullscreen = 1;
                POST_INITIALISE_MESSAGE;
            }
            POST_INITIALISE_MESSAGE;
            break;
        }
        else
        {
            GameInpCheckMouse();
            AudSoundPlay();					// Restart sound
            bLoading = 0;
            break;
        }
    }

    case MENU_PREVIOUSGAMES1:
    case MENU_PREVIOUSGAMES2:
    case MENU_PREVIOUSGAMES3:
    case MENU_PREVIOUSGAMES4:
    case MENU_PREVIOUSGAMES5:
    case MENU_PREVIOUSGAMES6:
    case MENU_PREVIOUSGAMES7:
    case MENU_PREVIOUSGAMES8:
    case MENU_PREVIOUSGAMES9:
    case MENU_PREVIOUSGAMES10:
    {
        BurnerLoadDriver(szPrevGames[id - MENU_PREVIOUSGAMES1]);
        break;
    }
    case MENU_STARTNET:
        if (Init_Network())
        {
            MessageBox(hMainWnd, LoadStringEx(hAppInst, IDS_ERR_NO_NETPLAYDLL, true), LoadStringEx(hAppInst, IDS_ERR_ERROR, true), MB_OK);
            break;
        }
        if (!kNetGame)
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            DrvExit();
            DoNetGame();
            MenuEnableItems();
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        }
        break;
    case MENU_QUIT:
        AudBlankSound();
        if (nVidFullscreen)
        {
            nVidFullscreen = 0;
            VidExit();
        }
        if (bDrvOkay)
        {
            DrvExit();
            if (kNetGame)
            {
                kNetGame = 0;
                Kaillera_End_Game();
                DeActivateChat();
                PostQuitMessage(0);
            }
            bCheatsAllowed = true;						// reenable cheats netplay has ended

            ScrnSize();
            ScrnTitle();
            MenuEnableItems();
            nDialogSelect = -1;
            nBurnDrvActive = ~0U;

            POST_INITIALISE_MESSAGE;
        }
        break;

    case MENU_EXIT:
        if (kNetGame)
        {
            kNetGame = 0;
            //				kailleraEndGame();
            Kaillera_End_Game();
            DeActivateChat();
        }
        PostQuitMessage(0);
        return;

    case MENU_PAUSE:
        if (bDrvOkay && !kNetGame)
        {
            SetPauseMode(!bRunPause);
        }
        else
        {
            SetPauseMode(0);
        }
        break;

    case MENU_INPUT:
        AudBlankSound();
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, false);
            InpdCreate();
        }
        break;

    case MENU_DIPSW:
        AudBlankSound();
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            InpDIPSWCreate();
        }
        break;

    case MENU_SETCPUCLOCK:
        AudBlankSound();
        CPUClockDialog();
        MenuEnableItems();
        GameInpCheckMouse();
        break;
    case MENU_RESETCPUCLOCK:
        nBurnCPUSpeedAdjust = 0x0100;
        MenuEnableItems();
        break;

    case MENU_MEMCARD_CREATE:
        if (bDrvOkay && UseDialogs() && !kNetGame && (BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            MemCardEject();
            MemCardCreate();
            MemCardInsert();
            GameInpCheckMouse();
        }
        break;
    case MENU_MEMCARD_SELECT:
        if (bDrvOkay && UseDialogs() && !kNetGame && (BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            MemCardEject();
            MemCardSelect();
            MemCardInsert();
            GameInpCheckMouse();
        }
        break;
    case MENU_MEMCARD_INSERT:
        if (!kNetGame && (BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            MemCardInsert();
        }
        break;
    case MENU_MEMCARD_EJECT:
        if (!kNetGame && (BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            MemCardEject();
        }
        break;

    case MENU_MEMCARD_TOGGLE:
        if (bDrvOkay && !kNetGame && (BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            MemCardToggle();
        }
        break;

    case MENU_STATE_LOAD_DIALOG:
        if (UseDialogs() && !kNetGame)
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudSoundStop();
            StatedLoad(0);
            GameInpCheckMouse();
            AudSoundPlay();
        }
        break;
    case MENU_STATE_SAVE_DIALOG:
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            StatedSave(0);
            GameInpCheckMouse();
        }
        return;
    case MENU_STATE_PREVSLOT:
    {
        TCHAR szString[256];

        nSavestateSlot--;
        if (nSavestateSlot < 1)
        {
            nSavestateSlot = 1;
        }
        _sntprintf(szString, 256, LoadStringEx(hAppInst, IDS_STATE_ACTIVESLOT, true), nSavestateSlot);
        VidSNewShortMsg(szString);
        PausedRedraw();
        break;
    }
    case MENU_STATE_NEXTSLOT:
    {
        TCHAR szString[256];

        nSavestateSlot++;
        if (nSavestateSlot > 8)
        {
            nSavestateSlot = 8;
        }
        _sntprintf(szString, 256, LoadStringEx(hAppInst, IDS_STATE_ACTIVESLOT, true), nSavestateSlot);
        VidSNewShortMsg(szString);
        PausedRedraw();
        break;
    }
    case MENU_STATE_UNDO:
        scrnSSUndo();
        break;
    case MENU_STATE_LOAD_SLOT:
        if (bDrvOkay && !kNetGame)
        {
            if (StatedLoad(nSavestateSlot) == 0)
            {
                VidSNewShortMsg(LoadStringEx(hAppInst, IDS_STATE_LOADED, true));
            }
            else
            {
                VidSNewShortMsg(LoadStringEx(hAppInst, IDS_STATE_LOAD_ERROR, true), 0xFF3F3F);
            }
            PausedRedraw();
        }
        break;
    case MENU_STATE_SAVE_SLOT:
        if (bDrvOkay)
        {
            if (StatedSave(nSavestateSlot) == 0)
            {
                VidSNewShortMsg(LoadStringEx(hAppInst, IDS_STATE_SAVED, true));
            }
            else
            {
                VidSNewShortMsg(LoadStringEx(hAppInst, IDS_STATE_SAVE_ERROR, true), 0xFF3F3F);
                SetPauseMode(1);
            }
            PausedRedraw();
        }
        break;

    case MENU_ALLRAM:
        bDrvSaveAll = !bDrvSaveAll;
        break;

    case MENU_NOSTRETCH:
        bVidCorrectAspect = 0;
        bVidFullStretch = 0;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_STRETCH:
        bVidFullStretch = true;
        if (bVidFullStretch)
        {
            bVidCorrectAspect = 0;
        }
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_ASPECT:
        bVidCorrectAspect = true;
        if (bVidCorrectAspect)
        {
            bVidFullStretch = 0;
        }
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_TRIPLE:
        bVidTripleBuffer = !bVidTripleBuffer;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_BLITTER_1:
        VidSelect(0);
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_BLITTER_2:
        VidSelect(1);
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_BLITTER_3:
        VidSelect(2);
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_BLITTER_4:
        VidSelect(3);
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_BLITTER_5:
        VidSelect(4);
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_RES_ARCADE:
        bVidArcaderesHor = !bVidArcaderesHor;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        break;

    case MENU_SINGLESIZESCREEN:
        nScreenSizeHor = 1;
        bVidArcaderesHor = false;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        break;
    case MENU_DOUBLESIZESCREEN:
        nScreenSizeHor = 2;
        bVidArcaderesHor = false;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        break;
    case MENU_TRIPLESIZESCREEN:
        nScreenSizeHor = 3;
        bVidArcaderesHor = false;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        break;
    case MENU_QUADSIZESCREEN:
        nScreenSizeHor = 4;
        bVidArcaderesHor = false;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        break;

    case MENU_RES_1:
        nVidHorWidth = VidPreset[0].nWidth;
        nVidHorHeight = VidPreset[0].nHeight;
        bVidArcaderesHor = false;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
            nVidWidth	= nVidHorWidth;
            nVidHeight	= nVidHorHeight;
        }
        break;
    case MENU_RES_2:
        nVidHorWidth = VidPreset[1].nWidth;
        nVidHorHeight = VidPreset[1].nHeight;
        bVidArcaderesHor = false;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
            nVidWidth	= nVidHorWidth;
            nVidHeight	= nVidHorHeight;
        }
        break;
    case MENU_RES_3:
        nVidHorWidth = VidPreset[2].nWidth;
        nVidHorHeight = VidPreset[2].nHeight;
        bVidArcaderesHor = false;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
            nVidWidth	= nVidHorWidth;
            nVidHeight	= nVidHorHeight;
        }
        break;
    case MENU_RES_4:
        nVidHorWidth = VidPreset[3].nWidth;
        nVidHorHeight = VidPreset[3].nHeight;
        bVidArcaderesHor = false;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
            nVidWidth	= nVidHorWidth;
            nVidHeight	= nVidHorHeight;
        }
        break;


    case MENU_RES_OTHER:
        bVidArcaderesHor = false;
        nScreenSizeHor = 0;
        if ((bDrvOkay) && !(BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeHor;
            bVidArcaderes = bVidArcaderesHor;
        }
        AudBlankSound();
        InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        ResCreate(HORIZONTAL_ORIENTED_RES);
        GameInpCheckMouse();
        break;

    // Vertical
    case MENU_RES_ARCADE_VERTICAL:
        bVidArcaderesVer = !bVidArcaderesVer;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        break;

    case MENU_SINGLESIZESCREEN_VERTICAL:
        nScreenSizeVer = 1;
        bVidArcaderesVer = false;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        break;
    case MENU_DOUBLESIZESCREEN_VERTICAL:
        nScreenSizeVer = 2;
        bVidArcaderesVer = false;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        break;
    case MENU_TRIPLESIZESCREEN_VERTICAL:
        nScreenSizeVer = 3;
        bVidArcaderesVer = false;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        break;
    case MENU_QUADSIZESCREEN_VERTICAL:
        nScreenSizeVer = 4;
        bVidArcaderesVer = false;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        break;

    case MENU_RES_1_VERTICAL:
        nVidVerWidth = VidPresetVer[0].nWidth;
        nVidVerHeight = VidPresetVer[0].nHeight;
        bVidArcaderesVer = false;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
            nVidWidth	= nVidVerWidth;
            nVidHeight	= nVidVerHeight;
        }
        break;
    case MENU_RES_2_VERTICAL:
        nVidVerWidth = VidPresetVer[1].nWidth;
        nVidVerHeight = VidPresetVer[1].nHeight;
        bVidArcaderesVer = false;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
            nVidWidth	= nVidVerWidth;
            nVidHeight	= nVidVerHeight;
        }
        break;
    case MENU_RES_3_VERTICAL:
        nVidVerWidth = VidPresetVer[2].nWidth;
        nVidVerHeight = VidPresetVer[2].nHeight;
        bVidArcaderesVer = false;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
            nVidWidth	= nVidVerWidth;
            nVidHeight	= nVidVerHeight;
        }
        break;
    case MENU_RES_4_VERTICAL:
        nVidVerWidth = VidPresetVer[3].nWidth;
        nVidVerHeight = VidPresetVer[3].nHeight;
        bVidArcaderesVer = false;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
            nVidWidth	= nVidVerWidth;
            nVidHeight	= nVidVerHeight;
        }
        break;

    case MENU_RES_OTHER_VERTICAL:
        bVidArcaderesVer = false;
        nScreenSizeVer = 0;
        if ((bDrvOkay) && (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL))
        {
            nScreenSize = nScreenSizeVer;
            bVidArcaderes = bVidArcaderesVer;
        }
        AudBlankSound();
        InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        ResCreate(VERTICAL_ORIENTED_RES);
        GameInpCheckMouse();
        break;

    case MENU_16:
        nVidDepth = 16;
        break;

    case MENU_24:
        nVidDepth = 24;
        break;

    case MENU_32:
        nVidDepth = 32;
        break;

    case MENU_GAMMA_DO:
        bDoGamma = !bDoGamma;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;

    case MENU_GAMMA_DARKER:
        nGamma = 1.25;
        ComputeGammaLUT();
        bDoGamma = 1;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;

    case MENU_GAMMA_LIGHTER:
        nGamma = 0.80;
        ComputeGammaLUT();
        bDoGamma = 1;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;

    case MENU_GAMMA_OTHER:
    {
        if (UseDialogs())
        {
            double nOldGamma = nGamma;
            bDoGamma = 1;
            if (bDrvOkay)
            {
                if (nVidSelect == 1)
                {
                    VidInit();
                }
                SetBurnHighCol(nVidImageDepth);
            }
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            GammaDialog();
            if (nGamma > 0.999 && nGamma < 1.001)
            {
                nGamma = nOldGamma;
                bDoGamma = 0;
                if (nVidSelect == 1)
                {
                    VidInit();
                }
                SetBurnHighCol(nVidImageDepth);
            }
            else
            {
                bDoGamma = 1;
                ComputeGammaLUT();
            }
            if (bDrvOkay)
            {
                VidRecalcPal();
            }
            GameInpCheckMouse();
        }
        break;
    }

    case MENU_GAMMA_USE_HARDWARE:
        bVidUseHardwareGamma = 1;
        bHardwareGammaOnly = 0;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            VidRecalcPal();
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;
    case MENU_GAMMA_HARDWARE_ONLY:
        bVidUseHardwareGamma = 1;
        bHardwareGammaOnly = 1;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            VidRecalcPal();
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;
    case MENU_GAMMA_SOFTWARE_ONLY:
        bVidUseHardwareGamma = 0;
        bHardwareGammaOnly = 0;
        if (bDrvOkay)
        {
            if (nVidSelect == 1)
            {
                VidInit();
            }
            SetBurnHighCol(nVidImageDepth);
            VidRecalcPal();
            if (bRunPause)
            {
                VidRedraw();
            }
        }
        break;

    case MENU_FULL:
        if (bDrvOkay || nVidFullscreen)
        {
            nVidFullscreen = !nVidFullscreen;
            POST_INITIALISE_MESSAGE;
        }
        return;

    case MENU_AUTOSWITCHFULL:
        bVidAutoSwitchFull = !bVidAutoSwitchFull;
        break;

    case MENU_BASIC_MEMAUTO:
    case MENU_SOFTFX_MEMAUTO:
        nVidTransferMethod = -1;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_BASIC_VIDEOMEM:
    case MENU_SOFTFX_VIDEOMEM:
        nVidTransferMethod = 0;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_BASIC_SYSMEM:
    case MENU_SOFTFX_SYSMEM:
        nVidTransferMethod = 1;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_SINGLESIZEWINDOW:
    case MENU_DOUBLESIZEWINDOW:
    case MENU_TRIPLESIZEWINDOW:
    case MENU_QUADSIZEWINDOW:
		nWindowSize = id - MENU_SINGLESIZEWINDOW;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_MAXIMUMSIZEWINDOW:
        nWindowSize = 9999;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_MONITORAUTOCHECK:
        bMonitorAutoCheck = !bMonitorAutoCheck;
        if (bMonitorAutoCheck) MonitorAutoCheck();
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_ASPECTNORMAL:
        bMonitorAutoCheck = false;
        nVidScrnAspectX = 4;
        nVidScrnAspectY = 3;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_ASPECTLCD:
        bMonitorAutoCheck = false;
        nVidScrnAspectX = 5;
        nVidScrnAspectY = 4;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_ASPECTWIDE:
        bMonitorAutoCheck = false;
        nVidScrnAspectX = 16;
        nVidScrnAspectY = 9;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_ASPECTWIDELCD:
        bMonitorAutoCheck = false;
        nVidScrnAspectX = 16;
        nVidScrnAspectY = 10;
        POST_INITIALISE_MESSAGE;
        break;
    case MENU_MONITORMIRRORVERT:
        nVidRotationAdjust ^= 2;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_ROTATEVERTICAL:
        nVidRotationAdjust ^= 1;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_FORCE60HZ:
        bForce60Hz = !bForce60Hz;
        break;

    case MENU_VIDEOVSYNC:
        bVidVSync = !bVidVSync;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_AUTOFRAMESKIP:
        bAlwaysDrawFrames = !bAlwaysDrawFrames;
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_AUD_PLUGIN_1:
        AudSelect(0);
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_AUD_PLUGIN_2:
        AudSelect(1);
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_DSOUND_NOSOUND:
        if (!bDrvOkay)
        {
            nAudSampleRate[0] = 0;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_DSOUND_11025:
        if (!bDrvOkay)
        {
            nAudSampleRate[0] = 11025;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_DSOUND_22050:
        if (!bDrvOkay)
        {
            nAudSampleRate[0] = 22050;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_DSOUND_44100:
        if (!bDrvOkay)
        {
            nAudSampleRate[0] = 44100;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_DSOUND_48000:
        if (!bDrvOkay)
        {
            nAudSampleRate[0] = 48000;
            POST_INITIALISE_MESSAGE;
        }
        break;

    case MENU_XAUDIO_NOSOUND:
        if (!bDrvOkay)
        {
            nAudSampleRate[1] = 0;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_XAUDIO_11025:
        if (!bDrvOkay)
        {
            nAudSampleRate[1] = 11025;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_XAUDIO_22050:
        if (!bDrvOkay)
        {
            nAudSampleRate[1] = 22050;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_XAUDIO_44100:
        if (!bDrvOkay)
        {
            nAudSampleRate[1] = 44100;
            POST_INITIALISE_MESSAGE;
        }
        break;
    case MENU_XAUDIO_48000:
        if (!bDrvOkay)
        {
            nAudSampleRate[1] = 48000;
            POST_INITIALISE_MESSAGE;
        }
        break;

    case MENU_FRAMES:
        if (UseDialogs())
        {
            if (!bDrvOkay)
            {
                //					AudBlankSound();
                NumDialCreate(0);
                POST_INITIALISE_MESSAGE;
            }
        }
        break;

    case MENU_INTERPOLATE_0:
        nInterpolation = 0;
        break;
    case MENU_INTERPOLATE_1:
        nInterpolation = 1;
        break;
    case MENU_INTERPOLATE_3:
        nInterpolation = 3;
        break;

    case MENU_INTERPOLATE_FM_0:
        nFMInterpolation = 0;
        break;
    case MENU_INTERPOLATE_FM_1:
        nFMInterpolation = 1;
        break;
    case MENU_INTERPOLATE_FM_3:
        nFMInterpolation = 3;
        break;

    case MENU_DSOUND_BASS:
        nAudDSPModule[0] = !nAudDSPModule[0];
        break;

    case MENU_XAUDIO_BASS:
        nAudDSPModule[1] ^= 1;
        break;

    case MENU_XAUDIO_REVERB:
        nAudDSPModule[1] ^= 2;
        break;

    case MENU_WLOGSTART:
        AudBlankSound();
        WaveLogStart();
        break;

    case MENU_WLOGEND:
        AudBlankSound();
        WaveLogStop();
        break;

    case MENU_AUTOPAUSE:
        bAutoPause = !bAutoPause;
        break;

    case MENU_PROCESSINPUT:
        bAlwaysProcessKeyboardInput = !bAlwaysProcessKeyboardInput;
        break;

    case MENU_DISPLAYINDICATOR:
        nVidSDisplayStatus = !nVidSDisplayStatus;
        //			VidRedraw();
        VidPaint(2);
        break;

    case MENU_LANGUAGE_GL_SELECT:
        if (UseDialogs())
        {
            FBALocaliseGamelistLoadTemplate();
        }
        break;
    case MENU_LANGUAGE_GL_EXPORT:
        if (UseDialogs())
        {
            FBALocaliseGamelistCreateTemplate();
        }
        break;
    case MENU_LANGUAGE_GL_RESET:
        szGamelistLocalisationTemplate[0] = _T('\0');
        nGamelistLocalisationActive = false;
        break;
    case MENU_INPUT_AUTOFIRE_RATE_1:
        nAutoFireRate = 22;
        break;
    case MENU_INPUT_AUTOFIRE_RATE_2:
        nAutoFireRate = 12;
        break;
    case MENU_INPUT_AUTOFIRE_RATE_3:
        nAutoFireRate =  8;
        break;
    case MENU_INPUT_AUTOFIRE_RATE_4:
        nAutoFireRate =  4;
        break;

    case MENU_PRIORITY_REALTIME:
        nAppThreadPriority = THREAD_PRIORITY_TIME_CRITICAL;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;
    case MENU_PRIORITY_HIGH:
        nAppThreadPriority = THREAD_PRIORITY_HIGHEST;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;
    case MENU_PRIORITY_ABOVE_NORMAL:
        nAppThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;
    case MENU_PRIORITY_NORMAL:
        nAppThreadPriority = THREAD_PRIORITY_NORMAL;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;
    case MENU_PRIORITY_BELOW_NORMAL:
        nAppThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;
    case MENU_PRIORITY_LOW:
        nAppThreadPriority = THREAD_PRIORITY_LOWEST;
        SetThreadPriority(GetCurrentThread(), nAppThreadPriority);
        break;

    case MENU_CLRMAME_PRO_XML:
        if (UseDialogs())
        {
            CreateDatfileWindows(DAT_ARCADE_ONLY);
        }
        break;

    case MENU_ENABLECHEAT:
        AudBlankSound();
        InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
        InpCheatCreate();
        break;

    case MENU_CHEATSEARCH_START:
    {
        CheatSearchStart();

        TCHAR szText[100];
        _stprintf(szText, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_NEW, true));
        VidSAddChatMsg(NULL, 0xFFFFFF, szText, 0xFFBFBF);

        EnableMenuItem(hMenu, MENU_CHEATSEARCH_NOCHANGE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_CHANGE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_DECREASE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_INCREASE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_DUMPFILE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_EXIT, MF_ENABLED | MF_BYCOMMAND);
        break;
    }

    case MENU_CHEATSEARCH_NOCHANGE:
    {
        TCHAR tmpmsg[256];
        unsigned int nValues = CheatSearchValueNoChange();

        _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_ADD_MATCH, true), nValues);
        VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);

        if (nValues <= CHEATSEARCH_SHOWRESULTS)
        {
            for (unsigned int i = 0; i < nValues; i++)
            {
                _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_RESULTS, true), CheatSearchShowResultAddresses[i], CheatSearchShowResultValues[i]);
                VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);
            }
        }
        break;
    }

    case MENU_CHEATSEARCH_CHANGE:
    {
        TCHAR tmpmsg[256];
        unsigned int nValues = CheatSearchValueChange();

        _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_ADD_MATCH, true), nValues);
        VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);

        if (nValues <= CHEATSEARCH_SHOWRESULTS)
        {
            for (unsigned int i = 0; i < nValues; i++)
            {
                _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_RESULTS, true), CheatSearchShowResultAddresses[i], CheatSearchShowResultValues[i]);
                VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);
            }
        }
        break;
    }

    case MENU_CHEATSEARCH_DECREASE:
    {
        TCHAR tmpmsg[256];
        unsigned int nValues = CheatSearchValueDecreased();

        _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_ADD_MATCH, true), nValues);
        VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);

        if (nValues <= CHEATSEARCH_SHOWRESULTS)
        {
            for (unsigned int i = 0; i < nValues; i++)
            {
                _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_RESULTS, true), CheatSearchShowResultAddresses[i], CheatSearchShowResultValues[i]);
                VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);
            }
        }
        break;
    }

    case MENU_CHEATSEARCH_INCREASE:
    {
        TCHAR tmpmsg[256];

        unsigned int nValues = CheatSearchValueIncreased();

        _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_ADD_MATCH, true), nValues);
        VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);

        if (nValues <= CHEATSEARCH_SHOWRESULTS)
        {
            for (unsigned int i = 0; i < nValues; i++)
            {
                _stprintf(tmpmsg, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_RESULTS, true), CheatSearchShowResultAddresses[i], CheatSearchShowResultValues[i]);
                VidSAddChatMsg(NULL, 0xFFFFFF, tmpmsg, 0xFFBFBF);
            }
        }
        break;
    }

    case MENU_CHEATSEARCH_DUMPFILE:
    {
        CheatSearchDumptoFile();
        break;
    }

    case MENU_CHEATSEARCH_EXIT:
    {
        CheatSearchExit();

        TCHAR szText[100];
        _stprintf(szText, LoadStringEx(hAppInst, IDS_CHEAT_SEARCH_EXIT, true));
        VidSAddChatMsg(NULL, 0xFFFFFF, szText, 0xFFBFBF);

        EnableMenuItem(hMenu, MENU_CHEATSEARCH_NOCHANGE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_CHANGE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_DECREASE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_INCREASE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_DUMPFILE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CHEATSEARCH_EXIT, MF_GRAYED | MF_BYCOMMAND);
        break;
    }

    case MENU_SAVEGAMEINPUTNOW:
        ConfigGameSave(true);
        break;

    case MENU_SAVEGAMEINPUT:
        bSaveInputs = !bSaveInputs;
        break;

    case MENU_SAVESET:
        ConfigAppSave();
        break;

    case MENU_LOADSET:
        ConfigAppLoad();
        POST_INITIALISE_MESSAGE;
        break;

    case MENU_ABOUT:
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            AboutCreate();
            GameInpCheckMouse();
        }
        break;
    case MENU_SYSINFO:
        if (UseDialogs())
        {
            InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
            AudBlankSound();
            SystemInfoCreate();
            GameInpCheckMouse();
        }
        break;
    case MENU_WWW_HOME:
        if (!nVidFullscreen)
        {
            ShellExecute(NULL, _T("open"), _T("http://www.fbalpha.com/"), NULL, NULL, SW_SHOWNORMAL);
        }
        break;

    case MENU_WWW_NSFORUM:
        if (!nVidFullscreen)
        {
            ShellExecute(NULL, _T("open"), _T("http://neosource.1emu.net/forums/"), NULL, NULL, SW_SHOWNORMAL);
        }
        break;

        //		default:
        //			printf("  * Command %i sent.\n");
    }

    switch (nVidSelect)
    {
    case 0:
    {
        switch (id)
        {
        // Options for the Default DirectDraw blitter
        case MENU_BASIC_NORMAL:
            bVidScanlines = 0;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_BASIC_SCAN:
            bVidScanlines = 1;
            bVidScanHalf = 0;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_SCAN50:
            bVidScanlines = 1;
            bVidScanHalf = 1;
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_BASIC_ROTSCAN:
            bVidScanRotate = !bVidScanRotate;
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_FORCE_FLIP:
            bVidForceFlip = !bVidForceFlip;
            POST_INITIALISE_MESSAGE;
            break;
        }
        break;
    }
    case 1:
    {
        switch (id)
        {
        //	Options for the Direct3D blitter
        case MENU_DISABLEFX:
            bVidBilinear = 0;
            bVidScanlines = 0;
            nVidBlitterOpt[nVidSelect] &= 0xF40200FF;
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_BILINEAR:
            bVidBilinear = !bVidBilinear;
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;

        case MENU_PHOSPHOR:
            bVidScanDelay = !bVidScanDelay;
            break;

        case MENU_ENHANCED_NORMAL:
            nVidBlitterOpt[nVidSelect] &= ~0x00110000;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_ENHANCED_SCAN:
            bVidScanlines = !bVidScanlines;
            nVidBlitterOpt[nVidSelect] &= ~0x00010000;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_RGBEFFECTS:
            nVidBlitterOpt[nVidSelect] &= ~0x00100000;
            nVidBlitterOpt[nVidSelect] |= 0x00010000;
            bVidScanlines = 0;
            ScrnSize();
            VidInit();
            if (bVidScanlines)
            {
                ScrnSize();
                VidInit();
            }
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;
        case MENU_3DPROJECTION:
            nVidBlitterOpt[nVidSelect] &= ~0x00010000;
            nVidBlitterOpt[nVidSelect] |= 0x00100000;
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_EFFECT_AUTO:
            nVidBlitterOpt[nVidSelect] &= ~0x001000000;
            nVidBlitterOpt[nVidSelect] |= 0x00030000;
            POST_INITIALISE_MESSAGE;
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;
        case MENU_EFFECT_01:
        case MENU_EFFECT_02:
        case MENU_EFFECT_03:
        case MENU_EFFECT_04:
        case MENU_EFFECT_05:
        case MENU_EFFECT_06:
        case MENU_EFFECT_07:
        case MENU_EFFECT_08:
        case MENU_EFFECT_09:
        case MENU_EFFECT_0A:
        case MENU_EFFECT_0B:
        case MENU_EFFECT_0C:
        case MENU_EFFECT_0D:
        case MENU_EFFECT_0E:
        case MENU_EFFECT_0F:
        case MENU_EFFECT_10:
            nVidBlitterOpt[nVidSelect] &= ~0x001300FF;
            nVidBlitterOpt[nVidSelect] |= 0x00010008 + id - MENU_EFFECT_01;
            POST_INITIALISE_MESSAGE;
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;

        case MENU_ENHANCED_ROTSCAN:
            bVidScanRotate = !bVidScanRotate;
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_PRESCALE:
            nVidBlitterOpt[nVidSelect] ^= 0x01000000;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_ENHANCED_SCANINTENSITY:
            if (UseDialogs())
            {
                InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
                AudBlankSound();
                if (!bVidScanlines)
                {
                    bVidScanlines = 1;
                    ScrnSize();
                    VidInit();
                    VidRedraw();
                }
                if (nVidBlitterOpt[nVidSelect] & 0x00010000)
                {
                    nVidBlitterOpt[nVidSelect] &= ~0x00010000;
                    ScrnSize();
                    VidInit();
                    VidRedraw();
                }
                ScanlineDialog();
                GameInpCheckMouse();
            }
            break;

        case MENU_PHOSPHORINTENSITY:
            if (UseDialogs())
            {
                InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
                AudBlankSound();
                PhosphorDialog();
                bVidScanDelay = 1;
            }
            break;

        case MENU_3DSCREENANGLE:
            if (UseDialogs())
            {
                InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
                AudBlankSound();
                if ((nVidBlitterOpt[nVidSelect] & 0x00100000) == 0)
                {
                    nVidBlitterOpt[nVidSelect] &= ~0x00010000;
                    nVidBlitterOpt[nVidSelect] |= 0x00100000;
                    ScrnSize();
                    VidInit();
                    VidRedraw();
                }
                ScreenAngleDialog();
                if (!bRunPause)
                {
                    GameInpCheckMouse();
                }
            }
            break;

        case MENU_FORCE_16BIT:
            bVidForce16bit = !bVidForce16bit;
            VidInit();
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;
        case MENU_TEXTUREMANAGE:
            if (nVidTransferMethod != 0)
            {
                nVidTransferMethod = 0;
            }
            else
            {
                nVidTransferMethod = 1;
            }
            VidInit();
            if (bVidOkay && (bRunPause || !bDrvOkay))
            {
                VidRedraw();
            }
            break;
        }
        break;
    }
    
    case 2:
        switch (id)
        {
        // Options for the DirectX Graphics 9 blitter
        case MENU_DX9_POINT:
            nVidBlitterOpt[nVidSelect] &= ~(3 << 24);
            nVidBlitterOpt[nVidSelect] |=  (0 << 24);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_LINEAR:
            nVidBlitterOpt[nVidSelect] &= ~(3 << 24);
            nVidBlitterOpt[nVidSelect] |=  (1 << 24);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_CUBIC:
            nVidBlitterOpt[nVidSelect] &= ~(3 << 24);
            nVidBlitterOpt[nVidSelect] |=  (2 << 24);
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_DX9_CUBIC_LIGHT:
            dVidCubicB = 0.0;
            dVidCubicC = 0.0;
            VidRedraw();
            break;
        case MENU_DX9_CUBIC_BSPLINE:
            dVidCubicB = 1.0;
            dVidCubicC = 0.0;
            VidRedraw();
            break;
        case MENU_DX9_CUBIC_NOTCH:
            dVidCubicB =  3.0 / 2.0;
            dVidCubicC = -0.25;
            VidRedraw();
            break;
        case MENU_DX9_CUBIC_OPTIMAL:
            dVidCubicB = 1.0 / 3.0;
            dVidCubicC = 1.0 / 3.0;
            VidRedraw();
            break;
        case MENU_DX9_CUBIC_CATMULL:
            dVidCubicB = 0.0;
            dVidCubicC = 0.5;
            VidRedraw();
            break;
        case MENU_DX9_CUBIC_SHARP:
            dVidCubicB = 0.0;
            dVidCubicC = 1.0;
            VidRedraw();
            break;
        case MENU_EXP_SCAN:
            bVidScanlines = !bVidScanlines;
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_EXP_SCANINTENSITY:
            if (UseDialogs())
            {
                InputSetCooperativeLevel(false, bAlwaysProcessKeyboardInput);
                AudBlankSound();
                if (!bVidScanlines)
                {
                    bVidScanlines = 1;
                    ScrnSize();
                    VidInit();
                    VidRedraw();
                }
                ScanlineDialog();
                GameInpCheckMouse();
            }
            break;

        case MENU_DX9_FPTERXTURES:
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_DX9_FORCE_PS14:
            nVidBlitterOpt[nVidSelect] ^=  (1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;

        case MENU_DX9_CUBIC0:
            nVidBlitterOpt[nVidSelect] &= ~(7 << 28);
            nVidBlitterOpt[nVidSelect] |=  (0 << 28);

            nVidBlitterOpt[nVidSelect] |=  (1 <<  8);
            nVidBlitterOpt[nVidSelect] |=  (1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_CUBIC1:
            nVidBlitterOpt[nVidSelect] &= ~(7 << 28);
            nVidBlitterOpt[nVidSelect] |=  (1 << 28);

            nVidBlitterOpt[nVidSelect] |=  (1 <<  8);
            nVidBlitterOpt[nVidSelect] |=  (1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_CUBIC2:
            nVidBlitterOpt[nVidSelect] &= ~(7 << 28);
            nVidBlitterOpt[nVidSelect] |=  (2 << 28);

            nVidBlitterOpt[nVidSelect] |=  (1 <<  8);
            nVidBlitterOpt[nVidSelect] |=  (1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_CUBIC3:
            nVidBlitterOpt[nVidSelect] &= ~(7 << 28);
            nVidBlitterOpt[nVidSelect] |=  (3 << 28);

            nVidBlitterOpt[nVidSelect] |=  (1 <<  8);
            nVidBlitterOpt[nVidSelect] |=  (1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;
        case MENU_DX9_CUBIC4:
            nVidBlitterOpt[nVidSelect] &= ~(7 << 28);
            nVidBlitterOpt[nVidSelect] |=  (4 << 28);

            nVidBlitterOpt[nVidSelect] &= ~(1 <<  8);
            nVidBlitterOpt[nVidSelect] &= ~(1 <<  9);
            POST_INITIALISE_MESSAGE;
            break;

        }
        break;
    }

    if (hwndChat)
    {
        switch (codeNotify)
        {
        case EN_CHANGE:
        {
            bEditTextChanged = true;
            SendMessage(hwndChat, WM_GETTEXT, (WPARAM)MAX_CHAT_SIZE + 1, (LPARAM)EditText);
            break;
        }
        case EN_KILLFOCUS:
        {
            SetFocus(hwndChat);
            break;
        }
        case EN_MAXTEXT:
        {
            VidSNewShortMsg(LoadStringEx(hAppInst, IDS_NETPLAY_TOOMUCH, true), 0xFF3F3F);
            break;
        }
        }
    }

    MenuUpdate();
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

static void OnSize(HWND, UINT state, int cx, int cy)
{
    if (state == SIZE_MINIMIZED)
    {
        bMaximised = false;
    }
    else
    {
        bool bSizeChanged = false;
        if (hwndChat)
        {
            MoveWindow(hwndChat, 0, cy - 32, cx, 32, FALSE);
        }

        if (state == SIZE_MAXIMIZED)
        {
            if (!bMaximised)
            {
                bSizeChanged = true;
            }
            bMaximised = true;
        }
        if (state == SIZE_RESTORED)
        {
            if (bMaximised)
            {
                bSizeChanged = true;
            }
            bMaximised = false;
        }

        if (bSizeChanged)
        {
            RefreshWindow(true);
        }
        else
        {
            RefreshWindow(false);
        }
    }
}

static void OnEnterSizeMove(HWND)
{
    RECT rect;

    AudBlankSound();

    GetClientRect(hMainWnd, &rect);
    nPrevWidth = rect.right;
    nPrevHeight = rect.bottom;
}

static void OnExitSizeMove(HWND)
{
    RECT rect;

    GetClientRect(hMainWnd, &rect);
    if (rect.right != nPrevWidth || rect.bottom != nPrevHeight)
    {
        RefreshWindow(true);
    }
}

static void OnEnterIdle(HWND /*hwnd*/, UINT /*source*/, HWND /*hwndSource*/)
{
    MSG Message;

    // Modeless dialog is idle
    while (kNetGame && !PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE))
    {
        RunIdle();
    }
}

static LRESULT CALLBACK ScrnProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE,			OnCreate);
        HANDLE_MSG(hWnd, WM_ACTIVATEAPP,	OnActivateApp);
        HANDLE_MSGB(hWnd, WM_PAINT,			OnPaint);
        HANDLE_MSG(hWnd, WM_CLOSE,			OnClose);
        HANDLE_MSG(hWnd, WM_DESTROY,		OnDestroy);
        HANDLE_MSG(hWnd, WM_COMMAND,		OnCommand);

    // We can't use the macro from windowsx.h macro for this one
    case WM_SYSCOMMAND:
    {
        if (OnSysCommand(hWnd, (UINT)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)))
        {
            return 0;
        }
        break;
    }
    // - dink - handle return from Hibernation
    case WM_POWERBROADCAST:
    {
        if (wParam == PBT_APMRESUMESUSPEND || wParam == PBT_APMSUSPEND)
        {
            bBackFromHibernation = 1;
        }
        break;
    }
	// - dink - end
	case WM_ENTERMENULOOP:
		bRunPause++;
		break;
	case WM_EXITMENULOOP:
		bRunPause--;
		break;
    HANDLE_MSG(hWnd, WM_SIZE,			OnSize);
    HANDLE_MSG(hWnd, WM_ENTERSIZEMOVE,	OnEnterSizeMove);
    HANDLE_MSG(hWnd, WM_EXITSIZEMOVE,	OnExitSizeMove);
    HANDLE_MSG(hWnd, WM_ENTERIDLE,		OnEnterIdle);
    HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK,	OnLButtonDblClk);
    HANDLE_MSG(hWnd, WM_NOTIFY,			OnNotify);
    HANDLE_MSG(hWnd, WM_DISPLAYCHANGE,	OnDisplayChange);
    }

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}


static int ScrnRegister()
{
    WNDCLASSEX WndClassEx;
    ATOM Atom = 0;

    // Register the window class
    memset(&WndClassEx, 0, sizeof(WndClassEx)); 		// Init structure to all zeros
    WndClassEx.cbSize			= sizeof(WndClassEx);
    WndClassEx.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_CLASSDC;// These cause flicker in the toolbar
    WndClassEx.lpfnWndProc		= ScrnProc;
    WndClassEx.hInstance		= hAppInst;
    WndClassEx.hIcon			= LoadIcon(hAppInst, MAKEINTRESOURCE(IDI_APP));
    WndClassEx.hCursor			= LoadCursor(NULL, IDC_ARROW);
    WndClassEx.hbrBackground	= static_cast<HBRUSH>( GetStockObject ( BLACK_BRUSH ));
    WndClassEx.lpszClassName	= szClass;
	WndClassEx.lpszMenuName     = MAKEINTRESOURCE(IDR_MENU);
    // Register the window class with the above information:
    Atom = RegisterClassEx(&WndClassEx);
    if (Atom)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int ScrnSize()
{
    int w, h, ew, eh;
    int nScrnWidth, nScrnHeight;
    int nBmapWidth = nVidImageWidth, nBmapHeight = nVidImageHeight;
    int nGameAspectX = 4, nGameAspectY = 3;
    int nMaxSize;

    SystemParametersInfo(SPI_GETWORKAREA, 0, &SystemWorkArea, 0);	// Find the size of the visible WorkArea

    if (hMainWnd == NULL || nVidFullscreen)
    {
        return 1;
    }

    if (bDrvOkay)
    {
        if ((BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL) && (nVidRotationAdjust & 1))
        {
            BurnDrvGetVisibleSize(&nBmapHeight, &nBmapWidth);
            BurnDrvGetAspect(&nGameAspectY, &nGameAspectX);
        }
        else
        {
            BurnDrvGetVisibleSize(&nBmapWidth, &nBmapHeight);
            BurnDrvGetAspect(&nGameAspectX, &nGameAspectY);
        }

        if (nBmapWidth <= 0 || nBmapHeight <= 0)
        {
            return 1;
        }
    }

    nDragX = GetSystemMetrics(SM_CXDRAG) / 2;
    nDragY = GetSystemMetrics(SM_CYDRAG) / 2;

    nScrnWidth = SystemWorkArea.right - SystemWorkArea.left;
    nScrnHeight = SystemWorkArea.bottom - SystemWorkArea.top;
    nMaxSize = nWindowSize+1;

    // Find the width and height
    w = nScrnWidth;
    h = nScrnHeight;

    // Find out how much space is taken up by the borders
    ew = GetSystemMetrics(SM_CXSIZEFRAME) << 1;
    eh = GetSystemMetrics(SM_CYSIZEFRAME) << 1;

    // Visual Studio 2012 (seems to have an issue with these, other reports on the web about it too
#if defined _MSC_VER
#if _MSC_VER >= 1700
    // using the old XP supporting SDK we don't need to alter anything
#if !defined BUILD_VS_XP_TARGET
    ew <<= 1;
    eh <<= 1;
#endif
#endif
#endif

    eh += GetSystemMetrics(SM_CYCAPTION);
    eh += nMenuHeight;

    if (!bVidScanlines)
    {
        // Subtract the border space
        w -= ew;
        h -= eh;
    }

    if ((bVidCorrectAspect || bVidFullStretch))
    {
        int ww = w;
        int hh = h;

        do
        {
            if (nBmapWidth < nBmapHeight && bVidScanRotate)
            {
                if (ww > nBmapWidth * nMaxSize)
                {
                    ww = nBmapWidth * nMaxSize;
                }
                if (hh > ww * nVidScrnAspectX * nGameAspectY * nScrnHeight / (nScrnWidth * nVidScrnAspectY * nGameAspectX))
                {
                    hh = ww * nVidScrnAspectX * nGameAspectY * nScrnHeight / (nScrnWidth * nVidScrnAspectY * nGameAspectX);
                }
            }
            else
            {
                if (hh > nBmapHeight * nMaxSize)
                {
                    hh = nBmapHeight * nMaxSize;
                }
                if (ww > hh * nVidScrnAspectY * nGameAspectX * nScrnWidth / (nScrnHeight * nVidScrnAspectX * nGameAspectY))
                {
                    ww = hh * nVidScrnAspectY * nGameAspectX * nScrnWidth / (nScrnHeight * nVidScrnAspectX * nGameAspectY);
                }
            }
        }
        while ((ww > w || hh > h) && nMaxSize-- > 1);
        w =	ww;
        h = hh;
    }
    else
    {
        while ((nBmapWidth * nMaxSize > w || nBmapHeight * nMaxSize > h) && nMaxSize > 1)
        {
            nMaxSize--;
        }

        if (w > nBmapWidth * nMaxSize || h > nBmapHeight * nMaxSize)
        {
            w = nBmapWidth * nMaxSize;
            h = nBmapHeight * nMaxSize;
        }
    }

    if (!bDrvOkay)
    {
        if (w < 304) w = 304;
        if (h < 224) h = 224;
    }

    RECT rect = { 0, 0, w, h };
    VidImageSize(&rect, nBmapWidth, nBmapHeight);
    w = rect.right - rect.left + ew;
    h = rect.bottom - rect.top + eh;

    MenuUpdate();
    bMaximised = false;
	SetWindowPos(hMainWnd,NULL,0,0,w,h,SWP_NOMOVE|SWP_NOZORDER);
    return 0;
}

int ScrnTitle()
{
    TCHAR szText[1024] = _T("");

    // Create window title
    if (bDrvOkay)
    {
        TCHAR *pszPosition = szText;
        TCHAR *pszName = BurnDrvGetText(DRV_FULLNAME);

        pszPosition += _sntprintf(szText, 1024, _T("%s"), pszName);
        while ((pszName = BurnDrvGetText(DRV_NEXTNAME | DRV_FULLNAME)) != NULL)
        {
            if (pszPosition + _tcslen(pszName) - 1024 > szText)
            {
                break;
            }
            pszPosition += _stprintf(pszPosition, _T(SEPERATOR_2) _T("%s"), pszName);
        }
    }
    else
    {
        _stprintf(szText, _T(APP_TITLE) _T( " V%.20s"), szAppBurnVer);
    }

    SetWindowText(hMainWnd, szText);
    return 0;
}


// Init the screen window (create it)
int ScrnInit()
{
    int nWindowStyles, nWindowExStyles;

    ScrnExit();

    if (ScrnRegister() != 0)
    {
        return 1;
    }

    if (nVidFullscreen)
    {
        nWindowStyles = WS_POPUP;
        nWindowExStyles = 0;
		nMenuHeight = 0;
    }
    else
    {
        nWindowStyles = WS_OVERLAPPEDWINDOW;
        nWindowExStyles = 0;
		nMenuHeight = GetSystemMetrics(SM_CYMENU);
    }
	
    hMainWnd = CreateWindowEx(nWindowExStyles, szClass, _T(APP_TITLE), nWindowStyles,
                              0, 0, 0, 0,									   			// size of window
                              NULL, NULL, hAppInst, NULL);
    if (hMainWnd == NULL)
    {
        ScrnExit();
        return 1;
    }
	MenuCreate();
    if (!nVidFullscreen)
    {
        ScrnTitle();
        ScrnSize();
    }
	WndInMid(hMainWnd, NULL);
    return 0;
}

// Exit the screen window (destroy it)
int ScrnExit()
{
    // Ensure the window is destroyed
    DeActivateChat();

    if (hMainWnd)
    {
        DestroyWindow(hMainWnd);
        hMainWnd = NULL;
    }

    UnregisterClass(szClass, hAppInst);		// Unregister the scrn class
    return 0;
}

void Reinitialise()
{
    POST_INITIALISE_MESSAGE;
    VidReInitialise();
}
