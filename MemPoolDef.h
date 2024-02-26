/*内存池宏定义*/
#ifndef __MEMPOOLDEF_H
#define __MEMPOOLDEF_H
#include <malloc.h>

/**
  *描述：内存池分配模式选择
  *1：固定MemPool大小，由MemPool的管理系统为Area自动分配Block
  *2：固定MemPool大小，并且给出各个Area的期望Block数量，MemPool的管理系统会尽量为Area分配到期望Block数量
  *3：由Area决定MemPool的大小，需要手动预估所设置Area不超过MemPool可申请到的系统内存
  */
#define __MEMPOOL_MODE 3	//目前只实现了模式 3

/*MP内存信息中的地址信息显示开关*/
#define __MP_SHOW_ADDR 0 //0：关，1：开

/*区信息中的地址信息显示开关*/
#define __AREA_SHOW_ADDR 1 //0：关，1：开

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
/* 64位系统 */
typedef unsigned long long MP_uintx_t;
#else
/* 32位系统 */
typedef unsigned int MP_uintx_t;
#endif

#endif // !__MEMPOOLDEF_H

