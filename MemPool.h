#ifndef __MEMPOOL_H
#define __MEMPOOL_H
#include "MemPoolDef.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct BLOCK//��
{
	struct BLOCK* pBlockNext;/*��һ����*/
}BlockTypedef;

typedef struct BLOCKSTACK
{
	BlockTypedef* pTop;/*ջ����*/
	BlockTypedef* pBottom;/*ջ�ײ�*/
}BlockStackTypedef;//�ɷ����ջ

typedef struct AREA//��
{
	MP_uintx_t Total_Count;	/*���� �ܴ洢�����*/
	MP_uintx_t Used_Count;	/*���� ��ʹ�õĴ洢�����*/
	MP_uintx_t Block_Size;	/*���� �洢���С�������ͣ�*/
	MP_uintx_t Addr_Base;		/*������ַ*/
	BlockStackTypedef* pBlockStack;/*�ɷ����ջ*/
}AreaTypedef;

typedef struct AREALIST//������
{
	AreaTypedef* pArea;			/*��*/
	struct AREALIST* pAreaNext;	/*��һ����*/
}AreaListTypedef;

typedef struct MEMPOOLT//�ڴ�� 
{
	MP_uintx_t ID;				/*��ID*/
	MP_uintx_t Pool_Size;		/*���ܵĴ�С*/
	MP_uintx_t Addr_Base;		/*�ع�����ڴ����ַ*/
	MP_uintx_t PoolSys_Size;	/*�ڴ�ع���ϵͳռ�ô�С*/
	MP_uintx_t Used_Size;		/*���ѱ�ʹ�õ��ڴ��С*/
	AreaListTypedef* pAreaList;	/*����*/
}MemPoolTypedef;

typedef struct KMALLOC//�����ڴ�ռ������ƿ�
{
	MP_uintx_t* pAddr;//�����ռ��׵�ַ(���ʼ����ַ�����佫���׵�ַ��ʼ)
	MP_uintx_t Size;//�����ռ��С(���ʼ����С)
	MP_uintx_t Used_Count;//��ʹ�ÿռ�(���ʼ��Ϊ0)
}KMALLOCTypedef;

extern MemPoolTypedef MP;//MP�洢�ڴ����Ϣ��Ϊ�ڴ�صĿ�����Ĭ�ϲ��洢�ڴ���Ϣ�еĵ�ַ��Ϣ����ֹ�û�ͨ����ַ�޸��ڴ�����ݣ�������MemPoolDef.h�ļ���ͨ���޸ĺ���ʾ��ַ

/*Mem Pool API----------------------------------------------------------BEGIN*/

MP_STATE MP_setMemPool(MP_uintx_t AddrTop, MP_uintx_t PoolSize);//�����ڴ��
MP_STATE MP_addArea(MP_uintx_t BlockSize, MP_uintx_t TotalCount);//����ڴ�ط���
MP_STATE MP_initMemPool(void);//��ʼ���ڴ��
void* MP_malloc(MP_uintx_t size);//���ڴ�ط����ڴ�
MP_STATE MP_free(void* Addr);//���ڴ���ͷ��ڴ�
void* MP_Kmalloc(KMALLOCTypedef* Continuous, MP_uintx_t size);//�����ڴ�ռ������
unsigned int MP_readAreaNumber(void);//��������
AreaTypedef MP_readAreaMsg(unsigned int num);//��ָ������Ϣ(��ջ��ַ��������ַʼ��Ϊ��)
unsigned int MP_readBlockNumber(void);//��������
MP_STATE MP_init(void);//�ڴ�س�ʼ��
/*Mem Pool API----------------------------------------------------------END*/





#ifdef __cplusplus
}
#endif




#endif // !__MEMPOOL_H


