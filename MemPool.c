#include "MemPool.h"
#include <stdio.h>
static MemPoolTypedef *MemPool;
MemPoolTypedef MP;//MP�洢�ڴ����Ϣ��Ϊ�ڴ�صĿ�����Ĭ�ϲ��洢�ڴ���Ϣ�еĵ�ַ��Ϣ����ֹ�û�ͨ����ַ�޸��ڴ�����ݣ�������MemPoolDef.h�ļ���ͨ���޸ĺ���ʾ��ַ

/*static--------------------------------------------------BEGIN*/
static MP_uintx_t MP_readPoolSysSize(MemPoolTypedef* pMemPool);//���ڴ��ϵͳ��С
static MP_uintx_t MP_readPoolBlockSize(MemPoolTypedef* pMemPool);//���ڴ���ܵĿ��С
static AreaListTypedef* MP_createAreaList(void);//����������
static MP_uintx_t MP_readAreaCount(AreaListTypedef*);//���ذ�����������
static MP_STATE MP_pushAreaList(AreaListTypedef*, MP_uintx_t, MP_uintx_t);//���������в���һ�������ڵ�
static MP_STATE MP_initBlockStack(MemPoolTypedef* pMemPool);//��ʼ����ջ
static MP_STATE MP_copyMemPoolToMP(MemPoolTypedef* pMemPool, MemPoolTypedef* MP);//����pMemPool��MP
static MP_STATE MP_pushBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t Addr, MP_uintx_t key);//����ջ
static char MP_emptyBlockStack(BlockStackTypedef* pBlockStack);//�жϿ�ջ�Ƿ�Ϊ��
static void* MP_popBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t lock);//���ջ
static void MP_sortAreaList(AreaListTypedef* pNode);//��������
/*static--------------------------------------------------END*/

/**
  * @brief  �����ڴ�أ�__MEMPOOL_MODE = 3ʱ��PoolSize��AddrBase����Ч�ģ������ɣ�
  * @param  AddrTop		�ڴ�ػ���ַ
  * @param	PoolSize	�ڴ�ش�С
  * @retval MP_STATE	�ڴ��״̬
  */
MP_STATE MP_setMemPool(MP_uintx_t AddrBase, MP_uintx_t PoolSize)
{	
#if (__MEMPOOL_MODE == 3)
	if (MemPool == MP_NULL) {
		MemPool = (MemPoolTypedef*)malloc(sizeof(MemPoolTypedef));
		if (MemPool == MP_NULL)return MP_ERR;
	}
	MemPool->Addr_Base = (MP_uintx_t)MP_NULL;
	MemPool->PoolSys_Size = 0;
	MemPool->Pool_Size = 0;
	MemPool->Used_Size = 0;
	MemPool->pAreaList = MP_NULL;
	MemPool->ID = 1;
#endif
	return MP_OK;
}

/**
  * @brief  ����ڴ�ط���
  * @param  BlockSize	��Ĵ�С(4��������)
  * @param	TotalCount	���ܵ�����
  * @retval MP_STATE	�ڴ��״̬
  */
MP_STATE MP_addArea(MP_uintx_t BlockSize, MP_uintx_t TotalCount)
{
	if (BlockSize <= 0 || TotalCount <= 0)return MP_ERR;
	if ((BlockSize % 4) != 0)BlockSize = BlockSize + (4 - (BlockSize % 4));
#if (__MEMPOOL_MODE == 3)
	if (MemPool->pAreaList == MP_NULL) {
		MemPool->pAreaList = MP_createAreaList();
		if (MemPool->pAreaList == MP_NULL)return MP_ERR;
	}
	if (MP_pushAreaList(MemPool->pAreaList, BlockSize, TotalCount) == MP_ERR)return MP_ERR;
#endif
	return MP_OK;
}

/**
  * @brief  ��ʼ���ڴ��
  * @param  ��
  * @retval MP_STATE	�ڴ��״̬
  */
MP_STATE MP_initMemPool(void)
{
	MP_uintx_t BlockTotalSize = 0;//�ܵĿ�������ռ�Ĵ�С
	MP_uintx_t BlockTotalCount = 0;
#if (__MEMPOOL_MODE == 3)
	MemPool->PoolSys_Size = MP_readPoolSysSize(MemPool);
	BlockTotalSize = MP_readPoolBlockSize(MemPool);
	MemPool->Pool_Size = MemPool->PoolSys_Size + BlockTotalSize;
	if (BlockTotalSize <= 0) {
		free(MemPool);
		return MP_ERR;
	}
	if (MemPool->pAreaList != MP_NULL) {
		for (AreaListTypedef* Area = MemPool->pAreaList->pAreaNext; Area != NULL; Area = Area->pAreaNext) {
			BlockTotalCount += Area->pArea->Total_Count;			
		}
	}
	MemPool->Used_Size = MemPool->PoolSys_Size;
	/*�����ڴ�*/
	MemPool->Addr_Base = (MP_uintx_t)malloc((BlockTotalSize+ (sizeof(BlockTypedef) * BlockTotalCount)));
#endif
	MP_sortAreaList(MemPool->pAreaList);
	if (MP_initBlockStack(MemPool) == MP_ERR)return MP_ERR;
	MP_copyMemPoolToMP(MemPool, &MP);
	return MP_OK;
}

/**
  * @brief  ����pMemPool��MP
  * @param  pMemPool
  * @param  MP
  * @retval �ڴ��״̬
  */
static MP_STATE MP_copyMemPoolToMP(MemPoolTypedef* pMemPool, MemPoolTypedef* MP)
{
	if (pMemPool == MP_NULL)return MP_ERR;
	if (MP == MP_NULL)return MP_ERR;
	MP->ID = pMemPool->ID;
	MP->Pool_Size = pMemPool->Pool_Size;
#if __MP_SHOW_ADDR
	MP->Addr_Base = pMemPool->Addr_Base;
	MP->pAreaList = pMemPool->pAreaList;
#else
	MP->Addr_Base = (MP_uintx_t)MP_NULL;
	MP->pAreaList = MP_NULL;
#endif	
	MP->PoolSys_Size = pMemPool->PoolSys_Size;
	MP->Used_Size = pMemPool->Used_Size;
	
	return MP_OK;
}

/**
  * @brief  ���ڴ��ϵͳ��С
  * @param  pMemPool	�ڴ�ؿ��ƿ��ַ
  * @retval �ڴ��ϵͳ��С
  */
static MP_uintx_t MP_readPoolSysSize(MemPoolTypedef* pMemPool)
{	
	MP_uintx_t PoolSYS_Size = 0;//�ع���ϵͳ��С
	MP_uintx_t AreaCount = 0;//������
	MP_uintx_t BlockTotalCount = 0;//�������ܵĿ�����
	MP_uintx_t BlockTotalSize = 0;//�ܵĿ�������ռ�Ĵ�С
	AreaCount = MP_readAreaCount(pMemPool->pAreaList);//������
	if (pMemPool->pAreaList != MP_NULL) {
		for (AreaListTypedef* Area = pMemPool->pAreaList->pAreaNext; Area != NULL; Area = Area->pAreaNext) {
			BlockTotalCount += Area->pArea->Total_Count;
			BlockTotalSize += Area->pArea->Total_Count * Area->pArea->Block_Size;
		}
	}
	/*��������ع���ϵͳ��С*/
	PoolSYS_Size = sizeof(MemPoolTypedef) +//�ڴ�ؾ����С
		sizeof(AreaListTypedef) * (AreaCount + 1) +//�������ݴ�С
		sizeof(AreaTypedef) * AreaCount +//�������С
		sizeof(BlockStackTypedef) * AreaCount +//���Ŀ�ջ��С
		sizeof(BlockTypedef) * BlockTotalCount;//�������ܵĿ����ݴ�С
	return PoolSYS_Size;
}

/**
  * @brief  ���ڴ���ܵĿ��С
  * @param  pMemPool	�ڴ�ؿ��ƿ��ַ
  * @retval �ڴ���ܵĿ��С
  */
static MP_uintx_t MP_readPoolBlockSize(MemPoolTypedef* pMemPool)
{
	MP_uintx_t BlockTotalSize = 0;
	if (pMemPool->pAreaList != MP_NULL) {
		for (AreaListTypedef* Area = pMemPool->pAreaList->pAreaNext; Area != NULL; Area = Area->pAreaNext) {
			BlockTotalSize += Area->pArea->Total_Count * Area->pArea->Block_Size;
		}
	}
	return BlockTotalSize;
}

/**
  * @brief  �������ڴ�ռ������
  * @param  Continuous	�����ڴ�ռ������ƿ�
  * @param  size		�������Ŀռ��С
  * @retval �����ַ
  */
void* MP_Kmalloc(KMALLOCTypedef* Continuous, MP_uintx_t size)
{
	MP_uintx_t Addr = (MP_uintx_t)MP_NULL;
	if (Continuous->pAddr != MP_NULL) {
		if (Continuous->Size - Continuous->Used_Count >= size) {
			Addr = ((MP_uintx_t)Continuous->pAddr + Continuous->Used_Count);
			Continuous->Used_Count += size;
			return (void*)Addr;
		}
	}
	return MP_NULL;
}

/**
  * @brief  ��ʼ����ջ
  * @param  pMemPool �ڴ�ؿ��ƿ�
  * @retval �ڴ��״̬
  */
static MP_STATE MP_initBlockStack(MemPoolTypedef* pMemPool) 
{
	if (pMemPool == MP_NULL)return MP_ERR;
	if (pMemPool->pAreaList == MP_NULL)return MP_ERR;
	MP_uintx_t BlockTotalCount = 0;
	KMALLOCTypedef kmalloc;
#if (__MEMPOOL_MODE == 3)
	/*����������������ռ���׵�ַ��ռ���ڴ�ռ��С*/
	kmalloc.pAddr = (MP_uintx_t*)pMemPool->Addr_Base;
	for (AreaListTypedef* Area = MemPool->pAreaList->pAreaNext; Area != NULL; Area = Area->pAreaNext) {
		BlockTotalCount += Area->pArea->Total_Count;
	}
	kmalloc.Size = (pMemPool->Pool_Size - pMemPool->Used_Size) + (sizeof(BlockTypedef) * BlockTotalCount);
#endif
	kmalloc.Used_Count = 0;
	AreaListTypedef* pAreaList = pMemPool->pAreaList;
	BlockStackTypedef* pBlockStack = MP_NULL;
	BlockTypedef* pBlock = MP_NULL;
#if (__MEMPOOL_MODE == 3)
	for (pAreaList = pAreaList->pAreaNext; pAreaList != MP_NULL; pAreaList = pAreaList->pAreaNext) {
		pAreaList->pArea->pBlockStack = (BlockStackTypedef*)malloc(sizeof(BlockStackTypedef));
		if (pAreaList->pArea->pBlockStack == MP_NULL)return MP_ERR;
		pBlockStack = pAreaList->pArea->pBlockStack;
		pBlockStack->pTop = MP_NULL;
		pBlockStack->pBottom = MP_NULL;
		for (MP_uintx_t i = 0; i < pAreaList->pArea->Total_Count; i++) {
			/*������*/
			pBlock = (BlockTypedef*)MP_Kmalloc(&kmalloc, (pAreaList->pArea->Block_Size + sizeof(BlockTypedef)));//���ȡ����ĵ�ַ�ռ�
			/*�����ƿ��¼������ַ*/
			if (pAreaList->pArea->Addr_Base == (MP_uintx_t)MP_NULL) {
				pAreaList->pArea->Addr_Base = (MP_uintx_t)pBlock;
			}
			if (pBlock == MP_NULL)return MP_ERR;
			/*��������ջ*/
			pBlock->pBlockNext = pBlockStack->pTop;
			pBlockStack->pTop = pBlock;
		}
	}
#endif
	return MP_OK;
}

/**
  * @brief  ���ڴ�ط����ڴ�
  * @param  size �����ڴ��С
  * @retval �����ַ
  */
void* MP_malloc(MP_uintx_t size) 
{	
	/*��ȡ������ͷ�ڵ��ַ*/
	if (MemPool == MP_NULL)return MP_NULL;
	if (MemPool->pAreaList == MP_NULL)return MP_NULL;
	AreaListTypedef* pAreaList = MemPool->pAreaList;
	void* Addr = MP_NULL;
	if (size <= 0)return MP_NULL;
	size = size + (4 - (size % 4));
	for (pAreaList = pAreaList->pAreaNext; pAreaList != MP_NULL; pAreaList = pAreaList->pAreaNext) {
		if (pAreaList->pArea->Total_Count == pAreaList->pArea->Used_Count)continue;
		if (pAreaList->pArea->Block_Size >= size) {
			Addr = MP_popBlockStack(pAreaList->pArea->pBlockStack, 0xFFFFFFFF);
			if (Addr == MP_NULL)return MP_NULL;
			pAreaList->pArea->Used_Count++;
			MemPool->Used_Size += pAreaList->pArea->Block_Size;
			MP.Used_Size = MemPool->Used_Size;
			return Addr;
		}
	}
	return MP_NULL;
}

/**
  * @brief  ���ڴ���ͷ��ڴ�
  * @param  Addr �ͷ��ڴ��ַ
  * @retval �ڴ��״̬
  */
MP_STATE MP_free(void* Addr)
{
	MP_uintx_t tempAddr = (MP_uintx_t)Addr;
	if (MemPool == MP_NULL)return MP_ERR;
	if (MemPool->pAreaList == MP_NULL)return MP_ERR;
	AreaListTypedef* pAreaList = MemPool->pAreaList;
	for (pAreaList = pAreaList->pAreaNext; pAreaList != MP_NULL; pAreaList = pAreaList->pAreaNext) {
		if (pAreaList->pArea->Addr_Base <= tempAddr && tempAddr <= (pAreaList->pArea->Addr_Base + (pAreaList->pArea->Block_Size+sizeof(BlockTypedef)) * pAreaList->pArea->Total_Count)) {
			if (MP_pushBlockStack(pAreaList->pArea->pBlockStack, tempAddr, 0xFFFFFFFF) == MP_OK) {
				pAreaList->pArea->Used_Count--;
				MemPool->Used_Size -= pAreaList->pArea->Block_Size;
				MP.Used_Size = MemPool->Used_Size;
				return MP_OK;
			}
			else {
				return MP_ERR;
			}
		}
	}
	return MP_ERR;
}

/**
  * @brief	����ջ
  * @param	pBlockStack	��ջ��ַ
  * @param	Addr		��ջ��������Ŀռ��׵�ַ
  * @param  key			��Կ��(����ջ����Կ��ֵ=0xFFFFFFFF)
  * @retval	0�ǿգ�1��
  */
static MP_STATE MP_pushBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t Addr, MP_uintx_t key)
{	
	//��ȡ����ÿռ��׵�ַ�Ŀ��ַ
	BlockTypedef* pBlock = (BlockTypedef*)(Addr - sizeof(BlockTypedef));
	//�жϸÿ��Ƿ񱻳�ջ
	if ((MP_uintx_t)(pBlock->pBlockNext) != key) {
		return MP_ERR;		
	}
	pBlock->pBlockNext = pBlockStack->pTop;
	pBlockStack->pTop = pBlock;
	return MP_OK;
}

/**
  * @brief	�жϿ�ջ�Ƿ�Ϊ��
  * @param	pBlockStack	��ջ��ַ
  * @retval	0�ǿգ�1��
  */
static char MP_emptyBlockStack(BlockStackTypedef* pBlockStack)
{
	if (pBlockStack->pTop == pBlockStack->pBottom)return 1;		
	else return 0;		
}

/**
  * @brief	���ջ
  * @param	pBlockStack	��ջ��ַ
  * @param  lock		����(��ջ���ʶֵ=0xFFFFFFFF)
  * @retval	�鱣���ַ
  */
static void* MP_popBlockStack(BlockStackTypedef* pBlockStack,MP_uintx_t lock)
{
	//�ж�ջ�Ƿ�Ϊ��
	if (MP_emptyBlockStack(pBlockStack)) {
		return MP_NULL;
	}
	BlockTypedef* pBlock = pBlockStack->pTop;//pָ��ջ���ڵ�	
	pBlockStack->pTop= pBlockStack->pTop->pBlockNext;//ջ������һλ
	pBlock->pBlockNext = (BlockTypedef*)lock;//��ջ��ʶ
	return (void*)((MP_uintx_t)pBlock + (MP_uintx_t)sizeof(BlockTypedef));
}

/**
  * @brief  ����������
  * @param  ��
  * @retval ��ͷ����ַ
  */
static AreaListTypedef* MP_createAreaList(void)
{
#if (__MEMPOOL_MODE == 3)
	AreaListTypedef* pHead;
	pHead = (AreaListTypedef*)malloc(sizeof(AreaListTypedef));
	if (pHead == MP_NULL)return MP_NULL;
	pHead->pAreaNext = MP_NULL;
	pHead->pArea = MP_NULL;
#endif
	return pHead;
}

/**
  * @brief  ���ذ�����������
  * @param  *pNode ͷ�ڵ��ַ
  * @retval ������
  */
static MP_uintx_t MP_readAreaCount(AreaListTypedef* pNode)
{
	MP_uintx_t len = 0;
	if (pNode == MP_NULL)return 0;
	pNode = pNode->pAreaNext;
	while (pNode != NULL){
		len++;
		pNode = pNode->pAreaNext;
	}
	return len;
}

/**
  * @brief  ���������в���һ�������ڵ�
  * @param  *pNode		��ͷ�ڵ��ַ
  * @param  BlockSize	��Ĵ�С
  * @param	TotalCount	���ܵ�����
  * @retval MP_STATE	�ڴ��״̬
  */
static MP_STATE MP_pushAreaList(AreaListTypedef* pNode, MP_uintx_t BlockSize, MP_uintx_t TotalCount)
{
#if (__MEMPOOL_MODE == 3)
	AreaListTypedef* pNew;
	pNew = (AreaListTypedef*)malloc(sizeof(AreaListTypedef));
	if (pNew == MP_NULL)return MP_ERR;
	pNew->pAreaNext = pNode->pAreaNext;
	pNode->pAreaNext = pNew;
	/*���ֵ*/
	pNew->pArea = (AreaTypedef*)malloc(sizeof(AreaTypedef));
	if (pNew->pArea == MP_NULL)return MP_ERR;
	pNew->pArea->Block_Size = BlockSize;
	pNew->pArea->Total_Count = TotalCount;
	pNew->pArea->Addr_Base = 0;
	pNew->pArea->pBlockStack = MP_NULL;
	pNew->pArea->Used_Count = 0;
#endif
	return MP_OK;
}

/**
  * @brief  ��������
  * @param  *pHead ����ͷ�ڵ��ַ
  * @retval ��
  */
static void MP_sortAreaList(AreaListTypedef* pNode)
{
	MP_uintx_t i, j;
	MP_uintx_t len = 0;
	AreaListTypedef* p = MP_NULL, * q = MP_NULL;
	AreaTypedef temp;
	p = pNode->pAreaNext;
	while (p != NULL) {
		len++;
		p = p->pAreaNext;
	}
	p = pNode->pAreaNext;
	if (p != NULL) {
		for (i = 0, p = pNode->pAreaNext; i < len - 1; i++, p = p->pAreaNext) {
			if (p != NULL) {
				for (j = i + 1, q = p->pAreaNext; j < len; j++, q = q->pAreaNext) {
					if (p->pArea->Block_Size > q->pArea->Block_Size) {
						temp = *p->pArea;
						*p->pArea = *q->pArea;
						*q->pArea = temp;
					}
				}
			}
		}
	}
}

/**
  * @brief  ��������
  * @param  ��
  * @retval ������
  */
unsigned int MP_readAreaNumber(void)
{
	unsigned int len = 0;
	AreaListTypedef* pAreaList = MP_NULL;
	pAreaList = MemPool->pAreaList;
	while (pAreaList->pAreaNext!=MP_NULL) {
		pAreaList = pAreaList->pAreaNext;
		len++;
	}
	return len;
}

/**
  * @brief  ��ָ������Ϣ(��ջ��ַ��������ַʼ��Ϊ��)
  * @param  num ����
  * @retval ����Ϣ
  */
AreaTypedef MP_readAreaMsg(unsigned int num)
{
	unsigned int len = 0;
	AreaListTypedef* pAreaList = MP_NULL;
	AreaTypedef Area;
	Area.Addr_Base = (MP_uintx_t)MP_NULL;
	Area.pBlockStack = MP_NULL;
	Area.Block_Size = 0;
	Area.Total_Count = 0;
	Area.Used_Count = 0;
	pAreaList = MemPool->pAreaList;
	while (pAreaList->pAreaNext != MP_NULL) {
		pAreaList = pAreaList->pAreaNext;
		len++;
		if (len == num) {
#if __AREA_SHOW_ADDR
			Area.Addr_Base = pAreaList->pArea->Addr_Base;
			Area.pBlockStack = pAreaList->pArea->pBlockStack;
#endif
			Area.Block_Size = pAreaList->pArea->Block_Size;
			Area.Total_Count = pAreaList->pArea->Total_Count;
			Area.Used_Count = pAreaList->pArea->Used_Count;
			break;
		}
	}	
	return Area;
}

/**
  * @brief  ��������
  * @param  ��
  * @retval ������
  */
unsigned int MP_readBlockNumber(void)
{
	unsigned int len = 0;
	AreaListTypedef* pAreaList = MP_NULL;
	pAreaList = MemPool->pAreaList;
	while (pAreaList->pAreaNext != MP_NULL) {
		pAreaList = pAreaList->pAreaNext;
		len += pAreaList->pArea->Total_Count;
	}
	return len;
}

/*
@brief MP��ʼ��
@param ��
@retval MP_STATE ��ʼ��״̬
*/
MP_STATE MP_init(void)
{
	MP_STATE return_state= MP_setMemPool(0, 0);//�����ڴ�أ���Ϊʹ�õ���ģʽ3����ַ���С���ᱻ���ÿ������д
	if (return_state == MP_ERR) return return_state;
	for (MP_uintx_t i = 0; (8 * i + 8) <= 512; i++) {
		return_state=MP_addArea((8 + 8 * i), (4096 / (8 + 8 * i)));//������������������(4096 / (8 + 8 * i))��(8+ 8 * i)�ֽڷ����
	}
	
	if (return_state == MP_ERR) return return_state;
	return_state=MP_initMemPool();//��ʼ�ڴ��
	return return_state;

}