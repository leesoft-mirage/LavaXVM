#include "resource.h"
#include "define.h"

#define MAX_RAM 0x5000

int chtHowFind=0;
int chtBits=16;
byte chtPre[MAX_RAM];
byte chtFlag[MAX_RAM];
byte chtCode[0x100];
long chtValue;
STARTUPINFO chtStartUp;
PROCESS_INFORMATION chtProcInfo;
WNDPROC pEditProc;

long My10to2(char *num)
{
	long value;
	char t;
	value=0;
	while ((t=*num)) {
		value=value*10+(t-'0');
		num++;
	}
	return value;
}

byte My16to2(char *num)
{
	byte value;
	if (*num>'9') value=(*num-7)<<4;
	else value=(*num)<<4;
	num++;
	if (*num>'9') value|=(*num-0x37);
	else value|=(*num-'0');
	return value;
}

LRESULT CALLBACK ChtFind(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	char num[20],*p;
	int i;
	switch (message) {
	case WM_CLOSE:
		EndDialog(hWnd,0);
		break;
	case WM_INITDIALOG:
		SendDlgItemMessage(hWnd,IDC_EDIT,EM_SETLIMITTEXT,5,0);
		switch (chtHowFind) {
		case 0:SendDlgItemMessage(hWnd,IDC_EQU,BM_SETCHECK,BST_CHECKED,0);break;
		case 1:SendDlgItemMessage(hWnd,IDC_ABOVE,BM_SETCHECK,BST_CHECKED,0);break;
		case 2:SendDlgItemMessage(hWnd,IDC_BELOW,BM_SETCHECK,BST_CHECKED,0);break;
		default:SendDlgItemMessage(hWnd,IDC_DATA,BM_SETCHECK,BST_CHECKED,0);
		}
		if (chtBits==8)
			SendDlgItemMessage(hWnd,IDC_8bits,BM_SETCHECK,BST_CHECKED,0);
		else
			SendDlgItemMessage(hWnd,IDC_16bits,BM_SETCHECK,BST_CHECKED,0);
		break;
	case WM_COMMAND:
		if ((wParam>>16)==BN_CLICKED) {
			switch (wParam&0xffff) {
			case IDC_CALC:
				GetStartupInfo(&chtStartUp);
				CreateProcess(NULL,"calc.exe",NULL,
                     NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&chtStartUp,
                     &chtProcInfo);
				CloseHandle(chtProcInfo.hProcess);
				CloseHandle(chtProcInfo.hThread);
				break;
			case IDC_EQU:
				CheckRadioButton(hWnd,IDC_EQU,IDC_DATA,IDC_EQU);
				chtHowFind=0;
				break;
			case IDC_ABOVE:
				CheckRadioButton(hWnd,IDC_EQU,IDC_DATA,IDC_ABOVE);
				chtHowFind=1;
				break;
			case IDC_BELOW:
				CheckRadioButton(hWnd,IDC_EQU,IDC_DATA,IDC_BELOW);
				chtHowFind=2;
				break;
			case IDC_DATA:
				CheckRadioButton(hWnd,IDC_EQU,IDC_DATA,IDC_DATA);
				chtHowFind=3;
				break;
			case IDC_8bits:
				CheckRadioButton(hWnd,IDC_8bits,IDC_16bits,IDC_8bits);
				chtBits=8;
				break;
			case IDC_16bits:
				CheckRadioButton(hWnd,IDC_8bits,IDC_16bits,IDC_16bits);
				chtBits=16;
				break;
			case IDC_PREFIND:
				memcpy(chtPre,lRam+0x2000,MAX_RAM);
				memset(chtFlag,0,MAX_RAM);
				break;
			case IDC_FIND:
				SendDlgItemMessage(hWnd,IDC_LB,LB_RESETCONTENT,0,0);
				if (SendDlgItemMessage(hWnd,IDC_EDIT,WM_GETTEXT,6,(LPARAM)num))
					chtValue=My10to2(num);
				else
					chtValue=0;
				if (chtBits==8) {
					chtValue&=0xff;
					if (chtHowFind==3) {
						for (i=0;i<MAX_RAM;i++)
							if (lRam[0x2000+i]!=chtValue) chtFlag[i]=1;
					} else if (chtHowFind==0) {
						for (i=0;i<MAX_RAM;i++)
							if (lRam[0x2000+i]!=chtPre[i]) chtFlag[i]=1;
					} else if (chtHowFind==1) {
						for (i=0;i<MAX_RAM;i++)
							if (lRam[0x2000+i]<=chtPre[i]) chtFlag[i]=1;
					} else if (chtHowFind==2) {
						for (i=0;i<MAX_RAM;i++)
							if (lRam[0x2000+i]>=chtPre[i]) chtFlag[i]=1;
					}
					for (i=0;i<MAX_RAM;i++) {
						if (chtFlag[i]==0) {
							sprintf(num,"%4x--%2x",0x2000+i,lRam[0x2000+i]);
							p=num;
							while (*p) {
								if (*p==0x20) *p='0';
								else if (*p=='-') *p=0x20;
								p++;
							}
							SendDlgItemMessage(hWnd,IDC_LB,LB_ADDSTRING,0,(LPARAM)num);
						}
					}
				} else {
					if (chtHowFind==3) {
						for (i=0;i<MAX_RAM-1;i++)
							if (*(word *)(lRam+0x2000+i)!=chtValue) chtFlag[i]=1;
					} else if (chtHowFind==0) {
						for (i=0;i<MAX_RAM-1;i++)
							if (*(word *)(lRam+0x2000+i)!=*(word *)(chtPre+i)) chtFlag[i]=1;
					} else if (chtHowFind==1) {
						for (i=0;i<MAX_RAM-1;i++)
							if (*(word *)(lRam+0x2000+i)<=*(word *)(chtPre+i)) chtFlag[i]=1;
					} else if (chtHowFind==2) {
						for (i=0;i<MAX_RAM-1;i++)
							if (*(word *)(lRam+0x2000+i)>=*(word *)(chtPre+i)) chtFlag[i]=1;
					}
					for (i=0;i<MAX_RAM-1;i++) {
						if (chtFlag[i]==0) {
							sprintf(num,"%4x--%4x",0x2000+i,*(word *)(lRam+0x2000+i));
							p=num;
							while (*p) {
								if (*p==0x20) *p='0';
								else if (*p=='-') *p=0x20;
								p++;
							}
							SendDlgItemMessage(hWnd,IDC_LB,LB_ADDSTRING,0,(LPARAM)num);
						}
					}
				}
				memcpy(chtPre,lRam+0x2000,MAX_RAM);
				break;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void chtLoad()
{
	int i;
	char CheatName[MAX_PATH];
	FILE *fp;
	for (i=0;i<256;i++) chtCode[i]=0xff;
	strcpy(CheatName,CheatPath);
	strcat(CheatName,pRomName);
	strcat(CheatName,".cht");
	if ((fp=fopen(CheatName,"rb"))==NULL) return;
	fread(chtCode,1,250,fp);
	fclose(fp);
}

void chtExe()
{
	byte type,*p;
	word address,value;

	p=chtCode;
	while ((type=*p)!=0xff) {
		p++;
		address=*(word *)p;
		p+=2;
		value=*(word *)p;
		switch (type) {
		case 0: //byte锁定
			lRam[address]=(byte)value;
			break;
		case 1: //byte只增
			value&=0xff;
			if (lRam[address]>=value)
				*p=lRam[address];
			else
				lRam[address]=(byte)value;
			break;
		case 2: //word锁定
			*(word *)(lRam+address)=value;
			break;
		case 3: //word只增
			if (*(word *)(lRam+address)>=value)
				*(word *)p=*(word *)(lRam+address);
			else
				*(word *)(lRam+address)=value;
			break;
		}
		p+=2;
	}
}

LRESULT CALLBACK SubEdit(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	char digit;
	long ts,te;
	int t;
	switch (message) {
	case WM_CHAR:
		digit=(char)wParam;
		if (digit>='a' && digit<='f') {
			wParam&=0xffdf;
			digit&=0xdf;
		}
		if ((digit>='0' && digit<='9') || (digit>='A' && digit<='F')) {
			CallWindowProc(pEditProc,hWnd,EM_GETSEL,(WPARAM)&ts,(LPARAM)&te);
			if (ts==2 || ts==7) {
				CallWindowProc(pEditProc,hWnd,WM_KEYDOWN,0x27,0);
				ts++;
			}
			t=ts+1;
			CallWindowProc(pEditProc,hWnd,EM_SETSEL,ts,ts+1);
			CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
			if (ts==1 || ts==6)
				CallWindowProc(pEditProc,hWnd,WM_KEYDOWN,0x27,0);
		}
		break;
	case WM_KEYDOWN:
		if (wParam==0x25) {
			CallWindowProc(pEditProc,hWnd,EM_GETSEL,(WPARAM)&ts,(LPARAM)&te);
			if (ts==3 || ts==8)
				CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
			return CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
		} else if (wParam==0x27) {
			CallWindowProc(pEditProc,hWnd,EM_GETSEL,(WPARAM)&ts,(LPARAM)&te);
			if (ts==1 || ts==6)
				CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
			return CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_PASTE:
		break;
	default:
		return CallWindowProc(pEditProc,hWnd,message,wParam,lParam);
	}
	return 0;
}

LRESULT CALLBACK ChtEdit(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	FILE *fp;
	char CheatName[MAX_PATH];
	byte temp[10];
	char s[20],*p;
	LONG t,i;
	switch (message) {
	case WM_CLOSE:
		EndDialog(hWnd,0);
		break;
	case WM_INITDIALOG:
		pEditProc=(WNDPROC)SetWindowLong(GetDlgItem(hWnd,IDC_TEXT),GWL_WNDPROC,(LONG)SubEdit);
		SendDlgItemMessage(hWnd,IDC_TEXT,EM_LIMITTEXT,12,0);
		SendDlgItemMessage(hWnd,IDC_TEXT,WM_SETTEXT,0,(LPARAM)"00-0000-0000");
		strcpy(CheatName,CheatPath);
		strcat(CheatName,pRomName);
		strcat(CheatName,".cht");
		if ((fp=fopen(CheatName,"rb"))==NULL) return TRUE;
		while (fread(temp,1,5,fp)==5) {;
			sprintf(s,"%2x-%4x-%4x",temp[0],*(word *)(temp+1),*(word *)(temp+3));
			p=s;
			while (*p) {
				if (*p==0x20) *p='0';
				else if (*p>='a' && *p<='f') *p&=0xdf;
				p++;
			}
			SendDlgItemMessage(hWnd,IDC_LB,LB_ADDSTRING,0,(LPARAM)s);
		}
		fclose(fp);
		break;
	case WM_COMMAND:
		if ((wParam>>16)==BN_CLICKED) {
			switch (wParam&0xffff) {
			case IDC_ADD:
				SendDlgItemMessage(hWnd,IDC_TEXT,WM_GETTEXT,15,(LPARAM)s);
				SendDlgItemMessage(hWnd,IDC_LB,LB_ADDSTRING,0,(LPARAM)s);
				break;
			case IDC_DEL:
				if ((t=SendDlgItemMessage(hWnd,IDC_LB,LB_GETCURSEL,0,0))!=LB_ERR)
					SendDlgItemMessage(hWnd,IDC_LB,LB_DELETESTRING,t,0);
				break;
			case IDC_EDIT:
				if ((t=SendDlgItemMessage(hWnd,IDC_LB,LB_GETCURSEL,0,0))!=LB_ERR) {
					SendDlgItemMessage(hWnd,IDC_LB,LB_DELETESTRING,t,0);
					SendDlgItemMessage(hWnd,IDC_TEXT,WM_GETTEXT,15,(LPARAM)s);
					SendDlgItemMessage(hWnd,IDC_LB,LB_ADDSTRING,0,(LPARAM)s);
				}
				break;
			case IDC_SAVE:
				t=SendDlgItemMessage(hWnd,IDC_LB,LB_GETCOUNT,0,0);
				if (t==0) return TRUE;
				strcpy(CheatName,CheatPath);
				strcat(CheatName,pRomName);
				strcat(CheatName,".cht");
				if ((fp=fopen(CheatName,"wb"))==NULL) return TRUE;
				i=0;
				while (i<t) {
					SendDlgItemMessage(hWnd,IDC_LB,LB_GETTEXT,i,(LPARAM)s);
					temp[0]=My16to2(s);
					temp[1]=My16to2(s+5);
					temp[2]=My16to2(s+3);
					temp[3]=My16to2(s+10);
					temp[4]=My16to2(s+8);
					fwrite(temp,1,5,fp);
					i++;
				}
				fclose(fp);
				chtLoad();
				EndDialog(hWnd,0);
				break;
			}
		} else if ((wParam>>16)==LBN_SELCHANGE) {
			if ((t=SendDlgItemMessage(hWnd,IDC_LB,LB_GETCURSEL,0,0))!=LB_ERR) {
				SendDlgItemMessage(hWnd,IDC_LB,LB_GETTEXT,t,(LPARAM)s);
				SendDlgItemMessage(hWnd,IDC_TEXT,WM_SETTEXT,0,(LPARAM)s);
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}