/*
 * Timer.c
 *
 *  Created on: Jun 25, 2019
 *      Author: siasunhebo
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <Timer.h>
#include <sys/time.h>

void timer_thread(union sigval v)
{
	printf("timer_thread function %d\n", v.sival_int);
}

int init_timer(timer_t *timerid, struct sigevent *evp, struct itimerspec *it)
{
    if ( !evp || !it )
        return -1;

    memset(evp, 0, sizeof(struct sigevent));   //清零初始化

    evp->sigev_value.sival_int = 111;        //也是标识定时器的，这和timerid有什么区别？回调函数可以获得
    evp->sigev_notify = SIGEV_THREAD;        //线程通知的方式，派驻新线程
    evp->sigev_notify_function = timer_thread;   //线程函数地址

    if (timer_create(CLOCK_REALTIME, evp, timerid) == -1)
    {
        perror("fail to timer_create");
        return -1;;
    }

    printf("timer_create timerid = %d\n", *timerid);
    it->it_interval.tv_sec = 1;  // 后续按照该时间间隔
    it->it_interval.tv_nsec = 0;
    it->it_value.tv_sec = 1;     // 最初开始时间间隔
    it->it_value.tv_nsec = 0;

    return 0;

}


int start_timer(timer_t *timerid, struct itimerspec *it)
{
	if (it == NULL)
	{

		return -1;
	}

	if (timer_settime(*timerid, 0, it, NULL) == -1)
	{
		perror("failed to time_settime!\n");
		return -1;
	}
	return 0;
}

void thread_func(void *param)
{
	int *a = (int *)param;
	while(1)
	{
		sleep(1);
		printf("this is a thread\n");

	}

	*a = 100;
	printf("param = %d\n", *a);

}

//return 自CPU上电以来的时间，单位:毫秒
unsigned long long GetCpuRunTimeMs(void)
{
    unsigned long long uptime = 0;
    struct timespec on;
    if (clock_gettime(CLOCK_MONOTONIC, &on) == 0)
    {
        uptime = on.tv_sec * 1000ul;
        uptime += on.tv_nsec / 1000000ul;
    }
    return uptime;
}


//信号处理函数
void signal_func(int signo)
{
    static unsigned long long start_time = 0;

    switch (signo)
    {
        case SIGALRM:
            printf("%s:1s-----> take %llu ms,signo=SIGALRM(%d)\n", __FUNCTION__, GetCpuRunTimeMs()-start_time, signo);
            break;
        case SIGUSR1:
            printf("%s:1s-----> take %llu ms,signo=SIGUSR1(%d)\n", __FUNCTION__, GetCpuRunTimeMs()-start_time, signo);
        default:
            break;
    }
    start_time = GetCpuRunTimeMs();
}

//setitimer函数相关初始化
void vTimerInit(void)
{
    struct itimerval value, old_value;

    signal(SIGALRM, signal_func);

    value.it_value.tv_sec = 1;    //设置距离首次中断的时间
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 1;    //时间间隔
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &old_value);

}

int vTimerPthreadInit()
{
    timer_t timerid;
    struct sigevent evp;
    int ret;
    memset(&evp, 0, sizeof(struct sigevent));    //清零初始化


    evp.sigev_value.sival_int = 1111;
    evp.sigev_notify = SIGEV_THREAD;     //线程通知的方式派驻新线程
    evp.sigev_notify_function = timer_thread;    //线程函数地址

    ret = timer_create(CLOCK_REALTIME, &evp, &timerid);

    if (ret)
    {
        perror("timer_creat\n");
    }
    struct itimerspec it;
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_nsec = 0;
    it.it_value.tv_sec = 1;
    it.it_interval.tv_nsec = 0;

    ret = timer_settime(timerid, 0, &it, NULL);
    if (ret)
    {
        perror("timer_settimer");
    }
    return ret;

}

