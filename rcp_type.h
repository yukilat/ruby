#ifndef RCP_TYPE_H
#define RCP_TYPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "rcp_include.h"

/*************Tag base**************/ 


//Tag handler
typedef void (*Tag_handler)(void*);
//信号绑定函数
typedef void (*Signal_handler)(void*);

//Tag struct
typedef struct Tag
{
	struct	Tag* node;//指向下一个Tag对象
	char* name;//Tag name
	Tag_handler parseTag_handler;//Tag解析处理函数
	Tag_handler unparseTag_handler;//Tag反解析处理函数
}Tag;






/*********Category base***********/

//Category handler
typedef int (*Category_handler)(cJSON*,void*,char*);//处理load的函数


//Category struct
typedef struct Category
{
	struct Category* node;//指向下一个Category对象
	char* name;//Category name
	Tag* TagHead;//parseTag的头指针
	Category_handler parse_handler;//Category解析处理函数
	Category_handler unparse_handler;//Category反解析处理函数
	void* reliance;//依赖数据
}Category;

/********Category:data******************/


//Category:data 的数据成员
typedef struct Data_Member
{
    struct	Data_Member* node;//下一个数据成员
	char* name;//变量名
	Tag* Tag;//数据类型
	void* data;//数据长度
	unsigned short length;//数据长度

}Data_Member;

//Category:data下Tag的传入参数

typedef struct Category_Data_Tag
{
	void* data_adress;
	unsigned short length;
	cJSON* data_array;
}Category_Data_Tag;

/********Category:singal******************/

typedef struct Signal_Member
{
	struct Signal_Member* node;
	char* name;//信号名
	Tag* Tag;//信号类型
	void* data;
	Signal_handler handler;
}Signal_Member;



typedef struct Category_Signal_Tag
{
	void* data_adress;
	cJSON* stuff;
}Category_Signal_Tag;



/*************************************/





/***********rcp message*************/
typedef struct Receive_Message
{
	struct	Receive_Message* prev;
	struct  Receive_Message* next;
	unsigned short length;
	unsigned char* data;

}Receive_Message;










/************rcp administrator************/

typedef void* (CJSON_CDECL* MALLOC_USER)(size_t sz);
typedef void (CJSON_CDECL* FREE_USER)(void* ptr);

typedef struct RCP_ADMINISTRATOR
{
	Category* CategoryGroup;
	Receive_Message* ReceiveMessage;
	MALLOC_USER malloc_user;
	FREE_USER	free_user;


}RCP_ADMINISTRATOR;



/****************************************/

#ifdef __cplusplus
}
#endif

#endif 

