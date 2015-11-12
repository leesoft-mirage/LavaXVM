#include <windows.h>
#include <stdio.h>
#include <math.h>

//#define TextBuffer		0xf000 //故意这样，以检验是否地址相关
//#define string_stack		0xf200
//#define patbuf			0xfe00
//#define eval_stack		0xff00

#define LCD_WIDTH	320
#define LCD_HEIGHT	240
#define	SCROLL_CON	LCD_WIDTH*(LCD_HEIGHT+8)

#define FATAL(info) MessageBox(hwnd_main,info,"LavaX Virtual Machine",MB_ICONWARNING)
#define WM_LAVASTAR 0xb800
#define WM_STOPLINE 0xb801
#define	WM_STAREND  0xb802
#define	WM_STARSTOP 0xb803

#define XM_LBUTTONDOWN	1
#define XM_LBUTTONUP	2
#define XM_MOUSEMOVE	3 

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long a32;

struct TASK
{
	int		attrib;
	char	CD[MAX_PATH];
	int		first_file;
	int		curr_file;
	int		list_set;
	int		RamBits;
	word	graph_mode;
	word	bgcolor;
	word	fgcolor;
	byte	palette[256*3];
	int		ScreenWidth;
	int		ScreenHeight;
	byte	secret;
	byte	*pLAVA;
	byte	*lRam;
	byte	*lPC;
	a32		local_sp;
	a32		local_bp;
	byte	eval_top;
	a32		string_ptr;
};

struct MESSAGE
{
	word type;
	a32 wParam;
	a32 lParam;
};

extern HWND hwnd_main;
extern HWND hwnd_ide;
extern HINSTANCE hInstan;
extern HMENU hMenu;
extern int EmuRunning,PauseFlag,RamError,stop_line,stop_func,func_top;
extern int sline[256],sline_num;
extern int TVSCAN,GRAY,ENDVERIFY,CHECKRAM;
extern int ScreenWidth,ScreenHeight,ScreenDouble; //可变的屏幕宽高
extern int TotalFrame;
extern byte SaveTemp[0x80];
extern char FcPath[MAX_PATH];
extern char BmpPath[MAX_PATH];
extern char CheatPath[MAX_PATH];
extern char *pRomName;
extern byte VRam[0x1000000];//[65536];
extern byte *lRam;
extern struct TASK task[16]; //任务栈
extern int task_lev; //任务级
extern byte *ascii;//[1536];
extern byte *ascii8;//[2048];
extern byte *gbfont;//[81*94*24];
extern byte *gbfont16;//[81*94*32];
extern byte *pinyin;
extern byte ScreenBuffer[LCD_WIDTH*(LCD_HEIGHT*2+16)];
extern byte ScreenBufferX[320*336];
extern byte ScreenKey[6400];
extern byte *BmpData;
extern word graph_mode;
extern byte *pLAVA,*pNextLAVA;
extern long delay;
extern unsigned long timed;
extern byte old_keyb[256],cur_keyb[256];
extern byte lav_key;
extern byte wait_key;
extern int have_pen,have_keypad;
extern int pen_x,pen_y;
extern byte hardinput_rp,hardinput_wp; //硬件输入消息队列的读写指针
extern struct MESSAGE hardinput[256]; //LavaX虚拟机的硬件输入消息队列

extern int LavOpen(FILE *fp);
extern int TaskOpen(char *name);
extern void SetWindow(int width,int height);
extern void good_exit();
extern void SetSBuffer();
extern void InitScreen();
extern void WriteScreen(HDC hdc,int flag);
extern void GetBmp(char *fname);
extern void SetPalette();
extern void lav_setpalette(byte from,int num,byte *addr);
extern void Save_Palette();
extern void Load_Palette();
extern void mesDrawTitle();
extern void mesDrawTime();
extern void lavReset();
extern void lavRun();
extern void verify();
extern void check();
extern void chtLoad();
extern void chtExe();
extern void SelFile(char *path);
extern void file_list_key(char *path,int key);
extern void get_file_name(char *path,int index,char *name);
extern int getfilenum(char *path);
extern int getfilenum_ex(char *path,char *ext);
extern int findfile(int index,int num,char *namebuf,char *path);
extern int findfile_ex(int index,int num,char *namebuf,char *path,int namelen,char *ext);
extern LRESULT CALLBACK ChtFind(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
extern LRESULT CALLBACK ChtEdit(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);