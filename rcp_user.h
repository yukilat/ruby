#ifndef RCP_USER_H
#define RCP_USER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rcp_include.h"


#define RCP_UART_IMFORMATION_OUTPUT 1 //串口信息输出（需重定向到printf）
#define RCP_MEMORY_STRATEGY 0 //内存使用策略（0：基础库 1：内存池 2：自定义）









/************函数声明****************/



/************内存管理***************/
void* rcp_malloc(size_t size);
void rcp_free(void* p);

#if(RCP_MEMORY_STRATEGY==2)
void rcp_set_memory_hook(MALLOC_USER malloc_user, FREE_USER free_user);//设置内存钩子
#endif

/**************************/

/*************初始化*****************/
void rcp_init(void);
void rcp_administrator_init(void);

/*************user function**************/
void rcp_unparse			(const char* CategoryName, const char* ObjectName);
void rcp_task_handler		(void);


/***********管理结构体*************/
extern RCP_ADMINISTRATOR* rcp_administrator;
/********************************/

#ifdef __cplusplus
}
#endif


#endif 

