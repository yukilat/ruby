#include "MemPool.h"
#include <stdio.h>
static MemPoolTypedef *MemPool;
MemPoolTypedef MP;//MP存储内存池信息，为内存池的拷贝，默认不存储内存信息中的地址信息，防止用户通过地址修改内存池内容，可以在MemPoolDef.h文件中通过修改宏显示地址

/*static--------------------------------------------------BEGIN*/
static MP_uintx_t MP_readPoolSysSize(MemPoolTypedef* pMemPool);//读内存池系统大小
static MP_uintx_t MP_readPoolBlockSize(MemPoolTypedef* pMemPool);//读内存池总的块大小
static AreaListTypedef* MP_createAreaList(void);//创建区链表
static MP_uintx_t MP_readAreaCount(AreaListTypedef*);//读池包含区的数量
static MP_STATE MP_pushAreaList(AreaListTypedef*, MP_uintx_t, MP_uintx_t);//在区链表中插入一个新区节点
static MP_STATE MP_initBlockStack(MemPoolTypedef* pMemPool);//初始化块栈
static MP_STATE MP_copyMemPoolToMP(MemPoolTypedef* pMemPool, MemPoolTypedef* MP);//拷贝pMemPool到MP
static MP_STATE MP_pushBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t Addr, MP_uintx_t key);//块入栈
static char MP_emptyBlockStack(BlockStackTypedef* pBlockStack);//判断块栈是否为空
static void* MP_popBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t lock);//块出栈
static void MP_sortAreaList(AreaListTypedef* pNode);//区链排序
/*static--------------------------------------------------END*/

/**
  * @brief  设置内存池（__MEMPOOL_MODE = 3时，PoolSize与AddrBase是无效的，随便填即可）
  * @param  AddrTop		内存池基地址
  * @param	PoolSize	内存池大小
  * @retval MP_STATE	内存池状态
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
  * @brief  添加内存池分区
  * @param  BlockSize	块的大小(4的整数倍)
  * @param	TotalCount	块总的数量
  * @retval MP_STATE	内存池状态
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
  * @brief  初始化内存池
  * @param  无
  * @retval MP_STATE	内存池状态
  */
MP_STATE MP_initMemPool(void)
{
	MP_uintx_t BlockTotalSize = 0;//总的块所管理空间的大小
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
	/*申请内存*/
	MemPool->Addr_Base = (MP_uintx_t)malloc((BlockTotalSize+ (sizeof(BlockTypedef) * BlockTotalCount)));
#endif
	MP_sortAreaList(MemPool->pAreaList);
	if (MP_initBlockStack(MemPool) == MP_ERR)return MP_ERR;
	MP_copyMemPoolToMP(MemPool, &MP);
	return MP_OK;
}

/**
  * @brief  拷贝pMemPool到MP
  * @param  pMemPool
  * @param  MP
  * @retval 内存池状态
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
  * @brief  读内存池系统大小
  * @param  pMemPool	内存池控制块地址
  * @retval 内存池系统大小
  */
static MP_uintx_t MP_readPoolSysSize(MemPoolTypedef* pMemPool)
{	
	MP_uintx_t PoolSYS_Size = 0;//池管理系统大小
	MP_uintx_t AreaCount = 0;//区总数
	MP_uintx_t BlockTotalCount = 0;//所有区总的块数量
	MP_uintx_t BlockTotalSize = 0;//总的块所管理空间的大小
	AreaCount = MP_readAreaCount(pMemPool->pAreaList);//区总数
	if (pMemPool->pAreaList != MP_NULL) {
		for (AreaListTypedef* Area = pMemPool->pAreaList->pAreaNext; Area != NULL; Area = Area->pAreaNext) {
			BlockTotalCount += Area->pArea->Total_Count;
			BlockTotalSize += Area->pArea->Total_Count * Area->pArea->Block_Size;
		}
	}
	/*计算所需池管理系统大小*/
	PoolSYS_Size = sizeof(MemPoolTypedef) +//内存池句柄大小
		sizeof(AreaListTypedef) * (AreaCount + 1) +//区链数据大小
		sizeof(AreaTypedef) * AreaCount +//区句柄大小
		sizeof(BlockStackTypedef) * AreaCount +//区的块栈大小
		sizeof(BlockTypedef) * BlockTotalCount;//所有区总的块数据大小
	return PoolSYS_Size;
}

/**
  * @brief  读内存池总的块大小
  * @param  pMemPool	内存池控制块地址
  * @retval 内存池总的块大小
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
  * @brief  连续的内存空间分配器
  * @param  Continuous	连续内存空间分配控制块
  * @param  size		申请分配的空间大小
  * @retval 分配地址
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
  * @brief  初始化块栈
  * @param  pMemPool 内存池控制块
  * @retval 内存池状态
  */
static MP_STATE MP_initBlockStack(MemPoolTypedef* pMemPool) 
{
	if (pMemPool == MP_NULL)return MP_ERR;
	if (pMemPool->pAreaList == MP_NULL)return MP_ERR;
	MP_uintx_t BlockTotalCount = 0;
	KMALLOCTypedef kmalloc;
#if (__MEMPOOL_MODE == 3)
	/*计算块数据与其管理空间的首地址与占用内存空间大小*/
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
			/*创建块*/
			pBlock = (BlockTypedef*)MP_Kmalloc(&kmalloc, (pAreaList->pArea->Block_Size + sizeof(BlockTypedef)));//块获取管理的地址空间
			/*区控制块记录区基地址*/
			if (pAreaList->pArea->Addr_Base == (MP_uintx_t)MP_NULL) {
				pAreaList->pArea->Addr_Base = (MP_uintx_t)pBlock;
			}
			if (pBlock == MP_NULL)return MP_ERR;
			/*块入块管理栈*/
			pBlock->pBlockNext = pBlockStack->pTop;
			pBlockStack->pTop = pBlock;
		}
	}
#endif
	return MP_OK;
}

/**
  * @brief  从内存池分配内存
  * @param  size 分配内存大小
  * @retval 分配地址
  */
void* MP_malloc(MP_uintx_t size) 
{	
	/*获取区链表头节点地址*/
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
  * @brief  从内存池释放内存
  * @param  Addr 释放内存地址
  * @retval 内存池状态
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
  * @brief	块入栈
  * @param	pBlockStack	块栈地址
  * @param	Addr		入栈块所管理的空间首地址
  * @param  key			块钥匙(块入栈开锁钥匙值=0xFFFFFFFF)
  * @retval	0非空，1空
  */
static MP_STATE MP_pushBlockStack(BlockStackTypedef* pBlockStack, MP_uintx_t Addr, MP_uintx_t key)
{	
	//获取管理该空间首地址的块地址
	BlockTypedef* pBlock = (BlockTypedef*)(Addr - sizeof(BlockTypedef));
	//判断该块是否被出栈
	if ((MP_uintx_t)(pBlock->pBlockNext) != key) {
		return MP_ERR;		
	}
	pBlock->pBlockNext = pBlockStack->pTop;
	pBlockStack->pTop = pBlock;
	return MP_OK;
}

/**
  * @brief	判断块栈是否为空
  * @param	pBlockStack	块栈地址
  * @retval	0非空，1空
  */
static char MP_emptyBlockStack(BlockStackTypedef* pBlockStack)
{
	if (pBlockStack->pTop == pBlockStack->pBottom)return 1;		
	else return 0;		
}

/**
  * @brief	块出栈
  * @param	pBlockStack	块栈地址
  * @param  lock		块锁(出栈块标识值=0xFFFFFFFF)
  * @retval	块保存地址
  */
static void* MP_popBlockStack(BlockStackTypedef* pBlockStack,MP_uintx_t lock)
{
	//判断栈是否为空
	if (MP_emptyBlockStack(pBlockStack)) {
		return MP_NULL;
	}
	BlockTypedef* pBlock = pBlockStack->pTop;//p指向栈顶节点	
	pBlockStack->pTop= pBlockStack->pTop->pBlockNext;//栈顶下移一位
	pBlock->pBlockNext = (BlockTypedef*)lock;//出栈标识
	return (void*)((MP_uintx_t)pBlock + (MP_uintx_t)sizeof(BlockTypedef));
}

/**
  * @brief  创建区链表
  * @param  无
  * @retval 区头结点地址
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
  * @brief  读池包含区的数量
  * @param  *pNode 头节点地址
  * @retval 链表长度
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
  * @brief  在区链表中插入一个新区节点
  * @param  *pNode		区头节点地址
  * @param  BlockSize	块的大小
  * @param	TotalCount	块总的数量
  * @retval MP_STATE	内存池状态
  */
static MP_STATE MP_pushAreaList(AreaListTypedef* pNode, MP_uintx_t BlockSize, MP_uintx_t TotalCount)
{
#if (__MEMPOOL_MODE == 3)
	AreaListTypedef* pNew;
	pNew = (AreaListTypedef*)malloc(sizeof(AreaListTypedef));
	if (pNew == MP_NULL)return MP_ERR;
	pNew->pAreaNext = pNode->pAreaNext;
	pNode->pAreaNext = pNew;
	/*添加值*/
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
  * @brief  区链排序
  * @param  *pHead 区链头节点地址
  * @retval 无
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
  * @brief  读区总数
  * @param  无
  * @retval 区总数
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
  * @brief  读指定区信息(块栈地址与区基地址始终为空)
  * @param  num 区号
  * @retval 区信息
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
  * @brief  读块总数
  * @param  无
  * @retval 块总数
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
@brief MP初始化
@param 无
@retval MP_STATE 初始化状态
*/
MP_STATE MP_init(void)
{
	MP_STATE return_state= MP_setMemPool(0, 0);//设置内存池，因为使用的是模式3，地址与大小不会被采用可随便填写
	if (return_state == MP_ERR) return return_state;
	for (MP_uintx_t i = 0; (8 * i + 8) <= 512; i++) {
		return_state=MP_addArea((8 + 8 * i), (4096 / (8 + 8 * i)));//创建分区，分区包含(4096 / (8 + 8 * i))个(8+ 8 * i)字节分配块
	}
	
	if (return_state == MP_ERR) return return_state;
	return_state=MP_initMemPool();//初始内存池
	return return_state;

}