//游戏载入界面
//重新设计
//kerndev@foxmail.com

#include "burner.h"

int nDialogSelect				= -1;										// The driver which this dialog selected
int nOldDlgSelected				= -1;
bool bDialogCancel				= false;
bool bDrvSelected				= false;
HWND hSelDlg					= NULL;

static HWND hSelList			= NULL;
static HWND hParent				= NULL;
static char TreeBuilding		= 0;										// if 1, ignore TVN_SELCHANGED messages

static int CpsValue				= HARDWARE_PREFIX_CAPCOM >> 24;
static int MASKCPS				= 1 << CpsValue;
static int Cps2Value			= HARDWARE_PREFIX_CPS2 >> 24;
static int MASKCPS2				= 1 << Cps2Value;
static int Cps3Value			= HARDWARE_PREFIX_CPS3 >> 24;
static int MASKCPS3				= 1 << Cps3Value;
static int NeogeoValue			= HARDWARE_PREFIX_SNK >> 24;
static int MASKNEOGEO			= 1 << NeogeoValue;
static int PgmValue				= HARDWARE_PREFIX_IGS_PGM >> 24;
static int MASKPGM				= 1 << PgmValue;
static int MASKALL				= MASKCPS | MASKCPS2 | MASKCPS3 | MASKNEOGEO | MASKPGM;

#define UNAVAILABLE				(1 << 28)
#define AVAILABLE				(1 << 29)
#define SHOWCLONE				(1 << 30)
#define ASCIIONLY				(1 << 31)

int nLoadMenuShowX				= AVAILABLE;
int nLoadMenuBoardFilter	    = MASKALL;

typedef struct 
{
	int  nItem;
    int  nBurnDrvNo;
    bool bIsParent;
	bool bIsClone;
}NODEINFO;

static NODEINFO *pDrvNode;
static unsigned int nTmpDrvCount;	//已在列表中的数量

//初始化列表控件
static void SetupListControl(void)
{
	LVCOLUMN list_column;
	ZeroMemory(&list_column,sizeof(list_column));
	list_column.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	list_column.cx = 100;
	list_column.pszText = FBALoadStringEx(hAppInst, IDS_SEL_ZIPNAME, true);
	ListView_InsertColumn(hSelList,0,&list_column);

	list_column.cx = 400;
	list_column.pszText = FBALoadStringEx(hAppInst, IDS_SEL_FULLNAME, true);
	ListView_InsertColumn(hSelList,1,&list_column);

	ListView_SetExtendedListViewStyleEx(hSelList,LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER,LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
}

//检查是否正常工作
static bool IsBurnDrvWorking(int nDriver)
{
    int nOldnBurnDrvActive = nBurnDrvActive;
    nBurnDrvActive = nDriver;
    bool bStatus = BurnDrvIsWorking();
    nBurnDrvActive = nOldnBurnDrvActive;
    return bStatus;
}

//检查搜索关键字过滤
//匹配成功返回1
static int FilterSearch(void)
{
	TCHAR *pzFound;
	TCHAR szText[260];
	TCHAR szSearch[260];
	GetDlgItemText(hSelDlg, IDC_SEL_SEARCH, szSearch, sizeof(szSearch));
	if(szSearch[0])
	{
		_tcsupr(szSearch);
		//搜索zip文件名
		_tcscpy(szText,BurnDrvGetText(DRV_NAME));
		_tcsupr(szText);
		pzFound=_tcsstr(szText,szSearch);
		if(pzFound)
		{
			return 1;
		}
		//搜索游戏名
		_tcscpy(szText,BurnDrvGetText(DRV_FULLNAME));
		_tcsupr(szText);
		pzFound=_tcsstr(szText,szSearch);
		if(pzFound)
		{
			return 1;
		}
		return 0;
	}
	return 1;
}


//插入游戏到列表
static void InsertDrvItem(unsigned int nDrvNo)
{
	UINT flag;
	LVITEM item;
	NODEINFO* pNode;
	pNode = pDrvNode + nTmpDrvCount;
	
	memset(&item, 0, sizeof(item));
	item.iItem      = nTmpDrvCount++;
	pNode->nItem = ListView_InsertItem(hSelList,&item);
	pNode->nBurnDrvNo = nDrvNo;
	pNode->bIsParent  = (BurnDrvGetText(DRV_PARENT) != NULL);
	pNode->bIsClone   = BurnDrvGetFlags()&BDF_CLONE;

	item.iItem      = pNode->nItem;
	item.mask       = LVIF_TEXT;
	item.iSubItem   = 0;
	item.pszText    = BurnDrvGetText(DRV_NAME);
	ListView_SetItem(hSelList,&item);

	flag = (nLoadMenuShowX&ASCIIONLY) ? DRV_FULLNAME|DRV_ASCIIONLY : DRV_FULLNAME;
	item.iSubItem   = 1;
	item.pszText    = BurnDrvGetText(flag);
	ListView_SetItem(hSelList,&item);
}

// Make a tree-view control with all drivers
static void RebuildGameList()
{
    unsigned int i;
	nTmpDrvCount = 0;
    memset(pDrvNode, 0, nBurnDrvCount * sizeof(NODEINFO));
    
	SendMessage(hSelList, WM_SETREDRAW, FALSE, 0);	// disable redraw
	SendMessage(hSelList, LVM_DELETEALLITEMS,0,0);
	
    //添加游戏列表
    for (i = 0; i < nBurnDrvCount; i++)
    {
        nBurnDrvActive = i;
		int nHardware = 1 << (BurnDrvGetHardwareCode() >> 24);
        if ((nHardware & nLoadMenuBoardFilter) == 0)
        {
            continue;
        }
		if((!(nLoadMenuShowX & SHOWCLONE)) && (BurnDrvGetFlags()&BDF_CLONE))
		{
			continue;
		}
		if(avOk)
		{
			if((!(nLoadMenuShowX & AVAILABLE)) && gameAv[i])
			{
				continue;
			}
			if((!(nLoadMenuShowX & UNAVAILABLE)) && (!gameAv[i]))
			{
				continue;
			}
		}
		if (!FilterSearch())
		{
			continue;
		}
		InsertDrvItem(i);
    }
	SendMessage(hSelList, WM_SETREDRAW, TRUE, 0);
}

//选择OK
static void SelectOkay()
{
    NODEINFO* node;
    unsigned int nSelect;
	nSelect = ListView_GetSelectionMark(hSelList);
	if(nSelect < 0)
	{
		return;
	}
	node = pDrvNode+nSelect;
    if (!IsBurnDrvWorking(node->nBurnDrvNo))
    {
        TCHAR szWarningText[1024];
        _stprintf(szWarningText, _T("%s"), FBALoadStringEx(hAppInst, IDS_ERR_WARNING, true));
        if (MessageBox(hSelDlg, FBALoadStringEx(hAppInst, IDS_ERR_NON_WORKING, true), szWarningText, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING) == IDNO)
        {
            return;
        }
    }
    nDialogSelect = node->nBurnDrvNo;
    bDialogCancel = false;
	EndDialog(hSelDlg, 0);
}

static void RefreshPanel()
{
    CheckDlgButton(hSelDlg, IDC_CHECKAUTOEXPAND, (nLoadMenuShowX & SHOWCLONE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_CHECKAVAILABLE, (nLoadMenuShowX & AVAILABLE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_CHECKUNAVAILABLE, (nLoadMenuShowX & UNAVAILABLE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_SEL_ASCIIONLY, nLoadMenuShowX & ASCIIONLY ? BST_CHECKED : BST_UNCHECKED);
	
	CheckDlgButton(hSelDlg, IDC_CHECK1, nLoadMenuBoardFilter& MASKCPS ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hSelDlg, IDC_CHECK2, nLoadMenuBoardFilter& MASKCPS2 ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hSelDlg, IDC_CHECK3, nLoadMenuBoardFilter& MASKCPS3 ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hSelDlg, IDC_CHECK4, nLoadMenuBoardFilter& MASKPGM ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hSelDlg, IDC_CHECK5, nLoadMenuBoardFilter& MASKNEOGEO ? BST_CHECKED : BST_UNCHECKED);
}

static void UpdateCounter(void)
{
    TCHAR szRomsAvailableInfo[128];
    _stprintf(szRomsAvailableInfo, FBALoadStringEx(hAppInst, IDS_SEL_SETSTATUS, true), nTmpDrvCount, nBurnDrvCount);
    SendDlgItemMessage(hSelDlg, IDC_DRVCOUNT, WM_SETTEXT, 0, (LPARAM)szRomsAvailableInfo);
}

static void RebuildEverything()
{
    RefreshPanel();
    bDrvSelected = false;
    TreeBuilding = 1;
    RebuildGameList();
	UpdateCounter();
    TreeBuilding = 0;
}

static BOOL OnInitDialog(HWND hDlg)
{
	hSelDlg  = hDlg;
    hSelList = GetDlgItem(hDlg, IDC_LIST1);
	SetupListControl();
    RebuildEverything();
    WndInMid(hDlg, hParent);
    return TRUE;
}

static void OnCommand(HWND hDlg, WPARAM wParam)
{
	int wID;
	wID = LOWORD(wParam);
	switch(HIWORD(wParam))
	{
	case EN_CHANGE:
		if(wID==IDC_SEL_SEARCH)
		{
			RebuildEverything();
		}
		break;
	case BN_CLICKED:
		switch (wID)
        {
        case IDOK:
            SelectOkay();
            break;
        case IDROM:
            RomsDirCreate(hSelDlg);
            RebuildEverything();
            break;
        case IDRESCAN:
            bRescanRoms = true;
            CreateROMInfo(hSelDlg);
            RebuildEverything();
            break;
        case IDCANCEL:
            bDialogCancel = true;
            SendMessage(hDlg, WM_CLOSE, 0, 0);
            break;
        case IDC_CHECKAVAILABLE:
            nLoadMenuShowX ^= AVAILABLE;
            RebuildEverything();
            break;
        case IDC_CHECKUNAVAILABLE:
            nLoadMenuShowX ^= UNAVAILABLE;
            RebuildEverything();
            break;
        case IDC_CHECKAUTOEXPAND:
            nLoadMenuShowX ^= SHOWCLONE;
            RebuildEverything();
            break;
        case IDC_SEL_ASCIIONLY:
            nLoadMenuShowX ^= ASCIIONLY;
            RebuildEverything();
            break;
		case IDC_CHECK1:
			nLoadMenuBoardFilter ^= MASKCPS;
			RebuildEverything();
			break;
		case IDC_CHECK2:
			nLoadMenuBoardFilter ^= MASKCPS2;
			RebuildEverything();
			break;
		case IDC_CHECK3:
			nLoadMenuBoardFilter ^= MASKCPS3;
			RebuildEverything();
			break;
		case IDC_CHECK4:
			nLoadMenuBoardFilter ^= MASKPGM;
			RebuildEverything();
			break;
		case IDC_CHECK5:
			nLoadMenuBoardFilter ^= MASKNEOGEO;
			RebuildEverything();
			break;
        }
		break;
	}
}

//双击列表启动游戏
static BOOL OnListLDoubleClicked(HWND hDlg, NMITEMACTIVATE* pnmia)
{
	if (!TreeBuilding && (pnmia->iItem>=0))
	{
		SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, 1);
		SelectOkay();
		return 1;
	}
	return 0;
}

//自绘控件
static BOOL OnList1CustomDraw(HWND hDlg, LPNMLVCUSTOMDRAW lplvcd)
{
	NODEINFO* node;
    switch (lplvcd->nmcd.dwDrawStage)
    {
   	case CDDS_PREPAINT:
		SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
	    return 1;
    case CDDS_ITEMPREPAINT:
		node = pDrvNode+lplvcd->nmcd.dwItemSpec;
		if(!(lplvcd->nmcd.uItemState&CDIS_FOCUS))
		{
            //设置不工作的颜色
            if (!IsBurnDrvWorking(node->nBurnDrvNo))
            {
                lplvcd->clrText = RGB(220, 40, 40);
            }
			//设置克隆的颜色
            if (node->bIsClone)
            {
                lplvcd->clrText = RGB(128, 128, 128);
            }
		}
	    return 1;
    }
	return 0;
}

static BOOL OnNotify(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    NMHDR *pNmHdr = (NMHDR *)lParam;
	if(pNmHdr->idFrom == IDC_LIST1)
	{
		switch(pNmHdr->code)
		{
		case NM_DBLCLK:
			OnListLDoubleClicked(hDlg, (NMITEMACTIVATE*)lParam);
			return 1;
		case NM_CUSTOMDRAW:
			OnList1CustomDraw(hDlg, (LPNMLVCUSTOMDRAW)lParam);
			return 1;
		}
	}
	return 0;
}

static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hDlg);
		return TRUE;
	case WM_COMMAND:
		OnCommand(hDlg, wParam);
		return 0;
	case WM_CLOSE:
		bDialogCancel = true;
        nDialogSelect = nOldDlgSelected;
        EndDialog(hSelDlg, 0);
		return 1;
	case WM_NOTIFY:
		return OnNotify(hDlg, wParam, lParam);
	}
    return 0;
}

int PopupSelectDialog(HWND hParentWND)
{
    int nOldSelect = nBurnDrvActive;
	hParent = hParentWND;
    if(bDrvOkay)
    {
        nOldDlgSelected = nBurnDrvActive;
    }
	pDrvNode = (NODEINFO *)malloc(nBurnDrvCount * sizeof(NODEINFO));
	memset(pDrvNode, 0, nBurnDrvCount * sizeof(NODEINFO));
    FBADialogBox(hAppInst, MAKEINTRESOURCE(IDD_SELNEW), hParent, (DLGPROC)DialogProc);
	free(pDrvNode);
    hSelDlg = NULL;
    nBurnDrvActive = nOldSelect;
    return bDialogCancel?-1:nDialogSelect;
}

