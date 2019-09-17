/*
 * main.c
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */


#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <can.h>
#include <MPI204A.h>
#include <time.h>
#include <signal.h>
#include <Timer.h>
#include <PGV150.h>
#include <common.h>
#include <Movement.h>
#include <1200.h>
#include <TCP/TCP.h>
#include <MPI204A.h>
#include <zconf.h>
#include <windows.h>

extern struct Coordinate_Class PGV150_coor;

extern canBus_t *CAN0;
//struct Coordinate_Class AGV_Current_Coor_InWorld;
//struct Coordinate_Class Virtual_AGV_Current_Coor_InWorld; //虚拟的AGV坐标用于路径跟随
Coordinate_Class_t Destination_Coor;    //目标坐标
Velocity_Class_t Virtual_AGV_Current_Velocity_InAGV;    //虚拟的AGV速度，用于运动控制
extern Velocity_Class_t Target_Velocity_InAGV;     //目标速度

extern Coordinate_Class_t Target_Coor_InWorld;

Coordinate_Class_t AGV_Target_Coor_InWorld;               //AGV在世界坐标系下的期望坐标

Coordinate_Class_t Error_Coor_InAGV;   //偏差坐标

Velocity_Class_t AGV_Target_Velocity_InAGV;               //AGV在AGV坐标系下的期望速度

MPI204A_t IMU;

struct Velocity_Class AGV_Current_Velocity_By_Encoder;    //PLC上传由编码器得到速度
struct Velocity_Class AGV_Current_Velocity_InAGV;         //AGV当前速度
extern Interpolation_State_Enum_m Interpolation_State;

extern int command_x;
extern int command_y;
extern int command_angle;

struct Interpolation_Parameter_t Interpolation_Parameter;    //插补参数、参数暂存

//extern char revbuf[LENGTH];     //PLC接收数组
char buffer[8];    //发送PLC数组
char sendbuf[8] = {0x12,0x34,0x56,0x78,0x12,0x34,0x56,0x78};
float VL_1200;    //发送给PLC左轮速度
float VR_1200;    //发送给PLC右轮速度
PLC_DATA_t Get_PLC_DATA;

Coordinate_Class_t Destination_Coor_InWorld;     //目标位置


float MPI204A_Angle = 0.0;   //IMU角度值
float MPI204A_AngleRate = 0.0;    //IMU角速度
int virtual_agv_coor_init_flag = 0;   //1为地标更新
struct Interpolation_Parameter_t Interpolation_Parameter_m;
wheel_paramter_t wheelParamter;

int PGV_rx_flag;
extern int len_r;


float angle_error;    //角度误差
float PID_error, error_last, error_next;      //PID误差输入
float PID_result = 0.0;


int Add_Command_Line, Add_Command_Rotate;

extern float angle;







void paramter_define()
{

    wheelParamter.line_slowest_time = 0.63;
    wheelParamter.motor_max_rotationl_velocity_soft = 1400.0;
    wheelParamter.motor_min_rotationl_velocity_soft = 1000.0;
    wheelParamter.WHEEL_DIAMETER = 150.0;
    wheelParamter.REDUCATION_RATIO = 15.0;
    wheelParamter.wheel_acceleration_time = 0.6366;

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
	//	printf("IMU.angleRate = %f , AGV_Current_Velocity_By_Encoder.angular_velocity_angle = %f" ,  IMU.angleRate , AGV_Current_Velocity_By_Encoder.angular_velocity_angle);
    //AGV_Current_Velocity_InAGV.angular_velocity_angle = IMU.angleRate;
	AGV_Current_Velocity_InAGV.velocity_x = AGV_Current_Velocity_By_Encoder.velocity_x;
	AGV_Current_Velocity_InAGV.velocity_y = 0;

	if (data_ok == 1)     //相机读取到数据
	{
        data_ok = 0;
        angle = 0.0;
       //Coordinate_Class Coor_delta = {0};
        Coor_delta.x_coor = 0 ;
        Coor_delta.y_coor = 0 ;
        Coor_delta.angle_coor = 0 ;
        Coor_delta.angle_rad = 0 ;
		AGV_Current_Coor_InWorld = PGV150_coor;   //更新全局坐标
        printf("11111111PGV150_coor.x = %f, PGV150_coor.y = %f, PGV150_coor.angle = %f\n", PGV150_coor.x_coor, PGV150_coor.y_coor, PGV150_coor.angle_coor);

	}
	else
	{
		AGV_Current_Coor_InWorld = Odom_Calib(AGV_Current_Velocity_InAGV.velocity_x, AGV_Current_Velocity_InAGV.angular_velocity_angle);    //无二维码部分进行里程推算
		printf("22222222current_Coor_X = %f, current_Coor_Y = %f, current_Coor_Angle = %f\n", AGV_Current_Coor_InWorld.x_coor, AGV_Current_Coor_InWorld.y_coor, AGV_Current_Coor_InWorld.angle_coor);
	}
}
enum Interpolation_State_Enum State;


int Run_Movement_Class(Coordinate_Class_t Current_Coor, struct Interpolation_Parameter_t Interpolation_Parameter)
{
	struct Coordinate_Class Coor = virtual_agv_coor_init_flag ? Virtual_AGV_Current_Coor_InWorld : Current_Coor;
	//printf("Virtual_AGV_Current_Coor_InWorld.x_coor = %f , Virtual_AGV_Current_Coor_InWorld.y_coor = %f \n", Virtual_AGV_Current_Coor_InWorld.x_coor ,  Virtual_AGV_Current_Coor_InWorld.y_coor);
	//printf("flag = %d\n", virtual_agv_coor_init_flag);
	/*if ( fabs(PGV150_coor.x_coor - Destination_Coor.x_coor ) <10  &&  fabs( PGV150_coor.y_coor - Destination_Coor.y_coor ) <10 )
		virtual_agv_coor_init_flag = 0;
	else
		virtual_agv_coor_init_flag = 1;
   */
    if ((Add_Command_Line == 1) )
    {
    switch (State)
    {
    case No_Interpolation:  //未插补
    	if (Movement_Init(Coor, Interpolation_Parameter_m, Interpolation_Parameter) == 1)
    	{
    		State = IS_Interpolating;    //IS_Interpolating
    	}
    	else if (Movement_Init(Coor, Interpolation_Parameter_m, Interpolation_Parameter) == -1)
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
    	if (!Movement_Cal_Velocity(Current_Coor, Interpolation_Parameter))    //插补完成
    	{
    		State = No_Interpolation;
    		virtual_agv_coor_init_flag = 0;
    		current_distance = 0;
    		//Virtual_AGV_Current_Coor_InWorld = Destination_Coor_InWorld;
            Virtual_AGV_Current_Velocity_InAGV = Target_Velocity_InAGV;

    	}
    	else   //还在插补，获取虚拟的坐标和速度
    	{
    		//Virtual_AGV_Current_Coor_InWorld = Target_Coor_InWorld;
            Virtual_AGV_Current_Velocity_InAGV = Target_Velocity_InAGV;
    	}
    	break;
    default:
    	break;
    }
    }
    else
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





//由触摸屏得到目标坐标
void Gcode_G0()
{
    Destination_Coor.x_coor = command_x * 1.0;
    Destination_Coor.y_coor = command_y * 1.0;
    Destination_Coor.angle_coor = command_angle * 1.0;
    //printf("Destination_Coor_X = %f, Destination_Coor_Y = %f, Destination_Coor_A = %f\n", Destination_Coor.x_coor, Destination_Coor.y_coor, Destination_Coor.angle_coor);
    //printf("C_X = %d, C_Y = %d, C_A = %d\n", command_x, command_y, command_angle);


    //Coordinate_Class_t Destination_Coor_temp;
    if ((Add_Command_Line == 1) || (Add_Command_Rotate == 1))
    {
        //Destination_Coor_temp = Destination_Coor;
        Destination_Coor_InWorld = Destination_Coor;
        Interpolation_State = No_Interpolation;
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

    //printf("Err_Angle = %f\n", Error_Coor_InAGV.angle_coor);

    vx = abs(Target_Velocity_InAGV.velocity_x);

    //printf("vx = %f\n", vx);

    //如果直行
    if (Add_Command_Line == 1)
    {
        if ((Error_Coor_InAGV.x_coor < 20.0) && (Error_Coor_InAGV.y_coor < 10.0) && (Error_Coor_InAGV.angle_coor < 10.0) && (Error_Coor_InAGV.x_coor > -20.0) && (Error_Coor_InAGV.y_coor > -10.0))
        {

            VL_1200 = 0.0;
            VR_1200 = 0.0;
            stop = 1;

        //virtual_agv_coor_init_flag = 0;
        }
        else
        {
            VL_1200 = vx  + PID_result;
            VR_1200 = vx  -  PID_result;
            stop = 0;
            //printf("PID_result = %f " , PID_result);
        }
    }
    //如果旋转
    else if (Add_Command_Rotate == 1)
    {
        if ((Error_Coor_InAGV.angle_coor <= 10.0) && (Error_Coor_InAGV.angle_coor > -10.0))
        {
            VL_1200 = 0.0;
            VR_1200 = 0.0;
            stop = 1;
    //    virtual_agv_coor_init_flag = 0;
        }
        else
        {
            VL_1200 =  130.905;
            VR_1200 = -130.905;
            stop = 0;
        }
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










int main ()
{
	/*static struct Coordinate_Class
	{
		float x_coor;	//x坐标(mm)
		float y_coor;   //y坐标(mm)
		float angle_coor; //角度坐标(°)
		float angle_rad;	//角度坐标(弧度)
	}Coor_delta;    //设置结构体Coor_delta为静态结构变量，用于累加
	*/
	//	static struct Coordinate_Class Coor_delta;


//	setvbuf(stdout,(char*)NULL, _IONBF, 0);

	int temp;
	typedef union{
		int fdata;
		char ldata[4];
	}VL;
	typedef union{
		int fdata;
		char ldata[4];
	}VR;

	VR Vr;
	VL Vl;

	VR_1200 = 0.0;
	VL_1200 = 0.0;
	//float list[2] = {12.3, 54.2};
	enum Interpolation_State_Enum State;

    paramter_define();

	MPI204A_init();      //IMU初始化
	//pthread_t thread;
	PGV_init();          //相机初始化
    get_PLC_init();      //1200数据接收初始化

    //pthread_t t_id;
    get_pthread();
    get_PID();
    //get_PGV_Send_Pthread();
    //get_PGV_Rcv_Pthread();
    //vTimerPthreadInit();
	while(1)
	{
	    printf("VL_1200=%d, VR_1200=%d\n", Vl.fdata, Vr.fdata);
        Vl.fdata = (int)(VL_1200 * 1000);
        Vr.fdata = (int)(VR_1200 * 1000);
        buffer[0] = Vl.ldata[0];
        buffer[1] = Vl.ldata[1];
        buffer[2] = Vl.ldata[2];
        buffer[3] = Vl.ldata[3];
        buffer[4] = Vr.ldata[0];
        buffer[5] = Vr.ldata[1];
        buffer[6] = Vr.ldata[2];
        buffer[7] = Vr.ldata[3];



		get_PLC_Data();

        Gcode_G0();  //获取坐标值

		Prase_Sensor_Data();    //传感器处理
		Location_AGV();         //AGV定位
        Process_Movement_Command();
       //printf("virtual_agv_coor_init_flag = %d\n ", virtual_agv_coor_init_flag);
		//Run_Movement_Class(Interpolation_State, AGV_Current_Coor_InWorld, Interpolation_Parameter);
        Movement_Control();

	}

}
