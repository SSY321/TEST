//
// Created by siasunhebo on 7/25/19.
//

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

#define MAX_PACKET_SIZE 22

char* encode_packet(const char *text);
char* parse_packet(struct Buffer *buf);

char* encode_packet(const char *text)
{
    int body_len = strlen(text);
    if (body_len > MAX_PACKET_SIZE -15)
    {
        return NULL;
    }

}



char* parse_packet(struct Buffer *buf)
{
    if (buf->size == 0)
    {
        return NULL;
    }
    int head_len;
    int body_len;

    {
    	char *body1;
        body1 = memchr(buf->data, '|', buf->size);
        printf("body = %s\n", body1);
        if (body1 == NULL)
        {
            printf("[Partial Packet] header not ready, buffer %d\n", buf->size);
            return NULL;
        }
        body1 ++;
        head_len = body1 - buf->data;
    }

    {
        char header[20];
        memcpy(header, buf->data, head_len - 1);
        header[head_len - 1] = '\0';
       // body_len = atoi(header);
       body_len = 26;
    }

/*
    if (buf->size < head_len + body_len)
    {
        printf("[Partial Packet] body not ready, buffer %d\n", buf->size);
        return NULL;
    }
*/
    char *body = malloc(body_len + 1);
    if (body_len > 0)
    {
        //memcpy(body, buf->data + head_len, body_len);
        memcpy(body, buf->data, body_len);
    }
    body[body_len] = '\0';

    //buffer_del(buf, head_len + body_len);
    buffer_del(buf, body_len);
    return body;
}


#endif //IMUTEST_PROTOCOL_H
