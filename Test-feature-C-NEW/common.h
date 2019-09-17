/*
 * common.h
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */

#ifndef COMMON_H_
#define COMMON_H_



#define _BV(n) (1 << n)
#define time_s  0.01
#define M_PI 3.1415926
#define wheel_distance 581.8

#define KP 400
#define KI 1.1
#define KD 300



typedef struct Coordinate_Class
{
	float x_coor;	//x坐标(mm)
	float y_coor;   //y坐标(mm)
	float angle_coor; //角度坐标(°)
	float angle_rad;	//角度坐标(弧度)

}Coordinate_Class_t;

typedef struct Velocity_Class
{
	float velocity_x;   //x方向速度 mm/s
	float velocity_y;   //y方向速度 mm/s
	float angular_velocity_rad;   //选择角速度rad/s
	float angular_velocity_angle;  //旋转角速度°/s
	float angular_velocity_mm;     //旋转角速度mm/s

}Velocity_Class_t;

typedef struct wheel_paramter
{
    float wheel_max_line_velocity;
    float wheel_min_line_velocity;
    float wheel_acceleration_line_velocity;    //车轮最大加减速度
    float line_slowest_time;                   //最低速移动时间
    float motor_max_rotationl_velocity_soft;   //软件定义电机最高转速3000rpm/min
    float motor_min_rotationl_velocity_soft;   //软件定义点击最低转速100rpm/min
    float WHEEL_DIAMETER;                      //轮径
    float REDUCATION_RATIO;                    //减速比
    float wheel_acceleration_time;             //车轮最大加减速时间(从最低速到最高速)s

}wheel_paramter_t;



#endif /* COMMON_H_ */
