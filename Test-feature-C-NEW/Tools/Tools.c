//
// Created by maxiao on 8/25/19.
//

#include <include/user_function.h>
#include "Tools.h"
#include "PGV150.h"

uint16_t AGV_Head_Dir = X_FORWARD;   //车头方向，默认与x轴正方形同向

void UpdateAgvHeaderDirToNow()
{
    if ((angle_deviation < 45.0) || ((angle_deviation > 315.0) && (angle_deviation < 360.0)))
    {
        AGV_Head_Dir = X_FORWARD;
    }
    else if ((angle_deviation > 45.0) && (angle_deviation < 135.0))
    {
        AGV_Head_Dir = Y_FORWARD;
    }
    else if ((angle_deviation > 135.0) && (angle_deviation <225.0))
    {
        AGV_Head_Dir = X_BACKWARD;
    }
    else if ((angle_deviation > 225.0) && (angle_deviation < 315.0))
    {
        AGV_Head_Dir = Y_BACKWARD;
    }
}