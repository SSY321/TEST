//
// Created by siasunhebo on 9/12/19.
//
#include <common.h>
#include <main.h>
#include <PGV150/PGV150.h>
#include <zconf.h>
#include <MPI204A/MPI204A.h>
#include "agv.h"

wheel_paramter_t wheelParamter;
extern struct Interpolation_Parameter_t Interpolation_Parameter;    //插补参数、参数暂存

struct Coordinate_Class Virtual_AGV_Current_Coor_InWorld; //虚拟的AGV坐标用于路径跟随
extern struct Coordinate_Class PGV150_coor;   //PGV相机读到的坐标

extern int len_r;
int PGV_rx_flag;
int virtual_agv_coor_init_flag = 0;   //1为地标更新

extern canBus_t *CAN0;
MPI204A_t IMU;

struct Velocity_Class AGV_Current_Velocity_InAGV;         //AGV当前速度
struct Velocity_Class AGV_Current_Velocity_By_Encoder;    //PLC上传由编码器得到速度

struct Coordinate_Class AGV_Current_Coor_InWorld;
Coordinate_Class_t AGV_Target_Coor_InWorld;               //AGV在世界坐标系下的期望坐标
Coordinate_Class_t Error_Coor_InAGV;                      //偏差坐标

extern float angle;

float angle_error;    //角度误差
float PID_error, error_last, error_next;      //PID误差输入
float PID_result = 0.0;






void paramter_define()
{

    wheelParamter.line_slowest_time = 3.0;
    wheelParamter.motor_max_rotationl_velocity_soft = 3000.0;
    wheelParamter.motor_min_rotationl_velocity_soft = 300.0;
    wheelParamter.WHEEL_DIAMETER = 150.0;
    wheelParamter.REDUCATION_RATIO = 15.0;
    wheelParamter.wheel_acceleration_time = 3.0;

    wheelParamter.wheel_max_line_velocity = wheelParamter.motor_max_rotationl_velocity_soft * wheelParamter.WHEEL_DIAMETER * M_PI / 60.0 / wheelParamter.REDUCATION_RATIO;
    wheelParamter.wheel_min_line_velocity = wheelParamter.motor_min_rotationl_velocity_soft * wheelParamter.WHEEL_DIAMETER * M_PI / 60.0 / wheelParamter.REDUCATION_RATIO;
    wheelParamter.wheel_acceleration_line_velocity = (wheelParamter.wheel_max_line_velocity - wheelParamter.wheel_min_line_velocity) / wheelParamter.wheel_acceleration_time;

    Interpolation_Parameter.max_velocity_abs = wheelParamter.wheel_max_line_velocity;
    Interpolation_Parameter.min_velocity_abs = wheelParamter.wheel_min_line_velocity;
    Interpolation_Parameter.accleration_abs = wheelParamter.wheel_acceleration_line_velocity;
    Interpolation_Parameter.slow_time_abs = wheelParamter.line_slowest_time;
}

void Prase_Sensor_Data()
{
    PGV_Send_data();           //串口发送
    usleep(60);
    PGV_Rev();            //串口接受

    if (len_r ==21)
    {
        PGV_rx_flag = 1;   //接收数据正常
    }

    if (PGV_rx_flag == 1)
    {
        PGV_rx_flag = 0;
        //PGV解算
        if (PGV_AnalyzeData() == 1)
        {
            PGV_Cal_Coor();


            if (!virtual_agv_coor_init_flag)
            {
                virtual_agv_coor_init_flag = 1;
                Virtual_AGV_Current_Coor_InWorld = PGV150_coor;
            }
        }
    }

    //PGV_AnalyzeData();    //数据解算
    CanSendThread(CAN0);
    CanRecvThread(CAN0);  //初始化can0通道
    IMU = MPI204A_Analyze_Data();  //解析imu角度及角速度

}



void Location_AGV()
{
    AGV_Current_Velocity_InAGV.angular_velocity_angle = 0.84 * IMU.angleRate + 0.16 * AGV_Current_Velocity_By_Encoder.angular_velocity_angle;  //角速度加权融合
    //AGV_Current_Velocity_InAGV.angular_velocity_angle = IMU.angleRate;
    AGV_Current_Velocity_InAGV.velocity_x = AGV_Current_Velocity_By_Encoder.velocity_x;
    AGV_Current_Velocity_InAGV.velocity_y = 0;


    if (data_ok == 1)     //相机读取到数据
    {
        data_ok = 0;
        angle = 0.0;
        AGV_Current_Coor_InWorld = PGV150_coor;   //更新全局坐标
        printf("PGV150_coor.x = %f, PGV150_coor.y = %f, PGV150_coor.angle = %f\n", PGV150_coor.x_coor, PGV150_coor.y_coor, PGV150_coor.angle_coor);

    }
    else
    {
        AGV_Current_Coor_InWorld = Odom_Calib(AGV_Current_Velocity_InAGV.velocity_x, AGV_Current_Velocity_InAGV.angular_velocity_angle);    //无二维码部分进行里程推算
        printf("current_Coor_X = %f, current_Coor_Y = %f, current_Coor_Angle = %f\n", AGV_Current_Coor_InWorld.x_coor, AGV_Current_Coor_InWorld.y_coor, AGV_Current_Coor_InWorld.angle_coor);
    }
}

void *PID_fun(void *t)
{
    while (1)
    {
        if (AGV_Target_Coor_InWorld.angle_coor == 180.0 && AGV_Current_Coor_InWorld.angle_coor < 0.0)
            angle_error = AGV_Target_Coor_InWorld.angle_coor - (AGV_Current_Coor_InWorld.angle_coor + 360.0);
        else
            angle_error = AGV_Target_Coor_InWorld.angle_coor - AGV_Current_Coor_InWorld.angle_coor;

        if (AGV_Target_Coor_InWorld.angle_coor == 0.0)
            PID_error = Error_Coor_InAGV.y_coor * 0.55 + angle_error * 0.45;
        else if (AGV_Target_Coor_InWorld.angle_coor == 180.0)
            PID_error = -Error_Coor_InAGV.y_coor * 0.55 + angle_error * 0.45;
        else if (AGV_Target_Coor_InWorld.angle_coor == 90.0)
            PID_error = -Error_Coor_InAGV.x_coor * 0.55 + angle_error * 0.45;
        else if (AGV_Target_Coor_InWorld.angle_coor == -90.0)
            PID_error = Error_Coor_InAGV.x_coor * 0.55 + angle_error * 0.45;

        PID_result = KP * (PID_error - error_next) + KI * PID_error + KD * (PID_error - 2 * error_next + error_last);
        if (PID_result < -100)
        {
            PID_result = -100.0;
        }
        else if (PID_result > 100)
        {
            PID_result = 100.0;
        }
        error_last = error_next;
        error_next = PID_error;
        usleep(20);
        //printf("PID_result = %s\n", PID_result);
    }
}



void get_PID()
{
    int rc;
    long t;
    pthread_t tid;
    rc = pthread_create(&tid, NULL, PID_fun, (void *)t);
    if (rc)
    {
        printf("PID pthread create failed!\n");
        return;
    }
}