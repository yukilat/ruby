#include "rcp_message.h"

/*
@brief 创建一个消息队列
@param 无
@retval Receive_Message*:消息队列指针
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
@brief 判断消息队列是否为空
@param front:队列首部指针
@retval unsigned char:判断结果（0：空 1：非空）
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
@brief 获取消息队列尾部指针
@param front:消息队列首部指针
@retval Receive_Message*:消息队列尾部指针
*/
Receive_Message* rcp_get_MessageQueueRear(Receive_Message* front)
{
	Receive_Message* rear = front;
	while (rear->next != NULL)
		rear = rear->next;
	
	return rear;
}


/*
@brief 向消息队列写入消息
@param front:消息队列首部指针的地址
@param Data:消息数据地址
@param length:消息长度
@retval 无
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
@brief 从消息队列读出消息
@param front:消息队列首部指针
@retval Receive_Message*:消息指针
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
@brief 释放一条消息的空间
@param Message:消息指针
@retval 无
*/
void rcp_delete_Message(Receive_Message* Message)
{
	if (Message == NULL) return;
	rcp_free(Message->data);
	rcp_free(Message);
}

/***************************************************/

/*
@brief 接收一条消息
@param Data : 消息数据地址
@param length : 消息长度
@retval 无
*/
void rcp_Receive_Message(unsigned char* Data, unsigned short length)
{
	rcp_Write_Message(&rcp_administrator->ReceiveMessage, Data, length);
}


/*
@brief 取出一条消息
@param 无
@retval Receive_Message*:消息指针
*/
Receive_Message* rcp_take_Message(void)
{
	return rcp_Read_Message(rcp_administrator->ReceiveMessage);
}
