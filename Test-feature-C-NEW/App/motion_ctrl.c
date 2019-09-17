//
// Created by maxiao on 8/26/19.
//

#include "stdio.h"
#include "stdlib.h"
#include <stdint-gcc.h>
#include <common.h>
#include <main.h>
#include "motion_ctrl.h"
#include "Movement.h"

uint16_t ethCommBreakFlag = 0x00;
uint16_t Motionstyle      = MOTIONSTATE_ONSTOPING;

extern int virtual_agv_coor_init_flag;
extern struct Coordinate_Class Virtual_AGV_Current_Coor_InWorld;
struct Interpolation_Parameter_t Interpolation_Parameter_m;
extern Coordinate_Class_t Destination_Coor_InWorld;     //目标位置
Velocity_Class_t Virtual_AGV_Current_Velocity_InAGV;
extern Velocity_Class_t Target_Velocity_InAGV;     //目标速度
extern Coordinate_Class_t Target_Coor_InWorld;
struct Interpolation_Parameter_t Interpolation_Parameter;    //插补参数、参数暂存
extern struct Coordinate_Class AGV_Current_Coor_InWorld;

extern int Add_Command_Line, Add_Command_Rotate;

extern Coordinate_Class_t AGV_Target_Coor_InWorld;               //AGV在世界坐标系下的期望坐标
Velocity_Class_t AGV_Target_Velocity_InAGV;               //AGV在AGV坐标系下的期望速度
extern Coordinate_Class_t Error_Coor_InAGV;                      //偏差坐标

extern float VL_1200;    //发送给PLC左轮速度
extern float VR_1200;    //发送给PLC右轮速度

float wControl;   //旋转角速度

extern float PID_result;

enum Interpolation_State_Enum State;

int Run_Movement_Class(Coordinate_Class_t Current_Coor, struct Interpolation_Parameter_t Interpolation_Parameter_temp)
{
    struct Coordinate_Class Coor = virtual_agv_coor_init_flag ? Virtual_AGV_Current_Coor_InWorld : Current_Coor;
    switch (State)
    {
        case No_Interpolation:  //未插补
            if (Movement_Init(Coor, Interpolation_Parameter_m, Interpolation_Parameter_temp) == 1)
            {
                State = IS_Interpolating;    //IS_Interpolating
            }
            else if (Movement_Init(Coor, Interpolation_Parameter_m, Interpolation_Parameter_temp) == -1)
            {
                State = No_Interpolation;
            }
            else
            {
                State = IS_Interpolated;   //IS_Interpolated
                return 1;    //插补失败，要移动距离小于阈值
            }
            break;
        case IS_Interpolating:   //插补中
            if (!Movement_Cal_Velocity(Current_Coor, Interpolation_Parameter_temp))    //插补完成
            {
                State = IS_Interpolated;
                Virtual_AGV_Current_Coor_InWorld = Destination_Coor_InWorld;
                Virtual_AGV_Current_Velocity_InAGV = Target_Velocity_InAGV;

            }
            else   //还在插补，获取虚拟的坐标和速度
            {
                Virtual_AGV_Current_Coor_InWorld = Target_Coor_InWorld;
                Virtual_AGV_Current_Velocity_InAGV = Target_Velocity_InAGV;
            }
            break;
        default:
            break;


    }
    return 0;

}


void Process_Movement_Command()
{
    static enum Interpolation_State_Enum Interpolation_State;
    static int Is_Parsing_Movement = 0;     //指示当前是否在执行运动指令
    if (!Is_Parsing_Movement)
    {
        Is_Parsing_Movement = !Run_Movement_Class(AGV_Current_Coor_InWorld, Interpolation_Parameter);   //执行运动指令并返回结果

    }
    else
    {
        Is_Parsing_Movement = !Run_Movement_Class(AGV_Current_Coor_InWorld, Interpolation_Parameter);    //执行运动指令并返回结果
    }
}


void Movement_Control()
{

    float vx;

    int stop;


    if ((Add_Command_Line == 1) || (Add_Command_Rotate == 1))
    {
        AGV_Target_Coor_InWorld = Destination_Coor_InWorld;    //获取期望坐标
        AGV_Target_Velocity_InAGV = Virtual_AGV_Current_Velocity_InAGV;    //获取期望速
    }
    else
    {
        AGV_Target_Coor_InWorld = AGV_Current_Coor_InWorld;    //期望坐标是当前坐标
        AGV_Target_Velocity_InAGV.velocity_x = 0.0;
        AGV_Target_Velocity_InAGV.velocity_y = 0.0;
        AGV_Target_Velocity_InAGV.angular_velocity_rad = 0.0;
        AGV_Target_Velocity_InAGV.angular_velocity_angle = 0.0;
        AGV_Target_Velocity_InAGV.angular_velocity_mm = 0.0;
    }
    Error_Coor_InAGV.x_coor = AGV_Target_Coor_InWorld.x_coor - AGV_Current_Coor_InWorld.x_coor;
    Error_Coor_InAGV.y_coor = AGV_Target_Coor_InWorld.y_coor - AGV_Current_Coor_InWorld.y_coor;
    if (AGV_Current_Coor_InWorld.angle_coor >= 0.0)
    {
        Error_Coor_InAGV.angle_coor = AGV_Target_Coor_InWorld.angle_coor - AGV_Current_Coor_InWorld.angle_coor;
    }
    else if (AGV_Current_Coor_InWorld.angle_coor < 0.0 && AGV_Current_Coor_InWorld.angle_coor >= -100.0)
    {
        Error_Coor_InAGV.angle_coor = AGV_Target_Coor_InWorld.angle_coor - AGV_Current_Coor_InWorld.angle_coor;
    }
    else if (AGV_Current_Coor_InWorld.angle_coor < -100.0 && AGV_Current_Coor_InWorld.angle_coor >= -180)
    {
        Error_Coor_InAGV.angle_coor = AGV_Target_Coor_InWorld.angle_coor - abs(AGV_Current_Coor_InWorld.angle_coor);
    }
    Error_Coor_InAGV.angle_coor = abs(Error_Coor_InAGV.angle_coor);

    printf("Err_Angle = %f\n", Error_Coor_InAGV.angle_coor);

    vx = abs(AGV_Target_Velocity_InAGV.velocity_x);

    printf("vx = %f\n", vx);

    //如果直行
    if (Add_Command_Line == 1)
    {
        if ((Error_Coor_InAGV.x_coor < 7.0) && (Error_Coor_InAGV.y_coor < 7.0) && (Error_Coor_InAGV.angle_coor < 10.0) && (Error_Coor_InAGV.x_coor > -7.0) && (Error_Coor_InAGV.y_coor > -7.0))
        {
            VL_1200 = 0.0;
            VR_1200 = 0.0;
            stop = 1;
        }
        else
        {
            VL_1200 = vx + PID_result;
            VR_1200 = vx - PID_result;
            stop = 0;
        }
    }

/*
        //如果旋转
    else if (Add_Command_Rotate == 1)
    {
        if ((Error_Coor_InAGV.angle_coor <= 10.0) && (Error_Coor_InAGV.angle_coor > -10.0))
        {
            VL_1200 = 0.0;
            VR_1200 = 0.0;
            stop = 1;
        }
        else
        {
            wControl = 0.45;
            VL_1200 =  wControl * wheel_distance / 2.0;
            VR_1200 = -wControl * wheel_distance / 2.0;
            //VL_1200 =  130.905;
            //VR_1200 = -130.905;
            stop = 0;
        }
    }
*/
}