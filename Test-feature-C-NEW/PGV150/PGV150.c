/*
 * PGV150.c
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */


#include <UART.h>
#include <stdio.h>
#include <PGV150.h>
#include <zconf.h>
#include <pthread.h>
#include <string.h>


int fd, fd_r;    //文件描述符
int err, err_r;                           //返回调用函数的状态
char PGV_Send_buff[2] = {0};
char PGV_Rev_buff[21];
static int len_s = 0;
int len_r;
extern int PGV_rx_flag;

struct Coordinate_Class PGV150_coor;
//用来存放每一次读到的数据
char read_data[21] = {0};
//存放一个完整的数据帧，方便处理
char read_buff[21] = {0};

char cmd_buff[21];


int len_sum = 0;





void PGV_init()
{
	fd = open_com_dev(DEV_UART);
    fd_r = open_com_dev(DEV_UART);
    //fd_r = fd;

	do{
		err = init_com_dev(fd,115200,8,'e',1);
		printf("Set Port Exactly!\n");
		} while(0 == err || 0 == fd);

    do{
        err_r = init_com_dev(fd_r,115200,8,'e',1);
        printf("Set Port Exactly!\n");
    } while(0 == err_r || 0 == fd_r);



	//err_r = init_com_dev(fd,115200,8,'e',1);
}

void *PGV_Send(void *t)
{
    while (1)
    {
    	PGV_Send_buff[0] = 0xC8;
    	PGV_Send_buff[1] = 0x37;
        len_s = send_com_dev(fd,PGV_Send_buff,2);
        if(len_s > 0)
        {
            printf(" %d send data successful\n", len_s);
            //return len_s;
        }
        else
        {
            printf("send data failed!\n");
            //return -1;
        }
        sleep(0.06);
    }


}



void PGV_Send_data()
{
    PGV_Send_buff[0] = 0xC8;
    PGV_Send_buff[1] = 0x37;
    len_s = send_com_dev(fd,PGV_Send_buff,2);
    if(len_s > 0)
    {
        //printf(" %d send data successful\n", len_s);
        //return len_s;
    }
    else
    {
        printf("send data failed!\n");
        //return -1;
    }
   // sleep(0.06);

}



void get_PGV_Send_Pthread()
{
    int rc;
    long t;
    pthread_t tid;
    rc = pthread_create(&tid, NULL, PGV_Send, (void *)t);
    if (rc)
    {
        printf("PGV pthread create failed!\n");
        return;
    }
}

void get_PGV_Rcv_Pthread()
{
    int rc;
    long t;
    pthread_t tid;
    rc = pthread_create(&tid, NULL, PGV_Rcv, (void *)t);
    if (rc)
    {
        printf("PGV RCV pthread create failed!\n");
        return;
    }
}


void PGV_Rev()
{

    int temp, i;



    //sleep(10);
    len_r = rcv_com_dev(fd_r, PGV_Rev_buff,21);



    len_sum += len_r;
    if (len_r <= 0)   //没有数据继续等待读取
    {
       len_sum = 0;
       return;
    }
    if ((len_r > 0) && (len_sum < 21)) //如果数据大于0但总长度小于21，则继续
    {
        for (i = 0; i < len_r; i++)
        {
           cmd_buff[len_sum + i - len_r] = PGV_Rev_buff[i];
        }
       return;
    }
    else if ((len_r > 0) && (len_sum >= 21))
    {
       for (i = 0; i < len_r; i++)
       {
            cmd_buff[len_sum + i - len_r] = PGV_Rev_buff[i];
        }
        len_sum = 0;
        if (strlen(cmd_buff) == 21)
        {
            PGV_rx_flag = 1;   //接收数据正常
            printf("PGV_rx sucess\n");
            //sleep(1);
        }
    }
    printf("len_r = %d\n", len_r);
        //sleep(10);






/*
	len_sum += len_r;

	if (len_r < 0)   //没有数据继续等待读取
    {
	    len_sum = 0;
        return;
    }
	if ((len_r > 0) && (len_sum < 21)) //如果数据大于0但总长度小于21，则继续
    {
        for (i = 0; i < len_r; i++)
        {
            cmd_buff[len_sum + i - len_r] = PGV_Rev_buff[i];
        }
        return;
    }
	else if ((len_r > 0) && (len_sum >= 21))
    {
        for (i = 0; i < len_r; i++)
        {
            cmd_buff[len_sum + i - len_r] = PGV_Rev_buff[i];
        }
        len_sum = 0;
        if (sizeof(cmd_buff) == 21)
        {
            PGV_rx_flag = 1;   //接收数据正常
            //sleep(1);
        }

    }

    printf("len_sum = %d\n", sizeof(cmd_buff));
*/

   // sleep(1);
    /*
	if(len_r > 0)
	{
		PGV_Rev_buff[len_r] = '\0';
		printf("PGV_len_r = %d\n", len_r);


		printf("receive data is %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"
				"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",PGV_Rev_buff[0], PGV_Rev_buff[1], PGV_Rev_buff[2], PGV_Rev_buff[3],
				PGV_Rev_buff[4], PGV_Rev_buff[5], PGV_Rev_buff[6], PGV_Rev_buff[7], PGV_Rev_buff[8], PGV_Rev_buff[9], PGV_Rev_buff[10],PGV_Rev_buff[11],
				PGV_Rev_buff[12], PGV_Rev_buff[13], PGV_Rev_buff[14], PGV_Rev_buff[15], PGV_Rev_buff[16], PGV_Rev_buff[17], PGV_Rev_buff[18], PGV_Rev_buff[19],
				PGV_Rev_buff[20], PGV_Rev_buff[21]);
		printf("len = %d\n",len_r);

		//return len_r;
	}
	*/
}

void *PGV_Rcv(void *t)
{
    char read_tmp[21] = {0};
    int return_flag = 0;
    int i;
    unsigned int xor_temp = 0;
    int rx_xor_flag = 0;


    //存放读取到的字节数
    while (1)
    {
        memset(read_tmp, 0, sizeof(read_tmp));
        if (rcv_com_dev(fd_r, read_tmp, sizeof(read_tmp)))
        {
            //数据拼接
            printf("read_tmp = %s\n", read_tmp);
            for (i = 0; i < sizeof(read_tmp); i++)
            {
                if (read_tmp[i] == 0x0A)
                {
                    memset(read_data, 0, sizeof(read_data));
                    char tmp[5] = {0};
                    tmp[0] = read_tmp[i];
                    strcat(read_data, tmp);
                }
                else if (xor_temp == read_tmp[20])
                {
                    char tmp[5] = {0};
                    tmp[0] = read_tmp[i];
                    strcat(read_data, tmp);
                    return_flag = 1;
                    memset(read_buff, 0, sizeof(read_buff));
                    memcpy(read_buff, read_data, sizeof(read_data));
                }
                else
                {
                    char tmp[5] = {0};
                    tmp[0] = read_tmp[i];
                    strcat(read_data, tmp);
                }

            }

        }
        for (i = 0; i < 20; i++)
        {
            xor_temp ^= read_data[i]; //前20个字节的异或校验
        }

        rx_xor_flag = (xor_temp == PGV_Rev_buff[20]); //检验校验码

    }

}







int PGV_AnalyzeData()
{
	unsigned int xor_temp = 0;
	int rx_xor_flag = 0;
	int i;
	int data_temp;

	for (i = 0; i < 20; i++)
	{
		xor_temp ^= PGV_Rev_buff[i]; //前20个字节的异或校验
	}
/*
	if ((xor_temp == PGV_Rev_buff[20]) && (PGV_Rev_buff[0] != 0x02))
	{
		result = 1;

	}
	else
	{
		result = 0;
		return 0;
	}
*/

    //printf("xortemp = %x\n", xor_temp);
    //printf("PGV_RCV=%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", PGV_Rev_buff[0], PGV_Rev_buff[1], PGV_Rev_buff[2], PGV_Rev_buff[3], PGV_Rev_buff[4], PGV_Rev_buff[5], PGV_Rev_buff[6], PGV_Rev_buff[7], PGV_Rev_buff[8], PGV_Rev_buff[9], PGV_Rev_buff[10], PGV_Rev_buff[11], PGV_Rev_buff[12], PGV_Rev_buff[13], PGV_Rev_buff[14], PGV_Rev_buff[15], PGV_Rev_buff[16], PGV_Rev_buff[17], PGV_Rev_buff[18], PGV_Rev_buff[19], PGV_Rev_buff[20]);


	//rx_xor_flag = (xor_temp == PGV_Rev_buff[20]); //检验校验码
	if ((xor_temp == PGV_Rev_buff[20]) && (PGV_Rev_buff[20] != 0) && (PGV_Rev_buff[0] != 2))
	{
		rx_xor_flag = 1;
	//	printf("xortemp = %x\n", xor_temp);
	//	printf("PGV_RCV=%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", PGV_Rev_buff[0], PGV_Rev_buff[1], PGV_Rev_buff[2], PGV_Rev_buff[3], PGV_Rev_buff[4], PGV_Rev_buff[5], PGV_Rev_buff[6], PGV_Rev_buff[7], PGV_Rev_buff[8], PGV_Rev_buff[9], PGV_Rev_buff[10], PGV_Rev_buff[11], PGV_Rev_buff[12], PGV_Rev_buff[13], PGV_Rev_buff[14], PGV_Rev_buff[15], PGV_Rev_buff[16], PGV_Rev_buff[17], PGV_Rev_buff[18], PGV_Rev_buff[19], PGV_Rev_buff[20]);
	}

    if (rx_xor_flag != 1)
    {
    	return 0;

    }

	//校验通过
	warn_flag = (int)(PGV_Rev_buff[0] & _BV(2)); //提取错误标志
	warn = PGV_Rev_buff[18] << 7 | PGV_Rev_buff[19];	//错误代码

    data_temp = ((PGV_Rev_buff[2] & 0x07) << 21) + (PGV_Rev_buff[3] << 14) + (PGV_Rev_buff[4] << 7) + PGV_Rev_buff[5];
	x_temp = (data_temp & _BV(23)) ? (int)(-(_BV(24) - data_temp)) : (int)data_temp;
	//data_temp = (PGV_Rev_buff[2] & 0x07) * 0x80 * 0x4000 + PGV_Rev_buff[3] * 0x4000 + PGV_Rev_buff[4] * 0x80 + PGV_Rev_buff[5];
	//if (data_temp > 0x800000)
	//	x_temp = (0x1000000 - data_temp) * -1;
	//else
	//	x_temp = data_temp;

    //if ((x_temp > 100.0) || (x_temp < -100))
    //{
    	//printf("PGV_RCV=%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", PGV_Rev_buff[0], PGV_Rev_buff[1], PGV_Rev_buff[2], PGV_Rev_buff[3], PGV_Rev_buff[4], PGV_Rev_buff[5], PGV_Rev_buff[6], PGV_Rev_buff[7], PGV_Rev_buff[8], PGV_Rev_buff[9], PGV_Rev_buff[10], PGV_Rev_buff[11], PGV_Rev_buff[12], PGV_Rev_buff[13], PGV_Rev_buff[14], PGV_Rev_buff[15], PGV_Rev_buff[16], PGV_Rev_buff[17], PGV_Rev_buff[18], PGV_Rev_buff[19], PGV_Rev_buff[20]);
    //}


	data_temp = PGV_Rev_buff[6] * 0x80 + PGV_Rev_buff[7];

	if (data_temp > 0x2000)
		y_temp = (0x4000 - data_temp) * -1;
	else
		y_temp = data_temp;


	data_temp = PGV_Rev_buff[10] * 0x80 + PGV_Rev_buff[11];
	if (data_temp > 0x2000)
		angle_temp = (0x4000 - data_temp) * -1;
	else
		angle_temp = data_temp;

	data_temp = PGV_Rev_buff[14] * 0x80 * 0x4000 + PGV_Rev_buff[15] * 0x4000 + PGV_Rev_buff[16] * 0x80 + PGV_Rev_buff[17];
	tag_control_num = data_temp;

	x_deviation     = x_temp / 10.0;
	y_deviation     = y_temp / 10.0;
	angle_deviation = angle_temp;
	printf("x_deviation=%f, y_deviation=%f, angle_deviation=%f\n", x_deviation, y_deviation, angle_deviation);


	memset(cmd_buff, 0, sizeof(cmd_buff));   //一帧解析完成清掉

	return 1;
}

void PGV_Cal_Coor()
{
	PGV150_coor.x_coor = (tag_control_num % 5) * 100.0 + x_deviation;
	PGV150_coor.y_coor = (tag_control_num / 5) * 100.0 + y_deviation;
	if ((angle_deviation > 180.0) && (angle_deviation < 360.0))
	{
		PGV150_coor.angle_coor = 360.0 - angle_deviation;
	}
	else if (0 < angle_deviation <= 180.0)
	{
		PGV150_coor.angle_coor = - angle_deviation;
	}
	data_ok = 1;

	//sleep(10);

	printf("X_coor = %f, Y_coor = %f, Angle_coor = %f\n", PGV150_coor.x_coor, PGV150_coor.y_coor, PGV150_coor.angle_coor);
}




