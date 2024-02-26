#ifndef RCP_TAG_H
#define RCP_TAG_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rcp_include.h"



/*************Tag base*********************/

Tag* rcp_TagGroup_create	(void);
void rcp_add_TagToGroup		(Tag* head, const char* name, Tag_handler parse_handler, Tag_handler unparse_handler);
Tag* rcp_get_endTag			(Tag* head);
Tag* rcp_get_specificTag	(Tag* head, char* name);


/************Category:data Tag**************/
void rcp_Category_Data_ParseTag_double(void* pram);
void rcp_Category_Data_UnparseTag_double(void* pram);
void rcp_Category_Data_ParseTag_float(void* pram);
void rcp_Category_Data_UnparseTag_float(void* pram);
void rcp_Category_Data_ParseTag_int(void* pram);
void rcp_Category_Data_UnparseTag_int(void* pram);


void rcp_Category_Signal_ParseTag_double(void* pram);
void rcp_Category_Signal_UnparseTag_double(void* pram);







#ifdef __cplusplus
}
#endif

#endif // !RCP_TAG_H

