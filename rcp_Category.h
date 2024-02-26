#ifndef RCP_CATEGORY_H
#define RCP_CATEGORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rcp_include.h"


/****************Category base*******************/
Category*		rcp_CategoryGroup_create					(void);
void			rcp_add_CategoryToGroup						(Category* head, const char* name, Category_handler parse_handler, Category_handler unparse_handler);
Category*		rcp_get_endCategory							(Category* head);
Category*		rcp_get_specificCategory					(Category* head, char* name);
/****************Category Data User Function******************/
void			rcp_Category_Data_init						(Category* head);
void			rcp_Category_Data_Register_Data				(const char* name, const char* TagName, void* Data_adrss, unsigned short length);//注册一个数据成员
void			rcp_Category_Data_Register_Tag				(const char* TagName, Tag_handler ParseHandler, Tag_handler UnparseHandler);//注册一个Tag
int				rcp_Category_Data_ParseHandler				(cJSON* load, void* pram, char* name);
int				rcp_Category_Data_UnparseHandler			(cJSON* load, void* pram, char* name);


/***********************************************/
Data_Member*	rcp_Category_Data_RelianceGroup_create		(void);
void			rcp_Category_Data_add_DataMember			(Data_Member* DataHead, const char* name, const char* TagName, Tag* TagHead, void* adress, unsigned short length);
Data_Member*	rcp_Category_Data_get_endData				(Data_Member* head);
Data_Member*	rcp_Category_Data_get_specificData			(Data_Member* head, char* name);
/****************Category Singal User Function*******************************/
void			rcp_Category_Signal_init					(Category* head);
void			rcp_Category_Signal_Register_Signal			(const char* name, const char* TagName, void* Data_adrss, Signal_handler handler);
void			rcp_Category_Signal_Register_Tag			(const char* TagName, Tag_handler ParseHandler, Tag_handler UnparseHandler);
int				rcp_Category_Signal_ParseHandler			(cJSON* load, void* pram, char* name);
int				rcp_Category_Signal_UnparseHandler			(cJSON* load, void* pram, char* name);


Signal_Member*	rcp_Category_Signal_RelianceGroup_create	(void);
void			rcp_Category_Signal_add_SignalMember		(Signal_Member* SignalHead, const char* name, const char* TagName, Tag* TagHead, Signal_handler handler, void* data);
Signal_Member*	rcp_Category_Signal_get_endSignal			(Signal_Member* head);
Signal_Member*	rcp_Category_Signal_get_specificSignal		(Signal_Member* head, char* name);





/************************************************/
#if(RCP_UART_IMFORMATION_OUTPUT)
void			rcp_Query_Category							(void);
void			rcp_Category_Data_Query_Tag					(void);
void			rcp_Category_Data_Query_Data				(void);

#endif






#ifdef __cplusplus
}
#endif



#endif  

