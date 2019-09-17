//
// Created by siasunhebo on 7/5/19.
//

#ifndef IMUTEST_TCP_H
#define IMUTEST_TCP_H

#define PORT 		5000         		// The port which is communicate with server
#define LENGTH 	28          		// Buffer length


int socket_init();    //初始化套接字
int socket_send(char *buf);    //套接字发送
void *socket_rec(void *t);       //套接字接收



#endif //IMUTEST_TCP_H
