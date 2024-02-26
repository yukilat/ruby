#include "rcp_user.h"

RCP_ADMINISTRATOR* rcp_administrator;

/************内存管理***************/


/*
* @brief 分配内存空间（支持自定义）
* @param size 分配空间大小
* @retval void* 分配空间首地址
*/
void* rcp_malloc(size_t size)
{
	#if(RCP_MEMORY_STRATEGY==0)
		return	malloc(size);
	#elif(RCP_MEMORY_STRATEGY==1)
		return	MP_malloc(size);
	#else(RCP_MEMORY_STRATEGY==2)
		return rcp_administrator->malloc_user(size);
	#endif
		

}

/*
*@brief 释放rcp_malloc分配的空间
*@param p 被分配空间的首地址（需由rcp_malloc返回）
*/
void rcp_free(void* p)
{

	#if(RCP_MEMORY_STRATEGY==0)
		free(p);
	#elif(RCP_MEMORY_STRATEGY==1)
		MP_free(p);
	#else(RCP_MEMORY_STRATEGY==2)
		rcp_administrator->free_user(p);
	#endif

}


#if(RCP_MEMORY_STRATEGY==2)

void rcp_set_memory_hook(MALLOC_USER malloc_user,FREE_USER free_user)//设置内存钩子
{
	rcp_administrator->malloc_user = malloc_user;
	rcp_administrator->free_user = free_user;

}

#endif


/**************rcp 初始化****************/

void rcp_init(void)
{
	//内存池初始化
	#if(RCP_MEMORY_STRATEGY==1)
		MP_init();
	#endif
	//管理结构体初始化
	rcp_administrator_init();



}





void rcp_administrator_init(void)
{
	rcp_administrator = (RCP_ADMINISTRATOR*)rcp_malloc(sizeof(RCP_ADMINISTRATOR));//分配空间

	if (rcp_administrator == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  
		return;
	}
	rcp_administrator->CategoryGroup = rcp_CategoryGroup_create();
	rcp_administrator->ReceiveMessage = rcp_MessageQueue_Create();

	if (rcp_administrator->CategoryGroup==NULL|| rcp_administrator->ReceiveMessage==NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
				printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  
		return;
	}

	rcp_Category_Data_init(rcp_administrator->CategoryGroup);//Category：Data初始化
	rcp_Category_Signal_init(rcp_administrator->CategoryGroup);//Category：Signal初始化

}
/**************************************/

/**********rcp task handler************/

static void rcp_parse(char *Message)
{
	cJSON* Message_JSON = cJSON_Parse(Message);
	if (Message_JSON == NULL) return;
	cJSON* Category_JSON = cJSON_GetObjectItem(Message_JSON, "category");//解析Category名
	char* Category_str = cJSON_GetStringValue(Category_JSON);

	Category* SpecificCategory= rcp_get_specificCategory(rcp_administrator->CategoryGroup, Category_str);
	if (SpecificCategory == NULL)
	{
		cJSON_Delete(Message_JSON);//删除JSON对象（释放空间）
		return;
	}
	cJSON* load = cJSON_GetObjectItem(Message_JSON, "load");//获取负载

	SpecificCategory->parse_handler(load,SpecificCategory->reliance,NULL);//Category解析
	
	cJSON_Delete(Message_JSON);//删除JSON对象（释放空间）

}


void rcp_unparse(const char* CategoryName,const char* ObjectName)
{
	cJSON* Message_JSON = cJSON_CreateObject();

	Category* SpecificCategory = rcp_get_specificCategory(rcp_administrator->CategoryGroup, CategoryName);

	if (SpecificCategory == NULL)
	{
		cJSON_Delete(Message_JSON);
		return;
	}
	cJSON* load = cJSON_CreateObject();
	cJSON_AddItemToObject(Message_JSON, "category", cJSON_CreateString(SpecificCategory->name));
	cJSON_AddItemToObject(Message_JSON, "load", load);

	SpecificCategory->unparse_handler(load, SpecificCategory->reliance, ObjectName);

	char* Message = cJSON_Print(Message_JSON);
	printf("%s", Message);

	rcp_Receive_Message(Message, strlen(Message) + 1);



	cJSON_Delete(Message_JSON);


	
}


void rcp_task_handler(void)//rcp任务处理
{
	if (rcp_MessageQueue_isEmpty(rcp_administrator->ReceiveMessage))//如果消息队列存在消息
	{
		Receive_Message* Message = rcp_take_Message();//取出一条消息
		rcp_parse(Message->data);//解析该消息
		rcp_delete_Message(Message);//释放消息占用的空间
	}
}


