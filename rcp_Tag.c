#include "rcp_Tag.h"

/***********Tag base***************/

/*
@brief function:����TagGroup
@param ��
@retval Tag*:TagGroupͷָ��
*/
Tag* rcp_TagGroup_create(void)
{
	Tag* head = (Tag*)rcp_malloc(sizeof(Tag));
	head->node = NULL;
	head->name = NULL;
	head->parseTag_handler = NULL;
	head->unparseTag_handler = NULL;

	return head;
}

/*
@brief function����TagGroup���һ��Tag
@param head:Tagͷָ��
@param name:Tag����
@param handler:Tag������
@retval ��
*/
void rcp_add_TagToGroup(Tag* head, const char* name, Tag_handler parse_handler, Tag_handler unparse_handler)
{
	if (rcp_get_specificTag(head,name) != NULL) return;//�����ͬ��Tag
	Tag* nextTag= (Tag*)rcp_malloc(sizeof(Tag));
	nextTag->node = NULL;
	nextTag->name = NULL;
	nextTag->parseTag_handler = NULL;
	nextTag->unparseTag_handler = NULL;

	Tag* endTag = rcp_get_endTag(head);
	endTag->node = nextTag;
	endTag->name = (char*)rcp_malloc(sizeof(char) * strlen(name)+1);
	endTag->parseTag_handler = parse_handler;
	endTag->unparseTag_handler = unparse_handler;

	memcpy(endTag->name, name, sizeof(char) * strlen(name)+1);

}


/*
@brief function:��ȡTagGroup��β�ڵ�ָ�� 
@param head:Tag��ͷ�ڵ�
@retval Tag*:Tag��β�ڵ�
*/
Tag* rcp_get_endTag(Tag* head)
{
	Tag* endTag=head;
	while (endTag->node != NULL)
	{
		endTag = endTag->node;
	}
	return endTag;

}


/*
@brief function:��ȡ�ض����Ƶ�Tag�ڵ�
@param head:Tag��ͷ�ڵ�
@param name:Tag������
@retval Tag*:Tag���ض��ڵ�(��ƥ��ڵ�ʱ����NULL)

*/
Tag* rcp_get_specificTag(Tag* head, char* name)
{
	Tag* specificTag = head;
	while (specificTag->node != NULL)
	{
		if (strcmp(specificTag->name, name) == 0)
			return specificTag;
		else
			specificTag = specificTag->node;
	}
	return NULL;
}


/*************Tag handler*******************/
/*************Category:Data*********************/

//double����
void rcp_Category_Data_ParseTag_double(void* pram)
{
	Category_Data_Tag* parameter = (Category_Data_Tag*)pram;
	double* P_double = (double*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		P_double[i] = cJSON_GetNumberValue(cJSON_GetArrayItem(parameter->data_array,i));
	}

}

void rcp_Category_Data_UnparseTag_double(void* pram)
{
	Category_Data_Tag* parameter= (Category_Data_Tag*)pram;
	double* P_double=(double*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		cJSON_AddItemToArray(parameter->data_array, cJSON_CreateNumber(P_double[i]));
	}
}

//float����
void rcp_Category_Data_ParseTag_float(void* pram)
{
	Category_Data_Tag* parameter = (Category_Data_Tag*)pram;
	float* P_float = (float*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		P_float[i] = (float)cJSON_GetNumberValue(cJSON_GetArrayItem(parameter->data_array, i));
	}


}

void rcp_Category_Data_UnparseTag_float(void* pram)
{
	Category_Data_Tag* parameter = (Category_Data_Tag*)pram;
	float* P_float = (float*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		cJSON_AddItemToArray(parameter->data_array, cJSON_CreateNumber(P_float[i]));
	}
}
//int ����
void rcp_Category_Data_ParseTag_int(void* pram)
{
	Category_Data_Tag* parameter = (Category_Data_Tag*)pram;
	int* P_int = (int*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		P_int[i] = (int)cJSON_GetNumberValue(cJSON_GetArrayItem(parameter->data_array, i));
	}

}


void rcp_Category_Data_UnparseTag_int(void* pram)
{
	Category_Data_Tag* parameter = (Category_Data_Tag*)pram;
	int* P_int = (int*)parameter->data_adress;
	for (int i = 0; i < parameter->length; i++)
	{
		cJSON_AddItemToArray(parameter->data_array, cJSON_CreateNumber(P_int[i]));
	}

}








/*********Category:Signal******************/
//double����
void rcp_Category_Signal_ParseTag_double(void* pram)
{
	Category_Signal_Tag* parameter = (Category_Signal_Tag*)pram;
	double* P_double = parameter->data_adress;
	*P_double = cJSON_GetNumberValue(cJSON_GetObjectItem(parameter->stuff, "double"));
}

void rcp_Category_Signal_UnparseTag_double(void* pram)
{
	Category_Signal_Tag* parameter = (Category_Signal_Tag*)pram;
	double* P_double = parameter->data_adress;
	cJSON_AddItemToObject(parameter->stuff, "double", cJSON_CreateNumber(*P_double));
}
















