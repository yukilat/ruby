/*�ڴ�غ궨��*/
#ifndef __MEMPOOLDEF_H
#define __MEMPOOLDEF_H
#include <malloc.h>

/**
  *�������ڴ�ط���ģʽѡ��
  *1���̶�MemPool��С����MemPool�Ĺ���ϵͳΪArea�Զ�����Block
  *2���̶�MemPool��С�����Ҹ�������Area������Block������MemPool�Ĺ���ϵͳ�ᾡ��ΪArea���䵽����Block����
  *3����Area����MemPool�Ĵ�С����Ҫ�ֶ�Ԥ��������Area������MemPool�����뵽��ϵͳ�ڴ�
  */
#define __MEMPOOL_MODE 3	//Ŀǰֻʵ����ģʽ 3

/*MP�ڴ���Ϣ�еĵ�ַ��Ϣ��ʾ����*/
#define __MP_SHOW_ADDR 0 //0���أ�1����

/*����Ϣ�еĵ�ַ��Ϣ��ʾ����*/
#define __AREA_SHOW_ADDR 1 //0���أ�1����

typedef enum
{
	MP_OK = 0,
	MP_ERR = 1,
}MP_STATE;

#ifdef NULL
#define MP_NULL NULL
#else
#define MP_NULL 0
#endif

#if _WIN64
/* 64λϵͳ */
typedef unsigned long long MP_uintx_t;
#else
/* 32λϵͳ */
typedef unsigned int MP_uintx_t;
#endif

#endif // !__MEMPOOLDEF_H

