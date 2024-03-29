/*
 * Interpolation.c
 *
 *  Created on: Jun 28, 2019
 *      Author: siasunhebo
 */


#include <Interpolation.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//extern struct Interpolation_Parameter_t Interpolation_Parameter;    //插补参数、参数暂存

int Distance_Symbols = 1;  //指示待插补距离的符号
float Interpolation_Distance = 0.0;


float target_velocity = 0.0;
float target_distance = 0.0;

float acc_distance = 0.0;    //加速段距离(mm)
float const_distance = 0.0;  //匀速段距离(mm)
float dec_distance = 0.0;    //减速段距离(mm)
float slowly_distance = 0.0; //慢速段距离(mm)

float acceleration_time = 0.0;   //加速段时间
float const_time = 0.0;          //匀速段时间
float decelaration_time = 0.0;   //减速段时间
float slowly_time = 0.0;         //慢速段时间

Interpolation_State_Enum_m Interpolation_State;

///************************************
// FullName:  Update_Interpolation_Parameter
// Returns:   NULL
// Parameter: struct Interpolation_Parameter_t Input_Para
// Description: 更新待插补参数
//************************************
struct Interpolation_Parameter_t Update_Interpolation_Parameter(struct Interpolation_Parameter_t Input_Para)
{
    struct Interpolation_Parameter_t Interpolation_Parameter;
	Interpolation_Parameter = Input_Para;
}


//************************************
// FullName:  Interpolation_Init
// Returns:   int
// Parameter: float distance
// Description: 插补加减速时间及距离
//************************************
int Interpolation_Init(float distance, struct Interpolation_Parameter_t Interpolation_Parameter)
{
	float slowly_distance_temp = Interpolation_Parameter.slow_time_abs * Interpolation_Parameter.min_velocity_abs;   //计算最低速移动距离

    //加减速段移动距离，用来判断是否存在匀速段
	float distance_acc_dec_temp = (Interpolation_Parameter.max_velocity_abs * Interpolation_Parameter.max_velocity_abs - Interpolation_Parameter.min_velocity_abs * Interpolation_Parameter.min_velocity_abs) /Interpolation_Parameter.accleration_abs;
//	Distance_Symbols = (distance > 0.0 ? 1 : -1);
	Interpolation_Distance = distance;
    float input_distance_abs  = distance;
    input_distance_abs *= Distance_Symbols;

    //复位目标距离和速度
    target_velocity = target_distance = 0.0;

    //复位参数值
    acceleration_time = const_time = decelaration_time = slowly_time = 0.0;
    acc_distance = const_distance = dec_distance = slowly_distance = 0.0;

    printf("input_distance_abs = %f , slowly_distance_temp = %f , distance_acc_dec_temp = %f\n ", input_distance_abs, slowly_distance_temp, distance_acc_dec_temp);

    input_distance_abs -= slowly_distance_temp;    //先减去慢速段距离

    if ( input_distance_abs < 0)    //只有慢速段
    {
    	slowly_distance = input_distance_abs + slowly_distance_temp;    //慢速段移动距离
    	slowly_time = slowly_distance / Interpolation_Parameter.min_velocity_abs;    //慢速段时间(s)
        Interpolation_Parameter.max_velocity_abs = Interpolation_Parameter.min_velocity_abs;    //只有最低速
        printf("Only slow!!!!!!!!\n");
    }

   // else if ( input_distance_abs < distance_acc_dec_temp )    //不存在匀速段
   else if (( input_distance_abs +  slowly_distance_temp) < distance_acc_dec_temp )    //不存在匀速段
   //else if (( input_distance_abs +  slowly_distance_temp) < distance_acc_dec_temp && input_distance_abs > 0)    //不存在匀速段
    {
    	//计算加减速时间
    	Interpolation_Parameter.max_velocity_abs = sqrtf( ( input_distance_abs +  slowly_distance_temp) * Interpolation_Parameter.accleration_abs + Interpolation_Parameter.min_velocity_abs * Interpolation_Parameter.min_velocity_abs);
    	acceleration_time = (Interpolation_Parameter.max_velocity_abs - Interpolation_Parameter.min_velocity_abs) / Interpolation_Parameter.accleration_abs;
    	decelaration_time = acceleration_time;
    	printf("Acceleration deceleration!!!!!!!!!!\n");
    }
   else   //存在匀速段
  // else if (( input_distance_abs +  slowly_distance_temp) > distance_acc_dec_temp )     //存在匀速段
    {
    	//加减速时间
    	decelaration_time = acceleration_time = (Interpolation_Parameter.max_velocity_abs - Interpolation_Parameter.min_velocity_abs) / Interpolation_Parameter.accleration_abs;
    	//匀速时间
    	//const_time = (input_distance_abs  - distance_acc_dec_temp) / Interpolation_Parameter.max_velocity_abs;    //匀速时间
        printf("Acceleration and uniform deceleration!!!!!!!!!!!!!!!!!!\n");
    }

    //根据插补结果，圆整加减速、匀速时间,得到相应距离
    decelaration_time = acceleration_time = (unsigned long)((Interpolation_Parameter.max_velocity_abs - Interpolation_Parameter.min_velocity_abs) / Interpolation_Parameter.accleration_abs * 1000.0) / 1000.0;
    Interpolation_Parameter.max_velocity_abs = Interpolation_Parameter.min_velocity_abs + acceleration_time * Interpolation_Parameter.accleration_abs;
    //const_time = (unsigned long)(const_time * 1000.0) / 1000.0;
    acc_distance = dec_distance = (Interpolation_Parameter.max_velocity_abs * Interpolation_Parameter.max_velocity_abs - Interpolation_Parameter.min_velocity_abs * Interpolation_Parameter.min_velocity_abs) / ( Interpolation_Parameter.accleration_abs * 2.0) ;
    //acc_distance = dec_distance = (Interpolation_Parameter.max_velocity_abs + Interpolation_Parameter.min_velocity_abs) * (acceleration_time * 1.0) / 2.0;    //计算加减速距离
    //const_distance =( input_distance_abs + slowly_distance_temp ) - acc_distance - dec_distance ;  //计算匀速距离
    // const_distance = Interpolation_Parameter.max_velocity_abs * const_time;    //计算匀速距离
     const_distance = input_distance_abs  - distance_acc_dec_temp;    //计算匀速距离
     slowly_distance = input_distance_abs + slowly_distance_temp - acc_distance - dec_distance - const_distance;    //低速位移
     slowly_time = slowly_distance / Interpolation_Parameter.min_velocity_abs;    //计算总的慢速时间

   //printf(" max_velocity_abs = %f , min_velocity_abs = %f\n ", Interpolation_Parameter.max_velocity_abs , Interpolation_Parameter.min_velocity_abs);
    return 1;

}


//************************************
// FullName:  Interpolation_Cal_Velocity
// Returns:   int
// Parameter: float current_distance
// Description: 根据距离插补速度
//************************************
int Interpolation_Cal_Velocity(float current_distance, struct Interpolation_Parameter_t Interpolation_Parameter)
{
    Interpolation_State = IS_Interpolating;    //正在插补中
	current_distance *= Distance_Symbols;
	printf(" Interpolation_Parameter.max_velocity_abs = %f , current_distance = %f , acc_distance = %f , const_distance = %f  , slowly_distance = %f \n" , Interpolation_Parameter.max_velocity_abs , current_distance , acc_distance , const_distance , slowly_distance);
	//获取插补速度
	if (current_distance < 0.0)
	{
		target_velocity = Interpolation_Parameter.min_velocity_abs * Distance_Symbols;
        printf("11111         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	else if (current_distance < acc_distance)    //在加速区
	{
		target_velocity = sqrtf(2 * current_distance * Interpolation_Parameter.accleration_abs + Interpolation_Parameter.min_velocity_abs * Interpolation_Parameter.min_velocity_abs) * Distance_Symbols;
        printf("22222         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	else if (current_distance < (acc_distance + const_distance))    //在匀速区
	{
		target_velocity = Interpolation_Parameter.max_velocity_abs * Distance_Symbols;
		  printf("33333         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	else if (current_distance < (acc_distance + const_distance + dec_distance - 10 ))    //在减速区
	{
		target_velocity = sqrtf(Interpolation_Parameter.max_velocity_abs * Interpolation_Parameter.max_velocity_abs - 2 *  (current_distance - acc_distance - const_distance) * Interpolation_Parameter.accleration_abs) * Distance_Symbols;
		   printf("44444         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	else if (current_distance < (acc_distance + const_distance + dec_distance + slowly_distance - 10 ))
	{
		target_velocity = Interpolation_Parameter.min_velocity_abs * Distance_Symbols;
		   printf("55555         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	/*else if (current_distance > (acc_distance + const_distance + dec_distance + slowly_distance) )
    //else if (current_distance > (acc_distance + const_distance + dec_distance + slowly_distance) && fabs(current_distance - Current_Coor_InOrigin.x_coor) < 7 && fabs(current_distance - Current_Coor_InOrigin.y_coor)  < 7)
	{
		target_velocity = -Interpolation_Parameter.min_velocity_abs * Distance_Symbols;
	}
	*/
	else
	{
		float temp = abs(acc_distance + const_distance + dec_distance + slowly_distance - current_distance);
		target_velocity = 0.0;
		target_distance = 0.0;
		Interpolation_State = IS_Interpolated;
		   printf("66666        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}

	return (Interpolation_State != IS_Interpolated);    //返回插补结果，插补成功返回0
}






