/*********************************************************************
* 	�ړ��c�[�� �v���O�C��  for AviUtl
* 
* ��������A�C�f�B�A���肪�Ɓ[
* 
* 2003
* 	05/02:	����
* 	05/03:	�u�`�̃L�[�t���[���Ɂv�̓��삪�Ԉ���Ă����̂��C��
* 			�v���t�@�C���ύX�t���[���Ɉړ���ǉ�
* 
*********************************************************************/
#include <windows.h>
#include <commctrl.h>
#include "filter.h"


// �{�^��ID
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

// �`�b�v�e�L�X�g
#define TSTR_LSRK  "�O�̃L�[�t���[���Ɉړ�"
#define TSTR_NSRK  "���̃L�[�t���[���Ɉړ�"
#define TSTR_LMRK  "�O�̃}�[�N�t���[���Ɉړ�"
#define TSTR_NMRK  "���̃}�[�N�t���[���Ɉړ�"

#define TSTR_LPRF  "�O�̃v���t�@�C���̕ύX�t���[���Ɉړ�"
#define TSTR_NPRF  "���̃v���t�@�C���̕ύX�t���[���Ɉړ�"
#define TSTR_LKEY  "�O�̃L�[�ݒ�t���[���Ɉړ�"
#define TSTR_NKEY  "���̃L�[�ݒ�t���[���Ɉړ�"
#define TSTR_LDEL  "�O�̗D��Ԉ����ݒ�t���[���Ɉړ�"
#define TSTR_NDEL  "���̗D��Ԉ����ݒ�t���[���Ɉړ�"
#define TSTR_LCPY  "�O�̃R�s�[�ݒ�t���[���Ɉړ�"
#define TSTR_NCPY  "���̃R�s�[�ݒ�t���[���Ɉړ�"
#define TSTR_STRT  "�I���J�n�t���[���Ɉړ�"
#define TSTR_END   "�I���I���t���[���Ɉړ�"
#define TSTR_FRST  "�擪�̃t���[���Ɉړ�"
#define TSTR_LAST  "�Ō�̃t���[���Ɉړ�"

// �{�^���E�B���h�E�n���h��
HWND hNxtSrK,hLstSrK;
HWND hNxtMrk,hLstMrk;
HWND hLstPrf,hNxtPrf;
HWND hLstKey,hNxtKey;
HWND hNxtDel,hLstDel;
HWND hNxtCpy,hLstCpy;
HWND hStart, hEnd;
HWND hFirst, hLast;


// �v���g�^�C�v
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
//	FILTER_DLL�\����
//----------------------------
char filter_name[] = "�ړ��c�[��";
char filter_info[] = "�ړ��c�[�� ver0.02 by MakKi";
#define track_N 0
#if track_N
TCHAR *track_name[]   = { 0 };	// �g���b�N�o�[�̖��O
int   track_default[] = { 0 };	// �g���b�N�o�[�̏����l
int   track_s[]       = { 0 };	// �g���b�N�o�[�̉����l
int   track_e[]       = { 0 };	// �g���b�N�o�[�̏���l
#endif
#define check_N 0
#if check_N
TCHAR *check_name[]   = { 0 };	// �`�F�b�N�{�b�N�X
int   check_default[] = { 0 };	// �f�t�H���g
#endif


FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_DISP_FILTER	|
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_EX_INFORMATION,
	160,190,			// �ݒ�E�C���h�E�̃T�C�Y
	filter_name,		// �t�B���^�̖��O
	track_N,        	// �g���b�N�o�[�̐�
#if track_N
	track_name,     	// �g���b�N�o�[�̖��O�S
	track_default,  	// �g���b�N�o�[�̏����l�S
	track_s,track_e,	// �g���b�N�o�[�̐��l�̉������
#else
	NULL,NULL,NULL,NULL,
#endif
	check_N,      	// �`�F�b�N�{�b�N�X�̐�
#if check_N
	check_name,   	// �`�F�b�N�{�b�N�X�̖��O�S
	check_default,	// �`�F�b�N�{�b�N�X�̏����l�S
#else
	NULL,NULL,
#endif
	NULL,   	// �t�B���^�����֐�
	NULL,NULL,   	// �J�n��,�I�����ɌĂ΂��֐�
	NULL,        	// �ݒ肪�ύX���ꂽ�Ƃ��ɌĂ΂��֐�
	func_WndProc,	// �ݒ�E�B���h�E�v���V�[�W��
	NULL,NULL,   	// �V�X�e���Ŏg�p
	NULL,NULL,     	// �g���f�[�^�̈�
	filter_info,	// �t�B���^���
	NULL,			// �Z�[�u�J�n���O�ɌĂ΂��֐�
	NULL,			// �Z�[�u�I�����ɌĂ΂��֐�
	NULL,NULL,NULL,	// �V�X�e���Ŏg�p
	NULL,			// �g���̈揉���l
};

/*********************************************************************
*	DLL Export
*********************************************************************/
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}

/*====================================================================
*	�ݒ�E�B���h�E�v���V�[�W��
*===================================================================*/
BOOL func_WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void *editp, FILTER *fp )
{
	switch(message){
		case WM_FILTER_INIT:
			return wm_filter_init(fp);

		//---------------------------------------------�{�^������
		case WM_COMMAND:
			switch(LOWORD(wparam)){
				case ID_BTN_LSRK:		// �O�̃L�[�t���[���Ɉړ�
					return go_last_sourcekey(fp,editp);

				case ID_BTN_NSRK:		// ���̃L�[�t���[���Ɉړ�
					return go_next_sourcekey(fp,editp);

				case ID_BTN_LMRK:		// �O�̃}�[�N�t���[���Ɉړ�
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_MARKFRAME);

				case ID_BTN_NMRK:		// ���̃}�[�N�t���[���Ɉړ�
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_MARKFRAME);

				case ID_BTN_LPRF:		// �O�̃v���t�@�C���ύX�t���[���Ɉړ�
					return go_last_profilechange(fp,editp);

				case ID_BTN_NPRF:		// ���̃v���t�@�C���ύX�t���[���Ɉړ�
					return go_next_profilechange(fp,editp);

				case ID_BTN_LKEY:		// �O�̃L�[�ݒ�t���[���Ɉړ�
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_KEYFRAME);

				case ID_BTN_NKEY:		// ���̃L�[�ݒ�t���[���Ɉړ�
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_KEYFRAME);

				case ID_BTN_LDEL:		// �O�̗D��Ԉ����t���[���Ɉړ�
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_DELFRAME);

				case ID_BTN_NDEL:		// ���̗D��Ԉ����t���[���Ɉړ�
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_DELFRAME);

				case ID_BTN_LCPY:		// �O�̃R�s�[�t���[���Ɉړ�
					return go_last_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_NULLFRAME);

				case ID_BTN_NCPY:		// ���̃R�s�[�t���[���Ɉړ�
					return go_next_flag(fp,editp,EDIT_FRAME_EDIT_FLAG_NULLFRAME);

				case ID_BTN_FRST:		// �擪�̃t���[���Ɉړ�
					return go_first_frame(fp,editp);

				case ID_BTN_STRT:		// �I���J�n�t���[���Ɉړ�
					return go_start_frame(fp,editp);

				case ID_BTN_END:		// �I���I���t���[���Ɉړ�
					return go_end_frame(fp,editp);

				case ID_BTN_LAST:		// �Ō�̃t���[���Ɉړ�
					return go_last_frame(fp,editp);
			}
			break;

		case WM_KEYUP:	// ���C���E�B���h�E�֑���
		case WM_KEYDOWN:
		case WM_MOUSEWHEEL:
			SendMessage(GetWindow(hwnd, GW_OWNER), message, wparam, lparam);
			break;
	}

	return FALSE;
}


/*--------------------------------------------------------------------
*	�O�̃v���t�@�C���ύX�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_last_profilechange(FILTER* fp,void *editp)
{
	int  now;
	int  config;	// �v���t�@�C���ԍ�
	FRAME_STATUS fs;

	now = fp->exfunc->get_frame(editp);	// ���݂̕\���t���[��

	if(--now<0) return FALSE;	// ��O�̃t���[��

	// ��O�̃t���[���X�e�[�^�X�擾
	if(!fp->exfunc->get_frame_status(editp,now,&fs))
		return FALSE;	// �擾���s�̂Ƃ�

	config = fs.config;

	for(now-=1;now>=0;now--){
		// �t���[���X�e�[�^�X�擾
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// �擾���s�̂Ƃ�

		if(config != fs.config)	// �v���t�@�C���ԍ����Ⴄ�Ƃ�
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now+1);

	return TRUE;
}

/*--------------------------------------------------------------------
*	���̃v���t�@�C���ύX�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_next_profilechange(FILTER* fp,void *editp)
{
	int  now,num;
	int  config;	// �v���t�@�C���ԍ�
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);	// ���݂̕\���t���[��
	num  = fp->exfunc->get_frame_n(editp);	// ���t���[����

	if(num<=0) return FALSE;

	// ���݂̃t���[���X�e�[�^�X�擾
	if(!fp->exfunc->get_frame_status(editp,now,&fs))
		return FALSE;	// �擾���s�̂Ƃ�

	config = fs.config;

	for(now+=1;now<num-1;now++){
		// �t���[���X�e�[�^�X�擾
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// �擾���s�̂Ƃ�

		if(config != fs.config)	// �v���t�@�C���ԍ����Ⴄ�Ƃ�
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	�O�̃\�[�X�L�[�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_last_sourcekey(FILTER* fp,void *editp)
{
	int now;

	now  = fp->exfunc->get_frame(editp);	// ���݂̃t���[��

	for(now-=1;now>0;now--){
		if(fp->exfunc->is_keyframe(editp,now))
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	���̃\�[�X�L�[�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_next_sourcekey(FILTER* fp,void *editp)
{
	int now;
	int num;

	now  = fp->exfunc->get_frame(editp);	// ���݂̃t���[��
	num  = fp->exfunc->get_frame_n(editp);	// ���t���[����

	for(now+=1;now<num-1;now++){
		if(fp->exfunc->is_keyframe(editp,now))
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_first_frame()	�擪�̃t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_first_frame(FILTER* fp,void *editp)
{
	// �\���t���[���X�V
	fp->exfunc->set_frame(editp,0);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_start_frame()	�I���J�n�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_start_frame(FILTER* fp,void *editp)
{
	int s,e;

	// �I��͈͎擾
	if(!fp->exfunc->get_select_frame(editp,&s,&e))
		return FALSE;

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,s);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_end_frame()		�I���I���t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_end_frame(FILTER* fp,void *editp)
{
	int s,e;

	// �I��͈͎擾
	if(!fp->exfunc->get_select_frame(editp,&s,&e))
		return FALSE;

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,e);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_last_frame()		�Ō�̃t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_last_frame(FILTER* fp,void *editp)
{
	int last;

	// ���t���[�����擾
	last = fp->exfunc->get_frame_n(editp);

	// ���t���[�����O�ȉ��̂Ƃ�
	if(last<=0) return FALSE;

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,last-1);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_last_flag()		�O�́`�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_last_flag(FILTER* fp,void *editp,UINT flag)
{
	int now;
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);	// ���݂̃t���[��

	for(now-=1;now>0;now--){
		// �t���[���X�e�[�^�X�擾
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;	// �擾���s�̂Ƃ�

		if(fs.edit_flag & flag)	// �t���O�`�F�b�N
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	go_next_flag()		���́`�t���[���Ɉړ�
*-------------------------------------------------------------------*/
static BOOL go_next_flag(FILTER* fp,void *editp,UINT flag)
{
	int now,fnum;
	FRAME_STATUS fs;

	now  = fp->exfunc->get_frame(editp);
	fnum = fp->exfunc->get_frame_n(editp);	// ���t���[����

	// ���t���[�������O�ȉ�
	if(fnum<=0)  return FALSE;

	for(now+=1;now<fnum-1;now++){
		// �t���[���X�e�[�^�X�擾
		if(!fp->exfunc->get_frame_status(editp,now,&fs))
			return FALSE;

		if(fs.edit_flag & flag)	// �t���O�`�F�b�N
			break;
	}

	// �\���t���[���ύX
	fp->exfunc->set_frame(editp,now);

	return TRUE;
}

/*--------------------------------------------------------------------
*	filter_init()	������
*-------------------------------------------------------------------*/
static BOOL wm_filter_init(FILTER* fp)
{
	HICON hicon;

	// �{�^�������
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

	// �A�C�R���C���[�W���蓖��
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

	// �`�b�v�e�L�X�g���蓖��
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
*	CreateTooltip()	�c�[���`�b�v�����
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
