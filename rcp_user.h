#ifndef RCP_USER_H
#define RCP_USER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rcp_include.h"


#define RCP_UART_IMFORMATION_OUTPUT 1 //������Ϣ��������ض���printf��
#define RCP_MEMORY_STRATEGY 0 //�ڴ�ʹ�ò��ԣ�0�������� 1���ڴ�� 2���Զ��壩









/************��������****************/



/************�ڴ����***************/
void* rcp_malloc(size_t size);
void rcp_free(void* p);

#if(RCP_MEMORY_STRATEGY==2)
void rcp_set_memory_hook(MALLOC_USER malloc_user, FREE_USER free_user);//�����ڴ湳��
#endif

/**************************/

/*************��ʼ��*****************/
void rcp_init(void);
void rcp_administrator_init(void);

/*************user function**************/
void rcp_unparse			(const char* CategoryName, const char* ObjectName);
void rcp_task_handler		(void);


/***********����ṹ��*************/
extern RCP_ADMINISTRATOR* rcp_administrator;
/********************************/

#ifdef __cplusplus
}
#endif


#endif 

