// Menu handling
#include "burner.h"

HMENU hMenu = NULL;
static HMENU hBlitterMenu[8] = {NULL, };	// Handles to the blitter-specific sub-menus
static HMENU hAudioPluginMenu[8] = {NULL, };

bool bMenuDisplayed = false;
int nLastMenu = 0;
static int nRecursions = -1;
static HMENU hCurrentMenu;
static int nCurrentItem;
static int nCurrentItemFlags;

int nMenuHeight = 0;
int nWindowSize = 0;
TCHAR szPrevGames[SHOW_PREV_GAMES][32];

int OnMenuSelect(HWND, HMENU, int nItem, HMENU, UINT nFlags)
{
    //	printf("WM_MENUSELECT: %4i, %08x\n", nItem, nFlags);
    nCurrentItem = nItem;
    nCurrentItemFlags = nFlags;
    return 0;
}

int OnInitMenuPopup(HWND, HMENU hNewMenuPopup, UINT, BOOL bWindowsMenu)
{
    if (!bWindowsMenu)
    {
        if (!nRecursions)
        {
            bMenuDisplayed = true;
            hCurrentMenu = hNewMenuPopup;
        }
        nRecursions++;
    }
    return 0;
}

int OnUnInitMenuPopup(HWND, HMENU, UINT, BOOL)
{
    if (nRecursions <= 1)
    {
        bMenuDisplayed = false;
        if(!bAltPause)
        {
            if (bRunPause)
            {
                bRunPause = 0;
            }
            GameInpCheckMouse();
        }
    }
    nRecursions--;

    return 0;
}

int MenuCreate()
{
	if (hMenu == NULL)
    {
    	hMenu = GetMenu(hMainWnd);
        hBlitterMenu[0] = LoadMenu(hAppInst, MAKEINTRESOURCE(IDR_MENU_BLITTER_1));	// DirectDraw Standard blitter

        hAudioPluginMenu[0] = LoadMenu(hAppInst, MAKEINTRESOURCE(IDR_MENU_AUD_PLUGIN_1));
    }
	
	if (nVidFullscreen)
	{
		SetMenu(hMainWnd,NULL);
		return 0;
	}
	
    SetMenu(hMainWnd,hMenu);
    MenuEnableItems();
    MenuUpdate();

    bMenuDisplayed = false;
    nLastMenu = -1;

    return 0;
}

void MenuDestroy()
{
    if(hMenu)
    {
        hMenu = NULL;
    }
}

// Update the other gamma menuitem
static void CreateOtherGammaItem(bool bOther)
{
    TCHAR szItemText[256];
    MENUITEMINFO menuItem = {sizeof(MENUITEMINFO), MIIM_TYPE, MFT_STRING, 0, 0, NULL, NULL, NULL, 0, szItemText, 0, 0 };

    LoadString(hAppInst, IDS_MENU_2, szItemText, 256);
    if (bOther)
    {
        _stprintf(szItemText + _tcslen(szItemText), _T("\t(%1.2f)"), nGamma);
    }
    menuItem.cch = _tcslen(szItemText);
    SetMenuItemInfo(hMenu, MENU_GAMMA_OTHER, 0, &menuItem);
}

// Update the cpu speed menuitem
static void CreateCPUSpeedItem(bool bOther)
{
    TCHAR szItemText[256];
    MENUITEMINFO menuItem = {sizeof(MENUITEMINFO), MIIM_TYPE, MFT_STRING, 0, 0, NULL, NULL, NULL, 0, szItemText, 0, 0 };

    LoadString(hAppInst, IDS_MENU_3, szItemText, 256);
    if (bOther)
    {
        _stprintf(szItemText + _tcslen(szItemText), _T("\t(%d%%)"), nBurnCPUSpeedAdjust * 100 / 256);
    }
    menuItem.cch = _tcslen(szItemText);
    SetMenuItemInfo(hMenu, MENU_SETCPUCLOCK, 0, &menuItem);
}

// Update state slot menuitmes
static void CreateStateslotItems()
{
    TCHAR szItemText[256];
    MENUITEMINFO menuItem = {sizeof(MENUITEMINFO), MIIM_TYPE, MFT_STRING, 0, 0, NULL, NULL, NULL, 0, szItemText, 0, 0 };

    _sntprintf(szItemText, 256, LoadStringEx(hAppInst, IDS_MENU_4, true), nSavestateSlot);
    menuItem.cch = _tcslen(szItemText);
    SetMenuItemInfo(hMenu, MENU_STATE_LOAD_SLOT, 0, &menuItem);

    _sntprintf(szItemText, 256, LoadStringEx(hAppInst, IDS_MENU_5, true), nSavestateSlot);
    menuItem.cch = _tcslen(szItemText);
    SetMenuItemInfo(hMenu, MENU_STATE_SAVE_SLOT, 0, &menuItem);
}
// Update bullets, checkmarks, and item text
void MenuUpdate()
{
    int var;

    CreateStateslotItems();

    CheckMenuItem(hMenu, MENU_PAUSE, bAltPause ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_ALLRAM, bDrvSaveAll ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_SETCPUCLOCK, nBurnCPUSpeedAdjust != 0x0100 ? MF_CHECKED : MF_UNCHECKED);
    CreateCPUSpeedItem(nBurnCPUSpeedAdjust != 0x0100);

    if (bVidFullStretch)
    {
        var = MENU_STRETCH;
    }
    else
    {
        if (bVidCorrectAspect)
        {
            var = MENU_ASPECT;
        }
        else
        {
            var = MENU_NOSTRETCH;
        }
    }
    CheckMenuRadioItem(hMenu, MENU_NOSTRETCH, MENU_ASPECT, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_STRETCH, bVidFullStretch ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_TRIPLE, bVidTripleBuffer ? MF_CHECKED : MF_UNCHECKED);

    var = nVidSelect + MENU_BLITTER_1;
    CheckMenuRadioItem(hMenu, MENU_BLITTER_1, MENU_BLITTER_8, var, MF_BYCOMMAND);

    switch (nVidSelect)
    {
    case 0:
        if (bVidScanlines)
        {
            if (bVidScanHalf)
            {
                var = MENU_SCAN50;
            }
            else
            {
                var = MENU_BASIC_SCAN;
            }
        }
        else
        {
            var = MENU_BASIC_NORMAL;
        }
        CheckMenuRadioItem(hMenu, MENU_BASIC_NORMAL, MENU_SCAN50, var, MF_BYCOMMAND);
        CheckMenuItem(hMenu, MENU_BASIC_ROTSCAN, bVidScanRotate ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, MENU_FORCE_FLIP, bVidForceFlip ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, MENU_RES_ARCADE, bVidArcaderes ? MF_CHECKED : MF_UNCHECKED);
        break;
    }
    if (nGamma > 1.249 && nGamma < 1.251)
    {
        var = MENU_GAMMA_DARKER;
    }
    else
    {
        if (nGamma > 0.799 && nGamma < 0.801)
        {
            var = MENU_GAMMA_LIGHTER;
        }
        else
        {
            var = MENU_GAMMA_OTHER;
        }
    }
    if (var == MENU_GAMMA_OTHER)
    {
        CreateOtherGammaItem(1);
    }
    else
    {
        CreateOtherGammaItem(0);
    }
    if (!bDoGamma)
    {
        var = MENU_GAMMA_DO;
    }
    CheckMenuRadioItem(hMenu, MENU_GAMMA_DARKER, MENU_GAMMA_OTHER, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_GAMMA_DO, bDoGamma ? MF_CHECKED : MF_UNCHECKED);

    var = MENU_GAMMA_SOFTWARE_ONLY;
    if (bVidUseHardwareGamma)
    {
        if (bHardwareGammaOnly)
        {
            var = MENU_GAMMA_HARDWARE_ONLY;
        }
        else
        {
            var = MENU_GAMMA_USE_HARDWARE;
        }
    }
    CheckMenuRadioItem(hMenu, MENU_GAMMA_USE_HARDWARE, MENU_GAMMA_SOFTWARE_ONLY, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_AUTOSWITCHFULL, bVidAutoSwitchFull ? MF_CHECKED : MF_UNCHECKED);

    if (nVidTransferMethod == 0)
    {
        var = MENU_BASIC_VIDEOMEM;
    }
    else
    {
        if (nVidTransferMethod == -1)
        {
            var = MENU_BASIC_MEMAUTO;
        }
        else
        {
            var = MENU_BASIC_SYSMEM;
        }
    }
    CheckMenuRadioItem(hMenu, MENU_BASIC_MEMAUTO, MENU_BASIC_SYSMEM, var, MF_BYCOMMAND);

    if (nVidTransferMethod == 0)
    {
        var = MENU_SOFTFX_VIDEOMEM;
    }
    else
    {
        if (nVidTransferMethod == -1)
        {
            var = MENU_SOFTFX_MEMAUTO;
        }
        else
        {
            var = MENU_SOFTFX_SYSMEM;
        }
    }
    CheckMenuRadioItem(hMenu, MENU_SOFTFX_MEMAUTO, MENU_SOFTFX_SYSMEM, var, MF_BYCOMMAND);

    if (nWindowSize < 4)
    {
        var = MENU_SINGLESIZEWINDOW + nWindowSize;
    }
    else
    {
        var = MENU_MAXIMUMSIZEWINDOW;
    }
    CheckMenuRadioItem(hMenu, MENU_AUTOSIZE, MENU_MAXIMUMSIZEWINDOW, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_MONITORAUTOCHECK, bMonitorAutoCheck ? MF_CHECKED : MF_UNCHECKED);

    var = -1;
    if (nVidScrnAspectX == 4 && nVidScrnAspectY == 3)
    {
        var = MENU_ASPECTNORMAL;
    }
    else
    {
        if (nVidScrnAspectX == 5 && nVidScrnAspectY == 4)
        {
            var = MENU_ASPECTLCD;
        }
        else
        {
            if (nVidScrnAspectX == 16 && nVidScrnAspectY == 9)
            {
                var = MENU_ASPECTWIDE;
            }
            else
            {
                if (nVidScrnAspectX == 16 && nVidScrnAspectY == 10)
                {
                    var = MENU_ASPECTWIDELCD;
                }
            }
        }
    }
    CheckMenuRadioItem(hMenu, MENU_ASPECTNORMAL, MENU_ASPECTWIDELCD, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_MONITORMIRRORVERT, (nVidRotationAdjust & 2) ? MF_CHECKED : MF_UNCHECKED);

    CheckMenuItem(hMenu, MENU_ROTATEVERTICAL, !(nVidRotationAdjust & 1) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_FORCE60HZ, bForce60Hz ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_VIDEOVSYNC, bVidVSync ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_AUTOFRAMESKIP, !bAlwaysDrawFrames ? MF_CHECKED : MF_UNCHECKED);

    var = nAudSelect + MENU_AUD_PLUGIN_1;
    CheckMenuRadioItem(hMenu, MENU_AUD_PLUGIN_1, MENU_AUD_PLUGIN_8, var, MF_BYCOMMAND);

    switch (nAudSelect)
    {
    case 0:
    {
        var = MENU_DSOUND_NOSOUND;
        if (nAudSampleRate[0] > 0)
        {
            if (nAudSampleRate[0] <= 11025)
            {
                var = MENU_DSOUND_11025;
            }
            else
            {
                if (nAudSampleRate[0] <= 22050)
                {
                    var = MENU_DSOUND_22050;
                }
                else
                {
                    if (nAudSampleRate[0] <= 44100)
                    {
                        var = MENU_DSOUND_44100;
                    }
                    else
                    {
                        var = MENU_DSOUND_48000;
                    }
                }
            }
        }
        CheckMenuRadioItem(hMenu, MENU_DSOUND_NOSOUND, MENU_DSOUND_48000, var, MF_BYCOMMAND);
        CheckMenuItem(hMenu, MENU_DSOUND_BASS, nAudDSPModule[0] ? MF_CHECKED : MF_UNCHECKED);
        break;
    }
    }

    var = MENU_INTERPOLATE_0 + nInterpolation;
    if (bDrvOkay)
    {
        if ((BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            var = MENU_INTERPOLATE_0 + 1;
        }
    }
    CheckMenuRadioItem(hMenu, MENU_INTERPOLATE_0, MENU_INTERPOLATE_5, var, MF_BYCOMMAND);

    var = MENU_INTERPOLATE_FM_0 + nFMInterpolation;
    CheckMenuRadioItem(hMenu, MENU_INTERPOLATE_FM_0, MENU_INTERPOLATE_FM_5, var, MF_BYCOMMAND);

    CheckMenuItem(hMenu, MENU_AUTOPAUSE, bAutoPause ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, MENU_PROCESSINPUT, !bAlwaysProcessKeyboardInput ? MF_CHECKED : MF_UNCHECKED);
    EnableMenuItem(hMenu, MENU_PROCESSINPUT, bAutoPause?(MF_GRAYED|MF_BYCOMMAND) : (MF_ENABLED|MF_BYCOMMAND));
    CheckMenuItem(hMenu, MENU_DISPLAYINDICATOR, nVidSDisplayStatus ? MF_CHECKED : MF_UNCHECKED);
	switch(nAppThreadPriority)
	{
	case THREAD_PRIORITY_TIME_CRITICAL:
		var = MENU_PRIORITY_REALTIME;
		break;
	case THREAD_PRIORITY_HIGHEST:
		var = MENU_PRIORITY_HIGH;
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		var = MENU_PRIORITY_ABOVE_NORMAL;
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		var = MENU_PRIORITY_BELOW_NORMAL;
		break;
	case THREAD_PRIORITY_LOWEST:
		var = MENU_PRIORITY_LOW;
		break;
	default:
		var = MENU_PRIORITY_NORMAL;
		break;
	}
    CheckMenuRadioItem(hMenu, MENU_PRIORITY_REALTIME, MENU_PRIORITY_LOW, var, MF_BYCOMMAND);
    CheckMenuItem(hMenu, MENU_SAVEGAMEINPUT, bSaveInputs ? MF_CHECKED : MF_UNCHECKED);

    // Auto-Fire
    if (nAutoFireRate == 4)  var = MENU_INPUT_AUTOFIRE_RATE_4;
    if (nAutoFireRate == 8)  var = MENU_INPUT_AUTOFIRE_RATE_3;
    if (nAutoFireRate == 12) var = MENU_INPUT_AUTOFIRE_RATE_2;
    if (nAutoFireRate == 22) var = MENU_INPUT_AUTOFIRE_RATE_1;
    CheckMenuRadioItem(hMenu, MENU_INPUT_AUTOFIRE_RATE_1, MENU_INPUT_AUTOFIRE_RATE_4, var, MF_BYCOMMAND);

    // Previous games list
    for (int i = 0; i < SHOW_PREV_GAMES; i++)
    {
        MENUITEMINFO menuItemInfo;
        TCHAR szText[256] = _T("");
        int OldDrvSelect = nBurnDrvActive;

        memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);
        menuItemInfo.fType = MFT_STRING;
        menuItemInfo.fMask = MIIM_TYPE;

        if (_tcslen(szPrevGames[i]))
        {
            for (unsigned int j = 0; j < nBurnDrvCount; j++)
            {
                nBurnDrvActive = j;
                if (!_tcsicmp(szPrevGames[i], BurnDrvGetText(DRV_NAME)))
                {
                    _stprintf(szText, _T("%s\t%s"), BurnDrvGetText(DRV_FULLNAME), BurnDrvGetText(DRV_NAME));

                    break;
                }
            }

            // Check for &s and change to &&
            TCHAR szText2[256] = _T("");
            TCHAR *Tokens = NULL;
            int NumTokens = 0;

            Tokens = _tcstok(szText, _T("&"));
            while (Tokens != NULL)
            {
                if (NumTokens)
                {
                    _stprintf(szText2, _T("%s&&%s"), szText2, Tokens);
                }
                else
                {
                    _stprintf(szText2, _T("%s"), Tokens);
                }

                Tokens = _tcstok(NULL, _T("&"));
                NumTokens++;
            }

            menuItemInfo.dwTypeData = szText2;
            menuItemInfo.cch = _tcslen(szText2);

            SetMenuItemInfo(hMenu, MENU_PREVIOUSGAMES1 + i, FALSE, &menuItemInfo);
            EnableMenuItem(hMenu, MENU_PREVIOUSGAMES1 + i, MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            _tcscpy(szText, LoadStringEx(hAppInst, IDS_MENU_6, true));

            menuItemInfo.dwTypeData = szText;
            menuItemInfo.cch = _tcslen(szText);

            SetMenuItemInfo(hMenu, MENU_PREVIOUSGAMES1 + i, FALSE, &menuItemInfo);
            EnableMenuItem(hMenu, MENU_PREVIOUSGAMES1 + i, MF_GRAYED | MF_BYCOMMAND);
        }

        nBurnDrvActive = OldDrvSelect;
    }

#if defined BUILD_X64_EXE
    EnableMenuItem(hMenu, MENU_BLITTER_2, MF_GRAYED  | MF_BYCOMMAND);
#endif
}

void MenuEnableItems()
{
    if (hBlitterMenu[nVidSelect])
    {
        MENUITEMINFO myMenuItemInfo;
        myMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
        myMenuItemInfo.fMask = MIIM_SUBMENU | MIIM_STATE;
        myMenuItemInfo.fState = MFS_ENABLED;
        myMenuItemInfo.hSubMenu = GetSubMenu(hBlitterMenu[nVidSelect], 0);
        SetMenuItemInfo(GetSubMenu(hMenu, 1), 1, TRUE, &myMenuItemInfo);
    }
    else
    {
        MENUITEMINFO myMenuItemInfo;
        myMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
        myMenuItemInfo.fMask = MIIM_SUBMENU | MIIM_STATE;
        myMenuItemInfo.fState = MFS_GRAYED;
        myMenuItemInfo.hSubMenu = NULL;
        SetMenuItemInfo(GetSubMenu(hMenu, 1), 1, TRUE, &myMenuItemInfo);
    }

    if (hBlitterMenu[nAudSelect])
    {
        MENUITEMINFO myMenuItemInfo;
        myMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
        myMenuItemInfo.fMask = MIIM_SUBMENU | MIIM_STATE;
        myMenuItemInfo.fState = MFS_ENABLED;
        myMenuItemInfo.hSubMenu = GetSubMenu(hAudioPluginMenu[nAudSelect], 0);
        SetMenuItemInfo(GetSubMenu(hMenu, 2), 1, TRUE, &myMenuItemInfo);
    }
    else
    {
        MENUITEMINFO myMenuItemInfo;
        myMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
        myMenuItemInfo.fMask = MIIM_SUBMENU | MIIM_STATE;
        myMenuItemInfo.fState = MFS_GRAYED;
        myMenuItemInfo.hSubMenu = NULL;
        SetMenuItemInfo(GetSubMenu(hMenu, 2), 1, TRUE, &myMenuItemInfo);
    }

    // Auto-size image when software blitter effects anre enabled
    if ((nVidSelect == 1 && (nVidBlitterOpt[1] & 0x07000000) == 0x07000000) || (nVidSelect == 2 && nVidBlitterOpt[2] & 0x0100))
    {
        EnableMenuItem(GetSubMenu(hMenu, 1),  2,	MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(GetSubMenu(hMenu, 1),  7,	MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(GetSubMenu(hMenu, 1), 10,	MF_GRAYED | MF_BYPOSITION);
    }
    else
    {
        EnableMenuItem(GetSubMenu(hMenu, 1),  2,	MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(GetSubMenu(hMenu, 1),  7,	MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(GetSubMenu(hMenu, 1), 10,	MF_ENABLED | MF_BYPOSITION);
    }
    EnableMenuItem(GetSubMenu(hMenu, 1), 11,		MF_ENABLED | MF_BYPOSITION);

    if (bDrvOkay)
    {
        EnableMenuItem(hMenu, MENU_QUIT,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INPUT,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_FORCE60HZ,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_NOSOUND,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_11025,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_22050,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_44100,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_48000,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_NOSOUND,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_11025,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_22050,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_44100,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_48000,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INTERPOLATE_FM_0,	MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INTERPOLATE_FM_3,	MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_FRAMES,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SAVESNAP,			MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SAVEGAMEINPUTNOW,	MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SAVEHISCORES,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_USEBLEND,			MF_GRAYED  | MF_BYCOMMAND);

        EnableMenuItem(hMenu, MENU_CDIMAGE,				MF_GRAYED | MF_BYCOMMAND);

        EnableMenuItem(hMenu, MENU_AUD_PLUGIN_1, MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_AUD_PLUGIN_2, MF_GRAYED  | MF_BYCOMMAND);

        BurnDIPInfo bdi;
        if (BurnDrvGetDIPInfo(&bdi, 0) == 0)
        {
            EnableMenuItem(hMenu, MENU_DIPSW,			MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            EnableMenuItem(hMenu, MENU_DIPSW,			MF_GRAYED | MF_BYCOMMAND);
        }

        EnableMenuItem(hMenu, MENU_INTERPOLATE_1,		MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INTERPOLATE_3,		MF_ENABLED | MF_BYCOMMAND);

        EnableMenuItem(hMenu, MENU_MEMCARD_CREATE,		MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_SELECT,		MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_INSERT,		MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_EJECT,		MF_GRAYED | MF_BYCOMMAND);

        if ((BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO)
        {
            EnableMenuItem(hMenu, MENU_INTERPOLATE_1,				MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_INTERPOLATE_3,				MF_GRAYED | MF_BYCOMMAND);

            if (!kNetGame)
            {
                EnableMenuItem(hMenu, MENU_MEMCARD_CREATE,			MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_MEMCARD_SELECT,			MF_ENABLED | MF_BYCOMMAND);
                if (nMemoryCardStatus & 1)
                {
                    if (nMemoryCardStatus & 2)
                    {
                        EnableMenuItem(hMenu, MENU_MEMCARD_EJECT,	MF_ENABLED | MF_BYCOMMAND);
                    }
                    else
                    {
                        EnableMenuItem(hMenu, MENU_MEMCARD_INSERT,	MF_ENABLED | MF_BYCOMMAND);
                    }
                }
            }
        }

        if (kNetGame)
        {
            EnableMenuItem(hMenu, MENU_LOAD,			MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_STARTNET,		MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_EXIT,			MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_SETCPUCLOCK,		MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_RESETCPUCLOCK,	MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_PAUSE,			MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_ENABLECHEAT,		MF_GRAYED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_SNAPFACT,		MF_GRAYED | MF_BYCOMMAND);
        }
        else
        {
            EnableMenuItem(hMenu, MENU_LOAD,			MF_ENABLED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_STARTNET,		MF_ENABLED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_EXIT,			MF_ENABLED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_SETCPUCLOCK,		MF_ENABLED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_RESETCPUCLOCK,	(nBurnCPUSpeedAdjust != 0x0100 ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_PAUSE,			MF_ENABLED | MF_BYCOMMAND);
            if (pCheatInfo)
            {
                EnableMenuItem(hMenu, MENU_ENABLECHEAT,	MF_ENABLED | MF_BYCOMMAND);
            }

            if (CheatSearchInit())
            {
                EnableMenuItem(hMenu, MENU_CHEATSEARCH_START, MF_ENABLED | MF_BYCOMMAND);
            }
        }

        if (WaveLog)
        {
            EnableMenuItem(hMenu, MENU_WLOGEND,		MF_ENABLED | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_WLOGSTART,	MF_GRAYED  | MF_BYCOMMAND);
        }
        else
        {
            EnableMenuItem(hMenu, MENU_WLOGEND,		MF_GRAYED  | MF_BYCOMMAND);
            EnableMenuItem(hMenu, MENU_WLOGSTART,	MF_ENABLED | MF_BYCOMMAND);
        }

            if (kNetGame)
            {
                EnableMenuItem(hMenu, MENU_STARTRECORD,			MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STARTREPLAY,			MF_GRAYED  | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_SAVE_SLOT,		MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_SAVE_DIALOG,	MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_LOAD_SLOT,		MF_GRAYED  | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_LOAD_DIALOG,	MF_GRAYED  | MF_BYCOMMAND);
            }
            else
            {
                EnableMenuItem(hMenu, MENU_STARTRECORD,			MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STARTREPLAY,			MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_SAVE_SLOT,		MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_SAVE_DIALOG,	MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_LOAD_SLOT,		MF_ENABLED | MF_BYCOMMAND);
                EnableMenuItem(hMenu, MENU_STATE_LOAD_DIALOG,	MF_ENABLED | MF_BYCOMMAND);
            }
    }
    else
    {
        bAltPause = 0;

        EnableMenuItem(hMenu, MENU_LOAD,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_CREATE,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_SELECT,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_INSERT,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_MEMCARD_EJECT,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STARTNET,			MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STARTREPLAY,			MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STARTRECORD,			MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STOPREPLAY,			MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_QUIT,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_EXIT,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INPUT,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SETCPUCLOCK,			MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_RESETCPUCLOCK,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_PAUSE,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_FORCE60HZ,			MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DIPSW,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_ENABLECHEAT,			MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SAVEHISCORES,		MF_ENABLED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_USEBLEND,			MF_ENABLED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STATE_LOAD_SLOT,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STATE_LOAD_DIALOG,	MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STATE_SAVE_SLOT,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_STATE_SAVE_DIALOG,	MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_FRAMES,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_NOSOUND,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_11025,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_22050,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_44100,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_DSOUND_48000,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_NOSOUND,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_11025,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_22050,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_44100,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_XAUDIO_48000,				MF_ENABLED | MF_BYCOMMAND);
        //		EnableMenuItem(hMenu, MENU_INTERPOLATE_1,		MF_ENABLED | MF_BYCOMMAND);
        //		EnableMenuItem(hMenu, MENU_INTERPOLATE_3,		MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_WLOGSTART,			MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INTERPOLATE_FM_0,	MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_INTERPOLATE_FM_3,	MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_WLOGEND,				MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_SAVEGAMEINPUTNOW,		MF_GRAYED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_CDIMAGE,				MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_AUD_PLUGIN_1, 		MF_ENABLED  | MF_BYCOMMAND);
        EnableMenuItem(hMenu, MENU_AUD_PLUGIN_2,		 MF_ENABLED  | MF_BYCOMMAND);
    }
}

