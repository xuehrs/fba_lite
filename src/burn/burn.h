// FB Alpha - Emulator for MC68000/Z80 based arcade games
//            Refer to the "license.txt" file for more info

// Burner emulation library

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (_WIN32)
#define __cdecl
#endif

#ifndef MAX_PATH
#define MAX_PATH 	260
#endif

#include <time.h>


// Enable the MAME logerror() function in debug builds
// #define MAME_USE_LOGERROR

// Give access to the CPUID function for various compilers
#if defined (__GNUC__)
#define CPUID(f,ra,rb,rc,rd) __asm__ __volatile__ ("cpuid"											\
 													: "=a" (ra), "=b" (rb), "=c" (rc), "=d" (rd)	\
 													: "a"  (f)										\
 												   );
#elif defined (_MSC_VER)
#define CPUID(f,ra,rb,rc,rd) __asm { __asm mov		eax, f		\
									  __asm cpuid				\
									  __asm mov		ra, eax		\
									  __asm mov		rb, ebx		\
									  __asm mov		rc, ecx		\
									  __asm mov		rd, edx }
#else
#define CPUID(f,ra,rb,rc,rd)
#endif

#ifndef BUILD_X86_ASM
#undef CPUID
#define CPUID(f,ra,rb,rc,rd)
#endif

#ifdef _UNICODE
#define SEPERATOR_1 " \u2022 "
#define SEPERATOR_2 " \u25E6 "
#else
#define SEPERATOR_1 " ~ "
#define SEPERATOR_2 " ~ "
#endif

#ifdef _UNICODE
#define WRITE_UNICODE_BOM(file) { UINT16 BOM[] = { 0xFEFF }; fwrite(BOM, 2, 1, file); }
#else
#define WRITE_UNICODE_BOM(file)
#endif

typedef unsigned char						UINT8;
typedef signed char 						INT8;
typedef unsigned short						UINT16;
typedef signed short						INT16;
typedef unsigned int						UINT32;
typedef signed int							INT32;
#ifdef _MSC_VER
typedef signed __int64						INT64;
typedef unsigned __int64					UINT64;
#else
__extension__ typedef unsigned long long	UINT64;
__extension__ typedef long long				INT64;
#endif

#include "state.h"
#include "cheat.h"

extern INT32 nBurnVer;						// Version number of the library

enum BurnCartrigeCommand { CART_INIT_START, CART_INIT_END, CART_EXIT };

// ---------------------------------------------------------------------------
// Callbacks

// Application-defined rom loading function
extern INT32 (__cdecl *BurnExtLoadRom)(UINT8 *Dest, INT32 *pnWrote, INT32 i);

// Application-defined progress indicator functions
extern INT32 (__cdecl *BurnExtProgressRangeCallback)(double dProgressRange);
extern INT32 (__cdecl *BurnExtProgressUpdateCallback)(double dProgress, const TCHAR *pszText, bool bAbs);

// Application-defined catridge initialisation function
extern INT32 (__cdecl *BurnExtCartridgeSetupCallback)(BurnCartrigeCommand nCommand);

// Application-defined colour conversion function
extern UINT32 (__cdecl *BurnHighCol) (INT32 r, INT32 g, INT32 b, INT32 i);

// ---------------------------------------------------------------------------

extern UINT32 nCurrentFrame;

inline static INT32 GetCurrentFrame()
{
    return nCurrentFrame;
}

inline static void SetCurrentFrame(const UINT32 n)
{
    nCurrentFrame = n;
}

// ---------------------------------------------------------------------------
// Driver info structures

// ROMs

#define BRF_PRG				(1 << 20)
#define BRF_GRA				(1 << 21)
#define BRF_SND				(1 << 22)

#define BRF_ESS				(1 << 24)
#define BRF_BIOS			(1 << 25)
#define BRF_SELECT			(1 << 26)
#define BRF_OPT				(1 << 27)
#define BRF_NODUMP			(1 << 28)

struct BurnRomInfo
{
    char szName[100];
    UINT32 nLen;
    UINT32 nCrc;
    UINT32 nType;
};

struct BurnSampleInfo
{
    char szName[100];
    UINT32 nFlags;
};

// Inputs

#define BIT_DIGITAL			(1)

#define BIT_GROUP_ANALOG	(4)
#define BIT_ANALOG_REL		(4)
#define BIT_ANALOG_ABS		(5)

#define BIT_GROUP_CONSTANT	(8)
#define BIT_CONSTANT		(8)
#define BIT_DIPSWITCH		(9)

struct BurnInputInfo
{
    char *szName;
    UINT8 nType;
    union
    {
        UINT8 *pVal;					// Most inputs use a char*
        UINT16 *pShortVal;				// All analog inputs use a short*
    };
    char *szInfo;
};

// DIPs

struct BurnDIPInfo
{
    INT32 nInput;
    UINT8 nFlags;
    UINT8 nMask;
    UINT8 nSetting;
    char *szText;
};


// ---------------------------------------------------------------------------
// Common CPU definitions

#define CPU_IRQSTATUS_NONE	0
#define CPU_IRQSTATUS_ACK	1
#define CPU_IRQSTATUS_AUTO	2
#define CPU_IRQSTATUS_HOLD	4

#define CPU_IRQLINE0		0
#define CPU_IRQLINE1		1
#define CPU_IRQLINE2		2
#define CPU_IRQLINE3		3
#define CPU_IRQLINE4		4
#define CPU_IRQLINE5		5
#define CPU_IRQLINE6		6
#define CPU_IRQLINE7		7

#define CPU_IRQLINE_IRQ		CPU_IRQLINE0
#define CPU_IRQLINE_FIRQ	CPU_IRQLINE1
#define CPU_IRQLINE_NMI		0x20

#define MAP_READ		1
#define MAP_WRITE		2
#define MAP_FETCHOP		4
#define MAP_FETCHARG		8
#define MAP_FETCH		(MAP_FETCHOP|MAP_FETCHARG)
#define MAP_ROM			(MAP_READ|MAP_FETCH)
#define MAP_RAM			(MAP_ROM|MAP_WRITE)


// ---------------------------------------------------------------------------

extern bool bBurnUseMMX;
extern bool bBurnUseASMCPUEmulation;

extern UINT32 nFramesEmulated;
extern UINT32 nFramesRendered;
extern clock_t starttime;					// system time when emulation started and after roms loaded

extern bool bForce60Hz;

extern INT32 nBurnFPS;
extern INT32 nBurnCPUSpeedAdjust;

extern UINT32 nBurnDrvCount;			// Count of game drivers
extern UINT32 nBurnDrvActive;			// Which game driver is selected
extern UINT32 nBurnDrvSelect[8];		// Which games are selected (i.e. loaded but not necessarily active)

extern INT32 nMaxPlayers;

extern UINT8 *pBurnDraw;			// Pointer to correctly sized bitmap
extern INT32 nBurnPitch;						// Pitch between each line
extern INT32 nBurnBpp;						// Bytes per pixel (2, 3, or 4)

extern UINT8 nBurnLayer;			// Can be used externally to select which layers to show
extern UINT8 nSpriteEnable;			// Can be used externally to select which Sprites to show

extern INT32 nBurnSoundRate;					// Samplerate of sound
extern INT32 nBurnSoundLen;					// Length in samples per frame
extern INT16 *pBurnSoundOut;				// Pointer to output buffer

extern INT32 nInterpolation;					// Desired interpolation level for ADPCM/PCM sound
extern INT32 nFMInterpolation;				// Desired interpolation level for FM sound

extern UINT32 *pBurnDrvPalette;

#define PRINT_NORMAL	(0)
#define PRINT_UI		(1)
#define PRINT_IMPORTANT (2)
#define PRINT_ERROR		(3)

#ifndef bprintf
extern INT32 (__cdecl *bprintf) (INT32 nStatus, TCHAR *szFormat, ...);
#endif

INT32 BurnLibInit();
INT32 BurnLibExit();

INT32 BurnDrvInit();
INT32 BurnDrvExit();

INT32 BurnDrvCartridgeSetup(BurnCartrigeCommand nCommand);

INT32 BurnDrvFrame();
INT32 BurnDrvRedraw();
INT32 BurnRecalcPal();
INT32 BurnDrvGetPaletteEntries();

INT32 BurnSetProgressRange(double dProgressRange);
INT32 BurnUpdateProgress(double dProgressStep, const TCHAR *pszText, bool bAbs);

void BurnLocalisationSetName(char *szName, TCHAR *szLongName);

// ---------------------------------------------------------------------------
// Retrieve driver information

#define DRV_NAME		 (0)
#define DRV_DATE		 (1)
#define DRV_FULLNAME	 (2)
//#define DRV_MEDIUMNAME	 (3)
#define DRV_COMMENT		 (4)
#define DRV_MANUFACTURER (5)
#define DRV_SYSTEM		 (6)
#define DRV_PARENT		 (7)
#define DRV_BOARDROM	 (8)
#define DRV_SAMPLENAME	 (9)

#define DRV_NEXTNAME	 (1 << 8)
#define DRV_ASCIIONLY	 (1 << 12)
#define DRV_UNICODEONLY	 (1 << 13)

TCHAR *BurnDrvGetText(UINT32 i);
char *BurnDrvGetTextA(UINT32 i);

INT32 BurnDrvGetZipName(char **pszName, UINT32 i);
INT32 BurnDrvGetRomInfo(struct BurnRomInfo *pri, UINT32 i);
INT32 BurnDrvGetRomName(char **pszName, UINT32 i, INT32 nAka);
INT32 BurnDrvGetInputInfo(struct BurnInputInfo *pii, UINT32 i);
INT32 BurnDrvGetDIPInfo(struct BurnDIPInfo *pdi, UINT32 i);
INT32 BurnDrvGetVisibleSize(INT32 *pnWidth, INT32 *pnHeight);
INT32 BurnDrvGetVisibleOffs(INT32 *pnLeft, INT32 *pnTop);
INT32 BurnDrvGetFullSize(INT32 *pnWidth, INT32 *pnHeight);
INT32 BurnDrvGetAspect(INT32 *pnXAspect, INT32 *pnYAspect);
INT32 BurnDrvGetHardwareCode();
INT32 BurnDrvGetFlags();
bool BurnDrvIsWorking();
INT32 BurnDrvGetMaxPlayers();
INT32 BurnDrvSetVisibleSize(INT32 pnWidth, INT32 pnHeight);
INT32 BurnDrvSetAspect(INT32 pnXAspect, INT32 pnYAspect);
INT32 BurnDrvGetGenreFlags();
INT32 BurnDrvGetFamilyFlags();
INT32 BurnDrvGetSampleInfo(struct BurnSampleInfo *pri, UINT32 i);
INT32 BurnDrvGetSampleName(char **pszName, UINT32 i, INT32 nAka);

void Reinitialise();

extern bool bDoIpsPatch;
void IpsApplyPatches(UINT8 *base, char *rom_name);

// ---------------------------------------------------------------------------
// Flags used with the Burndriver structure

// Flags for the flags member
#define BDF_GAME_WORKING								(1 << 0)
#define BDF_ORIENTATION_FLIPPED							(1 << 1)
#define BDF_ORIENTATION_VERTICAL						(1 << 2)
#define BDF_BOARDROM									(1 << 3)
#define BDF_CLONE										(1 << 4)
#define BDF_BOOTLEG										(1 << 5)
#define BDF_PROTOTYPE									(1 << 6)
#define BDF_16BIT_ONLY									(1 << 7)
#define BDF_HACK										(1 << 8)
#define BDF_HOMEBREW									(1 << 9)
#define BDF_DEMO										(1 << 10)
#define BDF_HISCORE_SUPPORTED							(1 << 11)

// Flags for the hardware member
// Format: 0xDDEEFFFF, where EE: Manufacturer, DD: Hardware platform, FFFF: Flags (used by driver)

#define HARDWARE_PUBLIC_MASK							(0xFFFF0000)

#define HARDWARE_PREFIX_CARTRIDGE						((INT32)0x80000000)

#define HARDWARE_PREFIX_CAPCOM							(0x01000000)
#define HARDWARE_PREFIX_SNK								(0x05000000)
#define HARDWARE_PREFIX_CPS2							(0x07000000)
#define HARDWARE_PREFIX_IGS_PGM							(0x08000000)
#define HARDWARE_PREFIX_CPS3							(0x09000000)

#define HARDWARE_MISC_PRE90S							(HARDWARE_PREFIX_MISC_PRE90S)
#define HARDWARE_MISC_POST90S							(HARDWARE_PREFIX_MISC_POST90S)

#define HARDWARE_CAPCOM_CPS1							(HARDWARE_PREFIX_CAPCOM | 0x00010000)
#define HARDWARE_CAPCOM_CPS1_QSOUND 					(HARDWARE_PREFIX_CAPCOM | 0x00020000)
#define HARDWARE_CAPCOM_CPS1_GENERIC 					(HARDWARE_PREFIX_CAPCOM | 0x00030000)
#define HARDWARE_CAPCOM_CPSCHANGER						(HARDWARE_PREFIX_CAPCOM | 0x00040000)
#define HARDWARE_CAPCOM_CPS2							(HARDWARE_PREFIX_CPS2 | 0x00010000)
#define HARDWARE_CAPCOM_CPS2_SIMM						(0x0002)

#define HARDWARE_SNK_NEOGEO								(HARDWARE_PREFIX_SNK | 0x00010000)
#define HARDWARE_SNK_SWAPP								(0x0001)	// Swap code roms
#define HARDWARE_SNK_SWAPV								(0x0002)	// Swap sound roms
#define HARDWARE_SNK_SWAPC								(0x0004)	// Swap sprite roms
#define HARDWARE_SNK_CMC42								(0x0008)	// CMC42 encryption chip
#define HARDWARE_SNK_CMC50								(0x0010)	// CMC50 encryption chip
#define HARDWARE_SNK_ALTERNATE_TEXT						(0x0020)	// KOF2000 text layer banks
#define HARDWARE_SNK_SMA_PROTECTION						(0x0040)	// SMA protection
#define HARDWARE_SNK_KOF2K3								(0x0080)	// KOF2K3 hardware
#define HARDWARE_SNK_ENCRYPTED_M1						(0x0100)	// M1 encryption
#define HARDWARE_SNK_P32								(0x0200)	// SWAP32 P ROMs
#define HARDWARE_SNK_SPRITE32							(0x0400)

#define HARDWARE_SNK_CONTROLMASK						(0xF000)
#define HARDWARE_SNK_JOYSTICK							(0x0000)	// Uses joysticks
#define HARDWARE_SNK_PADDLE								(0x1000)	// Uses joysticks or paddles
#define HARDWARE_SNK_TRACKBALL							(0x2000)	// Uses a trackball
#define HARDWARE_SNK_4_JOYSTICKS						(0x3000)	// Uses 4 joysticks
#define HARDWARE_SNK_MAHJONG							(0x4000)	// Uses a special mahjong controller
#define HARDWARE_SNK_GAMBLING							(0x5000)	// Uses gambling controls

#define HARDWARE_SNK_MVS								(HARDWARE_PREFIX_SNK | 0x00020000)
#define HARDWARE_SNK_NEOCD								(HARDWARE_PREFIX_SNK | 0x00030000)
#define HARDWARE_SNK_DEDICATED_PCB						(HARDWARE_PREFIX_SNK | 0x00040000)

#define HARDWARE_IGS_PGM								(HARDWARE_PREFIX_IGS_PGM)
#define HARDWARE_IGS_USE_ARM_CPU						(0x0001)

#define HARDWARE_CAPCOM_CPS3							(HARDWARE_PREFIX_CPS3)
#define HARDWARE_CAPCOM_CPS3_NO_CD   					(0x0001)

// flags for the genre member
#define GBF_HORSHOOT									(1 << 0)
#define GBF_VERSHOOT									(1 << 1)
#define GBF_SCRFIGHT									(1 << 2)
#define GBF_VSFIGHT										(1 << 3)
#define GBF_BIOS										(1 << 4)
#define GBF_BREAKOUT									(1 << 5)
#define GBF_CASINO										(1 << 6)
#define GBF_BALLPADDLE									(1 << 7)
#define GBF_MAZE										(1 << 8)
#define GBF_MINIGAMES									(1 << 9)
#define GBF_PINBALL										(1 << 10)
#define GBF_PLATFORM									(1 << 11)
#define GBF_PUZZLE										(1 << 12)
#define GBF_QUIZ										(1 << 13)
#define GBF_SPORTSMISC									(1 << 14)
#define GBF_SPORTSFOOTBALL								(1 << 15)
#define GBF_MISC										(1 << 16)
#define GBF_MAHJONG										(1 << 17)
#define GBF_RACING										(1 << 18)
#define GBF_SHOOT										(1 << 19)

// flags for the family member
#define FBF_MSLUG										(1 << 0)
#define FBF_SF											(1 << 1)
#define FBF_KOF											(1 << 2)
#define FBF_DSTLK										(1 << 3)
#define FBF_FATFURY										(1 << 4)
#define FBF_SAMSHO										(1 << 5)
#define FBF_19XX										(1 << 6)
#define FBF_SONICWI										(1 << 7)
#define FBF_PWRINST										(1 << 8)

#ifdef __cplusplus
} // End of extern "C"
#endif

