#include "define.h"

#define FILE_INFO WIN32_FIND_DATA
#define FIND_FIRST(name,info) FindFirstFile((name),(info))
#define FIND_NEXT(info) FindNextFile(find_handle,(info))
#define FILE_IS_DIR(info) ((info).dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#define FILE_NAME(info) ((info).cFileName)

#define UP_KEY 0x14
#define DOWN_KEY 0x15
#define UU_KEY 0x13
#define DD_KEY 0xe

#define ATTR_READONLY	1
#define ATTR_HIDE		2
#define ATTR_SYSTEM		4
#define ATTR_VOL		8
#define ATTR_DIR		16

extern void update_lcd_0();
extern void XorLine(int s,int e);
extern byte wait_key;
extern a32 TextBuffer;
extern byte curr_RPS,curr_CPR;

int total_file,first_file,curr_file;
int list_set;
char ext_name[16][16];
byte attr_mask,attr_val;
int ext_num;

int get_total_files(char *path)
{
	FILE_INFO file_info;
	HANDLE find_handle;
	char full_name[MAX_PATH];
	int total;

	total=1;
	strcpy(full_name,path);
	strcat(full_name,"*.*");
	find_handle=FIND_FIRST(full_name,&file_info);
	while (find_handle!=INVALID_HANDLE_VALUE) {
		if (strcmp(FILE_NAME(file_info),".")==0) ;
		else if (strcmp(FILE_NAME(file_info),"..")==0) ;
		else if (strlen(FILE_NAME(file_info))>14) ; //文件名超过14个字符
		else total++;
		if (FIND_NEXT(&file_info)==0) {
			FindClose(find_handle);
			break;
		}
	}
	return total;
}

void get_ext_name(char *name,char *ext)
{
	int i,k,len;
	char c;
	len=strlen(name);
	k=-1;
	for (i=len-1;i>=0;i--) {
		if (name[i]=='.') {
			k=i+1;
			break;
		}
	}
	if (k<0 || k>=len) {
		ext[0]=0;
		return;
	}
	for (i=0;i<8;i++) {
		c=name[k++];
		if (c==0) break;
		ext[i]=toupper(c);
	}
	ext[i]=0;
}

int if_ext(char *ext)
{
	int i;

	if (ext[0]) {
		for (i=0;i<ext_num;i++) {
			if (strcmp(ext,ext_name[i])==0) return 1;
		}
		return 0;
	} else {
		if (ext_name[0][0]) return 0;
		return 1;
	}
}

int if_ext_name(char *name)
{
	char ext[16];

	if (ext_num==0) return 1;
	get_ext_name(name,ext);
	return if_ext(ext);
}

int get_total_files_ex(char *path)
{
	FILE_INFO file_info;
	HANDLE find_handle;
	char full_name[MAX_PATH];
	int total;

	total=1;
	strcpy(full_name,path);
	strcat(full_name,"*.*");
	find_handle=FIND_FIRST(full_name,&file_info);
	while (find_handle!=INVALID_HANDLE_VALUE) {
		if (strcmp(FILE_NAME(file_info),".")==0) ;
		else if (strcmp(FILE_NAME(file_info),"..")==0) ;
		//else if (strlen(FILE_NAME(file_info))>14) ; //文件名超过14个字符
		else if (!if_ext_name(FILE_NAME(file_info))) ; //扩展名不符
		else if ((file_info.dwFileAttributes&attr_mask)!=attr_val) ; //文件属性不对
		else total++;
		if (FIND_NEXT(&file_info)==0) {
			FindClose(find_handle);
			break;
		}
	}
	return total;
}

void get_file_name(char *path,int index,char *name)
{
	FILE_INFO file_info;
	HANDLE find_handle;
	char full_name[MAX_PATH];
	int id;

	if (index==0) {
		strcpy(name,"..");
		return;
	}
	id=1;
	strcpy(full_name,path);
	strcat(full_name,"*.*");
	find_handle=FIND_FIRST(full_name,&file_info);
	while (find_handle!=INVALID_HANDLE_VALUE) {
		if (strcmp(FILE_NAME(file_info),".")==0) ;
		else if (strcmp(FILE_NAME(file_info),"..")==0) ;
		else if (strlen(FILE_NAME(file_info))>14) ; //文件名超过14个字符
		else {
			if (id==index) {
				strcpy(name,FILE_NAME(file_info));
				return;
			}
			id++;
		}
		if (FIND_NEXT(&file_info)==0) {
			FindClose(find_handle);
			break;
		}
	}
	name[0]=0;
}

void get_file_name_ex(char *path,int index,char *name,int len)
{
	FILE_INFO file_info;
	HANDLE find_handle;
	char full_name[MAX_PATH];
	int id;

	if (index==0) {
		strcpy(name,"..");
		return;
	}
	id=1;
	strcpy(full_name,path);
	strcat(full_name,"*.*");
	find_handle=FIND_FIRST(full_name,&file_info);
	while (find_handle!=INVALID_HANDLE_VALUE) {
		if (strcmp(FILE_NAME(file_info),".")==0) ;
		else if (strcmp(FILE_NAME(file_info),"..")==0) ;
		//else if (strlen(FILE_NAME(file_info))>14) ; //文件名超过14个字符
		else if (!if_ext_name(FILE_NAME(file_info))) ; //扩展名不符
		else if ((file_info.dwFileAttributes&attr_mask)!=attr_val) ; //文件属性不对
		else {
			if (id==index) {
				strncpy(name,FILE_NAME(file_info),len-1);
				return;
			}
			id++;
		}
		if (FIND_NEXT(&file_info)==0) {
			FindClose(find_handle);
			break;
		}
	}
	name[0]=0;
}

void disp_file(char *path)
{
	int i;
	char name[16];

	memset(lRam+TextBuffer,' ',curr_CPR*curr_RPS);
	for (i=0;i<curr_RPS;i++) {
		if (i+first_file<total_file) {
			get_file_name(path,i+first_file,name);
			if (name[0])
				memcpy(lRam+TextBuffer+i*curr_CPR,name,strlen(name));
		}
	}
	update_lcd_0();
	XorLine((curr_file-first_file)*16,(curr_file-first_file)*16+15);
}

void SelFile(char *path)
{

	total_file=get_total_files(path);
	if (!(list_set&0x80)) {
		first_file=0;
		curr_file=1;
	}
	if (curr_file<first_file) first_file=curr_file;
	if (curr_file>=first_file+curr_RPS)  first_file=curr_file-(curr_RPS-1);
	if (first_file>=total_file) first_file=total_file-1;
	if (curr_file>=total_file) curr_file=total_file-1;
	disp_file(path);
	wait_key=2;
}

void file_list_key(char *path,int key)
{
	switch (key) {
	case UP_KEY:
		if (curr_file) {
			curr_file--;
			if (curr_file<first_file) first_file=curr_file;
			disp_file(path);
		}
		break;
	case UU_KEY:
		if (first_file>=curr_RPS) {
			first_file-=curr_RPS;
			curr_file-=curr_RPS;
			disp_file(path);
		} else if (first_file) {
			curr_file-=first_file;
			first_file=0;
			disp_file(path);
		}
		break;
	case DOWN_KEY:
		if (curr_file+1<total_file) {
			curr_file++;
			if (curr_file>first_file+curr_RPS-1) first_file=curr_file-(curr_RPS-1);
			disp_file(path);
		}
		break;
	case DD_KEY:
		if (first_file+curr_RPS<total_file) {
			first_file+=curr_RPS;
			curr_file+=curr_RPS;
			if (curr_file>=total_file) curr_file=total_file-1;
			disp_file(path);
		}
		break;
	}
}

int findfile(int index,int num,char *namebuf,char *path)
{
	int i;

	for (i=0;i<num;i++) {
		get_file_name(path,i+index,namebuf+i*16);
		if (!(namebuf[i*16])) break;
	}
	return i;
}

void get_ext(char *ext)
{
	int i,j,k;
	char c;

	attr_mask=ATTR_SYSTEM|ATTR_HIDE;
	attr_val=0;
	for (;;) {
		c=*ext;
		if (c=='+') {
			c=*(++ext);
			switch (c) {
			case 'h':
				attr_mask&=~ATTR_HIDE;
				ext++;
				break;
			case 's':
				attr_mask&=~ATTR_SYSTEM;
				ext++;
				break;
			}
		} else if (c=='!') {
			c=*(++ext);
			switch (c) {
			case 'd':
				attr_mask|=ATTR_DIR;
				attr_val|=ATTR_DIR;
				ext++;
				break;
			case 'f':
				attr_mask|=ATTR_DIR;
				attr_val&=~ATTR_DIR;
				ext++;
				break;
			}
		} else break;
	}

	if (ext[0]=='*') {
		ext_num=0;
		return;
	}
	k=0;
	for (i=0;i<16;i++) {
		for (j=0;j<8;j++) {
			c=ext[k++];
			if (c==';' || c==0) break;
			ext_name[i][j]=toupper(c);
		}
		ext_name[i][j]=0;
		if (c==0) break;
	}
	ext_num=i+1;
}

int findfile_ex(int index,int num,char *namebuf,char *path,int namelen,char *ext)
{
	int i;

	get_ext(ext);
	for (i=0;i<num;i++) {
		get_file_name_ex(path,i+index,namebuf+i*namelen,namelen);
		if (!(namebuf[i*namelen])) break;
	}
	return i;
}

int getfilenum(char *path)
{
	return get_total_files(path)-1;
}

int getfilenum_ex(char *path,char *ext)
{
	get_ext(ext);
	return get_total_files_ex(path)-1;
}