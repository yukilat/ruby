#include "rcp_user.h"

RCP_ADMINISTRATOR* rcp_administrator;

/************�ڴ����***************/


/*
* @brief �����ڴ�ռ䣨֧���Զ��壩
* @param size ����ռ��С
* @retval void* ����ռ��׵�ַ
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
*@brief �ͷ�rcp_malloc����Ŀռ�
*@param p ������ռ���׵�ַ������rcp_malloc���أ�
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

void rcp_set_memory_hook(MALLOC_USER malloc_user,FREE_USER free_user)//�����ڴ湳��
{
	rcp_administrator->malloc_user = malloc_user;
	rcp_administrator->free_user = free_user;

}

#endif


/**************rcp ��ʼ��****************/

void rcp_init(void)
{
	//�ڴ�س�ʼ��
	#if(RCP_MEMORY_STRATEGY==1)
		MP_init();
	#endif
	//����ṹ���ʼ��
	rcp_administrator_init();



}





void rcp_administrator_init(void)
{
	rcp_administrator = (RCP_ADMINISTRATOR*)rcp_malloc(sizeof(RCP_ADMINISTRATOR));//����ռ�

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

	rcp_Category_Data_init(rcp_administrator->CategoryGroup);//Category��Data��ʼ��
	rcp_Category_Signal_init(rcp_administrator->CategoryGroup);//Category��Signal��ʼ��

}
/**************************************/

/**********rcp task handler************/

static void rcp_parse(char *Message)
{
	cJSON* Message_JSON = cJSON_Parse(Message);
	if (Message_JSON == NULL) return;
	cJSON* Category_JSON = cJSON_GetObjectItem(Message_JSON, "category");//����Category��
	char* Category_str = cJSON_GetStringValue(Category_JSON);

	Category* SpecificCategory= rcp_get_specificCategory(rcp_administrator->CategoryGroup, Category_str);
	if (SpecificCategory == NULL)
	{
		cJSON_Delete(Message_JSON);//ɾ��JSON�����ͷſռ䣩
		return;
	}
	cJSON* load = cJSON_GetObjectItem(Message_JSON, "load");//��ȡ����

	SpecificCategory->parse_handler(load,SpecificCategory->reliance,NULL);//Category����
	
	cJSON_Delete(Message_JSON);//ɾ��JSON�����ͷſռ䣩

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


void rcp_task_handler(void)//rcp������
{
	if (rcp_MessageQueue_isEmpty(rcp_administrator->ReceiveMessage))//�����Ϣ���д�����Ϣ
	{
		Receive_Message* Message = rcp_take_Message();//ȡ��һ����Ϣ
		rcp_parse(Message->data);//��������Ϣ
		rcp_delete_Message(Message);//�ͷ���Ϣռ�õĿռ�
	}
}


