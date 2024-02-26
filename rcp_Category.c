#include "rcp_Category.h"


/****************Category base*******************/

/*
@brief function:创建一个CategoryGroup
@param 无
@retval Category*:Category的头指针
*/
Category* rcp_CategoryGroup_create(void)
{
	Category* head = (Category*)rcp_malloc(sizeof(Category));
	if (head == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
				printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  

		return NULL;
	}

	head->TagHead = NULL;
	head->node = NULL;
	head->name = NULL;
	head->parse_handler = NULL;
	head->unparse_handler = NULL;
	head->reliance = NULL;
	
	return head;
}
/*
@brief function:向CategoryGroup添加一个Category
@param head:CategoryGroup的头指针
@param name:Category的名字
@param parse_handler:Category的解析处理函数
@param unparse_handler:Category的反解析处理函数
@retval 无
*/
void rcp_add_CategoryToGroup(Category *head,const char *name, Category_handler parse_handler, Category_handler unparse_handler)
{
	if (head == NULL||name==NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_INVALID_POINTER, __FILE__, __LINE__);
		#endif  
		return;
	}

	if (rcp_get_specificCategory(head, name) != NULL)//检查是否存在同名
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_SAME_NAME_CATEGORY, __FILE__, __LINE__);
		#endif  
		return;//不创建同名Category
	}
	Category* NextCategory= (Category*)rcp_malloc(sizeof(Category));
	if (NextCategory == NULL)//检查内存
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  
		return;

	}

	NextCategory->TagHead = NULL;
	NextCategory->name = NULL;
	NextCategory->node = NULL;
	NextCategory->parse_handler = NULL;
	NextCategory->unparse_handler = NULL;
	NextCategory->reliance = NULL;


	Category* endCategory = rcp_get_endCategory(head);
	endCategory->node = NextCategory;
	endCategory->parse_handler = parse_handler;
	endCategory->unparse_handler = unparse_handler;
	endCategory->name= (char*)rcp_malloc(sizeof(char) * strlen(name) + 1);
	if (endCategory->name == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
				printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  
		return;
	}
	

	memcpy(endCategory->name, name, sizeof(char) * strlen(name) + 1);

}


/**
* @brief 获取CategoryGroup的尾节点
* @param head:CategoryGroup的头指针
* @retval Category*:CategoryGroup的尾节点
*/
Category* rcp_get_endCategory(Category* head)
{
	if (head == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_INVALID_POINTER, __FILE__, __LINE__);
		#endif  
		return NULL;
	}
	Tag* endCategory = head;
	while (endCategory->node != NULL)
	{
		endCategory = endCategory->node;
	}
	return endCategory;

}

/*
@brief function:获取特定的Category节点
@param head:CategoryGroup的头指针
@param name:Category的名字
@retval Category*:特定节点指针
*/
Category* rcp_get_specificCategory(Category* head, char* name)
{
	if (head == NULL || name==NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_INVALID_POINTER, __FILE__, __LINE__);
		#endif  
		return NULL;
	}
	Tag* specificCategory = head;
	while (specificCategory->node != NULL)
	{
		if (strcmp(specificCategory->name, name) == 0)
			return specificCategory;
		else
			specificCategory = specificCategory->node;
	}
	return NULL;
}


/*
@brief function:为Category添加reliance
@param head:CategoryGroup的头指针
@param name:Category的名字
@param reliance:附属数据的指针
@retval 无
*/
void rcp_add_relianceToCategory(Category* head, const char* name, void* reliance)
{
	if (head == NULL || name == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
			printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_INVALID_POINTER, __FILE__, __LINE__);
		#endif  
		return;
	}
	Category* category = rcp_get_specificCategory(head, name);
	if (category == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
				printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_REGISTER_CATEGORY, __FILE__, __LINE__);
		#endif  
		return;
	}
	category->reliance = reliance;
}





/*********Category:data***************************/

/*
@brief function:初始化Category_Data
@param head:CategoryGroup的头指针
@retval 无
*/
void rcp_Category_Data_init(Category* head)
{
	rcp_add_CategoryToGroup(head, "data", rcp_Category_Data_ParseHandler, rcp_Category_Data_UnparseHandler);
	Category* Category_Data = rcp_get_specificCategory(head, "data");

	Category_Data->TagHead = rcp_TagGroup_create();//创建TagGroup
	//添加Tag
	rcp_add_TagToGroup(Category_Data->TagHead, "double", rcp_Category_Data_ParseTag_double, rcp_Category_Data_UnparseTag_double);
	rcp_add_TagToGroup(Category_Data->TagHead, "float", rcp_Category_Data_ParseTag_float, rcp_Category_Data_UnparseTag_float);
	rcp_add_TagToGroup(Category_Data->TagHead, "int", rcp_Category_Data_ParseTag_int, rcp_Category_Data_UnparseTag_int);

	Category_Data->reliance = rcp_Category_Data_RelianceGroup_create();//创建Data_Member

}


/*
@brief function:添加一个数据成员
@param name:变量名
@param TagName:Tag名
@param adress:数据地址
@param length:数据长度
@retval 无
*/
void rcp_Category_Data_Register_Data(const char* name, const char* TagName, void* Data_adrss, unsigned short length)
{
	Category* Category_Data = rcp_get_specificCategory(rcp_administrator->CategoryGroup, "data");
	rcp_Category_Data_add_DataMember((Data_Member*)Category_Data->reliance, name, TagName, Category_Data->TagHead, Data_adrss, length);
}

/*
@brief function：向Category:Data TagGroup添加一个Tag 
@param name:Tag名称
@param ParseHandler：Tag数据解析处理函数
@param UnparseHandler：Tag数据反解析处理函数
@retval 无
*/
void rcp_Category_Data_Register_Tag(const char* TagName, Tag_handler ParseHandler, Tag_handler UnparseHandler)
{
	Category* Category_Data = rcp_get_specificCategory(rcp_administrator->CategoryGroup, "data");
	rcp_add_TagToGroup(Category_Data->TagHead, TagName, ParseHandler, UnparseHandler);
}








//Handler

/*
@brief function:Category:data的解析处理函数 
@param load:JSON load数据主体
@param pram:Category的依赖数据
@param name:消息名
@retval 返回状态
*/
int rcp_Category_Data_ParseHandler(cJSON* load,void *pram,char* name)
{
	Data_Member* Data_head = (Data_Member*)pram;

	cJSON* data_name_json = cJSON_GetObjectItem(load, "name");//解析变量名
	char* data_name = cJSON_GetStringValue(data_name_json);
	Data_Member* specific_Data = rcp_Category_Data_get_specificData(Data_head, data_name);
	if (specific_Data == NULL) return 0;

	cJSON* tag_name_json = cJSON_GetObjectItem(load, "tag");
	char* tag_name = cJSON_GetStringValue(tag_name_json);
	if (strcmp(tag_name, specific_Data->Tag->name)) return 0;

	cJSON* data_length_json = cJSON_GetObjectItem(load, "length");
	unsigned short data_length = cJSON_GetNumberValue(data_length_json);


	cJSON* Data_array = cJSON_GetObjectItem(load, "stuff");

	Category_Data_Tag Tag_Pram;

	Tag_Pram.data_adress = specific_Data->data;
	Tag_Pram.data_array = Data_array;
	Tag_Pram.length = data_length < specific_Data->length ? data_length : specific_Data->length;

	specific_Data->Tag->parseTag_handler(&Tag_Pram);

	return 1;


}
/*
@brief function:Category:data的反解析处理函数
@param load:JSON load数据主体
@param pram:Category的依赖数据
@param name:消息名
@retval 返回状态
*/
int rcp_Category_Data_UnparseHandler(cJSON* load,void* pram,char* name)
{
	Data_Member* Data_head = (Data_Member*)pram;
	Data_Member* specific_Data = rcp_Category_Data_get_specificData(Data_head,name);

	cJSON* data_name_json = cJSON_CreateString(specific_Data->name);
	cJSON* tag_name_json = cJSON_CreateString(specific_Data->Tag->name);
	cJSON* data_length_json = cJSON_CreateNumber(specific_Data->length);
	cJSON* Data_array = cJSON_CreateArray();
	cJSON_AddItemToObject(load, "name", data_name_json);
	cJSON_AddItemToObject(load, "tag", tag_name_json);
	cJSON_AddItemToObject(load, "length", data_length_json);
	cJSON_AddItemToObject(load, "stuff", Data_array);

	Category_Data_Tag Tag_Pram;
	Tag_Pram.data_adress = specific_Data->data;
	Tag_Pram.data_array = Data_array;
	Tag_Pram.length = specific_Data->length;

	specific_Data->Tag->unparseTag_handler(&Tag_Pram);
	return 1;

}

//reliance


/*
@brief function:创建Data_Member
@param 无
@retval Data_Member的头指针
*/
Data_Member* rcp_Category_Data_RelianceGroup_create(void)
{
	Data_Member* Data_head = (Data_Member*)rcp_malloc(sizeof(Data_Member));
	if (Data_head == NULL)
	{
		#if (RCP_UART_IMFORMATION_OUTPUT)
				printf("error:%s----filename:%s----lineNum:%d", RCP_ERROR_NO_MEMORY, __FILE__, __LINE__);
		#endif  
		return NULL;
	}
	Data_head->data = NULL;
	Data_head->length = 0;
	Data_head->name = NULL;
	Data_head->node = NULL;
	Data_head->Tag = NULL;

	return Data_head;
}

/*
@brief function:添加一个数据成员
@param DataHead:数据成员头指针
@param name:变量名
@param TagName:Tag名
@param TagHead:TagGroup头指针
@param adress:数据地址
@param length:数据长度
@retval 无
*/
void rcp_Category_Data_add_DataMember(Data_Member* DataHead, const char* name, const char* TagName,Tag* TagHead, void* adress, unsigned short length)
{
	if (rcp_Category_Data_get_specificData(DataHead, name) != NULL) return;

	Data_Member* NextData= (Data_Member*)rcp_malloc(sizeof(Data_Member));
	NextData->data = NULL;
	NextData->length = 0;
	NextData->name = NULL;
	NextData->node = NULL;
	NextData->Tag = NULL;

	Data_Member* endData = rcp_Category_Data_get_endData(DataHead);
	endData->data = adress;
	endData->length = length;
	endData->name = (char*)rcp_malloc(sizeof(char) * strlen(name) + 1);
	endData->node = NextData;
	endData->Tag = rcp_get_specificTag(TagHead, TagName);

	memcpy(endData->name, name, sizeof(char) * strlen(name) + 1);


}
/*
@brief function:获取数据成员尾指针
@param head:数据成员头指针
@retval Data_Member*:数据成员尾指针
*/
Data_Member* rcp_Category_Data_get_endData(Data_Member* head)
{
	Data_Member* endData = head;
	while (endData->node != NULL)
	{
		endData = endData->node;
	}
	return endData;
}


/*
@brief function:获取指定名字的数据成员指针 
@param head:数据成员头指针
@param name:数据成员名
@retval Data_Member*: 指定名字的数据成员指针
*/
Data_Member* rcp_Category_Data_get_specificData(Data_Member* head, char* name)
{
	Data_Member* specificData = head;


	while (specificData->node != NULL)
	{

		if (strcmp(specificData->name, name) == 0)
			return specificData;
		else
			specificData = specificData->node;
	}
	return NULL;


}




/****************Category:singal*******************************/


void rcp_Category_Signal_init(Category* head)
{
	rcp_add_CategoryToGroup(head, "signal", rcp_Category_Signal_ParseHandler, rcp_Category_Signal_UnparseHandler);
	Category* Category_Signal = rcp_get_specificCategory(head, "signal");

	Category_Signal->TagHead = rcp_TagGroup_create();//创建TagGroup
	//添加Tag
	rcp_add_TagToGroup(Category_Signal->TagHead, "void", NULL, NULL);
	rcp_add_TagToGroup(Category_Signal->TagHead, "double", rcp_Category_Signal_ParseTag_double, rcp_Category_Signal_UnparseTag_double);
	

	Category_Signal->reliance = rcp_Category_Signal_RelianceGroup_create();//创建Signal_Member

}




/*
@brief function:添加一个信号成员
@param name:变量名
@param TagName:Tag名
@param adress:数据地址
@param handler:信号处理函数
@retval 无
*/
void rcp_Category_Signal_Register_Signal(const char* name, const char* TagName, void* Data_adrss,Signal_handler handler)
{

	Category* Category_Signal = rcp_get_specificCategory(rcp_administrator->CategoryGroup, "signal");//获取signal_category管理句柄
	rcp_Category_Signal_add_SignalMember((Signal_Member*)Category_Signal->reliance, name, TagName, Category_Signal->TagHead, handler,Data_adrss);

}

/*
@brief function：向Category:Signal TagGroup添加一个Tag
@param name:Tag名称
@param ParseHandler：Tag数据解析处理函数
@param UnparseHandler：Tag数据反解析处理函数
@retval 无
*/
void rcp_Category_Signal_Register_Tag(const char* TagName, Tag_handler ParseHandler, Tag_handler UnparseHandler)
{
	Category* Category_Signal = rcp_get_specificCategory(rcp_administrator->CategoryGroup, "signal");
	rcp_add_TagToGroup(Category_Signal->TagHead, TagName, ParseHandler, UnparseHandler);
}








//Handler

/*
@brief function:Category:Signal的解析处理函数
@param load:JSON load数据主体
@param pram:Category的依赖数据
@param name:消息名
@retval 返回状态
*/
int rcp_Category_Signal_ParseHandler(cJSON* load, void* pram, char* name)
{
	Signal_Member* Signal_head = (Signal_Member*)pram;

	cJSON* signal_name_json = cJSON_GetObjectItem(load, "name");//解析变量名
	char* signal_name = cJSON_GetStringValue(signal_name_json);
	Signal_Member* specific_Signal = rcp_Category_Signal_get_specificSignal(Signal_head, signal_name);
	if (specific_Signal == NULL) return 0;//未注册退出

	cJSON* tag_name_json = cJSON_GetObjectItem(load, "tag");
	char* tag_name = cJSON_GetStringValue(tag_name_json);
	if (strcmp(tag_name, specific_Signal->Tag->name)) return 0;//tag不相符退出



	cJSON* Data = cJSON_GetObjectItem(load, "stuff");//获取内容

	if (!strcmp(tag_name, "void"))//（特殊tag）表示不携带参数
	{
		if(specific_Signal->handler != NULL)
			specific_Signal->handler(NULL);
		return 1;
	}

	Category_Signal_Tag Tag_Pram;
	Tag_Pram.data_adress = specific_Signal->data;//函数参数地址
	Tag_Pram.stuff = Data;//参数（JSON）

	if(specific_Signal->Tag->parseTag_handler!=NULL)
		specific_Signal->Tag->parseTag_handler(&Tag_Pram);//参数传递

	if(specific_Signal->handler!=NULL)
		specific_Signal->handler(specific_Signal->data);//调用信号

	return 1;


}
/*
@brief function:Category:data的反解析处理函数
@param load:JSON load数据主体
@param pram:Category的依赖数据
@param name:消息名
@retval 返回状态
*/
int rcp_Category_Signal_UnparseHandler(cJSON* load, void* pram, char* name)
{
	Signal_Member* Signal_head = (Signal_Member*)pram;
	Signal_Member* specific_Signal = rcp_Category_Signal_get_specificSignal(Signal_head, name);

	cJSON* signal_name_json = cJSON_CreateString(specific_Signal->name);
	cJSON* tag_name_json = cJSON_CreateString(specific_Signal->Tag->name);
	cJSON_AddItemToObject(load, "name", signal_name_json);
	cJSON_AddItemToObject(load, "tag", tag_name_json);

	if (strcmp(specific_Signal->Tag->name, "void"))//如果tag为“void”则不需要任何数据内容
	{
		cJSON* Data = cJSON_CreateObject();

		cJSON_AddItemToObject(load, "stuff", Data);

		Category_Signal_Tag Tag_Pram;
		Tag_Pram.data_adress = specific_Signal->data;
		Tag_Pram.stuff = Data;

		specific_Signal->Tag->unparseTag_handler(&Tag_Pram);

	}
	
	return 1;
}






/*
@brief function:创建Signal_Member
@param 无
@retval Signal_Member的头指针
*/
Signal_Member* rcp_Category_Signal_RelianceGroup_create(void)
{

	Signal_Member* Signal_head = (Signal_Member*)rcp_malloc(sizeof(Signal_Member));
	Signal_head->handler = NULL;
	Signal_head->name = NULL;
	Signal_head->Tag = NULL;
	Signal_head->data = NULL;
	Signal_head->node = NULL;

	return Signal_head;
}


/*
@brief function:添加一个信号成员
@param SignalHead:信号成员头指针
@param name:信号名
@param TagName:Tag名
@param TagHead:TagGroup头指针
@param handler 绑定信号函数
@param data 参数地址
@retval 无
*/
void rcp_Category_Signal_add_SignalMember(Signal_Member* SignalHead, const char* name, const char* TagName,Tag* TagHead ,Signal_handler handler,void* data)
{

	if (rcp_Category_Signal_get_specificSignal(SignalHead, name) != NULL) return;
	Signal_Member* NextSignal = (Signal_Member*)rcp_malloc(sizeof(Signal_Member));
	NextSignal->name = NULL;
	NextSignal->node = NULL;
	NextSignal->Tag = NULL;
	NextSignal->handler = NULL;
	NextSignal->data = NULL;

	Signal_Member* endSignal = rcp_Category_Signal_get_endSignal(SignalHead);
	endSignal->name = (char*)rcp_malloc(sizeof(char) * strlen(name) + 1);
	endSignal->Tag = rcp_get_specificTag(TagHead, TagName);
	endSignal->handler = handler;
	endSignal->node=NextSignal;
	endSignal->data = data;

	memcpy(endSignal->name, name, sizeof(char) * strlen(name) + 1);


}

/*
@brief function:获取信号成员尾指针
@param head:信号成员头指针
@retval Data_Member*:信号成员尾指针
*/
Signal_Member* rcp_Category_Signal_get_endSignal(Signal_Member* head)
{
	Signal_Member* endSignal = head;
	while (endSignal->node != NULL)
	{
		endSignal = endSignal->node;
	}
	return endSignal;
}





/*
@brief function:获取指定名字的信号成员指针
@param head:信号成员头指针
@param name:信号成员名
@retval Data_Member*: 指定名字的信号成员指针
*/
Signal_Member* rcp_Category_Signal_get_specificSignal(Signal_Member* head, char* name)
{
	Signal_Member* specificSignal = head;

	while (specificSignal->node != NULL)
	{

		if (strcmp(specificSignal->name, name) == 0)
			return specificSignal;
		else
			specificSignal = specificSignal->node;
	}
	return NULL;


}

#if(RCP_UART_IMFORMATION_OUTPUT)


/*
@brief 通过串口输出所有已注册Category（必须已经重定向到printf）
@param 无
@retval 无
*/
void rcp_Query_Category(void)
{
	printf("\nCategory:");
	for (Category* P_Category = rcp_administrator->CategoryGroup; P_Category != NULL; P_Category = P_Category->node)
		if (P_Category->name != NULL)
			printf(" (%s) ", P_Category->name);
	printf("\n");
}





/*
@brief 通过串口输出Category:Data的所有已注册Tag（必须已经重定向到printf）
@param 无
@retval 无
*/
void rcp_Category_Data_Query_Tag(void)
{
	printf("\nCategory:Data Tag:");
	for (Tag* P_Tag = rcp_administrator->CategoryGroup->TagHead; P_Tag != NULL; P_Tag = P_Tag->node)
		if (P_Tag->name != NULL)
			printf(" (%s) ", P_Tag->name);
	printf("\n");
}


/*
@brief 通过串口输出Category:Data的所有已注册DataMember（必须已经重定向到printf）
@param 无
@retval 无
*/
void rcp_Category_Data_Query_Data(void)
{
	printf("\nCategory:Data DataMember:");
	for (Data_Member* P_Data = (Data_Member*)rcp_administrator->CategoryGroup->reliance; P_Data != NULL; P_Data = P_Data->node)
		if (P_Data->name != NULL)
			printf(" (%s:%s) ", P_Data->name, P_Data->Tag->name);
	printf("\n");
}




#endif



