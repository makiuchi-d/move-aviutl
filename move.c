/*********************************************************************
* 	移動ツール プラグイン  for AviUtl
* 
* 正明さんアイディアありがとー
* 
* 2003
* 	05/02:	完成
* 	05/03:	「～のキーフレームに」の動作が間違っていたのを修正
* 			プロファイル変更フレームに移動を追加
* 
*********************************************************************/
#include <windows.h>
#include <commctrl.h>
#include "filter.h"


// ボタンID
#define ID_BTN_LSRK   40001
#define ID_BTN_NSRK   40002
#define ID_BTN_LMRK   40003
#define ID_BTN_NMRK   40004
#define ID_BTN_LPRF   40005
#define ID_BTN_NPRF   40006
#define ID_BTN_LKEY   40007
#define ID_BTN_NKEY   40008
#define ID_BTN_LDEL   40009
#define ID_BTN_NDEL   40010
#define ID_BTN_LCPY   40011
#define ID_BTN_NCPY   40012
#define ID_BTN_STRT   40013
#define ID_BTN_END    40014
#define ID_BTN_FRST   40015
#define ID_BTN_LAST   40016

// チップテキスト
#define TSTR_LSRK  "前のキーフレームに移動"
#define TSTR_NSRK  "次のキーフレームに移動"
#define TSTR_LMRK  "前のマークフレームに移動"
#define TSTR_NMRK  "次のマークフレームに移動"

#define TSTR_LPRF  "前のプロファイルの変更フレームに移動"
#define TSTR_NPRF  "次のプロファイルの変更フレームに移動"
#define TSTR_LKEY  "前のキー設定フレームに移動"
#define TSTR_NKEY  "次のキー設定フレームに移動"
#define TSTR_LDEL  "前の優先間引き設定フレームに移動"
#define TSTR_NDEL  "次の優先間引き設定フレームに移動"
#define TSTR_LCPY  "前のコピー設定フレームに移動"
#define TSTR_NCPY  "次のコピー設定フレームに移動"
#define TSTR_STRT  "選択開始フレームに移動"
#define TSTR_END   "選択終了フレームに移動"
#define TSTR_FRST  "先頭のフレームに移動"
#define TSTR_LAST  "最後のフレームに移動"

// ボタンウィンドウハンドル
HWND hNxtSrK,hLstSrK;
HWND hNxtMrk,hLstMrk;
HWND hLstPrf,hNxtPrf;
HWND hLstKey,hNxtKey;
HWND hNxtDel,hLstDel;
HWND hNxtCpy,hLstCpy;
HWND hStart, hEnd;
HWND hFirst, hLast;


// プロトタイプ
static void CreateTooltip (HWND hwnd,HINSTANCE hinst,char *tipstr);
static BOOL wm_filter_init(FILTER* fp);
static BOOL go_next_flag(FILTER* fp,void *editp,UINT flag);
static BOOL go_last_flag(FILTER* fp,void *editp,UINT flag);
static BOOL go_first_frame(FILTER* fp,void *editp);
static BOOL go_start_frame(FILTER* fp,void *editp);
static BOOL go_end_frame(FILTER* fp,void *editp);
static BOOL go_last_frame(FILTER* fp,void *editp);
static BOOL go_last_sourcekey(FILTER* fp,void *editp);
static BOOL go_next_sourcekey(FILTER* fp,void *editp);
static BOOL go_last_profilechange(FILTER* fp,void *editp);
static BOOL go_next_profilechange(FILTER* fp,void *editp);


//----------------------------
//	FILTER_DLL構造体
//----------------------------
char filter_name[] = "移動ツール";
char filter_info[] = "移動ツール ver0.02 by MakKi";
#define track_N 0
#if track_N
TCHAR *track_name[]   = { 0 };	// トラックバーの名前
int   track_default[] = { 0 };	// トラックバーの初期値
int   track_s[]       = { 0 };	// トラックバーの下限値
int   track_e[]       = { 0 };	// トラックバーの上限値
#endif
#define check_N 0
#if check_N
TCHAR *check_name[]   = { 0 };	// チェックボックス
int   check_default[] = { 0 };	// デフォルト
#endif


FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_DISP_FILTER	|
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_EX_INFORMATION,
	160,190,			// 設定ウインドウのサイズ
	filter_name,		// フィルタの名前
	track_N,        	// トラックバーの数
#if track_N
	track_name,     	// トラックバーの名前郡
	track_default,  	// トラックバーの初期値郡
	track_s,track_e,	// トラックバーの数値の下限上限
#else
	NULL,NULL,NULL,NULL,
#endif
	check_N,      	// チェックボックスの数
#if check_N
	check_name,   	// チェックボックスの名前郡
	check_default,	// チェックボックスの初期値郡
#else
	NULL,NULL,
#endif
	NULL,   	// フィルタ処理関数
	NULL,NULL,   	// 開始時,終了時に呼ばれる関数
	NULL,        	// 設定が変更されたときに呼ばれる関数
	func_WndProc,	// 設定ウィンドウプロシージャ
	NULL,NULL,   	// システムで使用
	NULL,NULL,     	// 拡張データ領域
	filter_info,	// フィルタ情報
	NULL,			// セーブ開始直前に呼ばれる関数
	NULL,			// セーブ終了時に呼ばれる関数
	NULL,NULL,NULL,	// システムで使用
	NULL,			// 拡張領域初期値
};

/*********************************************************************
*	DLL Export
*********************************************************************/
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}

/*====================================================================
*	設定ウィンドウプロシージャ
*===================================================================*/
BOOL func_WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void *editp, FILTER *fp )
{
	switch(message){
		case WM_FILTER_INIT:
			return wm_filter_init(fp);

		//---------------------------------------------ボタン動作
		case WM_COMMAND:
			switch(LOWORD(wparam)){
				case ID_BTN_LSRK:		// 前のキーフレームに移動
					return go_last_sourcekey(fp,editp);

				case ID_BTN_NSRK:		// 次のキーフレームに移動
					return go_next_sourcekey(fp,editp);

				case ID_BTN_LMRK:		// 前のマークフレームに移動
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_MARKFRAME);

				case ID_BTN_NMRK:		// 次のマークフレームに移動
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_MARKFRAME);

				case ID_BTN_LPRF:		// 前のプロファイル変更フレームに移動
					return go_last_profilechange(fp,editp);

				case ID_BTN_NPRF:		// 次のプロファイル変更フレームに移動
					return go_next_profilechange(fp,editp);

				case ID_BTN_LKEY:		// 前のキー設定フレームに移動
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_KEYFRAME);

				case ID_BTN_NKEY:		// 次のキー設定フレームに移動
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_KEYFRAME);

				case ID_BTN_LDEL:		// 前の優先間引きフレームに移動
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_DELFRAME);

				case ID_BTN_NDEL:		// 次の優先間引きフレームに移動
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_DELFRAME);

				case ID_BTN_LCPY:		// 前のコピーフレームに移動
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_NULLFRAME);

				case ID_BTN_NCPY:		// 次のコピーフレームに移動
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_NULLFRAME);

				case ID_BTN_FRST:		// 先頭のフレームに移動
					return go_first_frame(fp,editp);

				case ID_BTN_STRT:		// 選択開始フレームに移動
					return go_start_frame(fp,editp);

				case ID_BTN_END:		// 選択終了フレームに移動
					return go_end_frame(fp,editp);

				case ID_BTN_LAST:		// 最後のフレームに移動
					return go_last_frame(fp,editp);
			}
			break;

		case WM_KEYUP:	// メインウィンドウへ送る
		case WM_KEYDOWN:
		case WM_MOUSEWHEEL:
			SendMessage(GetWindow(hwnd, GW_OWNER), message, wparam, lparam);
			break;
	}

	return FALSE;
}


/*--------------------------------------------------------------------
*	前のプロファイル変更フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_last_profilechange(FILTER* fp,void *editp)
{
	int  now;
	int  config;	// プロファイル番号
	FRAME_STATUS fs;

	now = fp->exfunc->get_frame(editp);	// 現在の表示フレーム

	if(--now<0) return FALSE;	// 一つ前のフレーム

	// 一つ前のフレームステータス取得
	if(!fp->exfunc->get_frame_status(editp,now,&fs))
		return FALSE;	// 取得失敗のとき

	config = fs.config;

	for(now-=1;now>=0;now--){
		// フレームステータス取得
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// 取得失敗のとき

		if(config != fs.config)	// プロファイル番号が違うとき
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now+1);

	return TRUE;
}

/*--------------------------------------------------------------------
*	次のプロファイル変更フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_next_profilechange(FILTER* fp,void *editp)
{
	int  now,num;
	int  config;	// プロファイル番号
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);	// 現在の表示フレーム
	num  = fp->exfunc->get_frame_n(editp);	// 総フレーム数

	if(num<=0) return FALSE;

	// 現在のフレームステータス取得
	if(!fp->exfunc->get_frame_status(editp,now,&fs))
		return FALSE;	// 取得失敗のとき

	config = fs.config;

	for(now+=1;now<num-1;now++){
		// フレームステータス取得
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// 取得失敗のとき

		if(config != fs.config)	// プロファイル番号が違うとき
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	前のソースキーフレームに移動
*-------------------------------------------------------------------*/
static BOOL go_last_sourcekey(FILTER* fp,void *editp)
{
	int now;

	now  = fp->exfunc->get_frame(editp);	// 現在のフレーム

	for(now-=1;now>0;now--){
		if(fp->exfunc->is_keyframe(editp,now))
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	次のソースキーフレームに移動
*-------------------------------------------------------------------*/
static BOOL go_next_sourcekey(FILTER* fp,void *editp)
{
	int now;
	int num;

	now  = fp->exfunc->get_frame(editp);	// 現在のフレーム
	num  = fp->exfunc->get_frame_n(editp);	// 総フレーム数

	for(now+=1;now<num-1;now++){
		if(fp->exfunc->is_keyframe(editp,now))
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_first_frame()	先頭のフレームに移動
*-------------------------------------------------------------------*/
static BOOL go_first_frame(FILTER* fp,void *editp)
{
	// 表示フレーム更新
	fp->exfunc->set_frame(editp,0);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_start_frame()	選択開始フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_start_frame(FILTER* fp,void *editp)
{
	int s,e;

	// 選択範囲取得
	if(!fp->exfunc->get_select_frame(editp,&s,&e))
		return FALSE;

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,s);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_end_frame()		選択終了フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_end_frame(FILTER* fp,void *editp)
{
	int s,e;

	// 選択範囲取得
	if(!fp->exfunc->get_select_frame(editp,&s,&e))
		return FALSE;

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,e);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_last_frame()		最後のフレームに移動
*-------------------------------------------------------------------*/
static BOOL go_last_frame(FILTER* fp,void *editp)
{
	int last;

	// 総フレーム数取得
	last = fp->exfunc->get_frame_n(editp);

	// 総フレーム数０以下のとき
	if(last<=0) return FALSE;

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,last-1);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_last_flag()		前の～フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_last_flag(FILTER* fp,void *editp,UINT flag)
{
	int now;
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);	// 現在のフレーム

	for(now-=1;now>0;now--){
		// フレームステータス取得
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// 取得失敗のとき

		if(fs.edit_flag & flag)	// フラグチェック
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_next_flag()		次の～フレームに移動
*-------------------------------------------------------------------*/
static BOOL go_next_flag(FILTER* fp,void *editp,UINT flag)
{
	int now,fnum;
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);
	fnum = fp->exfunc->get_frame_n(editp);	// 総フレーム数

	// 総フレーム数が０以下
	if(fnum<=0)  return FALSE;

	for(now+=1;now<fnum-1;now++){
		// フレームステータス取得
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;

		if(fs.edit_flag & flag)	// フラグチェック
			break;
	}

	// 表示フレーム変更
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	filter_init()	初期化
*-------------------------------------------------------------------*/
static BOOL wm_filter_init(FILTER* fp)
{
	HICON hicon;

	// ボタンを作る
	hLstSrK = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				5,  5, 36,36,fp->hwnd, (HMENU)ID_BTN_LSRK, fp->dll_hinst,NULL);
	hNxtSrK = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				41, 5, 36,36,fp->hwnd, (HMENU)ID_BTN_NSRK, fp->dll_hinst,NULL);
	hLstMrk = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				77, 5, 36,36,fp->hwnd, (HMENU)ID_BTN_LMRK, fp->dll_hinst,NULL);
	hNxtMrk = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				113,5, 36,36,fp->hwnd, (HMENU)ID_BTN_NMRK, fp->dll_hinst,NULL);

	hLstPrf = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				5  ,46, 36,36,fp->hwnd, (HMENU)ID_BTN_LPRF, fp->dll_hinst,NULL);
	hNxtPrf = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				41 ,46, 36,36,fp->hwnd, (HMENU)ID_BTN_NPRF, fp->dll_hinst,NULL);
	hLstKey = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				77, 46, 36,36,fp->hwnd, (HMENU)ID_BTN_LKEY, fp->dll_hinst,NULL);
	hNxtKey = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				113,46, 36,36,fp->hwnd, (HMENU)ID_BTN_NKEY, fp->dll_hinst,NULL);

	hLstDel = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				5  ,87, 36,36,fp->hwnd, (HMENU)ID_BTN_LDEL, fp->dll_hinst,NULL);
	hNxtDel = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				41 ,87, 36,36,fp->hwnd, (HMENU)ID_BTN_NDEL, fp->dll_hinst,NULL);
	hLstCpy = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				77, 87, 36,36,fp->hwnd, (HMENU)ID_BTN_LCPY, fp->dll_hinst,NULL);
	hNxtCpy = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				113,87, 36,36,fp->hwnd, (HMENU)ID_BTN_NCPY, fp->dll_hinst,NULL);

	hFirst  = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				5,  128, 36,36,fp->hwnd, (HMENU)ID_BTN_FRST, fp->dll_hinst,NULL);
	hStart  = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				41 ,128, 36,36,fp->hwnd, (HMENU)ID_BTN_STRT, fp->dll_hinst,NULL);
	hEnd    = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				77 ,128, 36,36,fp->hwnd, (HMENU)ID_BTN_END,  fp->dll_hinst,NULL);
	hLast   = CreateWindow("BUTTON",NULL,WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |BS_ICON,
				113,128, 36,36,fp->hwnd, (HMENU)ID_BTN_LAST, fp->dll_hinst,NULL);

	// アイコンイメージ割り当て
	hicon = LoadIcon(fp->dll_hinst,"iconLstSrK");
	SendMessage(hLstSrK,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtSrK");
	SendMessage(hNxtSrK,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconLstMrk");
	SendMessage(hLstMrk,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtMrk");
	SendMessage(hNxtMrk,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);

	hicon = LoadIcon(fp->dll_hinst,"iconLstPrf");
	SendMessage(hLstPrf,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtPrf");
	SendMessage(hNxtPrf,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconLstKey");
	SendMessage(hLstKey,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtKey");
	SendMessage(hNxtKey,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);

	hicon = LoadIcon(fp->dll_hinst,"iconLstDel");
	SendMessage(hLstDel,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtDel");
	SendMessage(hNxtDel,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconLstCpy");
	SendMessage(hLstCpy,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconNxtCpy");
	SendMessage(hNxtCpy,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);

	hicon = LoadIcon(fp->dll_hinst,"iconFirst");
	SendMessage(hFirst,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconStart");
	SendMessage(hStart,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconEnd");
	SendMessage(hEnd,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon(fp->dll_hinst,"iconLast");
	SendMessage(hLast,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);

	// チップテキスト割り当て
	CreateTooltip(hLstSrK,fp->dll_hinst,TSTR_LSRK);
	CreateTooltip(hNxtSrK,fp->dll_hinst,TSTR_NSRK);
	CreateTooltip(hLstMrk,fp->dll_hinst,TSTR_LMRK);
	CreateTooltip(hNxtMrk,fp->dll_hinst,TSTR_NMRK);

	CreateTooltip(hLstPrf,fp->dll_hinst,TSTR_LPRF);
	CreateTooltip(hNxtPrf,fp->dll_hinst,TSTR_NPRF);
	CreateTooltip(hLstKey,fp->dll_hinst,TSTR_LKEY);
	CreateTooltip(hNxtKey,fp->dll_hinst,TSTR_NKEY);

	CreateTooltip(hLstDel,fp->dll_hinst,TSTR_LDEL);
	CreateTooltip(hNxtDel,fp->dll_hinst,TSTR_NDEL);
	CreateTooltip(hLstCpy,fp->dll_hinst,TSTR_LCPY);
	CreateTooltip(hNxtCpy,fp->dll_hinst,TSTR_NCPY);

	CreateTooltip(hFirst,fp->dll_hinst,TSTR_FRST);
	CreateTooltip(hStart,fp->dll_hinst,TSTR_STRT);
	CreateTooltip(hEnd,  fp->dll_hinst,TSTR_END);
	CreateTooltip(hLast, fp->dll_hinst,TSTR_LAST);

	return FALSE;
}


/*--------------------------------------------------------------------
*	CreateTooltip()	ツールチップを作る
*-------------------------------------------------------------------*/
static void CreateTooltip (HWND hwnd,HINSTANCE hinst,char *tipstr)
{
                 // struct specifying control classes to register
    INITCOMMONCONTROLSEX iccex; 
    HWND hTTwnd;                 // handle to the ToolTip control
    TOOLINFO ti;

    /* INITIALIZE COMMON CONTROLS */
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    /* CREATE A TOOLTIP WINDOW */
    hTTwnd = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
        CW_USEDEFAULT,CW_USEDEFAULT,
        CW_USEDEFAULT,CW_USEDEFAULT,
        hwnd,
        NULL,
        hinst,
        NULL
        );

    SetWindowPos(hTTwnd,HWND_TOPMOST,
        0,0,0,0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hwnd, &(ti.rect));

    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.hinst = hinst;
    ti.uId = 0;
    ti.lpszText = tipstr;

    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    SendMessage(hTTwnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
} 
