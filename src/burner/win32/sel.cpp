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

#define UNAVAILABLE				(1 << 27)
#define AVAILABLE				(1 << 28)
#define AUTOEXPAND				(1 << 29)
#define SHOWSHORT				(1 << 30)
#define ASCIIONLY				(1 << 31)

int nLoadMenuShowX				= 0;
int nLoadMenuBoardTypeFilter	= 0;

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
	list_column.pszText = _T("ZIP");
	ListView_InsertColumn(hSelList,0,&list_column);

	list_column.cx = 400;
	list_column.pszText = _T("GAME");
	ListView_InsertColumn(hSelList,1,&list_column);

	ListView_SetExtendedListViewStyleEx(hSelList,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
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
	LV_ITEM item;
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

	item.iSubItem   = 1;
	item.pszText    = BurnDrvGetText(DRV_FULLNAME);
	ListView_SetItem(hSelList,&item);
}

// Make a tree-view control with all drivers
static int UpdateSelectList()
{
    unsigned int i;
	nTmpDrvCount = 0;
    memset(pDrvNode, 0, nBurnDrvCount * sizeof(NODEINFO));
    
	SendMessage(hSelList, WM_SETREDRAW, FALSE, 0);	// disable redraw
	SendMessage(hSelList, LVM_DELETEALLITEMS,0,0);

    // Add all the driver names to the list
    for (i = 0; i < nBurnDrvCount; i++)
    {
        nBurnDrvActive = i;
		int nHardware = 1 << (BurnDrvGetHardwareCode() >> 24);
        if ((nHardware & MASKALL) == 0)
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
    return 0;
}

//选择OK
static void SelectOkay()
{
    TV_ITEM TvItem;
    unsigned int nSelect = 0;
    HTREEITEM hSelectHandle = (HTREEITEM)SendMessage(hSelList, TVM_GETNEXTITEM, TVGN_CARET, ~0U);

    if (!hSelectHandle)	 			// Nothing is selected, return without closing the window
    {
        return;
    }

    TvItem.hItem = hSelectHandle;
    TvItem.mask = TVIF_PARAM;
    SendMessage(hSelList, TVM_GETITEM, 0, (LPARAM)&TvItem);
    nSelect = ((NODEINFO *)TvItem.lParam)->nBurnDrvNo;
	
    if (!IsBurnDrvWorking(nSelect))
    {
        TCHAR szWarningText[1024];
        _stprintf(szWarningText, _T("%s"), FBALoadStringEx(hAppInst, IDS_ERR_WARNING, true));
        if (MessageBox(hSelDlg, FBALoadStringEx(hAppInst, IDS_ERR_NON_WORKING, true), szWarningText, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING) == IDNO)
        {
            return;
        }
    }
    nDialogSelect = nSelect;
    bDialogCancel = false;
	EndDialog(hSelDlg, 0);
}

static void RefreshPanel()
{
    CheckDlgButton(hSelDlg, IDC_CHECKAUTOEXPAND, (nLoadMenuShowX & AUTOEXPAND) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_CHECKAVAILABLE, (nLoadMenuShowX & AVAILABLE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_CHECKUNAVAILABLE, (nLoadMenuShowX & UNAVAILABLE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_SEL_SHORTNAME, nLoadMenuShowX & SHOWSHORT ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hSelDlg, IDC_SEL_ASCIIONLY, nLoadMenuShowX & ASCIIONLY ? BST_CHECKED : BST_UNCHECKED);
}

static void UpdateCounter(void)
{
    TCHAR szRomsAvailableInfo[128];
    _stprintf(szRomsAvailableInfo, FBALoadStringEx(hAppInst, IDS_SEL_SETSTATUS, true), nTmpDrvCount, nBurnDrvCount, nBurnDrvCount-nTmpDrvCount);
    SendDlgItemMessage(hSelDlg, IDC_DRVCOUNT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szRomsAvailableInfo);
}

static void RebuildEverything()
{
    RefreshPanel();
    bDrvSelected = false;
    TreeBuilding = 1;
    UpdateSelectList();
	UpdateCounter();
    TreeBuilding = 0;
}

static BOOL OnInitDialog(HWND hDlg)
{
	hSelDlg  = hDlg;
    hSelList = GetDlgItem(hSelDlg, IDC_LIST1);
	pDrvNode = (NODEINFO *)malloc(nBurnDrvCount * sizeof(NODEINFO));
    memset(pDrvNode, 0, nBurnDrvCount * sizeof(NODEINFO));
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
            nLoadMenuShowX ^= AUTOEXPAND;
            RebuildEverything();
            break;
        case IDC_SEL_ASCIIONLY:
            nLoadMenuShowX ^= ASCIIONLY;
            RebuildEverything();
            break;
        }
		break;
	}
}

//双击列表启动游戏
static BOOL OnTree1DblClick(HWND hDlg, NMTREEVIEW* pnmtv)
{
	if (!TreeBuilding)
	{
		DWORD dwpos = GetMessagePos();
		TVHITTESTINFO thi;
		thi.pt.x	= GET_X_LPARAM(dwpos);
		thi.pt.y	= GET_Y_LPARAM(dwpos);

		MapWindowPoints(HWND_DESKTOP, hSelList, &thi.pt, 1);
		TreeView_HitTest(hSelList, &thi);
		HTREEITEM hSelectHandle = thi.hItem;
		if(hSelectHandle == NULL) return 1;
		TreeView_SelectItem(hSelList, hSelectHandle);

		NODEINFO* node;
		TVITEM TvItem;
	    TvItem.hItem = hSelectHandle;
	    TvItem.mask = TVIF_PARAM;
	    SendMessage(hSelList, TVM_GETITEM, 0, (LPARAM)&TvItem);
		node = (NODEINFO*)TvItem.lParam;
		nBurnDrvActive	= node->nBurnDrvNo;
        nDialogSelect	= node->nBurnDrvNo;
        bDrvSelected	= true;

		SelectOkay();
		SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, 1);
		return 1;
	}
	return 0;
}

//自绘控件
static BOOL OnTree1CustomDraw(HWND hDlg, LPNMLVCUSTOMDRAW lplvcd)
{
    switch (lplvcd->nmcd.dwDrawStage)
    {
   	case CDDS_PREPAINT:
		SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
	    return 1;
    case CDDS_ITEMPREPAINT:
		SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, CDRF_NOTIFYSUBITEMDRAW);
	    return 1;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		{
			int i;
			NODEINFO* node;
	        i = lplvcd->nmcd.dwItemSpec;
			node = pDrvNode+i;
	        //设置非选中状态的颜色
	        //if (!(TvItem.state & (TVIS_SELECTED | TVIS_DROPHILITED)))
	        {
	            //设置克隆的颜色
	            if (!node->bIsParent)
	            {
	                lplvcd->clrTextBk = RGB(0xD7, 0xD7, 0xD7);
	            }
	            //设置不工作的颜色
	            if (!IsBurnDrvWorking(node->nBurnDrvNo))
	            {
	                lplvcd->clrText = RGB(0x7F, 0x7F, 0x7F);
	            }
				//设置不可用的颜色
				if(!gameAv[node->nBurnDrvNo])
				{
					lplvcd->clrText = RGB(0xFF, 0x10, 0x10);
				}
	        }
	        break;
	    }
    }
	SetWindowLongPtr(hSelDlg, DWLP_MSGRESULT, CDRF_DODEFAULT);
	return 1;
}

//选中
static void OnTree1SelChanged(HWND hDlg)
{
    if (!TreeBuilding)
    {
        HTREEITEM hSelectHandle = (HTREEITEM)SendMessage(hSelList, TVM_GETNEXTITEM, TVGN_CARET, ~0U);
		NODEINFO* node;
		TVITEM TvItem;
	    TvItem.hItem = hSelectHandle;
	    TvItem.mask = TVIF_PARAM;
	    SendMessage(hSelList, TVM_GETITEM, 0, (LPARAM)&TvItem);
		node = (NODEINFO*)TvItem.lParam;
		nBurnDrvActive	= node->nBurnDrvNo;
        nDialogSelect	= node->nBurnDrvNo;
        bDrvSelected	= true;
    }
}

static BOOL OnNotify(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    NMHDR *pNmHdr = (NMHDR *)lParam;
	if(LOWORD(wParam) != IDC_LIST1)
	{
		return 0;
	}
	switch(pNmHdr->code)
	{
	case NM_DBLCLK:
		return OnTree1DblClick(hDlg, (NMTREEVIEW *)lParam);
	case NM_CUSTOMDRAW:
		return OnTree1CustomDraw(hDlg, (LPNMLVCUSTOMDRAW)lParam);
	case LVN_ITEMACTIVATE:
		OnTree1SelChanged(hDlg);
		return 0;
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
		return 0;
	case WM_NOTIFY:
		OnNotify(hDlg, wParam, lParam);
		break;
	}
    return 0;
}

int PopupSelectDialog(HWND hParentWND)
{
    int nOldSelect = nBurnDrvActive;

    if(bDrvOkay)
    {
        nOldDlgSelected = nBurnDrvActive;
    }
    hParent = hParentWND;

    FBADialogBox(hAppInst, MAKEINTRESOURCE(IDD_SELNEW), hParent, (DLGPROC)DialogProc);

    hSelDlg = NULL;
    if (pDrvNode)
    {
        free(pDrvNode);
        pDrvNode = NULL;
    }
    nBurnDrvActive = nOldSelect;
    return nDialogSelect;
}

