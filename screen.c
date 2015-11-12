#include "define.h"

int ScreenWidth=160,ScreenHeight=80; //可变的屏幕宽高
int ScreenDouble=1;

byte ScreenBuffer[LCD_WIDTH*(LCD_HEIGHT*2+16)];
byte ScreenBufferX[320*336];
byte ScreenBlack[320*8];
byte *BmpData=ScreenBuffer+LCD_WIDTH*8;

byte ScreenKey[6400];

typedef struct {
   BITMAPINFOHEADER bmiHeader; 
   RGBQUAD          bmiColors[256]; 
} BITMAPINFOX;

static BITMAPINFOX bmi,bmik,bmis;

static byte palette[256*3];

static byte syspalette[]={
	0x00,0x00,0x00,0xbf,0xbf,0xbf, //KEY调色
	0x00,0x00,0x00,0x00,0xff,0x00, //MESS调色
	0x00,0xc0,0x00,0x00,0x00,0x00, //黑绿2色
	0xff,0xff,0xff,0x00,0x00,0x00  //黑白2色
};

void calcPalette()
{
	int i;

	for (i=0;i<256;i++) {
		bmi.bmiColors[i].rgbBlue=palette[i*3];
		bmi.bmiColors[i].rgbGreen=palette[i*3+1];
		bmi.bmiColors[i].rgbRed=palette[i*3+2];
		bmi.bmiColors[i].rgbReserved=0;
	}
}

void Palette2()
{
	if (GRAY) //黑白变换
		memcpy(palette,syspalette+18,6);
	else
		memcpy(palette,syspalette+12,6);
	calcPalette();
}

void Palette16()
{
	int i;

	for (i=0;i<16;i++) {
		palette[i*3]=palette[i*3+1]=palette[i*3+2]=(15-i)*0x11;
	}
	calcPalette();
}

void Palette256()
{
	int i;
	byte lv9[]={0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xff};
	byte lv5[]={0,0x40,0x80,0xc0,0xff};

	palette[0]=palette[1]=palette[2]=0;
	palette[255*3]=palette[255*3+1]=palette[255*3+2]=255;
	for (i=0;i<225;i++) {
		palette[i*3+48]=lv5[(i/5)%5];//lv4[(i>>6)&3];
		palette[i*3+49]=lv9[i/25];//lv8[(i>>3)&7];
		palette[i*3+50]=lv5[i%5];//lv8[(i)&7];
	}
	calcPalette();
}

void SetPalette()
{
	palette[255*3]=palette[255*3+1]=palette[255*3+2]=0;
	if (graph_mode==4) Palette16();
	else if (graph_mode==8) Palette256();
	else Palette2();
}

void lav_setpalette(byte from,int num,byte *addr)
{
	int i;

	for (i=from;i<from+num;i++) {
		palette[i*3+2]=*addr++;
		palette[i*3+1]=*addr++;
		palette[i*3]=*addr++;
		addr++;
	}
	calcPalette();
}

void Load_Palette()
{
	memcpy(palette,task[task_lev].palette,256*3);
	calcPalette();
}

void Save_Palette()
{
	memcpy(task[task_lev].palette,palette,256*3);
}

void SetSBuffer()
{
	memset(BmpData,0,320*ScreenHeight);
	memset(ScreenBuffer,0,320*8);
	memset(ScreenBufferX,255,320*336); //清大屏
	memset(ScreenBlack,0,320*8);
}

void InitScreen()
{
	int i;

	ZeroMemory(&bmi.bmiHeader,sizeof(bmi.bmiHeader));
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=320;
	bmi.bmiHeader.biHeight=320;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=8;
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biClrUsed=256;

	ZeroMemory(&bmis.bmiHeader,sizeof(bmis.bmiHeader));
	bmis.bmiHeader.biSize=sizeof(bmis.bmiHeader);
	bmis.bmiHeader.biWidth=320;
	bmis.bmiHeader.biHeight=16;
	bmis.bmiHeader.biPlanes=1;
	bmis.bmiHeader.biBitCount=8;
	bmis.bmiHeader.biCompression=BI_RGB;
	bmis.bmiHeader.biClrUsed=16;

	ZeroMemory(&bmik.bmiHeader,sizeof(bmik.bmiHeader));
	bmik.bmiHeader.biSize=sizeof(bmik.bmiHeader);
	bmik.bmiHeader.biWidth=320;
	bmik.bmiHeader.biHeight=160;
	bmik.bmiHeader.biPlanes=1;
	bmik.bmiHeader.biBitCount=1;
	bmik.bmiHeader.biCompression=BI_RGB;
	bmik.bmiHeader.biClrUsed=2;

	for (i=0;i<2;i++) {
		bmik.bmiColors[i].rgbBlue=syspalette[i*3];
		bmik.bmiColors[i].rgbGreen=syspalette[i*3+1];
		bmik.bmiColors[i].rgbRed=syspalette[i*3+2];
		bmik.bmiColors[i].rgbReserved=0;
	}
	for (i=0;i<2;i++) {
		bmis.bmiColors[i].rgbBlue=syspalette[i*3+6];
		bmis.bmiColors[i].rgbGreen=syspalette[i*3+7];
		bmis.bmiColors[i].rgbRed=syspalette[i*3+8];
		bmis.bmiColors[i].rgbReserved=0;
	}

	SetSBuffer();
	SetPalette();
}

void WriteScreen(HDC hdc,int flag)
{
	int i,j;
	byte *p,*px,t;

	if (ScreenDouble) {
		p=ScreenBuffer;
		px=ScreenBufferX+(ScreenHeight+7)*640;
		for (i=0;i<ScreenHeight+8;i++) {
			for (j=0;j<ScreenWidth;j++) {
				*(px++)=*p;
				*(px++)=*(p++);
			}
			p+=320-ScreenWidth;
			px+=-640-ScreenWidth*2;
		}
		if (!TVSCAN) {
			px=ScreenBufferX;
			for (i=0;i<ScreenHeight+8;i++,px+=640)
				memcpy(px+320,px,320);
		} else {
			px=ScreenBufferX;
			for (i=0;i<ScreenHeight+8;i++,px+=640)
				memset(px+320,255,320);
		}
		if (flag) {
			SetDIBitsToDevice(hdc,0,(ScreenHeight+8)*2,320,8,0,0,0,8,ScreenBlack,(BITMAPINFO *)(&bmis),DIB_RGB_COLORS);
			SetDIBitsToDevice(hdc,0,(ScreenHeight+8)*2+8,320,160,0,0,0,160,ScreenKey,(BITMAPINFO *)(&bmik),DIB_RGB_COLORS);
		}
		SetDIBitsToDevice(hdc,0,0,320,16,0,0,0,16,ScreenBufferX+ScreenHeight*2*320,(BITMAPINFO *)(&bmis),DIB_RGB_COLORS);
		SetDIBitsToDevice(hdc,0,16,320,ScreenHeight*2,0,0,0,ScreenHeight*2,ScreenBufferX,(BITMAPINFO *)(&bmi),DIB_RGB_COLORS);
	} else {
		p=BmpData+(ScreenHeight-1)*320;
		px=ScreenBufferX;
		t=(320-ScreenWidth)/2;
		for (i=0;i<ScreenHeight;i++) {
			if (t) memset(px,255,t);
			px+=t;
			memcpy(px,p,ScreenWidth);
			px+=ScreenWidth;
			if (t) memset(px,255,t);
			px+=t;
			p-=320;
		}
		for (i=0;i<8;i++) {
			for (j=0;j<160;j++) {
				*(px++)=*p;
				*(px++)=*(p++);
			}
			p+=-480;
			memcpy(px,px-320,320);
			px+=320;
		}
		if (flag) {
			SetDIBitsToDevice(hdc,0,ScreenHeight+8*2,320,8,0,0,0,8,ScreenBlack,(BITMAPINFO *)(&bmis),DIB_RGB_COLORS);
			SetDIBitsToDevice(hdc,0,ScreenHeight+8*2+8,320,160,0,0,0,160,ScreenKey,(BITMAPINFO *)(&bmik),DIB_RGB_COLORS);
		}
		SetDIBitsToDevice(hdc,0,0,320,16,0,0,0,16,ScreenBufferX+ScreenHeight*320,(BITMAPINFO *)(&bmis),DIB_RGB_COLORS);
		SetDIBitsToDevice(hdc,0,16,320,ScreenHeight,0,0,0,ScreenHeight,ScreenBufferX,(BITMAPINFO *)(&bmi),DIB_RGB_COLORS);
	}
	//注:使用StretchDIBits比较SetDIBitsToDevice而言严重影响速度
}

BITMAPFILEHEADER bfh;
BITMAPINFOHEADER bih;

void GetBmp(char *fname)
{
	char tname[MAX_PATH];
	int i,len;
	FILE *fp;

	strcpy(tname,BmpPath);
	len=strlen(fname);
	for (i=len-1;i>=0;i--) {
		if (fname[i]=='.') fname[i]=0;
		else if (fname[i]=='\\' || fname[i]==':') {
			strcat(tname,fname+i+1);
			break;
		}
	}
	if (i<0) strcat(tname,"noname");
	len=strlen(tname);
	for (i=0;i<1000;i++) {
		tname[len]=0;
		if (i==0) sprintf(tname,"%s.bmp",tname);	
		else sprintf(tname,"%s%d.bmp",tname,i);
		fp=fopen(tname,"rb");
		if (fp==NULL) break;
		fclose(fp);
	}
	if (i>999) return;
	fp=fopen(tname,"wb");
	if (fp==NULL) return;
	memset(&bfh,0,sizeof(bfh));
	bfh.bfType=0x4d42;
	bfh.bfOffBits=sizeof(bfh)+sizeof(bih)+256*4;
	bfh.bfSize=bfh.bfOffBits+ScreenWidth*ScreenHeight;
	memset(&bih,0,sizeof(bih));
	bih.biBitCount=8;
	bih.biCompression=BI_RGB;
	bih.biHeight=ScreenHeight;
	bih.biPlanes=1;
	bih.biSize=sizeof(bih);
	bih.biWidth=ScreenWidth;
	fwrite(&bfh,1,sizeof(bfh),fp);
	fwrite(&bih,1,sizeof(bih),fp);
	for (i=0;i<256;i++) {
		putc(bmi.bmiColors[i].rgbBlue,fp);
		putc(bmi.bmiColors[i].rgbGreen,fp);
		putc(bmi.bmiColors[i].rgbRed,fp);
		putc(0,fp);
	}
	for (i=ScreenHeight-1;i>=0;i--)
		fwrite(BmpData+i*320,1,ScreenWidth,fp);
	fclose(fp);
}
