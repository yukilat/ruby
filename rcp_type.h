#ifndef RCP_TYPE_H
#define RCP_TYPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "rcp_include.h"

/*************Tag base**************/ 


//Tag handler
typedef void (*Tag_handler)(void*);
//�źŰ󶨺���
typedef void (*Signal_handler)(void*);

//Tag struct
typedef struct Tag
{
	struct	Tag* node;//ָ����һ��Tag����
	char* name;//Tag name
	Tag_handler parseTag_handler;//Tag����������
	Tag_handler unparseTag_handler;//Tag������������
}Tag;






/*********Category base***********/

//Category handler
typedef int (*Category_handler)(cJSON*,void*,char*);//����load�ĺ���


//Category struct
typedef struct Category
{
	struct Category* node;//ָ����һ��Category����
	char* name;//Category name
	Tag* TagHead;//parseTag��ͷָ��
	Category_handler parse_handler;//Category����������
	Category_handler unparse_handler;//Category������������
	void* reliance;//��������
}Category;

/********Category:data******************/


//Category:data �����ݳ�Ա
typedef struct Data_Member
{
    struct	Data_Member* node;//��һ�����ݳ�Ա
	char* name;//������
	Tag* Tag;//��������
	void* data;//���ݳ���
	unsigned short length;//���ݳ���

}Data_Member;

//Category:data��Tag�Ĵ������

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
	char* name;//�ź���
	Tag* Tag;//�ź�����
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

