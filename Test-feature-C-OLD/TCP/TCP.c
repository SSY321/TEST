//
// Created by siasunhebo on 7/5/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <TCP.h>
#include <pthread.h>


int sockfd;                        	// Socket file descriptor
int num;                    		// Counter of received bytes
int num1;
char revbuf[LENGTH];       		// Receive buffer

char sdbuf[]={"maxiao\n"};

//extern char buffer[8];  //发送PLC数组

struct sockaddr_in remote_addr;    	// Host address information

int socket_init()
{


    /* Get the Socket file descriptor */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("ERROR: Failed to obtain Socket Descriptor!\n");
        return (0);
    }

    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;              	// Protocol Family
    remote_addr.sin_port = htons(PORT);           		// Port number设置端口号 htons()16位整数从主机字节序转换为网络字节序
    inet_pton(AF_INET, "192.168.1.10", &remote_addr.sin_addr); 	// Net Address
    memset (remote_addr.sin_zero,0,8);                 	// Flush the rest of struct

    /* Try to connect the remote */
    if (connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
    {
        printf ("ERROR: Failed to connect to the host!\n");
        return (0);
    }
    else
    {
        printf ("OK: Have connected to the 192.168.1.2\n");
    }
}

int socket_send(char buf[])
{
    if((num = send(sockfd, buf, 8, 0)) == -1)
    {
        printf("ERROR: Failed to sent string.\n");
        close(sockfd);
        exit(1);
    }
    //printf("OK: Sent %d bytes sucessful, please enter again.\n", num);



}

void *socket_rec(void *t)
{

	while(1)
	{
        //socket_send(buffer);
		num = recv(sockfd, revbuf, LENGTH, 0);
		if (num < 0)
		{
			printf("ERROR: Receive string error!\n");
		}
		else if (num == 0)
		{
			break;
		}
		else
		{
			 printf ("OK: Receviced numbytes = %d\n", num);
		}
		/*
		switch(num)
		{
		    case -1:
		        printf("ERROR: Receive string error!\n");
		        close(sockfd);
		        break;

		    case  0:
		        close(sockfd);
		        break;

		    default:
		        printf ("OK: Receviced numbytes = %d\n", num);

		}
		*/
	}

    //memset (revbuf, 0, LENGTH);


	close(sockfd);
	pthread_exit(NULL);
//	return NULL;

    //revbuf[num] = '\0';
   // printf ("OK: Receviced string is: %s\n", revbuf);
   // return num;

}















