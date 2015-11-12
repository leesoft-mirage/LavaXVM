#include "define.h"

enum { Z_STRING=100,Z_SECRET,Z_TEXT,Z_GRAPH,Z_PRESET,Z_GBUF};

byte code_len[256]={
	9,1,2,4,2,2,2,2,2,2,
	2,2,2,Z_STRING,2,2,2,2,2,2,
	2,2,2,2,2,2,Z_TEXT,Z_GRAPH,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,3,3,3,
	2,3,3,0,0,Z_PRESET,Z_GBUF,Z_SECRET,0,2,
	2,2,2,2,2,2,2,2,2,2,
	2,2,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,
	9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,

	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	9,0,0,0,0,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,

	9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9	
};

byte code2_len[256]={
	9,1,2,4,3,3,3,3,3,3,
	3,3,3,Z_STRING,3,3,3,3,3,3,
	3,3,3,3,3,3,Z_TEXT,Z_GRAPH,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,3,3,3,
	3,3,4,0,0,Z_PRESET,Z_GBUF,Z_SECRET,0,2,
	2,2,2,2,2,2,2,2,2,2,
	2,2,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,
	1,0,1,1,0,9,9,9,9,9,
	9,9,9,9,9,9,9,9,

	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,

	9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9	
};

void check()
{
	byte *pz,t,*p;
	char mess[1024];
	byte secret,text,graph,gbuf,v,error;
	int len;
	char s[100];
	
	secret=0;
	graph=0;
	gbuf=0;
	text=0;
	error=0;
	v=0;

	pz=pLAVA+16;
	if (*(pLAVA+8)&0x80) p=code2_len;
	else p=code_len;
	for (;;) {
		t=*pz++;
		if (t==0) break;
		if (t<=0x51) {
			if (v<0x10) v=0x10;
		} else if (t<=0x6d) {
			if (v<0x30) v=0x30;
		} else if (t<=0x70) {
			if (v<0x34) v=0x34;
		} else if (t<=0x72) {
			if (v<0x35) v=0x35;
		} else if (t<128) {
			FATAL("非release版，不予认证!");
			return;
		} else if (t<=128+74) {
			if (v<0x10) v=0x10;
		} else if (t==128+80) {
			if (v<0x31) v=0x31;
		} else if (t<=128+84) {
			if (v<0x30) v=0x30;
		} else if (t==128+85) {
			if (v<0x31) v=0x31;
		}
		if (p[t]==9) {
			if (*pz) error=1; //最后一个字节错不算错
			break;
		}
		switch (p[t]) {
		case Z_STRING:
			while ((*pz++)^secret) ;
			break;
		case Z_TEXT:
			text=1;
			break;
		case Z_GRAPH:
			graph=1;
			break;
		case Z_GBUF:
			gbuf=1;
			break;
		case Z_SECRET:
			secret=*pz++;
			break;
		case Z_PRESET:
			if (*(pLAVA+8)&0x80) pz+=3;
			else pz+=2;
			len=*pz+(*(pz+1)<<8);
			pz+=len+2;
			break;
		default:
			pz+=p[t];
		}
	}
	mess[0]=0;
	if (graph) strcat(mess,"\n含有非跨平台指令_GRAPH");
	if (gbuf) strcat(mess,"\n含有非跨平台指令_GBUF");
	if (text) strcat(mess,"\n含有非跨平台指令_TEXT");
	if (error) {
		sprintf(s,"\n含有错误指令或函数在位置%d",pz-1-pLAVA);
		strcat(mess,s);
	}
	if (*(pLAVA+8)&0x80) {
		if (v<0x31) v=0x31;
	}
	if (*(pLAVA+8)&0x10) {
		if (v<0x33) v=0x33;
	}
	if ((*(pLAVA+8)&0x60)==0x40) {
		if (v<0x30) v=0x30;
	}
	if ((*(pLAVA+8)&0x60)==0x60) {
		if (v<0x31) v=0x31;
	}
	if (mess[0]==0) {
		if (v<=0x10 && (*(pLAVA+8)&0x90)==0)
			strcat(mess,"\n全平台适用");
		else {
			sprintf(mess,"\nLavaX%d.%d",v/16,v&0xf);
		}
	}
	FATAL(mess);
}
