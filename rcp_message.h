#ifndef RCP_MESSAGE_H
#define RCP_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rcp_include.h"

/************************/
Receive_Message*	rcp_MessageQueue_Create		(void);
unsigned char		rcp_MessageQueue_isEmpty	(Receive_Message* front);
Receive_Message*	rcp_get_MessageQueueRear	(Receive_Message* front);
void				rcp_Write_Message			(Receive_Message** front, unsigned char* Data, unsigned short length);
Receive_Message*	rcp_Read_Message			(Receive_Message* front);
void				rcp_delete_Message			(Receive_Message* Message);//释放一条消息的空间
void				rcp_Receive_Message			(unsigned char* Data, unsigned short length);//接收一条消息
Receive_Message*	rcp_take_Message			(void);//取出一条消息



#ifdef __cplusplus
}
#endif





#endif // !RCP_MESSAGE_H

