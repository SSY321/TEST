//
// Created by maxiao on 8/26/19.
//

#ifndef _MOTION_CTRL_H
#define _MOTION_CTRL_H

/*--------------------------------action mode define---------------------------------*/
#define ACTION_MODE_STOP        MOTIONSTATE_ONSTOPING
#define ACTION_MODE_GOAHEAD     MOTIONSTATE_GOSTRAIGHT
#define ACTION_MODE_GOBACK      MOTIONSTATE_GOBACKWARD
#define ACTION_MODE_TRUNLEFT    MOTIONSTATE_TRUNLEFT
#define ACTION_MODE_TRUNRIGHT   MOTIONSTATE_TRUNRIGHT


typedef enum
{
    MOTIONSTATE_ONSTOPING = 0,
    MOTIONSTATE_GOSTRAIGHT = 1,
    MOTIONSTATE_TRUNLEFT = 2,
    MOTIONSTATE_TRUNRIGHT = 3,
    MOTIONSTATE_ONLOADING = 4,
    MOTIONSTATE_GOBACKWARD = 5,
} motion_state_m;




int Run_Movement_Class(Coordinate_Class_t Current_Coor, struct Interpolation_Parameter_t Interpolation_Parameter_temp);
void Process_Movement_Command();
void Movement_Control();


#endif //IMUTEST_MOTION_CTRL_H
