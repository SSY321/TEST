//
// Created by maxiao on 8/26/19.
//

#ifndef _AGVCONTROL_H
#define _AGVCONTROL_H


//AGV状态定义
#define AGV_MODE_STANDBY     0    //待机模式
#define AGV_MODE_RUNNING     1    //运行模式
#define AGV_MODE_SUSPENDED   2    //挂起模式
#define AGV_MODE_OP          3    //操作模式


void AGV_Running();
void AGV_RUN();
void AGV_StandBy();
void AGV_Suspended();
void AGV_Op();


#endif //IMUTEST_AGVCONTROL_H
