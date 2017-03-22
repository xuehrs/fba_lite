// FB Alpha - Emulator for MC68000/Z80 based arcade games
//            Refer to the "license.txt" file for more info

// Main module

// #define USE_SDL					// define if SDL is used
// #define DONT_DISPLAY_SPLASH		// Prevent Splash screen from being displayed
#define APP_DEBUG_LOG			// log debug messages to zzBurnDebug.html

#include "burner.h"
#include <wininet.h>

#ifdef _MSC_VER
//  #include <winable.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif

#include "version.h"

HINSTANCE hAppInst = NULL;			// Application Instance
HANDLE hMainThread;
long int nMainThreadID;
int nAppThreadPriority = THREAD_PRIORITY_NORMAL;
int nAppShowCmd;

HACCEL hAccel = NULL;

int nAppVirtualFps = 6000;			// App fps * 100

TCHAR szAppBurnVer[16] = _T("");
TCHAR szAppExeName[EXE_NAME_SIZE + 1];

bool bCmdOptUsed = 0;
bool bAlwaysProcessKeyboardInput = false;
bool bMonitorAutoCheck = true;

// Used for the load/save dialog in commdlg.h (savestates, input replay, wave logging)
TCHAR szChoice[MAX_PATH] = _T("");
OPENFILENAME ofn;

#if defined (UNICODE)
char *TCHARToANSI(const TCHAR *pszInString, char *pszOutString, int nOutSize)
{

    static char szStringBuffer[1024];
    memset(szStringBuffer, 0, sizeof(szStringBuffer));

    char *pszBuffer = pszOutString ? pszOutString : szStringBuffer;
    int nBufferSize = pszOutString ? nOutSize * 2 : sizeof(szStringBuffer);

    if (WideCharToMultiByte(CP_ACP, 0, pszInString, -1, pszBuffer, nBufferSize, NULL, NULL))
    {
        return pszBuffer;
    }

    return NULL;
}

TCHAR *ANSIToTCHAR(const char *pszInString, TCHAR *pszOutString, int nOutSize)
{
    static TCHAR szStringBuffer[1024];

    TCHAR *pszBuffer = pszOutString ? pszOutString : szStringBuffer;
    int nBufferSize  = pszOutString ? nOutSize * 2 : sizeof(szStringBuffer);

    if (MultiByteToWideChar(CP_ACP, 0, pszInString, -1, pszBuffer, nBufferSize))
    {
        return pszBuffer;
    }

    return NULL;
}
#else
char *TCHARToANSI(const TCHAR *pszInString, char *pszOutString, int /*nOutSize*/)
{
    if (pszOutString)
    {
        strcpy(pszOutString, pszInString);
        return pszOutString;
    }

    return (char *)pszInString;
}

TCHAR *ANSIToTCHAR(const char *pszInString, TCHAR *pszOutString, int /*nOutSize*/)
{
    if (pszOutString)
    {
        _tcscpy(pszOutString, pszInString);
        return pszOutString;
    }

    return (TCHAR *)pszInString;
}
#endif

CHAR *astring_from_utf8(const char *utf8string)
{
    WCHAR *wstring;
    int char_count;
    CHAR *result;

    // convert MAME string (UTF-8) to UTF-16
    char_count = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, NULL, 0);
    wstring = (WCHAR *)malloc(char_count * sizeof(*wstring));
    MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, wstring, char_count);

    // convert UTF-16 to "ANSI code page" string
    char_count = WideCharToMultiByte(CP_ACP, 0, wstring, -1, NULL, 0, NULL, NULL);
    result = (CHAR *)malloc(char_count * sizeof(*result));
    if (result != NULL)
        WideCharToMultiByte(CP_ACP, 0, wstring, -1, result, char_count, NULL, NULL);

    if (wstring)
    {
        free(wstring);
        wstring = NULL;
    }
    return result;
}

char *utf8_from_astring(const CHAR *astring)
{
    WCHAR *wstring;
    int char_count;
    CHAR *result;

    // convert "ANSI code page" string to UTF-16
    char_count = MultiByteToWideChar(CP_ACP, 0, astring, -1, NULL, 0);
    wstring = (WCHAR *)malloc(char_count * sizeof(*wstring));
    MultiByteToWideChar(CP_ACP, 0, astring, -1, wstring, char_count);

    // convert UTF-16 to MAME string (UTF-8)
    char_count = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, NULL, 0, NULL, NULL);
    result = (CHAR *)malloc(char_count * sizeof(*result));
    if (result != NULL)
        WideCharToMultiByte(CP_UTF8, 0, wstring, -1, result, char_count, NULL, NULL);

    if (wstring)
    {
        free(wstring);
        wstring = NULL;
    }
    return result;
}

WCHAR *wstring_from_utf8(const char *utf8string)
{
    int char_count;
    WCHAR *result;

    // convert MAME string (UTF-8) to UTF-16
    char_count = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, NULL, 0);
    result = (WCHAR *)malloc(char_count * sizeof(*result));
    if (result != NULL)
        MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, result, char_count);

    return result;
}

char *utf8_from_wstring(const WCHAR *wstring)
{
    int char_count;
    char *result;

    // convert UTF-16 to MAME string (UTF-8)
    char_count = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, NULL, 0, NULL, NULL);
    result = (char *)malloc(char_count * sizeof(*result));
    if (result != NULL)
        WideCharToMultiByte(CP_UTF8, 0, wstring, -1, result, char_count, NULL, NULL);

    return result;
}

#if defined (FBA_DEBUG)
static TCHAR szConsoleBuffer[1024];
static int nPrevConsoleStatus = -1;

static HANDLE DebugBuffer;
static FILE *DebugLog = NULL;
static bool bEchoLog = true; // false;
#endif

void tcharstrreplace(TCHAR *pszSRBuffer, const TCHAR *pszFind, const TCHAR *pszReplace)
{
    if (pszSRBuffer == NULL || pszFind == NULL || pszReplace == NULL)
        return;

    int lenFind = _tcslen(pszFind);
    int lenReplace = _tcslen(pszReplace);
    int lenSRBuffer = _tcslen(pszSRBuffer) + 1;

    for(int i = 0; (lenSRBuffer > lenFind) && (i < lenSRBuffer - lenFind); i++)
    {
        if (!memcmp(pszFind, &pszSRBuffer[i], lenFind * sizeof(TCHAR)))
        {
            if (lenFind == lenReplace)
            {
                memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
                i += lenReplace - 1;
            }
            else if (lenFind > lenReplace)
            {
                memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
                i += lenReplace;
                int delta = lenFind - lenReplace;
                lenSRBuffer -= delta;
                memmove(&pszSRBuffer[i], &pszSRBuffer[i + delta], (lenSRBuffer - i) * sizeof(TCHAR));
                i--;
            }
            else
            {
                /* this part only works on dynamic buffers - the replacement string length must be smaller or equal to the find string length if this is commented out!
                	int delta = lenReplace - lenFind;
                	pszSRBuffer = (TCHAR *)realloc(pszSRBuffer, (lenSRBuffer + delta) * sizeof(TCHAR));
                	memmove(&pszSRBuffer[i + lenReplace], &pszSRBuffer[i + lenFind], (lenSRBuffer - i - lenFind) * sizeof(TCHAR));
                	lenSRBuffer += delta;
                	memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
                	i += lenReplace - 1; */
            }
        }
    }
}

#if defined (FBA_DEBUG)
// Debug printf to a file
static int __cdecl AppDebugPrintf(int nStatus, TCHAR *pszFormat, ...)
{
    va_list vaFormat;

    va_start(vaFormat, pszFormat);

    if (DebugLog)
    {

        if (nStatus != nPrevConsoleStatus)
        {
            switch (nStatus)
            {
            case PRINT_ERROR:
                _ftprintf(DebugLog, _T("</font><font color=#FF3F3F>"));
                break;
            case PRINT_IMPORTANT:
                _ftprintf(DebugLog, _T("</font><font color=#000000>"));
                break;
            default:
                _ftprintf(DebugLog, _T("</font><font color=#009F00>"));
            }
        }
        _vftprintf(DebugLog, pszFormat, vaFormat);
        fflush(DebugLog);
    }

    if (!DebugLog || bEchoLog)
    {
        _vsntprintf(szConsoleBuffer, 1024, pszFormat, vaFormat);

        if (nStatus != nPrevConsoleStatus)
        {
            switch (nStatus)
            {
            case PRINT_UI:
                SetConsoleTextAttribute(DebugBuffer,                                                       FOREGROUND_INTENSITY);
                break;
            case PRINT_IMPORTANT:
                SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case PRINT_ERROR:
                SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY);
                break;
            default:
                SetConsoleTextAttribute(DebugBuffer,                  FOREGROUND_GREEN |                   FOREGROUND_INTENSITY);
            }
        }

        tcharstrreplace(szConsoleBuffer, _T(SEPERATOR_1), _T(" * "));
        WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
    }

    nPrevConsoleStatus = nStatus;

    va_end(vaFormat);

    return 0;
}
#endif

int dprintf(TCHAR *pszFormat, ...)
{
#if defined (FBA_DEBUG)
    va_list vaFormat;
    va_start(vaFormat, pszFormat);

    _vsntprintf(szConsoleBuffer, 1024, pszFormat, vaFormat);

    if (nPrevConsoleStatus != PRINT_UI)
    {
        if (DebugLog)
        {
            _ftprintf(DebugLog, _T("</font><font color=#9F9F9F>"));
        }
        SetConsoleTextAttribute(DebugBuffer, FOREGROUND_INTENSITY);
        nPrevConsoleStatus = PRINT_UI;
    }

    if (DebugLog)
    {
        _ftprintf(DebugLog, szConsoleBuffer);
        fflush(DebugLog);
    }

    tcharstrreplace(szConsoleBuffer, _T(SEPERATOR_1), _T(" * "));
    WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
    va_end(vaFormat);
#else
    (void)pszFormat;
#endif

    return 0;
}

void CloseDebugLog()
{
#if defined (FBA_DEBUG)
    if (DebugLog)
    {

        _ftprintf(DebugLog, _T("</pre></body></html>"));

        fclose(DebugLog);
        DebugLog = NULL;
    }

    if (DebugBuffer)
    {
        CloseHandle(DebugBuffer);
        DebugBuffer = NULL;
    }

    FreeConsole();
#endif
}

int OpenDebugLog()
{
#if defined (FBA_DEBUG)
#if defined (APP_DEBUG_LOG)

    time_t nTime;
    tm *tmTime;

    time(&nTime);
    tmTime = localtime(&nTime);

    {
        // Initialise the debug log file

#ifdef _UNICODE
        DebugLog = _tfopen(_T("zzBurnDebug.html"), _T("wb"));

        if (ftell(DebugLog) == 0)
        {
            WRITE_UNICODE_BOM(DebugLog);

            _ftprintf(DebugLog, _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
            _ftprintf(DebugLog, _T("<html><head><meta http-equiv=Content-Type content=\"text/html; charset=unicode\"></head><body><pre>"));
        }
#else
        DebugLog = _tfopen(_T("zzBurnDebug.html"), _T("wt"));

        if (ftell(DebugLog) == 0)
        {
            _ftprintf(DebugLog, _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
            _ftprintf(DebugLog, _T("<html><head><meta http-equiv=Content-Type content=\"text/html; charset=windows-%i\"></head><body><pre>"), GetACP());
        }
#endif

        _ftprintf(DebugLog, _T("</font><font size=larger color=#000000>"));
        _ftprintf(DebugLog, _T("Debug log created by ") _T(APP_TITLE) _T(" v%.20s on %s\n<br>"), szAppBurnVer, _tasctime(tmTime));
    }
#endif

    {
        // Initialise the debug console

        COORD DebugBufferSize = { 80, 1000 };

        {

            // Since AttachConsole is only present in Windows XP, import it manually

#if _WIN32_WINNT >= 0x0500 && defined (_MSC_VER)
            // #error Manually importing AttachConsole() function, but compiling with _WIN32_WINNT >= 0x0500
            if (!AttachConsole(ATTACH_PARENT_PROCESS))
            {
                AllocConsole();
            }
#else
#define ATTACH_PARENT_PROCESS ((DWORD)-1)

            BOOL (WINAPI * pAttachConsole)(DWORD dwProcessId) = NULL;
            HINSTANCE hKernel32DLL = LoadLibrary(_T("kernel32.dll"));

            if (hKernel32DLL)
            {
                pAttachConsole = (BOOL (WINAPI *)(DWORD))GetProcAddress(hKernel32DLL, "AttachConsole");
            }
            if (pAttachConsole)
            {
                if (!pAttachConsole(ATTACH_PARENT_PROCESS))
                {
                    AllocConsole();
                }
            }
            else
            {
                AllocConsole();
            }
            if (hKernel32DLL)
            {
                FreeLibrary(hKernel32DLL);
            }

#undef ATTACH_PARENT_PROCESS
#endif

        }

        DebugBuffer = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_READ, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleScreenBufferSize(DebugBuffer, DebugBufferSize);
        SetConsoleActiveScreenBuffer(DebugBuffer);
        SetConsoleTitle(_T(APP_TITLE) _T(" Debug console"));

        SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        _sntprintf(szConsoleBuffer, 1024, _T("Welcome to the ") _T(APP_TITLE) _T(" debug console.\n"));
        WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);

        SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        if (DebugLog)
        {
            _sntprintf(szConsoleBuffer, 1024, _T("Debug messages are logged in zzBurnDebug.html"));
            if (!DebugLog || bEchoLog)
            {
                _sntprintf(szConsoleBuffer + _tcslen(szConsoleBuffer), 1024 - _tcslen(szConsoleBuffer), _T(", and echod to this console"));
            }
            _sntprintf(szConsoleBuffer + _tcslen(szConsoleBuffer), 1024 - _tcslen(szConsoleBuffer), _T(".\n\n"));
        }
        else
        {
            _sntprintf(szConsoleBuffer, 1024, _T("Debug messages are echod to this console.\n\n"));
        }
        WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
    }

    nPrevConsoleStatus = -1;

    bprintf = AppDebugPrintf;							// Redirect Burn library debug to our function
#endif

    return 0;
}

//ÆÁÄ»±ÈÀý¼ì²â
void MonitorAutoCheck()
{
    int x, y, s;

    x = GetSystemMetrics(SM_CXSCREEN);
    y = GetSystemMetrics(SM_CYSCREEN);
	s = x * 100 / y;
	if(s < 120)
	{
		return;
	}
	if(s < 130)
	{
		nVidScrnAspectX = 5;
        nVidScrnAspectY = 4;
		return;
	}
	if(s < 140)
	{
		nVidScrnAspectX = 4;
        nVidScrnAspectY = 3;
		return;
	}
	if(s < 170)
	{
		nVidScrnAspectX = 16;
        nVidScrnAspectY = 10;
		return;
	}
	if(s < 180)
	{
		nVidScrnAspectX = 16;
        nVidScrnAspectY = 9;
		return;
	}
}



static int AppInit()
{
#if defined (_MSC_VER) && defined (_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);			// Check for memory corruption
    _CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF);			//
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);				//
#endif

#if defined (FBA_DEBUG)
    OpenDebugLog();
#endif

    // Create a handle to the main thread of execution
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hMainThread, 0, false, DUPLICATE_SAME_ACCESS);

    // Init the Burn library
    BurnLibInit();

    // Load config for the application
    ConfigAppLoad();

    BurnerDoGameListLocalisation();

    if (bMonitorAutoCheck) MonitorAutoCheck();

#if 1 || !defined (FBA_DEBUG)
    // print a warning if we're running for the 1st time
    if (nIniVersion < nBurnVer)
    {
        ScrnInit();
        FirstUsageCreate();
        ConfigAppSave();								// Create initial config file
    }
#endif

    // Set the thread priority for the main thread
    SetThreadPriority(GetCurrentThread(), nAppThreadPriority);

    bCheatsAllowed = true;

    ComputeGammaLUT();

    if (VidSelect(nVidSelect))
    {
        nVidSelect = 0;
        VidSelect(nVidSelect);
    }

    hAccel = LoadAccelerators(hAppInst, MAKEINTRESOURCE(IDR_ACCELERATOR));

    // Build the ROM information
    CreateROMInfo(NULL);

    // Write a clrmame dat file if we are verifying roms
#if defined (ROM_VERIFY)
    create_datfile(_T("fba.dat"), 0);
#endif

    return 0;
}

static int AppExit()
{
    DrvExit();						// Make sure any game driver is exitted
    FreeROMInfo();
    MediaExit();
    BurnLibExit();					// Exit the Burn library
    BurnerExitGameListLocalisation();

    if (hAccel)
    {
        DestroyAcceleratorTable(hAccel);
        hAccel = NULL;
    }

    CloseHandle(hMainThread);

    CloseDebugLog();

    return 0;
}

void AppCleanup()
{
    WaveLogStop();

    AppExit();
}

int AppMessage(MSG *pMsg)
{
    if (IsDialogMessage(hInpdDlg, pMsg))	 return 0;
    if (IsDialogMessage(hInpCheatDlg, pMsg)) return 0;
    if (IsDialogMessage(hInpDIPSWDlg, pMsg)) return 0;
    if (IsDialogMessage(hDbgDlg, pMsg))		 return 0;

    if (IsDialogMessage(hInpsDlg, pMsg))	 return 0;
    if (IsDialogMessage(hInpcDlg, pMsg))	 return 0;

    return 1; // Didn't process this message
}

bool AppProcessKeyboardInput()
{
    if (hwndChat)
    {
        return false;
    }

    return true;
}

// Main program entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nShowCmd)
{
    DSCore_Init();
    DICore_Init();
    DDCore_Init();
    //Dx9Core_Init();

    // Provide a custom exception handler
    SetUnhandledExceptionFilter(ExceptionFilter);

    hAppInst = hInstance;

    // Make version string
    if (nBurnVer & 0xFF)
    {
        // private version (alpha)
        _stprintf(szAppBurnVer, _T("%x.%x.%x.%02x"), nBurnVer >> 20, (nBurnVer >> 16) & 0x0F, (nBurnVer >> 8) & 0xFF, nBurnVer & 0xFF);
    }
    else
    {
        // public version
        _stprintf(szAppBurnVer, _T("%x.%x.%x"), nBurnVer >> 20, (nBurnVer >> 16) & 0x0F, (nBurnVer >> 8) & 0xFF);
    }

    nAppShowCmd = nShowCmd;

    AppDirectory();								// Set current directory to be the applications directory

    // Make sure there are roms and cfg subdirectories
	CreateDirectory(_T("cheats"), NULL);
	CreateDirectory(_T("config"), NULL);
	CreateDirectory(_T("config/games"), NULL);
	CreateDirectory(_T("config/localisation"), NULL);
	CreateDirectory(_T("config/presets"), NULL);
	CreateDirectory(_T("roms"), NULL);
	CreateDirectory(_T("states"), NULL);

    //
    INITCOMMONCONTROLSEX initCC =
    {
        sizeof(INITCOMMONCONTROLSEX),
        ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_TREEVIEW_CLASSES,
    };
    InitCommonControlsEx(&initCC);

	//AllocConsole();
	//freopen("CONOUT$", "w+t", stdout);
	
    if (!(AppInit()))  						// Init the application
    {
        MediaInit();
        RunMessageLoop();					// Run the application message loop
    }

    ConfigAppSave();						// Save config for the application

    AppExit();								// Exit the application
    
	//fclose(stdout);
	//FreeConsole();
    return 0;
}
