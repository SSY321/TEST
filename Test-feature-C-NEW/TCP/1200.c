//
// Created by maxiao on 7/6/19.
//

#include <1200.h>
#include <TCP.h>
#include <stdio.h>
#include <common.h>
#include <pthread.h>
#include <net.h>
#include <buffer.h>
#include <protocol.h>
#include <errno.h>

//float command_x, command_y, command_angle;
float vx_ByEncoder, angular_velocity_angle_ByEncoder, angular_velocity_rad_ByEncoder;
int vx_ByEncoder_int;

extern struct Velocity_Class AGV_Current_Velocity_By_Encoder;

extern int Add_Command_Line;
extern int Add_Command_Rotate;

int command_x;
int command_y;
int command_angle;

extern char revbuf[LENGTH];
struct Buffer buf;
extern char buffer[8];  //发送PLC数组
int sock;

typedef union{
	short fdata;
	char ldata[2];
}PLC_Data;    //PLC数据联合体

int temp;
Coordinate_Class_t PLC_command_coor;
PLC_Data vx_byEncoder;    //PLC上传编码器得到速度
PLC_Data angular_velocity_angle_byEncoder;   //PLC上传编码器得到车体旋转角速度
PLC_Data angular_velocity_rad_byEncoder;
PLC_Data Add_Command_rotate, Add_Command_line;
PLC_Data Command_X, Command_Y, Command_Angle;


void get_PLC_init()
{
    sock = sock_connect("192.168.1.10", 5000);
    if (sock == -1)
    {
        printf("error: %s\n", strerror(errno));
        exit(0);
        //return NULL;
    }
    printf("connnect 192.168.1.10:5000\n");

}

void get_pthread()
{
	int rc;
	long t;
	pthread_t tid;
	rc = pthread_create(&tid, NULL, fuck_tcp, (void *)t);
	if (rc)
	{
		printf("pthread create failed!\n");
		return;
	}
}


void *fuck_tcp(void *t)
{


    buffer_init(&buf);

    while (1)
    {
        PLC_send(buffer);     //套接字发送(放主函数会有接收延迟)
        char tmp[26];
        int len = read(sock, tmp, sizeof(tmp));    //套接字接收
        printf("1200 REV LEN=%d\n", len);
        if (len <= 0)
        {
            printf("received %d, exit.\n", len);
            exit(0);
        }
        if (buffer_add(&buf, tmp, len) == -1)
        {
            printf("error, too large packet\n");
            exit(0);
        }
        int n = 0;
        while (1)
        {
            char *msg = parse_packet(&buf);
            if (!msg)
            {
                break;
            }
            n ++;
            printf("< %s\n", msg);
            if (n > 1)
            {
                printf(" [Mergerd Packed]\n");
            }
            free(msg);
        }
    }
    return 0;


}

int PLC_send(char buf[])
{
    int num;
    if((num = send(sock, buf, 8, 0)) == -1)
    {
        printf("ERROR: Failed to sent string.\n");
        close(sock);
        exit(1);
    }
    //printf("OK: Sent %d bytes sucessful, please enter again.\n", num);



}




void get_PLC_Data()
{
    PLC_DATA_t get_PLC_Data;

	//套接字接收
	//socket_rec();
	//vx_byEncoder速度转换
	vx_byEncoder.ldata[0] = buf.data[4];
	vx_byEncoder.ldata[1] = buf.data[5];
	vx_ByEncoder = vx_byEncoder.fdata / 1000.0;  //PLC传输为整数，需除以1000

    AGV_Current_Velocity_By_Encoder.velocity_x = vx_ByEncoder;

	//angular_velocity_angle_byEncoder速度转换
	angular_velocity_angle_byEncoder.ldata[0] = buf.data[8];
	angular_velocity_angle_byEncoder.ldata[1] = buf.data[9];
	angular_velocity_angle_ByEncoder = angular_velocity_angle_byEncoder.fdata / 1000.0;

	AGV_Current_Velocity_By_Encoder.angular_velocity_angle = angular_velocity_angle_ByEncoder;

	//angular_velocity_rad_byEncoder速度转换
	angular_velocity_rad_byEncoder.ldata[0] = buf.data[12];
	angular_velocity_rad_byEncoder.ldata[1] = buf.data[13];
	angular_velocity_rad_ByEncoder = angular_velocity_rad_byEncoder.fdata / 1000.0;

	AGV_Current_Velocity_By_Encoder.angular_velocity_rad = angular_velocity_rad_ByEncoder;

	Add_Command_rotate.ldata[0] = buf.data[16];
	Add_Command_rotate.ldata[1] = buf.data[17];
    Add_Command_Rotate = Add_Command_rotate.fdata;
    //get_PLC_Data.command_rotate = Add_Command_rotate.fdata;

	Add_Command_line.ldata[0] = buf.data[18];
	Add_Command_line.ldata[1] = buf.data[19];
    Add_Command_Line = Add_Command_line.fdata;
    //get_PLC_Data.command_line = Add_Command_line.fdata;

	Command_X.ldata[0] = buf.data[20];
	Command_X.ldata[1] = buf.data[21];
	command_x = Command_X.fdata;
	//get_PLC_Data.command_coor.x = Command_X.fdata;

	Command_Y.ldata[0] = buf.data[22];
	Command_Y.ldata[1] = buf.data[23];
	command_y = Command_Y.fdata;
    //get_PLC_Data.command_coor.y = Command_Y.fdata;

	Command_Angle.ldata[0] = buf.data[24];
	Command_Angle.ldata[1] = buf.data[25];
	command_angle = Command_Angle.fdata;
	//get_PLC_Data.command_coor.angle = Command_Angle.fdata;

	printf("C_X = %d, C_Y = %d, C_A = %d\n", command_x, command_y, command_angle);
	//printf("Add_line = %d, Add_rotate = %d\n", Add_Command_Line, Add_Command_Rotate);
	//printf("vx_ByEncoder = %f, angular_velocity_angle_ByEncoder = %f, angular_velocity_rad_byEncoder = %f\n", vx_ByEncoder, angular_velocity_angle_ByEncoder, angular_velocity_rad_ByEncoder);
    //return get_PLC_Data;

}



Coordinate_Class_t get_PLC_Command_coor(char *revbuf)
{
    PLC_command_coor.x_coor = revbuf[16];
    PLC_command_coor.y_coor = revbuf[20];
    PLC_command_coor.angle_coor = revbuf[24];
    return PLC_command_coor;
}
