#include "define.h"

typedef void (*CODES)();

#define LTRUE 0xffffffff
#define LFALSE 0

#define CMDLINE	0x700

a32 TextBuffer,string_stack,eval_stack;

byte patbuf[0x400];

word sin90[]={
	0,18,36,54,71,89,107,125,
	143,160,178,195,213,230,248,265,
	282,299,316,333,350,367,384,400,
	416,433,449,465,481,496,512,527,
	543,558,573,587,602,616,630,644,
	658,672,685,698,711,724,737,749,
	761,773,784,796,807,818,828,839,
	849,859,868,878,887,896,904,912,
	920,928,935,943,949,956,962,968,
	974,979,984,989,994,998,1002,1005,
	1008,1011,1014,1016,1018,1020,1022,1023,
	1023,1024,1024
};

byte crc1[256]={
	0x0,0x21,0x42,0x63,0x84,0xa5,0xc6,0xe7,
	0x8,0x29,0x4a,0x6b,0x8c,0xad,0xce,0xef,
	0x31,0x10,0x73,0x52,0xb5,0x94,0xf7,0xd6,
	0x39,0x18,0x7b,0x5a,0xbd,0x9c,0xff,0xde,
	0x62,0x43,0x20,0x1,0xe6,0xc7,0xa4,0x85,
	0x6a,0x4b,0x28,0x9,0xee,0xcf,0xac,0x8d,
	0x53,0x72,0x11,0x30,0xd7,0xf6,0x95,0xb4,
	0x5b,0x7a,0x19,0x38,0xdf,0xfe,0x9d,0xbc,
	0xc4,0xe5,0x86,0xa7,0x40,0x61,0x2,0x23,
	0xcc,0xed,0x8e,0xaf,0x48,0x69,0xa,0x2b,
	0xf5,0xd4,0xb7,0x96,0x71,0x50,0x33,0x12,
	0xfd,0xdc,0xbf,0x9e,0x79,0x58,0x3b,0x1a,
	0xa6,0x87,0xe4,0xc5,0x22,0x3,0x60,0x41,
	0xae,0x8f,0xec,0xcd,0x2a,0xb,0x68,0x49,
	0x97,0xb6,0xd5,0xf4,0x13,0x32,0x51,0x70,
	0x9f,0xbe,0xdd,0xfc,0x1b,0x3a,0x59,0x78,
	0x88,0xa9,0xca,0xeb,0xc,0x2d,0x4e,0x6f,
	0x80,0xa1,0xc2,0xe3,0x4,0x25,0x46,0x67,
	0xb9,0x98,0xfb,0xda,0x3d,0x1c,0x7f,0x5e,
	0xb1,0x90,0xf3,0xd2,0x35,0x14,0x77,0x56,
	0xea,0xcb,0xa8,0x89,0x6e,0x4f,0x2c,0xd,
	0xe2,0xc3,0xa0,0x81,0x66,0x47,0x24,0x5,
	0xdb,0xfa,0x99,0xb8,0x5f,0x7e,0x1d,0x3c,
	0xd3,0xf2,0x91,0xb0,0x57,0x76,0x15,0x34,
	0x4c,0x6d,0xe,0x2f,0xc8,0xe9,0x8a,0xab,
	0x44,0x65,0x6,0x27,0xc0,0xe1,0x82,0xa3,
	0x7d,0x5c,0x3f,0x1e,0xf9,0xd8,0xbb,0x9a,
	0x75,0x54,0x37,0x16,0xf1,0xd0,0xb3,0x92,
	0x2e,0xf,0x6c,0x4d,0xaa,0x8b,0xe8,0xc9,
	0x26,0x7,0x64,0x45,0xa2,0x83,0xe0,0xc1,
	0x1f,0x3e,0x5d,0x7c,0x9b,0xba,0xd9,0xf8,
	0x17,0x36,0x55,0x74,0x93,0xb2,0xd1,0xf0
};
byte crc2[256]={
	0x0,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
	0x81,0x91,0xa1,0xb1,0xc1,0xd1,0xe1,0xf1,
	0x12,0x2,0x32,0x22,0x52,0x42,0x72,0x62,
	0x93,0x83,0xb3,0xa3,0xd3,0xc3,0xf3,0xe3,
	0x24,0x34,0x4,0x14,0x64,0x74,0x44,0x54,
	0xa5,0xb5,0x85,0x95,0xe5,0xf5,0xc5,0xd5,
	0x36,0x26,0x16,0x6,0x76,0x66,0x56,0x46,
	0xb7,0xa7,0x97,0x87,0xf7,0xe7,0xd7,0xc7,
	0x48,0x58,0x68,0x78,0x8,0x18,0x28,0x38,
	0xc9,0xd9,0xe9,0xf9,0x89,0x99,0xa9,0xb9,
	0x5a,0x4a,0x7a,0x6a,0x1a,0xa,0x3a,0x2a,
	0xdb,0xcb,0xfb,0xeb,0x9b,0x8b,0xbb,0xab,
	0x6c,0x7c,0x4c,0x5c,0x2c,0x3c,0xc,0x1c,
	0xed,0xfd,0xcd,0xdd,0xad,0xbd,0x8d,0x9d,
	0x7e,0x6e,0x5e,0x4e,0x3e,0x2e,0x1e,0xe,
	0xff,0xef,0xdf,0xcf,0xbf,0xaf,0x9f,0x8f,
	0x91,0x81,0xb1,0xa1,0xd1,0xc1,0xf1,0xe1,
	0x10,0x0,0x30,0x20,0x50,0x40,0x70,0x60,
	0x83,0x93,0xa3,0xb3,0xc3,0xd3,0xe3,0xf3,
	0x2,0x12,0x22,0x32,0x42,0x52,0x62,0x72,
	0xb5,0xa5,0x95,0x85,0xf5,0xe5,0xd5,0xc5,
	0x34,0x24,0x14,0x4,0x74,0x64,0x54,0x44,
	0xa7,0xb7,0x87,0x97,0xe7,0xf7,0xc7,0xd7,
	0x26,0x36,0x6,0x16,0x66,0x76,0x46,0x56,
	0xd9,0xc9,0xf9,0xe9,0x99,0x89,0xb9,0xa9,
	0x58,0x48,0x78,0x68,0x18,0x8,0x38,0x28,
	0xcb,0xdb,0xeb,0xfb,0x8b,0x9b,0xab,0xbb,
	0x4a,0x5a,0x6a,0x7a,0xa,0x1a,0x2a,0x3a,
	0xfd,0xed,0xdd,0xcd,0xbd,0xad,0x9d,0x8d,
	0x7c,0x6c,0x5c,0x4c,0x3c,0x2c,0x1c,0xc,
	0xef,0xff,0xcf,0xdf,0xaf,0xbf,0x8f,0x9f,
	0x6e,0x7e,0x4e,0x5e,0x2e,0x3e,0xe,0x1e
};

char *smode[]={"r","rb","r+","rb+","w","wb","w+","wb+","a","ab","a+","ab+"};
DWORD modeAccess[]={GENERIC_READ,GENERIC_READ,GENERIC_READ|GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE,
	GENERIC_WRITE,GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE,
	GENERIC_WRITE,GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE};
DWORD modeCreate[]={OPEN_EXISTING,OPEN_EXISTING,OPEN_EXISTING,OPEN_EXISTING,
	CREATE_ALWAYS,CREATE_ALWAYS,CREATE_ALWAYS,CREATE_ALWAYS,
	OPEN_ALWAYS,OPEN_ALWAYS,OPEN_ALWAYS,OPEN_ALWAYS};
char FileName[MAX_PATH];
byte *ascii;//[1536];
byte *ascii8;//[2048];
byte *gbfont;//[81*94*24];
byte *gbfont16;//[81*94*32];
byte *lPC; //lava程序指针
byte VRam[0x1000000];//[65536];
struct TASK task[16]; //任务栈
int task_lev; //任务级
byte *lRam;
byte eval_top;
byte secret;
long a1,a3,result;
long seed;
a32 string_ptr,local_bp,local_sp;
int RamBits;
a32 ramuses,ramusee,ramarm;
unsigned long timed;
int cur_funcid=0;
int func_stack[1024];
int func_top;
COPYDATASTRUCT copydata;
long d_line;
byte wait_key;
long delay;
int have_pen,have_keypad;
int pen_x,pen_y;

byte no_buf,lcmd,negative;
word xx,yy,x0,Y0,x1,Y1;
a32 m1l;
word graph_mode,bgcolor,fgcolor;
byte scr_x,scr_y,scr_mode,curr_RPS,curr_CPR;
word scr_off;
byte small_up,small_down,small_left;
unsigned long line_mode;
byte func_x;
byte negative_tbl[256];
byte fhave[3],fmode[3];
a32 fsize[3];
a32 foffset[3];
int curr_fnum;
HANDLE fhandle[3];
HANDLE FileHandle;
char CD[MAX_PATH];
char VirtualFullName[MAX_PATH];
a32 ListFileAddr; 

extern int list_set;
extern int first_file,curr_file;
extern unsigned long GetGBCodeByPY( unsigned int pos, byte *InputBuffer, byte *OutBuffer );

void setscreen(int mode);
void get_val();
void put_val();

float i2f(int i)
{
	return *(float*)(&i);
}

int f2i(float f)
{
	return *(int *)(&f);
}

void verify()
{
	a32 t;
	char s[100];
	static char lev[][5]={"ALL","8K","12K","16K","20K","×"};

	if (d_line>=0) {
		FATAL("非release版，不予认证!");
		return;
	}
	if (ramuses<0x2000 || ramuses>ramusee) t=5;
	else {
		t=ramusee+1-0x2000;
		if (t<=6*1024) t=0;
		else if (t<=8*1024) t=1;
		else if (t<=12*1024) t=2;
		else if (t<=16*1024) t=3;
		else if (t<=20*1024) t=4;
		else {
			if (ramarm)
				sprintf(s,"LavaX认证:LavaX %dK,ARM处理器优化\n\n本工具仅供参考，以LavaX官方认证机构公布结果为准。",(t+1023)/1024);
			else
				sprintf(s,"LavaX认证:LavaX %dK\n\n本工具仅供参考，以LavaX官方认证机构公布结果为准。",(t+1023)/1024);
			FATAL(s);
			return;
		}
	}
	if (ramarm)
		sprintf(s,"LavaX认证:LavaX %s,ARM处理器优化\n\n本工具仅供参考，以LavaX官方认证机构公布结果为准。",lev[t]);
	else
		sprintf(s,"LavaX认证:LavaX %s\n\n本工具仅供参考，以LavaX官方认证机构公布结果为准。",lev[t]);
	FATAL(s);
}

void make_negative()
{
	int i;
	byte t;
	for (i=0;i<256;i++) {
		t=0;
		if (i&0x80) t|=1;
		if (i&0x40) t|=2;
		if (i&0x20) t|=4;
		if (i&0x10) t|=8;
		if (i&0x8) t|=0x10;
		if (i&0x4) t|=0x20;
		if (i&0x2) t|=0x40;
		if (i&0x1) t|=0x80;
		negative_tbl[i]=t;
	}
}

void set_sys_ram()
{
	TextBuffer=		0x1400; //故意这样，以检验是否地址相关
	string_stack=	0x1c00;
	eval_stack=		0x1b00;
}

void Adjust_Window(int width,int height,int keypad)
{
	if (width*2<=320 && height*2<=320) ScreenDouble=1;
	else ScreenDouble=0;
	if (width!=ScreenWidth || height!=ScreenHeight || keypad!=have_keypad)
		SetWindow(width,height);
	ScreenWidth=width;
	ScreenHeight=height;
}

void lavReset()
{
	word t1,t2;

	lPC=pLAVA+16;
	if (*(pLAVA+8)&0x10) RamBits=32;
	else if (*(pLAVA+8)&0x80) RamBits=24;
	else RamBits=16;
	eval_top=0;
	d_line=-1;
	ramuses=0xf00000;
	ramusee=0x0;
	ramarm=1;
	if (RamBits>16) {
		memset(lRam+0x2000,0,(VRam+sizeof(VRam))-(lRam+0x2000));
		set_sys_ram();
		string_ptr=string_stack;
	} else {
		memset(&lRam[0x2000],0,0xe000); //清变量区
		set_sys_ram();
		string_ptr=string_stack;
	}
	bgcolor=0;
	switch (*(pLAVA+8)&0x60) {
	case 0x40:
		graph_mode=4;
		fgcolor=15;
		break;
	case 0x60:
		graph_mode=8;
		fgcolor=255;
		break;
	default:
		graph_mode=1;
		fgcolor=15;
	}
	switch (*(pLAVA+8)&1) {
	case 1:
		have_pen=1;
		have_keypad=0;
		break;
	default:
		have_pen=0;
		have_keypad=1;
	}
	SetPalette();
	list_set=0;
	secret=0;
	hardinput_rp=hardinput_wp=0;
	memset(cur_keyb,0,sizeof(cur_keyb));
	lav_key=0;
	delay=0;
	wait_key=0;
	t1=*(pLAVA+9)<<4;
	if (t1<160) t1=160;
	if (t1>320) t1=320;
	t2=*(pLAVA+10)<<4;
	if (t2<80) t2=80;
	if (t2>240) t2=240;
	Adjust_Window(t1,t2,have_keypad);
	if (!task_lev) SetSBuffer();
	line_mode=0;
	setscreen(0);
	_asm rdtsc
	_asm mov seed,eax
	seed|=1;
	memset(fhave,0,3);
	make_negative();
	strcpy(CD,FcPath);
}

void c_exit()
{
	int i;
	int ret_val;

	get_val();
	ret_val=a1;
	for (i=0;i<3;i++) { //关闭所有打开的文件
		if (fhave[i]) {
			fhave[i]=0;
			CloseHandle(fhandle[i]);
		}
	}
	if (task_lev) {
		task_lev--;
		lPC=task[task_lev].lPC;
		lRam=task[task_lev].lRam;
		eval_top=task[task_lev].eval_top;
		local_sp=task[task_lev].local_sp;
		local_bp=task[task_lev].local_bp;
		string_ptr=task[task_lev].string_ptr;
		curr_file=task[task_lev].curr_file;
		first_file=task[task_lev].first_file;
		list_set=task[task_lev].list_set;
		RamBits=task[task_lev].RamBits;
		graph_mode=task[task_lev].graph_mode;
		bgcolor=task[task_lev].bgcolor;
		fgcolor=task[task_lev].fgcolor;
		Load_Palette();
		if (ScreenWidth!=task[task_lev].ScreenWidth || ScreenHeight!=task[task_lev].ScreenHeight)
			Adjust_Window(task[task_lev].ScreenWidth,task[task_lev].ScreenHeight,have_keypad);
		secret=task[task_lev].secret;
		strcpy(CD,task[task_lev].CD);
		set_sys_ram();
		pLAVA=task[task_lev].pLAVA;
		pNextLAVA=task[task_lev+1].pLAVA;
		a1=ret_val;
		put_val();
		return;
	}
	EmuRunning=0;
	if (ENDVERIFY) verify();
}

void good_exit()
{
	a1=0;
	put_val();
	c_exit();
}

void bad_exit()
{
	a1=-1;
	put_val();
	c_exit();
}

void DispRamErr(a32 s,a32 e)
{
	char str[100],str2[100];
	if (RamError) return;
	RamError=1;
	if (s==e)
		sprintf(str,"错误的读写地址%x",s);
	else
		sprintf(str,"错误的读写地址%x-%x",s,e);
	if (d_line>=0) {
		sprintf(str2,"\n\n发生在%d行",d_line);
		strcat(str,str2);
	}
	FATAL(str);
	ramuses=0; //确保无法通过认证
	bad_exit();
}

void CheckAddr(a32 s,a32 e,int du)
{
	if (RamBits>16) {
		if (s>=0x2000 && e<0xf00000) {
			if (s<ramuses) ramuses=s;
			if (e>ramusee) ramusee=e;
			return;
		}
		if (!CHECKRAM) return;
		if (CHECKRAM==2) du&=1;
		if (s>=TextBuffer && e<TextBuffer+(((ScreenWidth-2)/6)&0xfe)*((ScreenHeight-1)/13)) {
			if (!(du&2)) DispRamErr(s,e);
		} else if (s>=string_stack && e<string_stack+0x400) {
			if (!(du&1)) DispRamErr(s,e);
		} else DispRamErr(s,e);
	} else {
		if (s>=0x2000 && e<0x8000) {
			if (s<ramuses) ramuses=s;
			if (e>ramusee) ramusee=e;
			return;
		}
		if (!CHECKRAM) return;
		if (CHECKRAM==2) du&=1;
		if (s>=TextBuffer && e<TextBuffer+(((ScreenWidth-2)/6)&0xfe)*((ScreenHeight-1)/13)) {
			if (!(du&2)) DispRamErr(s,e);
		} else if (s>=string_stack && e<string_stack+0x400) {
			if (!(du&1)) DispRamErr(s,e);
		} else DispRamErr(s,e);
	}
}

void div0_err()
{
	char str[100];

	if (d_line>=0) {
		sprintf(str,"除0错!\n\n发生在%d行",d_line);
		FATAL(str);
	} else {FATAL("除0错!");}
	ramuses=0; //确保无法通过认证
	bad_exit();
}

void sn_err()
{
	;
}

byte lRamRead(a32 addr)
{
	CheckAddr(addr,addr,7);
	return lRam[addr];
}

short lRamRead2(a32 addr)
{
	if (addr&1) ramarm=0;
	CheckAddr(addr,addr+1,7);
	return *(short *)(lRam+addr);
}

long lRamRead4(a32 addr)
{
	if (addr&3) ramarm=0;
	CheckAddr(addr,addr+3,7);
	return *(long *)(lRam+addr);
}

void lRamWrite(a32 addr,byte t)
{
	CheckAddr(addr,addr,6);
	lRam[addr]=t;
}

void lRamWrite2(a32 addr,short t)
{
	if (addr&1) ramarm=0;
	CheckAddr(addr,addr+1,6);
	*(short *)(lRam+addr)=t;
}

void lRamWrite4(a32 addr,long t)
{
	if (addr&3) ramarm=0;
	CheckAddr(addr,addr+3,6);
	*(long *)(lRam+addr)=t;
}

void lmemcpy(a32 src,a32 obj,int size,int src_du,int obj_du)
{
	if (size) {
		if (src_du!=100)
			CheckAddr(src,src+size-1,src_du);
		if (obj_du!=100)
			CheckAddr(obj,obj+size-1,obj_du);
		memcpy(lRam+src,lRam+obj,size);
	}
}

byte getcode()
{
	return *(lPC++);
}

a32 get_vaddr()
{
	a32 t;
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	return t;
}

a32 get_lvaddr()
{
	a32 t;
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	return t+local_bp;
}

a32 get_gaddr()
{
	a32 t;
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	eval_top-=4;
	t+=lRam[eval_stack+eval_top]+(lRam[eval_stack+eval_top+1]<<8);
	if (RamBits>16) t+=lRam[eval_stack+eval_top+2]<<16;
	return t;
}

a32 get_lgaddr()
{
	a32 t;
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	t+=local_bp;
	eval_top-=4;
	t+=lRam[eval_stack+eval_top]+(lRam[eval_stack+eval_top+1]<<8);
	if (RamBits>16) t+=lRam[eval_stack+eval_top+2]<<16;
	return t;
}

void get_val()
{
	eval_top-=4;
	a1=*(long *)(lRam+eval_stack+eval_top);
}

void get_val2()
{
	eval_top-=4;
	a3=*(long *)(lRam+eval_stack+eval_top);
}

void get_vals()
{
	get_val2();
	get_val();
}

void put_val()
{
	*(long *)(lRam+eval_stack+eval_top)=a1;
	eval_top+=4;
}

void put_string()
{
	lRam[eval_stack+eval_top++]=(byte)string_ptr;
	lRam[eval_stack+eval_top++]=(byte)(string_ptr>>8);
	if (RamBits>16)
		lRam[eval_stack+eval_top++]=(byte)(string_ptr>>16);
	else
		lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_char()
{
	lRam[eval_stack+eval_top++]=getcode();
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_int()
{
	byte t;
	lRam[eval_stack+eval_top++]=getcode();
	t=getcode();
	lRam[eval_stack+eval_top++]=t;
	if (t&0x80) {
		lRam[eval_stack+eval_top++]=0xff;
		lRam[eval_stack+eval_top++]=0xff;
	} else {
		lRam[eval_stack+eval_top++]=0;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_long()
{
	lRam[eval_stack+eval_top++]=getcode();
	lRam[eval_stack+eval_top++]=getcode();
	lRam[eval_stack+eval_top++]=getcode();
	lRam[eval_stack+eval_top++]=getcode();
}

void push_vchar()
{
	lRam[eval_stack+eval_top++]=lRamRead(get_vaddr());
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_lvchar()
{
	lRam[eval_stack+eval_top++]=lRamRead(get_lvaddr());
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_vint()
{
	a32 t;
	byte t2;
	t=get_vaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	t2=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=t2;
	if (t2&0x80) {
		lRam[eval_stack+eval_top++]=0xff;
		lRam[eval_stack+eval_top++]=0xff;
	} else {
		lRam[eval_stack+eval_top++]=0;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_lvint()
{
	a32 t;
	byte t2;
	t=get_lvaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	t2=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=t2;
	if (t2&0x80) {
		lRam[eval_stack+eval_top++]=0xff;
		lRam[eval_stack+eval_top++]=0xff;
	} else {
		lRam[eval_stack+eval_top++]=0;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_vlong()
{
	a32 t;
	t=get_vaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	lRam[eval_stack+eval_top++]=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=lRamRead(t+2);
	lRam[eval_stack+eval_top++]=lRamRead(t+3);
}

void push_lvlong()
{
	a32 t;
	t=get_lvaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	lRam[eval_stack+eval_top++]=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=lRamRead(t+2);
	lRam[eval_stack+eval_top++]=lRamRead(t+3);
}

void push_gchar()
{
	lRam[eval_stack+eval_top++]=lRamRead(get_gaddr());
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_lgchar()
{
	lRam[eval_stack+eval_top++]=lRamRead(get_lgaddr());
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_gint()
{
	a32 t;
	byte t2;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	t2=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=t2;
	if (t2&0x80) {
		lRam[eval_stack+eval_top++]=0xff;
		lRam[eval_stack+eval_top++]=0xff;
	} else {
		lRam[eval_stack+eval_top++]=0;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_lgint()
{
	a32 t;
	byte t2;
	t=get_lgaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	t2=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=t2;
	if (t2&0x80) {
		lRam[eval_stack+eval_top++]=0xff;
		lRam[eval_stack+eval_top++]=0xff;
	} else {
		lRam[eval_stack+eval_top++]=0;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_glong()
{
	a32 t;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	lRam[eval_stack+eval_top++]=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=lRamRead(t+2);
	lRam[eval_stack+eval_top++]=lRamRead(t+3);
}

void push_lglong()
{
	a32 t;
	t=get_lgaddr();
	lRam[eval_stack+eval_top++]=lRamRead(t);
	lRam[eval_stack+eval_top++]=lRamRead(t+1);
	lRam[eval_stack+eval_top++]=lRamRead(t+2);
	lRam[eval_stack+eval_top++]=lRamRead(t+3);
}

void push_ax()
{
	a32 t;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>24)&0xff);
}

void push_achar()
{
	a32 t;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16) {
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
		lRam[eval_stack+eval_top++]=1;
	} else {
		lRam[eval_stack+eval_top++]=1;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_lachar()
{
	a32 t;
	t=get_lgaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16) {
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
		lRam[eval_stack+eval_top++]=1;
	} else {
		lRam[eval_stack+eval_top++]=1;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_aint()
{
	a32 t;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16) {
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
		lRam[eval_stack+eval_top++]=2;
	} else {
		lRam[eval_stack+eval_top++]=2;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_laint()
{
	a32 t;
	t=get_lgaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16) {
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
		lRam[eval_stack+eval_top++]=2;
	} else {
		lRam[eval_stack+eval_top++]=2;
		lRam[eval_stack+eval_top++]=0;
	}
}

void push_along()
{
	a32 t;
	t=get_gaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16)
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
	else
		lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_lalong()
{
	a32 t;
	t=get_lgaddr();
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16)
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
	else
		lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_string()
{
	byte t;
	put_string();
	for (;;) {
		t=getcode()^secret;
		lRam[string_ptr++]=t;
		if (t==0) break;
	}
	if (string_ptr>=string_stack+768)
		string_ptr=string_stack;
}

void push_llong()
{
	a32 t;
	t=get_vaddr()+local_bp;
	lRam[eval_stack+eval_top++]=(byte)(t&0xff);
	lRam[eval_stack+eval_top++]=(byte)((t>>8)&0xff);
	if (RamBits>16)
		lRam[eval_stack+eval_top++]=(byte)((t>>16)&0xff);
	else
		lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
}

void push_text()
{
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	FATAL("LavaX3.3版起不支持_TEXT");
	ramuses=0; //确保无法通过认证
	bad_exit();
}

void push_graph() //返回0表示不支持_GRAPH
{
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	FATAL("LavaX3.0版起不支持_GRAPH");
	ramuses=0; //确保无法通过认证
	bad_exit();
}

void push_gbuf() //返回0表示不支持_GBUF
{
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	lRam[eval_stack+eval_top++]=0;
	FATAL("LavaX3.0版起不支持_GBUF");
	ramuses=0; //确保无法通过认证
	bad_exit();
}

void push_sub0()
{
	get_val();
	a1=0-a1;
	put_val();
}

void inc_dec_com()
{
	long t;
	get_val2();
	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		if (a3&0x80000000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<24;
	} else {
		t=(a3>>16)&0x7f;
		if (a3&0x800000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<16;
	}
}

void inc_save()
{
	int t;
	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		a3&=0xffffff;
	} else {
		t=(a3>>16)&0x7f;
		a3&=0xffff;
	}
	if (t==1) lRamWrite(a3,(byte)(a1&0xff));
	else if (t==2) lRamWrite2(a3,(short)(a1&0xffff));
	else lRamWrite4(a3,a1);
}

void cal_inc()
{
	inc_dec_com();
	a1++;
	put_val();
	inc_save();
}

void cal_dec()
{
	inc_dec_com();
	a1--;
	put_val();
	inc_save();
}

void cal_INC()
{
	inc_dec_com();
	put_val();
	a1++;
	inc_save();
}

void cal_DEC()
{
	inc_dec_com();
	put_val();
	a1--;
	inc_save();
}

void cal_idx()
{
	byte t,t2;

	get_val2();
	t=getcode();
	if (t&0x80) a3+=local_bp;
	t2=t&0x1f;
	if (t2==1) a1=lRamRead(a3);
	else if (t2==2) a1=lRamRead2(a3);
	else a1=lRamRead4(a3);
	t=(t>>5)&3;
	switch (t) {
	case 0:
		a1++;
		put_val();
		break;
	case 1:
		a1--;
		put_val();
		break;
	case 2:
		put_val();
		a1++;
		break;
	case 3:
		put_val();
		a1--;
		break;
	}
	if (t2==1) lRamWrite(a3,(byte)(a1&0xff));
	else if (t2==2) lRamWrite2(a3,(short)(a1&0xffff));
	else lRamWrite4(a3,a1);
}

void cal_add()
{
	get_vals();
	a1+=a3;
	put_val();
}

void cal_sub()
{
	get_vals();
	a1-=a3;
	put_val();
}

void cal_and()
{
	get_vals();
	a1&=a3;
	put_val();
}

void cal_or()
{
	get_vals();
	a1|=a3;
	put_val();
}

void push_not()
{
	get_val();
	a1=a1^0xffffffff;
	put_val();
}

void cal_xor()
{
	get_vals();
	a1^=a3;
	put_val();
}

void cal_land()
{
	get_vals();
	if (a1 && a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_lor()
{
	get_vals();
	if (a1 || a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_lnot()
{
	get_val();
	if (a1==0) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_mul()
{
	get_vals();
	a1*=a3;
	put_val();
}

void cal_div()
{
	get_vals();
	if (a3==0) div0_err();
	else a1/=a3;
	put_val();
}

void cal_mod()
{
	get_vals();
	if (a3==0) div0_err();
	else a1%=a3;
	put_val();
}

void cal_lshift()
{
	get_vals();
	if (a3<0) a1=0;
	else if (a3==0) ;
	else a1<<=a3;
	put_val();
}

void cal_rshift()
{
	get_vals();
	if (a3<0) a1=0;
	else if (a3==0) ;
	else a1=(unsigned long)a1>>a3;
	put_val();
}

void cal_equ()
{
	get_vals();
	if (a1==a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_neq()
{
	get_vals();
	if (a1!=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_le()
{
	get_vals();
	if (a1<=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_ge()
{
	get_vals();
	if (a1>=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_great()
{
	get_vals();
	if (a1>a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_less()
{
	get_vals();
	if (a1<a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void c_let()
{
	long t,t1;
	get_vals();
	if (RamBits>16) {
		if (a1&0x80000000) t1=(a1+local_bp)&0xffffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffffff;
		t=(a1>>24)&0x7f;
	} else {
		if (a1&0x800000) t1=(a1+local_bp)&0xffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffff;
		t=(a1>>16)&0x7f;
	}
	if (t==1) lRamWrite(t1,(byte)(a3&0xff));
	else if (t==2) lRamWrite2(t1,(short)(a3&0xffff));
	else lRamWrite4(t1,a3);
	a1=a3;
	put_val();
}

void c_letx()
{
	byte t;

	get_vals();
	t=getcode();
	if (t&0x80) a1+=local_bp;
	t&=0x7f;
	//a1&=0xffffff;
	if (t==1) lRamWrite(a1,(byte)(a3&0xff));
	else if (t==2) lRamWrite2(a1,(short)(a3&0xffff));
	else lRamWrite4(a1,a3);
	a1=a3;
	put_val();
}

void c_ptr()
{
	get_val();
	if (RamBits>16)
		a1&=0xffffff;
	else
		a1&=0xffff;
	a1=lRamRead(a1);
	put_val();
}

void c_iptr()
{
	get_val();
	if (RamBits>16)
		a1&=0xffffff;
	else
		a1&=0xffff;
	a1=lRamRead(a1)+(lRamRead(a1+1)<<8);
	a1&=0xffff;
	if (a1&0x8000) a1|=0xffff0000; //负数
	put_val();
}

void c_lptr()
{
	get_val();
	if (RamBits>16)
		a1&=0xffffff;
	else
		a1&=0xffff;
	a1=lRamRead(a1)+(lRamRead(a1+1)<<8)+(lRamRead(a1+2)<<16)+(lRamRead(a1+3)<<24);
	put_val();
}

void c_cptr()
{
	get_val();
	if (RamBits>16)
		a1=(a1&0xffffff)|0x1000000;
	else
		a1=(a1&0xffff)|0x10000;
	put_val();
}

void c_ciptr()
{
	get_val();
	if (RamBits>16)
		a1=(a1&0xffffff)|0x2000000;
	else
		a1=(a1&0xffff)|0x20000;
	put_val();
}

void c_clptr()
{
	get_val();
	if (RamBits>16)
		a1=(a1&0xffffff)|0x4000000;
	else
		a1=(a1&0xffff)|0x40000;
	put_val();
}

void pop_val()
{
	get_val();
	result=a1;
}

void c_jmp()
{
	long t;
	t=getcode();
	t+=getcode()<<8;
	t+=getcode()<<16;
	lPC=pLAVA+t;
}

void c_jmpe()
{
	if (result==0) {
		c_jmp();
	} else {
		getcode();
		getcode();
		getcode();
	}
}

void c_jmpn()
{
	if (result) {
		c_jmp();
	} else {
		getcode();
		getcode();
		getcode();
	}
}

void set_sp()
{
	local_sp=getcode();
	local_sp+=getcode()<<8;
	if (RamBits>16) local_sp+=getcode()<<16;
	//if (local_sp&3) local_sp+=4-(local_sp&3); //令堆栈开始于4字节边界
}

void c_call()
{
	lRamWrite(local_sp,(byte)((lPC+3-pLAVA)&0xff));
	lRamWrite(local_sp+1,(byte)(((lPC+3-pLAVA)>>8)&0xff));
	lRamWrite(local_sp+2,(byte)(((lPC+3-pLAVA)>>16)&0xff));
	c_jmp();
}

void add_bp()
{
	long t;
	byte t2;
	int i;
	t=local_bp;
	local_bp=local_sp;
	lRamWrite(local_bp+3,(byte)(t&0xff));
	lRamWrite(local_bp+4,(byte)((t>>8)&0xff));
	if (RamBits>16) lRamWrite(local_bp+5,(byte)((t>>16)&0xff));
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	local_sp=local_bp+(t&0xffffff);
	//if (local_sp&3) local_sp+=4-(local_sp&3); //令堆栈开始于4字节边界
	t=getcode()*4;
	if (t) {
		eval_top-=(word)t;
		t2=eval_top;
		i=0;
		if (RamBits==32) {
			while (t) {
				lRamWrite(local_bp+8+i++,lRam[eval_stack+t2++]);
				t--;
			}
		} else if (RamBits>16) {
			while (t) {
				lRamWrite(local_bp+6+i++,lRam[eval_stack+t2++]);
				t--;
			}
		} else {
			while (t) {
				lRamWrite(local_bp+5+i++,lRam[eval_stack+t2++]);
				t--;
			}
		}
	}
}

void sub_bp()
{
	long t;
	local_sp=local_bp;
	t=lRamRead(local_bp);
	t+=lRamRead(local_bp+1)<<8;
	t+=lRamRead(local_bp+2)<<16;
	lPC=pLAVA+t;
	t=lRamRead(local_bp+3);
	t+=lRamRead(local_bp+4)<<8;
	if (RamBits>16) t+=lRamRead(local_bp+5)<<16;
	local_bp=t&0xffffff;
	if (func_top) cur_funcid=func_stack[--func_top];
}

void c_preset()
{
	int addr,len;
	addr=getcode();
	addr+=getcode()<<8;
	if (RamBits>16) addr+=getcode()<<16;
	len=getcode();
	len+=getcode()<<8;
	while (len) {
		lRamWrite(addr++,getcode());
		len--;
	}
}

void c_secret()
{
	secret=getcode();
}

void c_loadall()
{
	;
}

void getint()
{
	a3=getcode();
	a3+=getcode()<<8;
	if (a3&0x8000) a3|=0xffff0000;
}

void cal_qadd()
{
	get_val();
	getint();
	a1+=a3;
	put_val();
}

void cal_qsub()
{
	get_val();
	getint();
	a1-=a3;
	put_val();
}

void cal_qmul()
{
	get_val();
	getint();
	a1*=a3;
	put_val();
}

void cal_qdiv()
{
	get_val();
	getint();
	if (a3==0) div0_err();
	else a1/=a3;
	put_val();
}

void cal_qmod()
{
	get_val();
	getint();
	if (a3==0) div0_err();
	else a1%=a3;
	put_val();
}

void cal_qlshift()
{
	get_val();
	getint();
	a1<<=a3;
	put_val();
}

void cal_qrshift()
{
	get_val();
	getint();
	a1=(unsigned long)a1>>a3;
	put_val();
}

void cal_qequ()
{
	get_val();
	getint();
	if (a1==a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_qneq()
{
	get_val();
	getint();
	if (a1!=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_qgreat()
{
	get_val();
	getint();
	if (a1>a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_qless()
{
	get_val();
	getint();
	if (a1<a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_qge()
{
	get_val();
	getint();
	if (a1>=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_qle()
{
	get_val();
	getint();
	if (a1<=a3) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

int if_stop_line(int line)
{
	int i;

	for (i=0;i<sline_num;i++) {
		if (sline[i]==line) return 1;
	}
	return 0;
}

void c_icf()
{
	get_val();
	a1=f2i((float)a1);
	put_val();
}

void c_fci()
{
	get_val();
	a1=(long)(i2f(a1));
	put_val();
}

void c_lcc()
{
	get_val();
	a1&=0xff;
	put_val();
}

void c_lci()
{
	get_val();
	a1&=0xffff;
	if (a1&0x8000) a1|=0xffff0000;
	put_val();
}

void cal_addff()
{
	get_vals();
	a1=f2i(i2f(a1)+i2f(a3));
	put_val();
}

void cal_addf()
{
	get_vals();
	a1=f2i(i2f(a1)+a3);
	put_val();
}

void cal_add0f()
{
	get_vals();
	a1=f2i(a1+i2f(a3));
	put_val();
}

void cal_subff()
{
	get_vals();
	a1=f2i(i2f(a1)-i2f(a3));
	put_val();
}

void cal_subf()
{
	get_vals();
	a1=f2i(i2f(a1)-a3);
	put_val();
}

void cal_sub0f()
{
	get_vals();
	a1=f2i(a1-i2f(a3));
	put_val();
}

void cal_mulff()
{
	get_vals();
	a1=f2i(i2f(a1)*i2f(a3));
	put_val();
}

void cal_mulf()
{
	get_vals();
	a1=f2i(i2f(a1)*a3);
	put_val();
}

void cal_mul0f()
{
	get_vals();
	a1=f2i(a1*i2f(a3));
	put_val();
}

void cal_divff()
{
	get_vals();
	if (a3==0) div0_err();
	a1=f2i(i2f(a1)/i2f(a3));
	put_val();
}

void cal_divf()
{
	get_vals();
	if (a3==0) div0_err();
	a1=f2i(i2f(a1)/a3);
	put_val();
}

void cal_div0f()
{
	get_vals();
	if (a3==0) div0_err();
	a1=f2i(a1/i2f(a3));
	put_val();
}

void push_sub0f()
{
	get_val();
	a1=f2i(0-i2f(a1));
	put_val();
}

void cal_lessf()
{
	get_vals();
	if (i2f(a1)<i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_greatf()
{
	get_vals();
	if (i2f(a1)>i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_equf()
{
	get_vals();
	if (i2f(a1)==i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_neqf()
{
	get_vals();
	if (i2f(a1)!=i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_lef()
{
	get_vals();
	if (i2f(a1)<=i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void cal_gef()
{
	get_vals();
	if (i2f(a1)>=i2f(a3)) a1=LTRUE;
	else a1=LFALSE;
	put_val();
}

void c_f0()
{
	get_val();
	a1&=0x7fffffff;
	put_val();
}

void c_pass()
{
	getcode();
}

void c_void()
{
	;
}

void c_debug()
{
	int line;
	line=getcode();
	line+=getcode()<<8;
	line+=getcode()<<16;
	d_line=line;
	if (stop_line==-2 || line==stop_line || (stop_line==0 && (stop_func==0 || stop_func==cur_funcid)) || if_stop_line(line)) {
		PauseFlag=1;
		stop_func=cur_funcid;
		copydata.dwData=(local_bp&0xffff)|(cur_funcid<<16);
		copydata.cbData=0x800000;
		copydata.lpData=lRam;
		lRam[0]=(byte)(local_bp>>16);
		SendMessage(hwnd_ide,WM_COPYDATA,(WPARAM)hwnd_main,(LPARAM)&copydata);
		SendMessage(hwnd_ide,WM_STARSTOP,(WPARAM)line,0);
	}
}

void c_funcid()
{
	int line;
	line=getcode();
	line+=getcode()<<8;
	line+=getcode()<<16;
	func_stack[func_top++]=cur_funcid;
	cur_funcid=line;
}

CODES codes[]={
	sn_err,push_char,push_int,push_long,push_vchar,push_vint,push_vlong,
	push_gchar,push_gint,push_glong,push_achar,push_aint,push_along,push_string,
	push_lvchar,push_lvint,push_lvlong,push_lgchar,push_lgint,push_lglong,
	push_lachar,push_laint,push_lalong,push_along,push_lalong,push_llong,
	push_text,push_graph,push_sub0,cal_inc,cal_dec,cal_INC,cal_DEC,
	cal_add,cal_sub,cal_and,cal_or,push_not,cal_xor,cal_land,cal_lor,cal_lnot,
	cal_mul,cal_div,cal_mod,cal_lshift,cal_rshift,cal_equ,cal_neq,cal_le,cal_ge,cal_great,cal_less,
	c_let,c_ptr,c_cptr,pop_val,c_jmpe,c_jmpn,c_jmp,set_sp,c_call,add_bp,sub_bp,
	good_exit,c_preset,push_gbuf,c_secret,c_loadall,
	cal_qadd,cal_qsub,cal_qmul,cal_qdiv,cal_qmod,cal_qlshift,cal_qrshift,
	cal_qequ,cal_qneq,cal_qgreat,cal_qless,cal_qge,cal_qle,
	c_iptr,c_lptr,
	c_icf,c_fci,cal_addff,cal_addf,cal_add0f,cal_subff,cal_subf,cal_sub0f,
	cal_mulff,cal_mulf,cal_mul0f,cal_divff,cal_divf,cal_div0f,push_sub0f,
	cal_lessf,cal_greatf,cal_equf,cal_neqf,cal_lef,cal_gef,c_f0,
	c_ciptr,c_clptr,c_lcc,c_lci,c_letx,push_ax,cal_idx,c_pass,c_void,
	c_debug,c_funcid
};

void ByteAddr()
{
	m1l=yy*LCD_WIDTH+xx;
	if (!no_buf) m1l+=SCROLL_CON;
}

void put_dot()
{
	if (xx>=ScreenWidth) return; //在pc端允许操作最左列，毕竟不是所有LCD都带ICON的
	if (yy>=ScreenHeight) return;
	ByteAddr();
	if (graph_mode==1) {
		switch (lcmd) {
		case 0:
			BmpData[m1l]=0;
			break;
		case 1:
			BmpData[m1l]=1;
			break;
		case 2:
			BmpData[m1l]^=1;
			break;
		}
	} else if (graph_mode==4) {
		switch (lcmd) {
		case 0:
			BmpData[m1l]=(byte)bgcolor;
			break;
		case 1:
			BmpData[m1l]=(byte)fgcolor;
			break;
		case 2:
			BmpData[m1l]^=15;
			break;
		}
	} else {
		switch (lcmd) {
		case 0:
			BmpData[m1l]=(byte)bgcolor;
			break;
		case 1:
			BmpData[m1l]=(byte)fgcolor;
			break;
		case 2:
			BmpData[m1l]^=255;
			break;
		}
	}
}

word get_dot()
{
	ByteAddr();
	return BmpData[m1l];
}

void write_comm(word x,word y,word width,word height,byte *data)
{
	byte tdata[320*240];
	int i,j;
	byte t;
	word widths;
	int x_bak;
	static byte msktbl[]={128,64,32,16,8,4,2,1}; 
	byte *td,*disp,*tt;
	int temp;

	x_bak=x;
	if (graph_mode==1) {
		widths=(width+7)>>3;
		if ((width&7)==0 && negative) {
			td=tdata;
			memcpy(td,data,widths*height);
			for (i=0;i<height;i++)
				for (j=0;j<widths/2;j++) {
					t=td[i*widths+j];
					td[i*widths+j]=td[i*widths+widths-1-j];
					td[i*widths+widths-1-j]=t;
				}
			for (i=0;i<widths*height;i++)
				td[i]=negative_tbl[td[i]];
		} else
			td=data;

		if (y>=ScreenHeight) {
			temp=0x10000-y;
			if (height<=temp) return; //全在画面外，不需要画
			height-=temp;
			td+=temp*widths;
			y=0;
		}
		if (y+height>ScreenHeight) height=ScreenHeight-y; //剪去出底屏部分
		if (x_bak>=ScreenWidth) {
			temp=0x10000-x_bak;
			if (width<=temp) return; //全在画面外，不需要画
			width-=temp;
			td+=temp>>3;
			if (temp&7) {
				width++;
				x_bak=0-(temp&7);
				if (width>ScreenWidth-x_bak) width=ScreenWidth-x_bak;
			} else {
				x_bak=0;
				if (width>ScreenWidth) width=ScreenWidth;
			}
		} else {
			if (x_bak+width>ScreenWidth) width=ScreenWidth-x_bak;
		}

		for (i=0;i<height;i++,y++) {
			if (x_bak<0) {
				j=-x_bak;
				x=0;
			} else {
				j=0;
				x=x_bak;
			}
			xx=x;yy=y;
			ByteAddr();
			disp=BmpData+m1l;
			tt=td;
			t=*tt++;
			if ((lcmd&8) || (lcmd&7)==2) {
				switch (lcmd&7) {
				case 3:
					while (j<width) {		
						if (!(t&msktbl[j&7])) *disp=1;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				case 4:
					while (j<width) {		
						if (t&msktbl[j&7]) *disp=0;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				case 5:
					while (j<width) {		
						if (!(t&msktbl[j&7])) *disp^=1;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				default:
					while (j<width) {		
						if (t&msktbl[j&7]) *disp=0;
						else *disp=1;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				}
			} else {
				switch (lcmd&7) {
					case 3:
					while (j<width) {		
						if (t&msktbl[j&7]) *disp=1;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				case 4:
					while (j<width) {		
						if (!(t&msktbl[j&7])) *disp=0;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				case 5:
					while (j<width) {		
						if (t&msktbl[j&7]) *disp^=1;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				default:
					while (j<width) {		
						if (t&msktbl[j&7]) *disp=1;
						else *disp=0;
						disp++;
						if ((j&7)==7) t=*tt++;
						j++;	
					}
					break;
				}
			}
			td+=widths;
		}
	} else if (graph_mode==4) {
		widths=(width+1)>>1;	
		if ((width&1)==0 && negative) {
			td=tdata;
			memcpy(td,data,widths*height);
			for (i=0;i<height;i++)
				for (j=0;j<widths/2;j++) {
					t=td[i*widths+j];
					td[i*widths+j]=td[i*widths+widths-1-j];
					td[i*widths+widths-1-j]=t;
				}
			for (i=0;i<widths*height;i++) {
				t=td[i];
				td[i]=(t<<4)|(t>>4);
			}
		} else
			td=data;
			
		if (y>=ScreenHeight) {
			temp=0x10000-y;
			if (height<=temp) return; //全在画面外，不需要画
			height-=temp;
			td+=temp*widths;
			y=0;
		}
		if (y+height>ScreenHeight) height=ScreenHeight-y; //剪去出底屏部分
		if (x_bak>=ScreenWidth) {
			temp=0x10000-x_bak;
			if (width<=temp) return; //全在画面外，不需要画
			width-=temp;
			td+=temp>>1;
			if (temp&1) {
				width++;
				x_bak=-1;
				if (width>ScreenWidth+1) width=ScreenWidth+1;
			} else {
				x_bak=0;
				if (width>ScreenWidth) width=ScreenWidth;
			}
		} else {
			if (x_bak+width>ScreenWidth) width=ScreenWidth-x_bak;
		}

		for (i=0;i<height;i++,y++) {
			if (x_bak==-1) {
				j=1;
				x=0;
			} else {
				j=0;
				x=x_bak;
			}
			xx=x;yy=y;
			ByteAddr();
			disp=BmpData+m1l;
			tt=td;
			t=*tt++;
			if ((lcmd&8) || (lcmd&7)==2) {
				switch (lcmd&7) {
				case 3:
					while (j<width) {		
						if (j&1) {
							*disp|=(t&0xf)^0xf;
							t=*tt++;
						} else *disp|=(t>>4)^0xf;
						disp++;
						j++;
					}
					break;
				case 4:
					while (j<width) {		
						if (j&1) {
							*disp&=(t&0xf)^0xf;
							t=*tt++;
						} else *disp&=(t>>4)^0xf;
						disp++;
						j++;
					}
					break;
				case 5:
					while (j<width) {		
						if (j&1) {
							*disp^=(t&0xf)^0xf;
							t=*tt++;
						} else *disp^=(t>>4)^0xf;
						disp++;
						j++;
					}
					break;													
				default:
					while (j<width) {		
						if (j&1) {
							*disp++=(t&0xf)^0xf;
							t=*tt++;
						} else *disp++=(t>>4)^0xf;
						j++;
					}
					break;
				}
			} else {
				switch (lcmd&7) {
				case 3:
					while (j<width) {		
						if (j&1) {
							*disp|=t&0xf;
							t=*tt++;
						} else *disp|=t>>4;
						disp++;
						j++;	
					}
					break;
				case 4:
					while (j<width) {		
						if (j&1) {
							*disp&=t&0xf;
							t=*tt++;
						} else *disp&=t>>4;
						disp++;
						j++;	
					}
					break;
				case 5:
					while (j<width) {		
						if (j&1) {
							*disp^=t&0xf;
							t=*tt++;
						} else *disp^=t>>4;
						disp++;
						j++;	
					}
					break;				
				default:
					while (j<width) {		
						if (j&1) {
							*disp++=t&0xf;
							t=*tt++;
						} else *disp++=t>>4;
						j++;
					}
				}
			}
			td+=widths;
		}
	} else {
		widths=width;	
		if (negative) {
			td=tdata;
			memcpy(td,data,widths*height);
			for (i=0;i<height;i++)
				for (j=0;j<widths/2;j++) {
					t=td[i*widths+j];
					td[i*widths+j]=td[i*widths+widths-1-j];
					td[i*widths+widths-1-j]=t;
				}
		} else
			td=data;
			
		if (y>=ScreenHeight) {
			temp=0x10000-y;
			if (height<=temp) return; //全在画面外，不需要画
			height-=temp;
			td+=temp*widths;
			y=0;
		}
		if (y+height>ScreenHeight) height=ScreenHeight-y; //剪去出底屏部分
		if (x_bak>=ScreenWidth) {
			temp=0x10000-x_bak;
			if (width<=temp) return; //全在画面外，不需要画
			width-=temp;
			td+=temp;
			x_bak=0;
		}
		if (x_bak+width>ScreenWidth) width=ScreenWidth-x_bak;

		if (lcmd==1) {
			xx=x_bak;yy=y;
			ByteAddr();
			disp=BmpData+m1l;
			j=height;
			while (j--) {
				memcpy(disp,td,width);
				td+=widths;
				disp+=LCD_WIDTH;
			}
			return;
		}

		for (i=0;i<height;i++,y++) {
			j=width;
			xx=x_bak;yy=y;
			ByteAddr();
			disp=BmpData+m1l;
			tt=td;
			if ((lcmd&8) || (lcmd&7)==2) {
				switch (lcmd&7) {
				case 3:
					while (j--) {
						*disp|=(*tt++)^0xff;
						disp++;
					}
					break;
				case 4:
					while (j--) {
						*disp&=(*tt++)^0xff;
						disp++;
					}
					break;
				case 5:
					while (j--) {
						*disp^=(*tt++)^0xff;
						disp++;
					}
					break;													
				default:
					while (j--) {
						*disp++=(*tt++)^0xff;
					}
					break;
				}
			} else {
				switch (lcmd&7) {
				case 3:
					while (j--) {
						*disp|=*tt++;
						disp++;
					}
					break;
				case 4:
					while (j--) {
						*disp&=*tt++;
						disp++;
					}
					break;
				case 5:
					while (j--) {
						*disp^=*tt++;
						disp++;
					}
					break;
				case 6:
					while (j--) {
						t=*tt++;
						if (t) *disp++=t;
						else disp++;
					}
					break;
				default:
					while (j--) {
						*disp++=*tt++;
					}
				}
			}
			td+=widths;
		}
	}
}

void font_6x12(word x,word y,byte c)
{
	byte *addr;
	byte t,t2;
	int i;
	if (graph_mode==1) {
		if (c>=128) memset(patbuf,0,12);
		else memcpy(patbuf,ascii+c*12,12);
	} else if (graph_mode==4) {
		if (c>=128) memset(patbuf,0,36);
		else {
			addr=ascii+c*12;
			for (i=0;i<12;i++) {
				t=*addr++;
				if (t&0x80) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&0x40) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*3]=t2;
				if (t&0x20) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&0x10) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*3+1]=t2;
				if (t&8) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&4) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*3+2]=t2;
			}
		}
	} else {
		if (c>=128) memset(patbuf,0,72);
		else {
			addr=ascii+c*12;
			for (i=0;i<12;i++) {
				t=*addr++;
				if (t&0x80) patbuf[i*6]=(byte)fgcolor;
				else patbuf[i*6]=(byte)bgcolor;
				if (t&0x40) patbuf[i*6+1]=(byte)fgcolor;
				else patbuf[i*6+1]=(byte)bgcolor;
				if (t&0x20) patbuf[i*6+2]=(byte)fgcolor;
				else patbuf[i*6+2]=(byte)bgcolor;
				if (t&0x10) patbuf[i*6+3]=(byte)fgcolor;
				else patbuf[i*6+3]=(byte)bgcolor;
				if (t&0x8) patbuf[i*6+4]=(byte)fgcolor;
				else patbuf[i*6+4]=(byte)bgcolor;
				if (t&0x4) patbuf[i*6+5]=(byte)fgcolor;
				else patbuf[i*6+5]=(byte)bgcolor;
			}
		}
	}
	write_comm(x,y,6,12,patbuf);
}

void font_8x16(word x,word y,byte c)
{
	byte *addr;
	byte t,t2;
	int i;
	if (graph_mode==1) {
		if (c>=128) memset(patbuf,0,16);
		else memcpy(patbuf,ascii8+c*16,16);
	} else if (graph_mode==4) {
		if (c>=128) memset(patbuf,0,64);
		else {
			addr=ascii8+c*16;
			for (i=0;i<16;i++) {
				t=*addr++;
				if (t&0x80) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&0x40) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*4]=t2;
				if (t&0x20) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&0x10) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*4+1]=t2;
				if (t&8) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&4) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*4+2]=t2;
				if (t&2) t2=fgcolor<<4;
				else t2=bgcolor<<4;
				if (t&1) t2|=fgcolor;
				else t2|=bgcolor;
				patbuf[i*4+3]=t2;
			}
		}
	} else {
		if (c>=128) memset(patbuf,0,128);
		else {
			addr=ascii8+c*16;
			for (i=0;i<16;i++) {
				t=*addr++;
				if (t&0x80) patbuf[i*8]=(byte)fgcolor;
				else patbuf[i*8]=(byte)bgcolor;
				if (t&0x40) patbuf[i*8+1]=(byte)fgcolor;
				else patbuf[i*8+1]=(byte)bgcolor;
				if (t&0x20) patbuf[i*8+2]=(byte)fgcolor;
				else patbuf[i*8+2]=(byte)bgcolor;
				if (t&0x10) patbuf[i*8+3]=(byte)fgcolor;
				else patbuf[i*8+3]=(byte)bgcolor;
				if (t&0x8) patbuf[i*8+4]=(byte)fgcolor;
				else patbuf[i*8+4]=(byte)bgcolor;
				if (t&0x4) patbuf[i*8+5]=(byte)fgcolor;
				else patbuf[i*8+5]=(byte)bgcolor;
				if (t&0x2) patbuf[i*8+6]=(byte)fgcolor;
				else patbuf[i*8+6]=(byte)bgcolor;
				if (t&0x1) patbuf[i*8+7]=(byte)fgcolor;
				else patbuf[i*8+7]=(byte)bgcolor;
			}
		}
	}
	write_comm(x,y,8,16,patbuf);
}

void font_12x12(word x,word y,byte c1,byte c2)
{
	byte *addr;
	byte t,t2;
	int i,j;
	if (c1<0xb0) addr=gbfont+(((c1-0xa1)*94+(c2-0xa1))*24);
	else addr=gbfont+(((c1-0xa7)*94+(c2-0xa1))*24);
	if (graph_mode==1) memcpy(patbuf,addr,24);
	else if (graph_mode==4) {
		for (i=0,j=0;i<24;i++) {
			t=*addr++;
			if (t&0x80) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&0x40) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[j++]=t2;
			if (t&0x20) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&0x10) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[j++]=t2;
			if (i&1) continue;
			if (t&8) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&4) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[j++]=t2;
			if (t&2) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&1) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[j++]=t2;
		}
	} else {
		for (i=0,j=0;i<24;i++) {
			t=*addr++;
			if (t&0x80) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x40) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x20) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x10) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (i&1) continue;
			if (t&0x8) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x4) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x2) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
			if (t&0x1) patbuf[j++]=(byte)fgcolor;
			else patbuf[j++]=(byte)bgcolor;
		}
	}
	write_comm(x,y,12,12,patbuf);
}

void font_16x16(word x,word y,byte c1,byte c2)
{
	byte *addr;
	byte t,t2;
	int i;
	if (c1<0xb0) addr=gbfont16+(((c1-0xa1)*94+(c2-0xa1))*32);
	else addr=gbfont16+(((c1-0xa7)*94+(c2-0xa1))*32);
	if (graph_mode==1) memcpy(patbuf,addr,32);
	else  if (graph_mode==4) {
		for (i=0;i<32;i++) {
			t=*addr++;
			if (t&0x80) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&0x40) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[i*4]=t2;
			if (t&0x20) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&0x10) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[i*4+1]=t2;
			if (t&8) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&4) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[i*4+2]=t2;
			if (t&2) t2=fgcolor<<4;
			else t2=bgcolor<<4;
			if (t&1) t2|=fgcolor;
			else t2|=bgcolor;
			patbuf[i*4+3]=t2;
		}
	} else {
		for (i=0;i<32;i++) {
			t=*addr++;
			if (t&0x80) patbuf[i*8]=(byte)fgcolor;
			else patbuf[i*8]=(byte)bgcolor;
			if (t&0x40) patbuf[i*8+1]=(byte)fgcolor;
			else patbuf[i*8+1]=(byte)bgcolor;
			if (t&0x20) patbuf[i*8+2]=(byte)fgcolor;
			else patbuf[i*8+2]=(byte)bgcolor;
			if (t&0x10) patbuf[i*8+3]=(byte)fgcolor;
			else patbuf[i*8+3]=(byte)bgcolor;
			if (t&0x8) patbuf[i*8+4]=(byte)fgcolor;
			else patbuf[i*8+4]=(byte)bgcolor;
			if (t&0x4) patbuf[i*8+5]=(byte)fgcolor;
			else patbuf[i*8+5]=(byte)bgcolor;
			if (t&0x2) patbuf[i*8+6]=(byte)fgcolor;
			else patbuf[i*8+6]=(byte)bgcolor;
			if (t&0x1) patbuf[i*8+7]=(byte)fgcolor;
			else patbuf[i*8+7]=(byte)bgcolor;
		}
	}
	write_comm(x,y,16,16,patbuf);
}

void block_comm()
{
	get_val();
	no_buf=a1&0x40;
	lcmd=a1&3;
	get_val();
	Y1=(word)a1;
	get_val();
	x1=(word)a1;
	get_val();
	Y0=(word)a1;
	get_val();
	x0=(word)a1;
}

void vline()
{
	xx=x0;
	if (Y0<Y1)
		for (yy=Y0;yy<=Y1;yy++) put_dot();
	else
		for (yy=Y1;yy<=Y0;yy++) put_dot();
}

void hline() 
//调用前必须调整Y0,x0,x1使其满足：
//Y0<ScreenHeight x0<ScreenWidth x1<ScreenWidth x0<=x1
{
	word width;
	byte *p;
	yy=Y0;
	xx=x0;
	ByteAddr();
	width=x1-x0+1;
	p=BmpData+m1l;
	//for (xx=x0;xx<=x1;xx++) put_dot();
	if (graph_mode==1) {
		switch (lcmd) {
		case 0:
			memset(p,0,width);
			break;
		case 1:
			memset(p,1,width);
			break;
		case 2:
			while (width--) {
				*p^=1;
				p++;
			}
			break;
		}
	} else if (graph_mode==4) {
		switch (lcmd) {
		case 0:
			memset(p,(byte)bgcolor,width);
			break;
		case 1:
			memset(p,(byte)fgcolor,width);
			break;
		case 2:
			while (width--) {
				*p^=15;
				p++;
			}
			break;
		}
	} else {
		switch (lcmd) {
		case 0:
			memset(p,(byte)bgcolor,width);
			break;
		case 1:
			memset(p,(byte)fgcolor,width);
			break;
		case 2:
			while (width--) {
				*p^=255;
				p++;
			}
			break;
		}
	}
}

void block_check()
{
	word t;
	if (Y0>Y1) {
		t=Y0;
		Y0=Y1;
		Y1=t;
	}
	if (x0>x1) {
		t=x0;
		x0=x1;
		x1=t;
	}
	if (x0>=ScreenWidth) x0=ScreenWidth-1;
	if (x1>=ScreenWidth) x1=ScreenWidth-1;
	if (Y0>=ScreenHeight) Y0=ScreenHeight-1;
	if (Y1>=ScreenHeight) Y1=ScreenHeight-1;
}

int hline_check()
{
	word t;
	if (Y0>=ScreenHeight) return 0; //线在屏幕外
	if (x0>x1) {
		t=x0;
		x0=x1;
		x1=t;
	}
	if (x0>=ScreenWidth) return 0; //线在屏幕外
	if (x1>=ScreenWidth) x1=ScreenWidth-1;
	return 1;
}

void block_draw()
{
	word t;
	block_check();
	t=Y1-Y0+1;
	while (t) {
		hline();
		Y0++;
		t--;
	}
}

void square_draw()
{
	word t;
	block_check();
	hline();
	vline();
	t=x0;
	x0=x1;
	vline();
	x0=t;
	Y0=Y1;
	hline();
}

void scr_up()
{
	memcpy(lRam+TextBuffer,lRam+TextBuffer+curr_CPR,curr_CPR*(curr_RPS-1));
	memset(lRam+TextBuffer+curr_CPR*(curr_RPS-1),0x20,curr_CPR);
	scr_off=curr_CPR*(curr_RPS-1);
	scr_x=0;
	scr_y=curr_RPS-1;
}

void cout(byte c)
{
	if (scr_y>=curr_RPS) scr_up();
	if (c==0x0d) return;
	if (c==0x0a) {
		scr_x=0;
		scr_y++;
		if (scr_y>=curr_RPS) scr_up();
		else scr_off=scr_y*curr_CPR;
		return;
	}
	if (c==9) c=0x20;
	lRam[TextBuffer+scr_off++]=c;
	if (++scr_x>=curr_CPR) {
		scr_x=0;
		scr_y++;
	}
}

void update_lcd_small() //Test 没有考虑反显
{
	byte c,c2;
	unsigned long mask;
	int i,j,color;
	if (curr_RPS<=8) mask=0x80;
	else if (curr_RPS<=16) mask=0x8000;
	else mask=0x800000;
	no_buf=0x40;
	lcmd=0;
	if (graph_mode==1) color=0;
	else color=bgcolor;
	for (i=0;i<small_up;i++)
		memset(BmpData+i*LCD_WIDTH,color,LCD_WIDTH);
	for (i=ScreenHeight-small_down;i<ScreenHeight;i++)
		memset(BmpData+i*LCD_WIDTH,color,LCD_WIDTH);
	for (i=1;i<curr_RPS;i++)
		memset(BmpData+(i*13+small_up-1)*LCD_WIDTH,color,LCD_WIDTH);
	Y0=0;Y1=ScreenHeight-1;
	for (i=0;i<small_left;i++) {
		x0=i;vline();
		x0=ScreenWidth-1-i;vline();
	}
	lcmd=1;
	for (i=0;i<curr_RPS;i++,mask>>=1) {
		if (mask&line_mode) continue;
		for (j=0;j<curr_CPR;j++) {
			c=lRam[TextBuffer+i*curr_CPR+j];
			if (c<128) font_6x12((word)(j*6+small_left),(word)(i*13+small_up),c);
			else {
				c2=lRam[TextBuffer+i*curr_CPR+j+1];
				font_12x12((word)(j*6+small_left),(word)(i*13+small_up),c,c2);
				j++;
			}
		}
	}
}

void update_lcd_large() //Test 没有考虑反显
{
	byte c,c2;
	unsigned long mask;
	int i,j;
	if (curr_RPS<=8) mask=0x80;
	else if (curr_RPS<=16) mask=0x8000;
	else mask=0x800000;
	no_buf=0x40;
	lcmd=1;
	for (i=0;i<curr_RPS;i++,mask>>=1) {
		if (mask&line_mode) continue;
		for (j=0;j<curr_CPR;j++) {
			c=lRam[TextBuffer+i*curr_CPR+j];
			if (c<128) font_8x16((word)(j<<3),(word)(i<<4),c);
			else {
				c2=lRam[TextBuffer+i*curr_CPR+j+1];
				font_16x16((word)(j<<3),(word)(i<<4),c,c2);
				j++;
			}
		}
	}
}

void update_lcd_0()
{
	negative=0;
	if (scr_mode==0) update_lcd_large();
	else update_lcd_small();
}

void next_arg()
{
	func_x+=4;
	a1=*(long *)(lRam+eval_stack+func_x);
}

void printint(int digit,int flag)
{
	char num[20];
	int i,real_digit;

	next_arg();
	sprintf(num,"%ld",a1);
	real_digit=strlen(num);
	if (digit==0 || real_digit>=digit) {
		for (i=0;;i++) {
			if (num[i]) cout(num[i]);
			else break;
		}
	} else {
		if (flag==0x80) {
			for (i=0;;i++) {
				if (num[i]) cout(num[i]);
				else break;
			}
			for (i=0;i<digit-real_digit;i++) {
				cout(' ');
			}
		} else if (flag==0x40) {
			for (i=0;i<digit-real_digit;i++) {
				cout('0');
			}
			for (i=0;;i++) {
				if (num[i]) cout(num[i]);
				else break;
			}
		} else {
			for (i=0;i<digit-real_digit;i++) {
				cout(' ');
			}
			for (i=0;;i++) {
				if (num[i]) cout(num[i]);
				else break;
			}
		}
	}
}

void printfloat()
{
	char num[20];
	int i,flag;
	next_arg();
	if (((a1>>23)&0xff)==0xff) {
		strcpy(num,"error");
	} else
		sprintf(num,"%g",i2f(a1));
	flag=0;
	for (i=0;;i++) {
		if (num[i]=='e' && i) flag=1;
		if (flag) flag++;
		if (flag==4) continue; //转换x.xxxxxe+0yy为x.xxxxxe+yy
		if (num[i]) cout(num[i]);
		else break;
	}
}

void PutPixelx(int x,int y)
{
	if (x<0 || y<0 || x>=ScreenWidth || y>=ScreenHeight) return;
	xx=x;yy=y;
	put_dot();
}

int circle_buf[320];

void put_dot4(int x0,int y0,int x,int y,int mode)
{
	if (mode) {
		if (y0-y>=0 && y0-y<ScreenHeight)
			if (x>=circle_buf[y0-y]) circle_buf[y0-y]=x;
		if (y0+y>=0 && y0+y<ScreenHeight)
			if (x>=circle_buf[y0+y]) circle_buf[y0+y]=x;	
	} else {
		if (x==0) {
			PutPixelx(x0,y0+y);
			PutPixelx(x0,y0-y);
		} else if (y==0) {
			PutPixelx(x0+x,y0);
			PutPixelx(x0-x,y0);
		} else {
			PutPixelx(x0+x,y0+y);
			PutPixelx(x0-x,y0+y);
			PutPixelx(x0+x,y0-y);
			PutPixelx(x0-x,y0-y);
		}
	}
}

void Ellipsex(short x0,short y0,word r1,word r2,int mode)
{
	int i,j,fxy,fx,fy,incx,incy,temp_x,temp_y;
	word delta_x,delta_y,distant_a,distant_b,circle_r,dot_start;

	if (mode) for (i=0;i<ScreenHeight;i++) circle_buf[i]=-1;
	distant_a=r1;
	distant_b=r2;
	dot_start=0;
	if (distant_a==0 && distant_a==0) {
		PutPixelx(x0,y0);
		return;
	}
	circle_r=(distant_a>distant_b)?distant_a:distant_b;
	incx=-1;
	incy=1;
	fy=1;
	fx=1-2*circle_r;
	fxy=0;
	delta_x=0;
	delta_y=0;
	temp_x=distant_a;
	temp_y=0;
	put_dot4(x0,y0,temp_x,temp_y,mode);
	do {
	if (fxy>=0) {
		delta_x+=distant_a;
		if (delta_x>=circle_r) {
			temp_x+=incx;
			delta_x-=circle_r;
			if (temp_x+1!=distant_a)
				put_dot4(x0,y0,temp_x,temp_y,mode);
		}
		fxy-=abs(fx);
		fx+=2;
		if (fx<0 || fx>=3) continue;
		incy=-incy;
		fy=-fy+2;
		fxy=-fxy;
	} else {
		delta_y+=distant_b;
		if (delta_y>=circle_r) {
			delta_y-=circle_r;
			temp_y+=incy;
			if ((temp_y==1 || temp_y==2) && dot_start==0) {
				put_dot4(x0,y0,distant_a,temp_y,mode);
			} else {
				dot_start=1;
				put_dot4(x0,y0,temp_x,temp_y,mode);
			}
		}
		fxy=fxy+abs(fy);
		fy+=2;
		if (fy<0 || fy>2) continue;
		incx=-incx;
		fx=-fx+2;
		fxy=-fxy;
	}
	} while (temp_x);
	if (mode) {
		for (i=0;i<ScreenHeight;i++) {
			if (circle_buf[i]>=0) {
				for (j=0;j<circle_buf[i]*2+1;j++) PutPixelx(x0-circle_buf[i]+j,i);
			}
		}
	}
}

void c_putchar()
{
	get_val();
	cout((byte)a1);
	update_lcd_0();
}

a32 fenxi_fmt(a32 s,int *digit,int *flag)
{
	byte c;
	int have_0,have_fu,total_wei;

	have_0=0;
	have_fu=0;
	total_wei=0;
	for (;;) {
		c=lRamRead(s++);
		if (c=='0') have_0=1;
		else if (c=='-') have_fu=1;
		else if (c>'0' && c<='9') {
			total_wei=c-'0';
			for (;;) {
				c=lRamRead(s++);
				if (c>='0' && c<='9') {
					total_wei=total_wei*10+c-'0';
				} else {
					s--;
					break;
				}
			}
		} else {
			s--;
			break;
		}
	}
	*digit=total_wei;
	if (total_wei) {
		if (have_fu) *flag=0x80;
		else if (have_0) *flag=0x40;
	} else *flag=0;
	return s;
}

void c_printf()
{
	a32 fmt_str,str2;
	byte c;
	int digit,flag;

	get_val();
	eval_top-=(byte)(a1<<2);
	func_x=eval_top;
	if (RamBits>16) fmt_str=*(long *)(lRam+eval_stack+eval_top)&0xffffff;
	else fmt_str=*(word *)(lRam+eval_stack+eval_top);
	for (;;) {
		c=lRamRead(fmt_str++);
		if (c==0) break;
		if (c=='%') {
			fmt_str=fenxi_fmt(fmt_str,&digit,&flag);
			c=lRamRead(fmt_str++);
			if (c==0) break;
			if (c=='d') printint(digit,flag);
			else if (c=='f') printfloat();
			else if (c=='%') cout(c);
			else if (c=='c') {
				next_arg();
				cout((byte)a1);
			} else if (c=='s') {
				next_arg();
				if (RamBits>16) str2=a1&0xffffff;
				else str2=(word)a1;
				for (;;) {
					c=lRamRead(str2++);
					if (c==0) break;
					if (c<128) cout(c);
					else {
						if (scr_x+1>=curr_CPR) cout(0x20);
						cout(c);
						c=lRamRead(str2++);
						if (c==0) break;
						cout(c);
					}
				}
			} else cout(c);
		} else if (c<128) {cout(c);
		} else {
			if (scr_x+1>=curr_CPR) cout(0x20);
			cout(c);
			c=lRamRead(fmt_str++);
			if (c==0) break;
			cout(c);
		}
	}
	update_lcd_0();
}

void c_strcpy()
{
	a32 t1,t2;
	int i;
	get_val2();
	get_val();
	if (RamBits>16) {
		t1=a1&0xffffff;
		t2=a3&0xffffff;
	} else {
		t1=(word)a1;
		t2=(word)a3;
	}
	for (i=0;;i++) if (lRamRead(t2+i)==0) break;
	i++;
	lmemcpy(t1,t2,i,2,1);
}

void c_strlen()
{
	a32 t;
	get_val();
	if (RamBits>16) t=a1&0xffffff;
	else t=(word)a1;
	for (a1=0;;a1++) if (lRamRead(t+a1)==0) break;
	put_val();
}

void setscreen(int mode)
{
	if (mode) {
		scr_mode=1;
		curr_CPR=((ScreenWidth-2)/6)&0xfe;
		curr_RPS=(ScreenHeight-1)/13;
		small_left=(ScreenWidth-curr_CPR*6)/2;
		small_up=(ScreenHeight-(curr_RPS*13-1))/2;
		small_down=ScreenHeight-(curr_RPS*13-1)-small_up;
	} else {
		scr_mode=0;
		curr_CPR=ScreenWidth/8;
		curr_RPS=ScreenHeight/16;
	}
	scr_x=0;scr_y=0;scr_off=0;
	memset(lRam+TextBuffer,0,curr_CPR*curr_RPS);
}

void c_setscreen()
{
	get_val();
	setscreen(a1&0xff);
}

void c_updatelcd()
{
	get_val();
	line_mode=a1;
	update_lcd_0();
	line_mode=0;
}

void c_writeblock()
{
	a32 data;
	word x,y,width,height;
	get_val();
	if (RamBits>16) data=a1&0xffffff;
	else data=(word)a1;
	get_val();
	no_buf=a1&0x40;
	negative=a1&0x20;
	lcmd=a1&0xf;
	get_val();
	height=(word)a1;
	get_val();
	width=(word)a1;
	get_val();
	y=(word)a1;
	get_val();
	x=(word)a1;
	if (width && height) {
		if (graph_mode==1)
			CheckAddr(data,data+((width+7)/8)*height-1,4);
		else
			CheckAddr(data,data+((width+1)/2)*height-1,4);
	}
	write_comm(x,y,width,height,lRam+data);
}

void scroll_to_lcd()
{
	memcpy(BmpData,BmpData+SCROLL_CON,LCD_WIDTH*LCD_HEIGHT);
}

void c_textout()
{
	byte t,c;
	word font_x,font_y;
	a32 addr;
	get_val();
	no_buf=a1&0x40;
	negative=a1&0x20;
	lcmd=a1&0xf;
	t=(byte)a1;
	get_val();
	if (RamBits>16) addr=a1&0xffffff;
	else addr=(word)a1;
	get_val();
	font_y=(word)a1;
	get_val();
	font_x=(word)a1;
	if ((t&0x80)==0) {
		for (;;) {
			if (font_x>=ScreenWidth) break;
			c=lRamRead(addr++);
			if (c==0) break;
			if (c<128) {
				font_6x12(font_x,font_y,c);
				font_x+=6;
			} else {
				t=lRamRead(addr++);
				font_12x12(font_x,font_y,c,t);
				font_x+=12;
			}
		}
	} else {
		for (;;) {
			if (font_x>=ScreenWidth) break;
			c=lRamRead(addr++);
			if (c==0) break;
			if (c<128) {
				font_8x16(font_x,font_y,c);
				font_x+=8;
			} else {
				t=lRamRead(addr++);
				font_16x16(font_x,font_y,c,t);
				font_x+=16;
			}
		}
	}
}

void c_block()
{
	block_comm();
	block_draw();
}

void c_rectangle()
{
	block_comm();
	square_draw();
}

void c_clearscreen()
{
	if (graph_mode==1)
		memset(BmpData+SCROLL_CON,0,LCD_WIDTH*LCD_HEIGHT);
	else {
		memset(BmpData+SCROLL_CON,bgcolor,LCD_WIDTH*LCD_HEIGHT);
	}
}

void c_abs()
{
	get_val();
	if (a1<0) a1=0-a1;
	put_val();
}

void c_rand()
{
	a1=seed*0x15a4e35+1;
	seed=a1;
	a1=(a1>>16)&0x7fff;
	put_val();
}

void c_srand()
{
	get_val();
	seed=a1;
}

void c_locate()
{
	byte t;
	get_val();
	t=(byte)a1;
	if (t>=curr_CPR) t=curr_CPR-1;
	scr_x=t;
	get_val();
	t=(byte)a1;
	if (t>=curr_RPS) t=curr_RPS-1;
	scr_y=t;
	scr_off=scr_y*curr_CPR+scr_x;
}

void c_point()
{
	get_val();
	no_buf=(a1&0x40)^0x40;
	lcmd=a1&3;
	get_val();
	yy=(word)a1;
	get_val();
	xx=(word)a1;
	put_dot();
}

void c_getpoint()
{
	get_val();
	yy=(word)a1;
	get_val();
	xx=(word)a1;
	no_buf=0x40;
	a1=get_dot();
	put_val();
}

void c_line()
{
	word delta_x,delta_y,distance,tt,xerr,yerr;
	word t;
	int incy;
	block_comm();
	no_buf^=0x40;
	if (x0==x1) {
		vline();
		return;
	}
	if (Y0==Y1) {
		if (hline_check())
			hline();
		return;
	}
	if (x1<x0) {
		t=x0;
		x0=x1;
		x1=t;
		t=Y0;
		Y0=Y1;
		Y1=t;
	}
	delta_x=x1-x0;
	delta_y=abs(Y1-Y0);
	if (Y1>Y0) incy=1;
	else incy=-1;
	distance=(delta_x>delta_y)?delta_x:delta_y;
	tt=0;
	xerr=0;
	yerr=0;
	xx=x0;
	yy=Y0;
	for (;;) {
		put_dot();
		xerr+=delta_x;
		yerr+=delta_y;
		if (xerr>=distance) {
			xerr-=distance;
			xx++;
		}
		if (yerr>=distance) {
			yerr-=distance;
			yy+=incy;
		}
		tt++;
		if (distance<tt) break;
	}
}

void c_box()
{
	byte t;
	get_val();
	lcmd=a1&3;
	no_buf=0x40;
	get_val();
	t=(byte)a1;
	get_val();
	Y1=(word)a1;
	get_val();
	x1=(word)a1;
	get_val();
	Y0=(word)a1;
	get_val();
	x0=(word)a1;
	if (t) block_draw();
	else square_draw();
}

void XorLine(int s,int e)
{
	x0=0;x1=ScreenWidth-1;
	Y0=s;Y1=e;
	lcmd=2;no_buf=0x40;
	block_draw();
}

void c_circle()
{
	word mode,r,x,y;
	get_val();
	no_buf=(a1&0x40)^0x40;
	lcmd=a1&3;
	get_val();
	mode=(word)a1;
	get_val();
	r=(word)a1;
	get_val();
	y=(word)a1;
	get_val();
	x=(word)a1;
	Ellipsex(x,y,r,r,mode);
}

void c_ellipse()
{
	word mode,ra,rb,x,y;
	get_val();
	no_buf=(a1&0x40)^0x40;
	lcmd=a1&3;
	get_val();
	mode=(word)a1;
	get_val();
	rb=(word)a1;
	get_val();
	ra=(word)a1;
	get_val();
	y=(word)a1;
	get_val();
	x=(word)a1;
	Ellipsex(x,y,ra,rb,mode);
}

void c_beep()
{
	;
}

void c_isalnum()
{
	get_val();
	a1=isalnum(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isalpha()
{
	get_val();
	a1=isalpha(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_iscntrl()
{
	get_val();
	a1=iscntrl(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isdigit()
{
	get_val();
	a1=isdigit(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isgraph()
{
	get_val();
	a1=isgraph(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_islower()
{
	get_val();
	a1=islower(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isprint()
{
	get_val();
	a1=isprint(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_ispunct()
{
	get_val();
	a1=ispunct(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isspace()
{
	get_val();
	a1=isspace(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isupper()
{
	get_val();
	a1=isupper(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_isxdigit()
{
	get_val();
	a1=isxdigit(a1);
	if (a1) a1=LTRUE;
	put_val();
}

void c_strcat()
{
	a32 t1,t2;
	byte c;
	get_val2();
	get_val();
	if (RamBits>16) {
		t1=a1&0xffffff;
		t2=a3&0xffffff;
	} else {
		t1=(word)a1;
		t2=(word)a3;
	}
	for (;;) {
		if (lRamRead(t1)==0) break;
		t1++;
	}
	for (;;) {
		c=lRamRead(t2++);
		lRamWrite(t1++,c);
		if (c==0) break;
	}
}

void c_strchr()
{
	byte *p;
	a32 t1;
	byte t2;
	get_val2();
	get_val();
	if (RamBits>16) t1=a1&0xffffff;
	else t1=(word)a1;
	t2=(byte)a3;
	CheckAddr(t1,t1+strlen(lRam+t1),0);
	p=strchr(lRam+t1,t2);
	if (p==NULL) a1=0;
	else a1=p-lRam;
	put_val();
}

void c_strcmp()
{
	a32 t1,t2;
	int t;
	get_val2();
	get_val();
	if (RamBits>16) {
		t1=a1&0xffffff;
		t2=a3&0xffffff;
	} else {
		t1=(word)a1;
		t2=(word)a3;
	}
	CheckAddr(t1,t1+strlen(lRam+t1),1);
	CheckAddr(t2,t2+strlen(lRam+t2),1);
	t=strcmp(lRam+t1,lRam+t2);
	if (t==0) a1=0;
	else if (t>0) a1=1;
	else a1=-1;
	put_val();
}

void c_strstr()
{
	byte *p;
	a32 t1,t2;
	get_val2();
	get_val();
	if (RamBits>16) {
		t1=a1&0xffffff;
		t2=a3&0xffffff;
	} else {
		t1=(word)a1;
		t2=(word)a3;
	}
	CheckAddr(t1,t1+strlen(lRam+t1),0);
	CheckAddr(t2,t2+strlen(lRam+t2),1);
	p=strstr(lRam+t1,lRam+t2);
	if (p==NULL) a1=0;
	else a1=p-lRam;
	put_val();
}

void c_tolower()
{
	byte c;
	get_val();
	c=(byte)a1;
	a1=tolower(c);
	put_val();
}

void c_toupper()
{
	byte c;
	get_val();
	c=(byte)a1;
	a1=toupper(c);
	put_val();
}

void c_memset()
{
	a32 len;
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val2();
	get_val();
	if (RamBits>16) a1&=0xffffff;
	else a1&=0xffff;
	if (len==0) return;
	CheckAddr(a1,a1+len-1,6);
	memset(lRam+a1,(word)a3,len);
}

void c_memcpy()
{
	a32 len;
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val2();
	get_val();
	if (RamBits>16) {
		a1&=0xffffff;
		a3&=0xffffff;
	} else {
		a1&=0xffff;
		a3&=0xffff;
	}
	if (len==0) return;
	lmemcpy(a1,a3,len,6,7);
}

void c_sprintf()
{
	a32 str1,fmt_str,str2;
	byte c;
	char num[20];
	int i,digit,flag,real_digit;

	get_val();
	eval_top-=(byte)(a1<<2);
	func_x=eval_top;
	if (RamBits>16) str1=*(long *)(lRam+eval_stack+eval_top)&0xffffff;
	else str1=*(word *)(lRam+eval_stack+eval_top);
	next_arg();
	if (RamBits>16) fmt_str=a1&0xffffff;
	else fmt_str=(word)a1;
	for (;;) {
		c=lRamRead(fmt_str++);
		if (c==0) break;
		if (c=='%') {
			fmt_str=fenxi_fmt(fmt_str,&digit,&flag);
			c=lRamRead(fmt_str++);
			if (c==0) break;
			if (c=='d') {
				next_arg();
				sprintf(num,"%ld",a1);
				real_digit=strlen(num);
				if (digit==0 || real_digit>=digit) {
					for (i=0;;i++) {
						if (num[i]) lRamWrite(str1++,num[i]);
						else break;
					}
				} else {
					if (flag==0x80) {
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,' ');
						}
					} else if (flag==0x40) {
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,'0');
						}
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
					} else {
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,' ');
						}
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
					}
				}
			} else if (c=='f') {
				int flag;
				next_arg();
				if (((a1>>23)&0xff)==0xff) {
					strcpy(num,"error");
				} else
					sprintf(num,"%g",i2f(a1));
				flag=0;
				for (i=0;;i++) {
					if (num[i]=='e' && i) flag=1;
					if (flag) flag++;
					if (flag==4) continue; //转换x.xxxxxe+0yy为x.xxxxxe+yy
					if (num[i]) lRamWrite(str1++,num[i]);
					else break;
				}
			} else if (c=='%') lRamWrite(str1++,c);
			else if (c=='c') {
				next_arg();
				lRamWrite(str1++,(byte)a1);
			} else if (c=='s') {
				next_arg();
				if (RamBits>16) str2=a1&0xffffff;
				else str2=(word)a1;
				for (;;) {
					c=lRamRead(str2++);
					if (c==0) break;
					lRamWrite(str1++,c);
				}
			} else lRamWrite(str1++,c);
		} else {
			lRamWrite(str1++,c);
		}
	}
	lRamWrite(str1,0);
}

void c_memmove()
{
	a32 len;
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val2();
	get_val();
	if (RamBits>16) {
		a1&=0xffffff;
		a3&=0xffffff;
	} else {
		a1&=0xffff;
		a3&=0xffff;
	}
	if (len==0) return;
	CheckAddr(a1,a1+len-1,6);
	CheckAddr(a3,a3+len-1,7);
	memmove(lRam+a1,lRam+a3,len);
}

void c_crc16()
{
	a32 t1,t2;
	byte c1,c2,x;
	get_val2();
	get_val();
	if (RamBits>16) {
		t1=a1&0xffffff;
		t2=a3&0xffffff;
	} else {
		t1=(word)a1;
		t2=(word)a3;
	}
	c1=0;c2=0;
	while (t2--) {
		x=c2^lRamRead(t1++);
		c2=crc2[x]^c1;
		c1=crc1[x];
	}
	a1=c1+(c2<<8);
	put_val();
}

void c_jiami()
{
	a32 str1,str2,len;
	int i;
	byte c;
	get_val();
	if (RamBits>16) str2=a1&0xffffff;
	else str2=(word)a1;
	get_val2();
	get_val();
	if (RamBits>16) str1=a1&0xffffff;
	else str1=(word)a1;
	if (RamBits>16) len=a3&0xffffff;
	else len=(word)a3;
	i=0;
	while (len--) {
		c=lRamRead(str2+i++);
		if (c==0) {
			c=lRamRead(str2);
			i=1;
		}
		CheckAddr(str1,str1,0);
		lRam[str1]^=c;
		str1++;
	}
}

void c_xdraw()
{
	byte t;
	byte tb[320];
	int i,j;
	get_val();
	t=(byte)a1;
	if (graph_mode==1) {
		if (t==0) {
			j=0;
			for (i=0;i<ScreenHeight;i++) {
				memcpy(BmpData+SCROLL_CON+j,BmpData+SCROLL_CON+j+1,ScreenWidth-1);
				BmpData[SCROLL_CON+j+ScreenWidth-1]=0;
				j+=LCD_WIDTH;
			}
		} else if (t==1) {
			j=0;
			for (i=0;i<ScreenHeight;i++) {
				memmove(BmpData+SCROLL_CON+j+1,BmpData+SCROLL_CON+j,ScreenWidth-1);
				BmpData[SCROLL_CON+j]=0;
				j+=LCD_WIDTH;
			}
		} else if (t==2) {
			memcpy(BmpData+SCROLL_CON,BmpData+SCROLL_CON+LCD_WIDTH,LCD_WIDTH*(ScreenHeight-1));
			memset(BmpData+SCROLL_CON+LCD_WIDTH*(ScreenHeight-1),0,LCD_WIDTH);
		} else if (t==3) {
			memmove(BmpData+SCROLL_CON+LCD_WIDTH,BmpData+SCROLL_CON,LCD_WIDTH*(ScreenHeight-1));
			memset(BmpData+SCROLL_CON,0,LCD_WIDTH);
		} else if (t==4) {
			for (i=0;i<ScreenHeight;i++)
				for (j=0;j<ScreenWidth/2;j++) {
					t=BmpData[SCROLL_CON+i*LCD_WIDTH+j];
					BmpData[SCROLL_CON+i*LCD_WIDTH+j]=BmpData[SCROLL_CON+i*LCD_WIDTH+ScreenWidth-1-j];
					BmpData[SCROLL_CON+i*LCD_WIDTH+ScreenWidth-1-j]=t;
				}
		} else if (t==5) {
			for (i=0;i<ScreenHeight/2;i++) {
				memcpy(tb,BmpData+SCROLL_CON+i*LCD_WIDTH,ScreenWidth);
				memcpy(BmpData+SCROLL_CON+i*LCD_WIDTH,BmpData+SCROLL_CON+(ScreenHeight-1-i)*LCD_WIDTH,ScreenWidth);
				memcpy(BmpData+SCROLL_CON+(ScreenHeight-1-i)*LCD_WIDTH,tb,ScreenWidth);
			}
		} else if (t==6) {
			memcpy(BmpData+SCROLL_CON,BmpData,LCD_WIDTH*LCD_HEIGHT);
		}
	} else {
		if (t==0) {
			j=0;
			for (i=0;i<ScreenHeight;i++) {
				memcpy(BmpData+SCROLL_CON+j,BmpData+SCROLL_CON+j+1,ScreenWidth-1);
				BmpData[SCROLL_CON+j+ScreenWidth-1]=(byte)bgcolor;
				j+=LCD_WIDTH;
			}
		} else if (t==1) {
			j=0;
			for (i=0;i<ScreenHeight;i++) {
				memmove(BmpData+SCROLL_CON+j+1,BmpData+SCROLL_CON+j,ScreenWidth-1);
				BmpData[SCROLL_CON+j]=(byte)bgcolor;
				j+=LCD_WIDTH;
			}
		} else if (t==2) {
			memcpy(BmpData+SCROLL_CON,BmpData+SCROLL_CON+LCD_WIDTH,LCD_WIDTH*(ScreenHeight-1));
			memset(BmpData+SCROLL_CON+LCD_WIDTH*(ScreenHeight-1),bgcolor,LCD_WIDTH);
		} else if (t==3) {
			memmove(BmpData+SCROLL_CON+LCD_WIDTH,BmpData+SCROLL_CON,LCD_WIDTH*(ScreenHeight-1));
			memset(BmpData+SCROLL_CON,bgcolor,LCD_WIDTH);
		} else if (t==4) {
			for (i=0;i<ScreenHeight;i++)
				for (j=0;j<ScreenWidth/2;j++) {
					t=BmpData[SCROLL_CON+i*LCD_WIDTH+j];
					BmpData[SCROLL_CON+i*LCD_WIDTH+j]=BmpData[SCROLL_CON+i*LCD_WIDTH+ScreenWidth-1-j];
					BmpData[SCROLL_CON+i*LCD_WIDTH+ScreenWidth-1-j]=t;
				}
		} else if (t==5) {
			for (i=0;i<ScreenHeight/2;i++) {
				memcpy(tb,BmpData+SCROLL_CON+i*LCD_WIDTH,ScreenWidth);
				memcpy(BmpData+SCROLL_CON+i*LCD_WIDTH,BmpData+SCROLL_CON+(ScreenHeight-1-i)*LCD_WIDTH,ScreenWidth);
				memcpy(BmpData+SCROLL_CON+(ScreenHeight-1-i)*LCD_WIDTH,tb,ScreenWidth);
			}
		} else if (t==6) {
			memcpy(BmpData+SCROLL_CON,BmpData,LCD_WIDTH*LCD_HEIGHT);
		}
	}
}

void c_getblock()
{
	word width,height;
	int i,j;
	byte t;

	get_val2();
	get_val();
	no_buf=a1&0x40;
	get_val();
	height=(word)a1;
	get_val();
	if (graph_mode==1)
		width=(word)a1>>3;
	else if (graph_mode==4)
		width=(word)(a1&0xfff8)>>1; //使宽度是8的整数倍
	else
		width=(word)a1;
	get_val();
	yy=(word)a1;
	get_val();
	if (graph_mode==8) xx=(word)a1;
	else xx=(word)(a1&0xfff8); //使xx是8的整数倍
	if (width==0 || height==0) return; //容错
	ByteAddr();
	if (RamBits>16) a3&=0xffffff;
	else a3&=0xffff;
	if (graph_mode==1) {
		for (i=0;i<height;i++) {
			for (j=0;j<width;j++) {
				t=0;
				if (BmpData[m1l++]) t|=0x80;
				if (BmpData[m1l++]) t|=0x40;
				if (BmpData[m1l++]) t|=0x20;
				if (BmpData[m1l++]) t|=0x10;
				if (BmpData[m1l++]) t|=0x8;
				if (BmpData[m1l++]) t|=0x4;
				if (BmpData[m1l++]) t|=0x2;
				if (BmpData[m1l++]) t|=0x1;
				lRamWrite(a3++,t);
			}
			m1l+=LCD_WIDTH-width*8;
			if (RamBits>16) a3&=0xffffff;
			else a3&=0xffff;
		}
	} else if (graph_mode==4) {
		for (i=0;i<height;i++) {
			for (j=0;j<width;j++) {
				t=(BmpData[m1l++]&0xf)<<4;
				t|=BmpData[m1l++]&0xf;
				lRamWrite(a3++,t);
			}
			m1l+=LCD_WIDTH-width*2;
			if (RamBits>16) a3&=0xffffff;
			else a3&=0xffff;
		}
	} else {
		for (i=0;i<height;i++) {
			for (j=0;j<width;j++) {
				lRamWrite(a3++,BmpData[m1l++]);
			}
			m1l+=LCD_WIDTH-width;
			if (RamBits>16) a3&=0xffffff;
			else a3&=0xffff;
		}
	}
}

void c_sin()
{
	get_val();
	a1=(a1&0xffff)%360;
	if (a1<90) a1=sin90[a1];
	else if (a1<180) a1=sin90[180-a1];
	else if (a1<270) a1=0-sin90[a1-180];
	else a1=0-sin90[360-a1];
	put_val();
}

void c_cos()
{
	get_val();
	a1=(a1&0xffff)%360;
	if (a1>=270) a1-=270;
	else a1+=90;
	if (a1<90) a1=sin90[a1];
	else if (a1<180) a1=sin90[180-a1];
	else if (a1<270) a1=0-sin90[a1-180];
	else a1=0-sin90[360-a1];
	put_val();
}

void c_fill()
{
	get_val();
	get_val();
	get_val();
}

void c_setgraphmode()
{
	int t;
	t=graph_mode;
	get_val();
	a1&=0xff;
	if (a1==1 || a1==4 || a1==8) {
		if (graph_mode!=(word)a1) {
			if (a1==4) {bgcolor=0;fgcolor=15;}
			else if (a1==8) {bgcolor=0;fgcolor=255;}
			if ((word)a1==1)
				memset(BmpData,0,LCD_WIDTH*LCD_HEIGHT);
			else
				memset(BmpData,bgcolor,LCD_WIDTH*LCD_HEIGHT);
			graph_mode=(word)a1;
			SetPalette();
		}
		a1=t;
	} else if (a1==0) a1=t;
	else a1=0;
	put_val();
}

void c_setbgcolor()
{
	get_val();
	if (graph_mode==8) bgcolor=a1&0xff;
	else bgcolor=a1&0xf;
}

void c_setfgcolor()
{
	get_val();
	if (graph_mode==8) fgcolor=a1&0xff;
	else fgcolor=a1&0xf;
}

void c_fade()
{
	int i;
	byte t,fa;
	byte *src,*obj;

	get_val();
	if (graph_mode==1) return;
	fa=(a1&0xf)^0xf;
	src=(byte *)BmpData+SCROLL_CON;
	obj=(byte *)BmpData;
	i=LCD_WIDTH*ScreenHeight;
	while (i) {
		t=*src++;
		*obj++=(t<fa)?fa:t;
		i--;
	}
}

void c_math()
{
	float t;

	get_val();
	switch (a1) {
	case 0: //GetVertion
		a1=0x100;
		break;
	case 7: //sin
		get_val();
		memcpy(&t,&a1,4);
		t=(float)sin(t);
		memcpy(&a1,&t,4);
		break;
	case 8: //cos
		get_val();
		memcpy(&t,&a1,4);
		t=(float)cos(t);
		memcpy(&a1,&t,4);
		break;
	case 9: //tan
		get_val();
		memcpy(&t,&a1,4);
		t=(float)tan(t);
		memcpy(&a1,&t,4);
		break;
	case 10: //asin
		get_val();
		memcpy(&t,&a1,4);
		t=(float)asin(t);
		memcpy(&a1,&t,4);
		break;
	case 11: //acos
		get_val();
		memcpy(&t,&a1,4);
		t=(float)acos(t);
		memcpy(&a1,&t,4);
		break;
	case 12: //atan
		get_val();
		memcpy(&t,&a1,4);
		t=(float)atan(t);
		memcpy(&a1,&t,4);
		break;
	case 13: //sqrt
		get_val();
		memcpy(&t,&a1,4);
		t=(float)sqrt(t);
		memcpy(&a1,&t,4);
		break;
	case 14: //exp
		get_val();
		memcpy(&t,&a1,4);
		t=(float)exp(t);
		memcpy(&a1,&t,4);
		break;
	case 15: //log
		get_val();
		memcpy(&t,&a1,4);
		t=(float)log(t);
		memcpy(&a1,&t,4);
		break;
	case 19: //abs
		get_val();
		memcpy(&t,&a1,4);
		if (t<0) t=-t;
		memcpy(&a1,&t,4);
		break;
	default:
		a1=0;
	}
	put_val();
}

//////////////////////////////

void c_getchar()
{
	wait_key=1;
	if (have_pen) pen_x=-1;
}

void c_inkey()
{
	if (lav_key<128) a1=0;
	else {
		a1=lav_key&0x7f;
		lav_key&=0x7f;
	}
	put_val();
}

int c_keyid(byte k)
{
	static byte direct_key[]={38,40,39,37};
	if (k>='a' && k<='z') k&=0xdf;
	else if (k>=0x1c && k<=0x1f) k=k-0x1c+112; //F1-F4
	else if (k>=20 && k<=23) k=direct_key[k-20];
	else if (k==19) k=33; //PageUp
	else if (k==14) k=34; //PageDown
	else if (k=='.') k=190;
	else if (k=='0' || k==' ' || k==13 || k==27) ;
	else if (k==26) k=16; //Shift
	else if (k==25) k=116; //F5作为求助
	else if (k==18) k=117; //F6作为输入法
	else k=0;
	return k;
}

byte c_keyval(byte k)
{
	static byte direct_key[]={23,20,22,21};
	static byte num_tbl[]="bnmghjtyu";
	if (k>='A' && k<='Z') k|=0x20;
	else if (k>=112 && k<=115) k=k-112+0x1c; //F1-F4
	else if (k>=37 && k<=40) k=direct_key[k-37];
	else if (k==33) k=19; //PageUp
	else if (k==34) k=14; //PageDown
	else if (k==190) k='.';
	else if (k=='0' || k==' ' || k==13 || k==27) ;
	else if (k==16) k=26; //Shift
	else if (k>='1' && k<='9') k=num_tbl[k-'1'];
	else if (k==116) k=25; //F5作为求助
	else if (k==117) k=18; //F6作为输入法
	else k=0;
	return k;
}

void c_checkkey()
{
	byte k;
	int i;
	get_val();
	k=(byte)a1;
	a1=LFALSE;
	if (k<128) {
		k=c_keyid(k);
		if (k && (cur_keyb[k]&0x80)) a1=LTRUE;
	} else {
		for (i=0;i<sizeof(cur_keyb);i++) {
			if (cur_keyb[i]&0x80) {
				k=c_keyval((byte)i);
				if (k) {
					a1=k;
					break;
				}
			}
		}
	}
	put_val();
}

void c_releasekey()
{
	byte k,t;
	int i;
	get_val();
	k=(byte)a1;
	t=k;
	if (k<128) {
		k=c_keyid(k);
		if (k) {
			cur_keyb[k]&=0x7f;
			if ((t|0x80)==lav_key) lav_key=t;
		}
	} else {
		lav_key=0;
		for (i=0;i<sizeof(cur_keyb);i++) cur_keyb[i]&=0x7f;
	}
}

void c_getword()
{
	get_val();
	c_getchar();
}

void c_delay()
{
	get_val();
	delay=(a1&0x7fff)*256/1000;
	_asm rdtsc
	_asm mov timed,eax
}

void c_getms()
{
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	a1=stime.wMilliseconds*256/1000;
	if (a1>=256) a1=255;
	put_val();
}

void GetDirName(char *fname)
{	
	char *p;
	a32 str;
	int i,t;

	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	CheckAddr(str,str+strlen(lRam+str),1);
	strcpy(fname,lRam+str);
	for (i=0;;i++) {
		if (fname[i]=='/') fname[i]='\\';
		if (fname[i]==0) break;
	}
	if (fname[0]=='\\') {
		strcpy(FileName,FcPath);
		strcat(FileName,fname+1);
	} else {
		strcpy(FileName,CD);
		strcat(FileName,fname);
	}
	GetFullPathName(FileName,250,fname,&p);
	t=strlen(fname);
	if (fname[t-1]!='\\') {
		fname[t]='\\';
		fname[t+1]=0;
	}
}

void c_chdir()
{
	char fname[MAX_PATH];

	GetDirName(fname);
	if (strstr(fname,FcPath)==NULL) a1=LFALSE; //Lee 为pc安全,不准访问其它目录
	else if (SetCurrentDirectory(fname)) {
		a1=LTRUE;
		strcpy(CD,fname);
	} else a1=LFALSE;
	put_val();
}

void c_makedir()
{
	char fname[MAX_PATH];

	GetDirName(fname);
	if (strstr(fname,FcPath)==NULL) a1=LFALSE; //Lee 为pc安全,不准访问其它目录
	else if (CreateDirectory(fname,NULL)) {
		a1=LTRUE;
	} else a1=LFALSE;
	put_val();
}

void get_name()
{
	char fname[MAX_PATH];
	char *p;
	a32 str;
	int i;
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	CheckAddr(str,str+strlen(lRam+str),1);
	strcpy(fname,lRam+str);
	for (i=0;;i++) {
		if (fname[i]=='/') fname[i]='\\';
		if (fname[i]==0) break;
	}
	if (fname[0]=='\\') {
		strcpy(FileName,FcPath);
		strcat(FileName,fname+1);
	} else {
		strcpy(FileName,CD);
		strcat(FileName,fname);
	}
	GetFullPathName(FileName,250,fname,&p);
	if (strstr(fname,FcPath)==NULL) FileName[0]=0; //Lee 为pc安全,不准访问其它目录
	strcpy(VirtualFullName,fname+strlen(FcPath)-1);
	for (i=0;;i++) {
		if (VirtualFullName[i]=='\\') VirtualFullName[i]='/';
		if (VirtualFullName[i]==0) break;
	}
}

void c_getfilenum()
{
	get_name();
	if (FileName[0]==0) a1=-1;
	else {
		a1=GetFileAttributes(FileName);
		if (a1==0xffffffff) ;
		else if (a1&FILE_ATTRIBUTE_DIRECTORY) {
			if (FileName[strlen(FileName)-1]!='\\')
				strcat(FileName,"\\");
			a1=getfilenum(FileName);
		} else {
			a1=-1;
		}
	}
	put_val();
}

void sys_getfilenum_ex()
{
	a32 extbuf;

	get_val();
	if (RamBits==16) extbuf=(word)a1;
	else extbuf=a1&0xffffff;
	get_name();
	if (FileName[0]==0) a1=-1;
	else {
		a1=GetFileAttributes(FileName);
		if (a1==0xffffffff) ;
		else if (a1&FILE_ATTRIBUTE_DIRECTORY) {
			if (FileName[strlen(FileName)-1]!='\\')
				strcat(FileName,"\\");
			a1=getfilenum_ex(FileName,(char *)(lRam+extbuf));
		} else {
			a1=-1;
		}
	}
}

void c_findfile()
{
	word index,num;
	a32 namebuf;

	get_val();
	if (RamBits==16) namebuf=(word)a1;
	else namebuf=a1&0xffffff;
	get_val();
	num=(word)a1;
	get_val();
	index=(word)a1;
	a1=findfile(index,num,(char *)(lRam+namebuf),CD);
	put_val();
}

void sys_findfile_ex()
{
	word index,num,len;
	a32 namebuf,extbuf;

	get_val();
	if (RamBits==16) extbuf=(word)a1;
	else extbuf=a1&0xffffff;
	get_val();
	len=(word)a1;
	get_val();
	if (RamBits==16) namebuf=(word)a1;
	else namebuf=a1&0xffffff;
	get_val();
	num=(word)a1;
	get_val();
	index=(word)a1;
	a1=findfile_ex(index,num,(char *)(lRam+namebuf),CD,len,(char *)(lRam+extbuf));
}

byte judge_mode()
{
	a32 str;
	byte mode;
	int i;
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	CheckAddr(str,str+strlen(lRam+str),1);
	for (i=0;i<12;i++)
		if (strcmp(lRam+str,smode[i])==0) break;
	mode=i+1;
	if (mode>12) mode=0;
	return mode;
}

void c_fopen()
{
	byte mode,this_file;
	int i;
	HANDLE fp;
	mode=judge_mode();
	get_name();
	if (mode) mode--;
	else {
		a1=0;
		put_val();
		return;
	}
	for (i=0;i<3;i++) if (fhave[i]==0) break;
	if (i==3) {
		a1=0;
		put_val();
		return;
	}
	this_file=i;
	fmode[this_file]=mode;
	if (!FileName[0]) fp=INVALID_HANDLE_VALUE;
	else fp=CreateFile(FileName,modeAccess[mode],0,NULL,modeCreate[mode],FILE_ATTRIBUTE_NORMAL,NULL);
	if (fp==INVALID_HANDLE_VALUE) {
		a1=0;
		put_val();
		return;
	}
	fhave[this_file]=1;
	fsize[this_file]=SetFilePointer(fp,0,NULL,FILE_END);
	if (mode>=8) foffset[this_file]=fsize[this_file];
	else {
		foffset[this_file]=0;
		SetFilePointer(fp,0,NULL,FILE_BEGIN);
	}
	fhandle[this_file]=fp;
	a1=this_file|0x80;
	put_val();
}

int check_handle()
{
	byte t;
	get_val();
	t=(byte)a1;
	if (t>=0x80 && t<0x83) {
		t&=3;
		if (fhave[t])
			FileHandle=fhandle[t];
		else return 0;
	} else return 0;
	curr_fnum=t;
	return t+1;
}

void c_fclose()
{
	int t;
	t=check_handle();
	if (t) {
		CloseHandle(FileHandle);
		fhave[curr_fnum]=0;
	}
}

void c_fseek()
{
	byte mode;
	long p;
	get_val();
	mode=(byte)a1;
	get_val();
	p=a1;
	if (!check_handle() || mode>2) {
		a1=-1;
		put_val();
		return;
	}
	switch (mode) {
	case 0:
		break;
	case 1:
		p+=foffset[curr_fnum];
		break;
	case 2:
		p+=fsize[curr_fnum];
		break;
	}
	if (p>=0 && p<=(long)fsize[curr_fnum]) {
		a1=SetFilePointer(FileHandle,p,NULL,FILE_BEGIN);
		if (a1!=-1) foffset[curr_fnum]=a1;
	} else a1=-1;
	put_val();
}

void c_ftell()
{
	if (!check_handle()) {
		a1=-1;
		put_val();
		return;
	}
	a1=foffset[curr_fnum];
	put_val();
}

void c_fread()
{
	a32 len,str;
	if (!check_handle()) {
		eval_top-=12;
		a1=0;
		put_val();
		return;
	}
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val();
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	if (modeAccess[fmode[curr_fnum]]&GENERIC_READ) {
		if (foffset[curr_fnum]+len>fsize[curr_fnum])
			len=fsize[curr_fnum]-foffset[curr_fnum]; //读文件不超过文件尾
		if (len) {
			CheckAddr(str,str+len-1,6);
			if (!ReadFile(FileHandle,lRam+str,len,&a1,NULL)) a1=0;
			foffset[curr_fnum]+=a1;
		} else a1=0;
	} else a1=0;
	put_val();
}

void c_fwrite()
{
	a32 len,str;
	if (!check_handle()) {
		eval_top-=12;
		a1=0;
		put_val();
		return;
	}
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val();
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	if (modeAccess[fmode[curr_fnum]]&GENERIC_WRITE) {
		if (len) {
			CheckAddr(str,str+len-1,7);
			if (!WriteFile(FileHandle,lRam+str,len,&a1,NULL)) a1=0;
			foffset[curr_fnum]+=a1;
			if (foffset[curr_fnum]>fsize[curr_fnum])
			fsize[curr_fnum]=foffset[curr_fnum];
		} else a1=0;
	} else a1=0;
	put_val();
}

void c_feof()
{
	if (!check_handle()) {
		a1=-1;
		put_val();
		return;
	}
	if (foffset[curr_fnum]==fsize[curr_fnum]) a1=-1;
	else a1=0;
	put_val();
}

void c_rewind()
{
	if (check_handle()) {
		SetFilePointer(FileHandle,0,NULL,FILE_BEGIN);
		foffset[curr_fnum]=0;
	}
}

void c_getc()
{
	byte c;

	if (!check_handle()) {
		a1=-1;
		put_val();
		return;
	}
	if (foffset[curr_fnum]==fsize[curr_fnum]) a1=-1; //结束
	else if (modeAccess[fmode[curr_fnum]]&GENERIC_READ) {
		if (!ReadFile(FileHandle,&c,1,&a1,NULL)) a1=0;
		if (a1==1) {
			a1=c;
			foffset[curr_fnum]++;
		} else a1=-1;	
	} else a1=-1;
	put_val();
}

void c_putc()
{
	byte c;
	if (!check_handle()) {
		get_val();
		a1=-1;
		put_val();
		return;
	}
	get_val();
	c=(byte)a1;
	if (modeAccess[fmode[curr_fnum]]&GENERIC_WRITE) {
		if (!WriteFile(FileHandle,&c,1,&a1,NULL)) a1=0;
		if (a1==1) {
			a1=c;
			foffset[curr_fnum]++;
			if (foffset[curr_fnum]>fsize[curr_fnum])
				fsize[curr_fnum]=foffset[curr_fnum];
		} else a1=-1;	
	} else a1=-1;
	put_val();
}

void c_deletefile()
{
	get_name();
	if (FileName[0]==0) a1=LFALSE;
	else {
		if (GetFileAttributes(FileName)&FILE_ATTRIBUTE_DIRECTORY) {
			if (RemoveDirectory(FileName)) a1=LTRUE;
			else a1=LFALSE;
		} else {
			if (DeleteFile(FileName)) a1=LTRUE;
			else a1=LFALSE;
		}
	}
	put_val();
}

void c_filelist()
{
	get_val();
	CheckAddr(a1,a1+16,0);
	ListFileAddr=a1;
	setscreen(0);
	line_mode=0;
	SelFile(CD);
}

void c_setlist()
{
	get_vals();
	if (a1) {
		curr_file=(a1&0xffff)-1;
		first_file=a1>>16;
	}
	list_set=a3;
}

void c_gettime()
{
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	get_val();
	if (RamBits>16) a1&=0xffffff;
	else a1&=0xffff;
	CheckAddr(a1,a1+7,0);
	*(word *)(lRam+a1)=stime.wYear;
	lRam[a1+2]=(byte)stime.wMonth;
	lRam[a1+3]=(byte)stime.wDay;
	lRam[a1+4]=(byte)stime.wHour;
	lRam[a1+5]=(byte)stime.wMinute;
	lRam[a1+6]=(byte)stime.wSecond;
	lRam[a1+7]=(byte)stime.wDayOfWeek;
}

void c_settime()
{
	SYSTEMTIME stime;
	get_val();
	if (RamBits>16) a1&=0xffffff;
	else a1&=0xffff;
	CheckAddr(a1,a1+7,0);
	stime.wYear=*(word *)(lRam+a1);
	stime.wMonth=lRam[a1+2];
	stime.wDay=lRam[a1+3];
	stime.wHour=lRam[a1+4];
	stime.wMinute=lRam[a1+5];
	stime.wSecond=lRam[a1+6];
	stime.wDayOfWeek=0;
	SetLocalTime(&stime);
}

void c_exec()
{
	long set;

	get_val();
	set=a1;
	get_val2();
	get_name();
	a1=-1;
	if (!FileName[0] || (task_lev==0 && RamBits==16)) { //16位小RAM无法加载执行lav程序
		put_val();
		return;
	}
	if (task_lev && (task[task_lev].attrib&0x80)) {
		put_val();
		return;
	}
	if (TaskOpen(FileName)) {
		put_val();
		return;
	}
	task[task_lev+1].attrib=set;
	task[task_lev].local_bp=local_bp;
	task[task_lev].local_sp=local_sp;
	task[task_lev].eval_top=eval_top;
	task[task_lev].string_ptr=string_ptr;
	task[task_lev].lRam=lRam;
	task[task_lev].lPC=lPC;
	task[task_lev].curr_file=curr_file;
	task[task_lev].first_file=first_file;
	task[task_lev].list_set=list_set;
	task[task_lev].RamBits=RamBits;
	task[task_lev].graph_mode=graph_mode;
	task[task_lev].bgcolor=bgcolor;
	task[task_lev].fgcolor=fgcolor;
	Save_Palette();
	task[task_lev].ScreenWidth=ScreenWidth;
	task[task_lev].ScreenHeight=ScreenHeight;
	task[task_lev].secret=secret;
	strcpy(task[task_lev].CD,CD);
	//strcpy(lRam+local_sp+CMDLINE,lRam+a3);
	strcpy(lRam+local_sp+CMDLINE,VirtualFullName);
	strcat(lRam+local_sp+CMDLINE," ");
	strcat(lRam+local_sp+CMDLINE,lRam+a3);
	wait_key=3;
}

void c_getcmdline()
{
	get_val();
	if (RamBits>16) a1&=0xffffff;
	else a1&=0xffff;
	CheckAddr(a1,a1+255,0);
	strcpy(lRam+a1,lRam+CMDLINE);
}

void FlmDecode()
{
	a32 src,obj,end_src;
	int len,type,i;
	byte t,t2;

	get_vals();
	if (RamBits>16) src=a1&0xffffff;
	else src=(word)a1;
	if (RamBits>16) obj=a3&0xffffff;
	else obj=(word)a3;
	len=lRamRead2(src)&0xffff;
	type=(len>>13)&7;
	len&=0x1fff;
	end_src=src+len;
	src+=2;
	if (type==0) lmemcpy(obj,src,len-2,6,7);
	else if (type==1) {
		for (;;) {
			if (src>=end_src) break;
			len=lRamRead(src++);
			if (len<64) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++) lRamWrite(obj++,0);
			} else if (len<128) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++) lRamWrite(obj++,0xff);
			} else if (len<192) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++)
					lRamWrite(obj++,lRamRead(src++));
			} else {
				len&=0x3f;
				if (!len) len=64;
				t=lRamRead(src++);
				for (i=0;i<len;i++)
					lRamWrite(obj++,t);
			}
		}
	} else if (type==2) {
		for (;;) {
			if (src>=end_src) break;
			len=lRamRead(src++);
			if (len<64) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++) obj++;
			} else if (len<128) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++) {
					t2=lRamRead(obj)+0xff;
					lRamWrite(obj++,t2);
				}
			} else if (len<192) {
				len&=0x3f;
				if (!len) len=64;
				for (i=0;i<len;i++) {
					t2=lRamRead(obj)+lRamRead(src++);
					lRamWrite(obj++,t2);
				}
			} else {
				len&=0x3f;
				if (!len) len=64;
				t=lRamRead(src++);
				for (i=0;i<len;i++) {
					t2=t+lRamRead(obj);
					lRamWrite(obj++,t2);
				}
			}
		}
	}
}

void PY2GB()
{
	a32 src,obj;

	get_vals();
	if (RamBits>16) src=a1&0xffffff;
	else src=(word)a1;
	if (RamBits>16) obj=a3&0xffffff;
	else obj=(word)a3;
	get_val();
	a1=GetGBCodeByPY(a1,lRam+src,lRam+obj);
}

void WriteTime(FILETIME *timesrc,a32 str)
{
	WORD fatdate,fattime;
	FILETIME time;

	FileTimeToLocalFileTime(timesrc,&time);
	FileTimeToDosDateTime(&time,&fatdate,&fattime);
	lRamWrite2(str,(word)((fatdate>>9)+1980));
	lRamWrite(str+2,(byte)((fatdate>>5)&0xf));
	lRamWrite(str+3,(byte)((fatdate)&0x1f));
	lRamWrite(str+4,(byte)((fattime>>11)));
	lRamWrite(str+5,(byte)((fattime>>5)&0x3f));
	lRamWrite(str+6,(byte)((fattime&0x1f)*2));
	lRamWrite(str+7,0);
}

void sys_GetFileAttributes()
{
	WIN32_FILE_ATTRIBUTE_DATA fa;
	a32 str;

	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=a1&0xffff;
	get_name();
	if (FileName[0]==0) {
		a1=0;
		return;
	}
	if (GetFileAttributesEx(FileName,GetFileExInfoStandard,&fa)==0) {
		a1=0;
		return;
	}
	lRamWrite(str++,(byte)(fa.dwFileAttributes&0x3f));
	lRamWrite4(str,fa.nFileSizeLow);
	str+=4;
	WriteTime(&fa.ftLastWriteTime,str);
	str+=8;
	WriteTime(&fa.ftCreationTime,str);
	str+=8;
	WriteTime(&fa.ftLastAccessTime,str);
	a1=1;
}

void c_system()
{
	get_val();
	switch (a1) {
	case 0: //GetPID
		//a1=('L'<<24)+1;
		a1=0;
		break;
	case 1: //SetBrightness
		get_val();
		break;
	case 2: //GetBrightness
		a1=0;
		break;
	case 3: //ComOpen;
	case 4: //ComClose;
	case 5: //ComWaitReady
		break;
	case 6: //ComSetTimer
		get_val();
		break;
	case 7: //ComGetc
		break;
	case 8: //ComPutc
		get_val();
		break;
	case 9: //ComRead
	case 10: //ComWrite
	case 11: //ComXor
		get_vals();
		break;
	case 12: //RamRead
		get_val();
		get_vals();
		break;
	case 13: //DiskReclaim
	case 14: //DiskCheck
		break;
	case 15: //FlmDecode
		FlmDecode();
		break;
	case 20: //PY2GB
		PY2GB();
		break;
	case 29: //FindFileEx
		sys_findfile_ex();
		break;
	case 30:
		sys_getfilenum_ex();
		break;
	case 31: //GetTickCount
		a1=(long)(GetTickCount()*0.256);
		break;
	case 32: //PeekMessage
		get_vals();
		if (RamBits>16) a1&=0xffffff;
		else a1&=0xffff;
		CheckAddr(a1,a1+9,0);
		if (hardinput_rp!=hardinput_wp) {
			*(word *)(lRam+a1)=hardinput[hardinput_rp].type;
			*(a32 *)(lRam+a1+2)=hardinput[hardinput_rp].wParam;
			*(a32 *)(lRam+a1+6)=hardinput[hardinput_rp].lParam;
			if (a3) hardinput_rp++;
			a1=-1;
		} else a1=0;
		break;
	case 33: //GetFileAttributes
		sys_GetFileAttributes();
		break;
	default:
		a1=0;
	}
	put_val();
}

void c_setpalette()
{
	a32 addr;

	get_val();
	if (RamBits>16) addr=a1&0xffffff;
	else addr=a1&0xffff;
	get_vals();
	a1&=0xff;
	a3&=0x7fff;
	if (a1+a3>256) a3=256-a1;
	lav_setpalette((byte)a1,a3,lRam+addr);
	a1=a3;
	put_val();
}

CODES codes2[]={
	c_putchar,c_getchar,c_printf,c_strcpy,c_strlen,c_setscreen,c_updatelcd,
	c_delay,c_writeblock,scroll_to_lcd,c_textout,c_block,c_rectangle,
	c_exit,c_clearscreen,c_abs,c_rand,c_srand,c_locate,c_inkey,c_point,
	c_getpoint,c_line,c_box,c_circle,c_ellipse,c_beep,c_isalnum,c_isalpha,
	c_iscntrl,c_isdigit,c_isgraph,c_islower,c_isprint,c_ispunct,c_isspace,
	c_isupper,c_isxdigit,c_strcat,c_strchr,c_strcmp,c_strstr,c_tolower,
	c_toupper,c_memset,c_memcpy,c_fopen,c_fclose,c_fread,c_fwrite,
	c_fseek,c_ftell,c_feof,c_rewind,c_getc,c_putc,c_sprintf,c_makedir,
	c_deletefile,c_getms,c_checkkey,c_memmove,c_crc16,c_jiami,c_chdir,
	c_filelist,c_gettime,c_settime,c_getword,c_xdraw,c_releasekey,c_getblock,
	c_sin,c_cos,c_fill,c_setgraphmode,c_setbgcolor,c_setfgcolor,
	c_setlist,c_fade,c_exec,c_findfile,c_getfilenum,c_system,c_math,
	c_setpalette,c_getcmdline
};

void lavRun()
{
	int i,t,xx;
	CODES code;
	
	xx=1000;
xxx:
	if (wait_key==1) {
		if (lav_key&0x80) {
			wait_key=0;
			lav_key&=0x7f;
			a1=lav_key;
			put_val();
		} else if (have_pen && pen_x>=0) {
			wait_key=0;
			a1=(pen_x<<8)|(pen_y<<16)|0xff000000;
			put_val();
		}
		return;
	} else if (wait_key==2) {
		if (lav_key&0x80) {
			lav_key&=0x7f;
			if (lav_key==0x1b) {
				lRam[ListFileAddr]=0;
				a1=LFALSE;
				put_val();
				wait_key=0;
			} else if (lav_key==0xd) {
				get_file_name(CD,curr_file,lRam+ListFileAddr);
				a1=(curr_file+1)+(first_file<<16);
				put_val();
				wait_key=0;
			} else if (lav_key==29 && (list_set&0x40)) { //F2(删除)有效
				get_file_name(CD,curr_file,lRam+ListFileAddr);
				a1=-1;
				put_val();
				wait_key=0;
			} else if (lav_key==25 && (list_set&0x20)) { //HELP有效
				get_file_name(CD,curr_file,lRam+ListFileAddr);
				a1=-2;
				put_val();
				wait_key=0;
			} else
				file_list_key(CD,lav_key);
		}
		return;
	} else if (wait_key==3) {
		t=0;
		for (i=0;i<255;i++) t|=cur_keyb[i]&0x80;
		if (!t) {
			task_lev++;
			lRam+=local_sp;
			lavReset();
			wait_key=0;
		}
		return; 
	}
	t=getcode();
	if (t&0x80) {
		code=codes2[t&0x7f];
		(*code)();
	} else {
		code=codes[t];
		(*code)();
		if (xx--) goto xxx;
	}
}