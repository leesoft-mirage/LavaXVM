#include "define.h"

#define	ONCEREAD	9				//N
#define MAX_SEQNO	416

#define	PY_TXT_OFS		0x3c9e
#define	PYHZ_IDX_OFS	0x395a
#define	PYHZ_DAT_OFS	0x0

byte *pinyin;

/* --------------------------
 * 折半查找输入串的序列号
 * 查不到的话返回0xffff
 * --------------------------*/
word get_seq_no(byte *InputBuffer)
{
	byte *ofs;
	int i;
	int top,mid,bot;
	byte ibuf[10],obuf[10];

	for( i = 0; i < 6; i++ ) {   //无论输入多少,取前6个字符
		ibuf[i] = *( InputBuffer + i );
		if( ibuf[i] == '\0')
			break;
		else
			ibuf[i] |= 0x20;			//format string all lowercase
	}
	ibuf[i] = '\0';

	top = 0;
	bot = MAX_SEQNO;
	while( top <= bot ) {
		mid = ( top + bot ) / 2;

		ofs = pinyin+ PY_TXT_OFS + 6 * mid;
		memcpy(obuf,ofs,6);

		i = strncmp( ibuf, obuf, 6 );
		if( i == 0 )
			return mid;
		else if( i > 0 ) 
			top = mid + 1;
		else
			bot = mid - 1;
	}
	return 0xffff;
}

unsigned long GetGBCodeByPY( unsigned int pos, byte *InputBuffer, byte *OutBuffer )
{
	word sum;			//gb 个数
	word gbidx[2];			//gb idx
	word seqno;
	byte *gbofs;		//gb 码值的sector and offset
	byte tmpidx[5];
	union {
		a32 value;
		word chr[2];
	} rtn;

	if( *InputBuffer == 0) {
		rtn.chr[0] = 0xffff;
		rtn.chr[1] = 0xffff;
		return rtn.value;
	}
	seqno = get_seq_no( InputBuffer );
	if( seqno == 0xffff ) {
		rtn.chr[0] = 0xffff;
		rtn.chr[1] = 0xffff;
		return rtn.value;
	}
	gbofs = pinyin + PYHZ_IDX_OFS + 2 * seqno;
	memcpy(tmpidx,gbofs,4);
	gbidx[0] = ( tmpidx[1] << 8 ) + tmpidx[0];
	gbidx[1] = ( tmpidx[3] << 8 ) + tmpidx[2];
	sum = ( gbidx[1] - gbidx[0] ) >> 1;
	if (pos>=sum) {
		rtn.chr[0] = 0xffff;
		rtn.chr[1] = 0xffff;
		return rtn.value;
	}
	gbofs = pinyin + PYHZ_DAT_OFS + gbidx[0] + (pos<<1);
	seqno = ( sum - pos >= ONCEREAD ) ? ONCEREAD : sum - pos;
	memcpy(OutBuffer,gbofs,2 * seqno);
	OutBuffer[2 * seqno]=0;
	rtn.chr[0] = seqno;
	rtn.chr[1] = sum;
 
	return rtn.value;
}