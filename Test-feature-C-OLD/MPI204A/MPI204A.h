/*
 * MPI204A.h
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */

#ifndef MPI204A_H_
#define MPI204A_H_

#include <can.h>

typedef struct MPI204A
{
    float angle;
    float angleRate;
}MPI204A_t;

void MPI204A_init();
MPI204A_t MPI204A_Analyze_Data();

#endif /* MPI204A_H_ */
