#include "rcp_message.h"

/*
@brief ����һ����Ϣ����
@param ��
@retval Receive_Message*:��Ϣ����ָ��
*/
Receive_Message* rcp_MessageQueue_Create(void)
{
	Receive_Message* Message = (Receive_Message*)rcp_malloc(sizeof(Receive_Message));
	Message->data = NULL;
	Message->length = 0;
	Message->prev = NULL;
	Message->next = NULL;
}



/*
@brief �ж���Ϣ�����Ƿ�Ϊ��
@param front:�����ײ�ָ��
@retval unsigned char:�жϽ����0���� 1���ǿգ�
*/
unsigned char rcp_MessageQueue_isEmpty(Receive_Message* front)
{
	Receive_Message* rear = rcp_get_MessageQueueRear(front);
	if (rear->data == NULL)
		return 0;
	else
		return 1;

}


/*
@brief ��ȡ��Ϣ����β��ָ��
@param front:��Ϣ�����ײ�ָ��
@retval Receive_Message*:��Ϣ����β��ָ��
*/
Receive_Message* rcp_get_MessageQueueRear(Receive_Message* front)
{
	Receive_Message* rear = front;
	while (rear->next != NULL)
		rear = rear->next;
	
	return rear;
}


/*
@brief ����Ϣ����д����Ϣ
@param front:��Ϣ�����ײ�ָ��ĵ�ַ
@param Data:��Ϣ���ݵ�ַ
@param length:��Ϣ����
@retval ��
*/
void rcp_Write_Message(Receive_Message** front,unsigned char* Data,unsigned short length)
{
	Receive_Message* Message = (Receive_Message*)rcp_malloc(sizeof(Receive_Message));
	Message->data = NULL;
	Message->length = 0;
	Message->prev = NULL;
	Message->next = *front;

	Receive_Message* load_message = *front;
	load_message->prev = Message;
	load_message->length = length;
	load_message->data = (unsigned char*)rcp_malloc(sizeof(unsigned char) * length);

	memcpy(load_message->data, Data, length);

	*front = Message;
}



/*
@brief ����Ϣ���ж�����Ϣ
@param front:��Ϣ�����ײ�ָ��
@retval Receive_Message*:��Ϣָ��
*/
Receive_Message* rcp_Read_Message(Receive_Message* front)
{
	if (!rcp_MessageQueue_isEmpty(front))return NULL;
	Receive_Message* rear = rcp_get_MessageQueueRear(front);
	
	Receive_Message* prev_message = rear->prev;
	prev_message->next = NULL;

	return rear;
}

/*
@brief �ͷ�һ����Ϣ�Ŀռ�
@param Message:��Ϣָ��
@retval ��
*/
void rcp_delete_Message(Receive_Message* Message)
{
	if (Message == NULL) return;
	rcp_free(Message->data);
	rcp_free(Message);
}

/***************************************************/

/*
@brief ����һ����Ϣ
@param Data : ��Ϣ���ݵ�ַ
@param length : ��Ϣ����
@retval ��
*/
void rcp_Receive_Message(unsigned char* Data, unsigned short length)
{
	rcp_Write_Message(&rcp_administrator->ReceiveMessage, Data, length);
}


/*
@brief ȡ��һ����Ϣ
@param ��
@retval Receive_Message*:��Ϣָ��
*/
Receive_Message* rcp_take_Message(void)
{
	return rcp_Read_Message(rcp_administrator->ReceiveMessage);
}
