//
// Created by siasunhebo on 7/6/19.
//

#ifndef IMUTEST_1200_H
#define IMUTEST_1200_H

#include <common.h>

typedef struct Command_coor
{
    int x;
    int y;
    int angle;
} Command_coor_t;

typedef struct PLC_DATA
{
    Velocity_Class_t Velocity;
    int command_line;
    int command_rotate;
    Command_coor_t command_coor;

}PLC_DATA_t;


void get_PLC_init();   //初始化接收1200数据
Coordinate_Class_t get_PLC_Command_coor(char *revbuf);    //接收目标地址
void get_PLC_Data();  //接收PLC数据(除以1000)
void get_pthread();
void *fuck_tcp(void *t);
int PLC_send(char buf[]);

#endif //IMUTEST_1200_H
