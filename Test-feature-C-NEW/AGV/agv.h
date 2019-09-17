//
// Created by siasunhebo on 8/26/19.
//

#ifndef _AGV_H
#define _AGV_H

//错误事件定义
#define NO_EVENT                           0    //没有事件
#define GS_COMM_BREAK_EVENT                1    //导航传感器通讯中断事件
#define OUT_OF_TRACK_EVENT                 2    //脱轨
#define SERVO_INVALID_EVENT                3    //伺服失效事件
#define CONTROL_RESUMABLE_STOP_EVENT       4    //网络通信中断停车事件
#define WAY_POINT_ERROR                    5    //路线点校验错误
#define BMS_COM_OUTOFTM                    6


void paramter_define();    //车辆参数定义
void Prase_Sensor_Data();  //传感器处理
void Location_AGV();       //车体定位
void get_PID();            //获取PID偏差值

#endif //IMUTEST_AGV_H
