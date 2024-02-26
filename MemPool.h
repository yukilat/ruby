#ifndef __MEMPOOL_H
#define __MEMPOOL_H
#include "MemPoolDef.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct BLOCK//块
{
	struct BLOCK* pBlockNext;/*下一个块*/
}BlockTypedef;

typedef struct BLOCKSTACK
{
	BlockTypedef* pTop;/*栈顶部*/
	BlockTypedef* pBottom;/*栈底部*/
}BlockStackTypedef;//可分配块栈

typedef struct AREA//区
{
	MP_uintx_t Total_Count;	/*该区 总存储块个数*/
	MP_uintx_t Used_Count;	/*该区 已使用的存储块个数*/
	MP_uintx_t Block_Size;	/*该区 存储块大小（区类型）*/
	MP_uintx_t Addr_Base;		/*区基地址*/
	BlockStackTypedef* pBlockStack;/*可分配块栈*/
}AreaTypedef;

typedef struct AREALIST//区链表
{
	AreaTypedef* pArea;			/*区*/
	struct AREALIST* pAreaNext;	/*下一个区*/
}AreaListTypedef;

typedef struct MEMPOOLT//内存池 
{
	MP_uintx_t ID;				/*池ID*/
	MP_uintx_t Pool_Size;		/*池总的大小*/
	MP_uintx_t Addr_Base;		/*池管理的内存基地址*/
	MP_uintx_t PoolSys_Size;	/*内存池管理系统占用大小*/
	MP_uintx_t Used_Size;		/*池已被使用的内存大小*/
	AreaListTypedef* pAreaList;	/*区链*/
}MemPoolTypedef;

typedef struct KMALLOC//连续内存空间分配控制块
{
	MP_uintx_t* pAddr;//连续空间首地址(请初始化地址，分配将从首地址开始)
	MP_uintx_t Size;//连续空间大小(请初始化大小)
	MP_uintx_t Used_Count;//已使用空间(请初始化为0)
}KMALLOCTypedef;

extern MemPoolTypedef MP;//MP存储内存池信息，为内存池的拷贝，默认不存储内存信息中的地址信息，防止用户通过地址修改内存池内容，可以在MemPoolDef.h文件中通过修改宏显示地址

/*Mem Pool API----------------------------------------------------------BEGIN*/

MP_STATE MP_setMemPool(MP_uintx_t AddrTop, MP_uintx_t PoolSize);//设置内存池
MP_STATE MP_addArea(MP_uintx_t BlockSize, MP_uintx_t TotalCount);//添加内存池分区
MP_STATE MP_initMemPool(void);//初始化内存池
void* MP_malloc(MP_uintx_t size);//从内存池分配内存
MP_STATE MP_free(void* Addr);//从内存池释放内存
void* MP_Kmalloc(KMALLOCTypedef* Continuous, MP_uintx_t size);//连续内存空间分配器
unsigned int MP_readAreaNumber(void);//读区总数
AreaTypedef MP_readAreaMsg(unsigned int num);//读指定区信息(块栈地址与区基地址始终为空)
unsigned int MP_readBlockNumber(void);//读块总数
MP_STATE MP_init(void);//内存池初始化
/*Mem Pool API----------------------------------------------------------END*/





#ifdef __cplusplus
}
#endif




#endif // !__MEMPOOL_H


