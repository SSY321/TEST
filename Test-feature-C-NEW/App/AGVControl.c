//
// Created by maxaio on 8/26/19.
//

#include <stdint-gcc.h>
#include <include/user_function.h>
#include <common.h>
#include <main.h>
#include "AGV/agv.h"
#include "motion_ctrl.h"
#include "Tools/Tools.h"
#include "AGVControl.h"


extern Velocity_Class_t Target_Velocity_InAGV;     //目标速度
extern struct Coordinate_Class AGV_Current_Coor_InWorld;
extern Interpolation_State_Enum_m Interpolation_State;

extern Coordinate_Class_t Error_Coor_InAGV;                      //偏差坐标

static uint16_t nEvent = NO_EVENT;    //当前所响应的事件


static uint16_t OutOfTrack = 0x00;    //脱轨，默认为不脱轨
static uint16_t GSMissing  = 0x00;    //导航信息丢失

uint16_t twoDMissingCnt;
extern float wControl;   //旋转角速度

uint16_t nAgvWorkMode = AGV_MODE_STANDBY;

extern int Add_Command_Rotate;

extern float VL_1200;    //发送给PLC左轮速度
extern float VR_1200;    //发送给PLC右轮速度




void AGV_RUN()
{
    switch (nAgvWorkMode)
    {
        case AGV_MODE_STANDBY:
            AGV_StandBy();
            break;
        case AGV_MODE_RUNNING:
            AGV_Running();
            break;
        case AGV_MODE_SUSPENDED:
            AGV_Suspended();
            break;
        case AGV_MODE_OP:
            AGV_Op();
            break;
        default:
            break;
    }

}

//待机操作函数
void AGV_StandBy()
{
    nAgvWorkMode = AGV_MODE_RUNNING;

    //ToDo: 各状态及变量清零
}

//手动操作函数
void AGV_Op()
{

}


//挂起函数
void AGV_Suspended()
{
    //ToDo: 关掉驱动器使能，速度清零
    switch (nEvent)
    {
        case OUT_OF_TRACK_EVENT:      //如果脱轨，检查是否回归坐标点
        {
            if (OutOfTrack == 0x00)
            {
                nAgvWorkMode = AGV_MODE_RUNNING;
                //ToDo: 速度等参数清零
            }
            break;
        }
        case GS_COMM_BREAK_EVENT:    //如果导航通讯中断
        {
            if (GSMissing == 0x00)
            {
                nAgvWorkMode = AGV_MODE_RUNNING;
                //ToDo: 速度等参数清零
            }
            break;
        }
        case CONTROL_RESUMABLE_STOP_EVENT:     //如果网络中断
        {
            if (ethCommBreakFlag == 0x00)
            {
                nAgvWorkMode = AGV_MODE_RUNNING;
                //ToDo: 速度等参数清零
            }
            break;
        }
        //驱动器
        case SERVO_INVALID_EVENT:            //如果伺服失效
        {
            //ToDo: 判断伺服是否恢复 速度等参数清零
            break;
        }
        default:
            break;
    }
}


void AGV_Running()
{
    if (NO_EVENT == nEvent)
    {
        if (ethCommBreakFlag == 0x01)    //通讯中断
        {
            nEvent = CONTROL_RESUMABLE_STOP_EVENT;     //可恢复停车标志位
        }
        else if (OutOfTrack == 0x01)     //检查导航脱轨事件
        {
            nEvent = OUT_OF_TRACK_EVENT;
        }
        else if (GSMissing == 0x01)      //检查地标点是否丢失
        {
            nEvent = GS_COMM_BREAK_EVENT;
        }

        else
        {
/*********************************************直行****************************************************/
            if ((Motionstyle == ACTION_MODE_GOAHEAD) || (Motionstyle == ACTION_MODE_GOBACK))
            {
                UpdateAgvHeaderDirToNow();
                Process_Movement_Command();
                Movement_Control();
                if (Target_Velocity_InAGV.velocity_x > 1.0)
                {
                    ++ twoDMissingCnt;
                    if (twoDMissingCnt > 100)
                    {
                        OutOfTrack = 0x01;

                    }

                }
            }
/********************************************左转******************************************************/
            else if (Motionstyle == ACTION_MODE_TRUNLEFT)
            {
                UpdateAgvHeaderDirToNow();
                Process_Movement_Command();
                Movement_Control();

                if ((Error_Coor_InAGV.angle_coor <= 10.0) && (Error_Coor_InAGV.angle_coor > -10.0))
                {
                    VL_1200 = 0.0;
                    VR_1200 = 0.0;
                }
                else
                {
                    wControl = 0.45;
                    VL_1200 =  wControl * wheel_distance / 2.0;
                    VR_1200 = -wControl * wheel_distance / 2.0;
                    //VL_1200 =  130.905;
                    //VR_1200 = -130.905;

                }

                if (wControl > 0.4)
                {
                    ++ twoDMissingCnt;
                    if (twoDMissingCnt > 100)
                    {
                        OutOfTrack = 0x01;
                    }
                    //ToDo: 旋转速度插补 Rotate_Process_Movement_Command()
                    //ToDo: 旋转运动控制 Rotate_Movemnet_control_left()
                }


            }
/********************************************右转******************************************************/
            else if (Motionstyle == ACTION_MODE_TRUNRIGHT)
            {
                UpdateAgvHeaderDirToNow();
                Process_Movement_Command();
                Movement_Control();

                if ((Error_Coor_InAGV.angle_coor <= 10.0) && (Error_Coor_InAGV.angle_coor > -10.0))
                {
                    VL_1200 = 0.0;
                    VR_1200 = 0.0;
                }
                else
                {
                    wControl = 0.45;
                    VL_1200 = -wControl * wheel_distance / 2.0;
                    VR_1200 = wControl * wheel_distance / 2.0;
                    //VL_1200 =  130.905;
                    //VR_1200 = -130.905;

                }
                if (wControl > 0.4)
                {
                    ++ twoDMissingCnt;
                    if (twoDMissingCnt > 100)
                    {
                        OutOfTrack = 0x01;
                    }
                    //ToDo: 旋转速度插补 Rotate_Process_Movement_Command()
                    //ToDo: 旋转运动控制 Rotate_Movemnet_control_Right()
                }

            }
        }
    }
    else if (nEvent != NO_EVENT)
    {
        if ((nEvent == GS_COMM_BREAK_EVENT) && ((nEvent == ACTION_MODE_TRUNRIGHT) || (nEvent == ACTION_MODE_TRUNLEFT)))
        {

            Motionstyle = ACTION_MODE_STOP;
        }
        //恢复直行
        if ((nEvent == ACTION_MODE_GOAHEAD) || (nEvent == ACTION_MODE_GOBACK))
        {
            UpdateAgvHeaderDirToNow();
            Process_Movement_Command();
            if (Target_Velocity_InAGV.velocity_x > 1.0)
            {
                ++ twoDMissingCnt;
                if (twoDMissingCnt > 100)
                {
                    OutOfTrack = 0x01;

                }
                Process_Movement_Command();
                //ToDo: 前进后退速度相反
                //ToDo: 运动控制 Movement_control()
            }
        }

    }



}