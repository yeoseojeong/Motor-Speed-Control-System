/*
 * AppScheduling.c
 *
 *  Created on: 2024. 2. 20.
 *      Author: JJu0_0
 */
#include "AppScheduling.h"
#include "DRIVER_STM.h"
#include "IfxPort.h"
#include "GtmTomPwm_.h"
#include "Configurationisr_.h"

#define enca &MODULE_P02,1
#define encb &MODULE_P02,0




typedef struct
{
        uint32 u32nuCnt1ms;
        uint32 u32nuCnt10ms;
        uint32 u32nuCnt100ms;
}TestCnt;


static void AppTask1ms(void);
static void AppTask10ms(void);
static void AppTask100ms(void);

float32 encoder_count = 0, encoder_count_d = 0;
float32 theta=0,theta_d=0;
float32 w=0;
float32 w_fil,w_fil_d = 0;
float32 seconds = 0;
float32 w_ref = 0;
float32 w_speed = 2.5;               // Personal project => normal speed Case
float32 Vin = 0;                        // Personal project => Vin
float32 error_w,error_w_int,error_w_int_old = 0;
float32 kp = 0.3;                             // Personal project => P gain
float32 ki = 0.4;                             // Personal project => I gain

boolean flag = 0;
boolean M_dir = 1;

int o_state = 0;
int c_state = 0;

TestCnt stTestCnt;

/* **************************************** function call ***********************************************/

void AppNoTask(void);
uint32 LPF();
boolean M_dir_control();
float32 make_PI_V();

static void AppTask1ms(void)
{

/* **************************************** matke_theta,w ***********************************************/

    seconds += 0.001;

    theta = (encoder_count)*(2*3.14/48);
    w = (theta - theta_d) / 0.001;
    theta_d = theta;

    w_fil = LPF(w_fil_d,w,0.001,150);
    w_fil_d = w_fil;


/* **************************************** make_signal ***********************************************/

        if(seconds<4){
            w_ref=0;
        }
        else if(seconds>4 && seconds<= 19){
            w_ref = 0.06667*w_speed*(2*3.14)*(seconds-4);
        }
        else if(seconds>19 && seconds <= 26){
            w_ref=w_speed*(3.14*2);
        }
        else if(seconds>26 && seconds <= 41){
            w_ref=w_speed*(3.14*2)-0.06667*w_speed*(2*3.14)*(seconds-26);
        }
        else if(seconds>41){
            w_ref = 0;
        }

    /* **************************************** make_signal_PI Controller ***********************************************/
        Vin = make_PI_V(w_ref);


        g_GtmTomPwmHl.tOn[0] = Vin/12;
        GtmTomPwmHl_run();

}
static void AppTask10ms(void)
{
    stTestCnt.u32nuCnt10ms++;
}

static void AppTask100ms(void)
{
    stTestCnt.u32nuCnt100ms++;
}


void AppScheduling(void)
{
    AppNoTask();

    if(stSchedulingInfo.u8nuScheduling1msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        AppTask1ms();

        if(stSchedulingInfo.u8nuScheduling10msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            AppTask10ms();
        }
        if(stSchedulingInfo.u8nuScheduling100msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            AppTask100ms();
        }
    }
}








void AppNoTask(void){
    IfxPort_setPinModeInput(enca, IfxPort_Mode_inputPullDown); // A = 3번 포트
    IfxPort_setPinModeInput(encb, IfxPort_Mode_inputPullDown); // B = 2번 포트

    boolean ENCA = IfxPort_getPinState(enca);
    boolean ENCB = IfxPort_getPinState(encb);

    if(ENCA == FALSE && ENCB == FALSE){
        c_state = 0;

    }
    else if(ENCA == TRUE && ENCB == FALSE){
        c_state = 1;

    }
    else if(ENCA == FALSE && ENCB == TRUE){
        c_state = 2;

    }
    else{
        c_state = 3;
    }

    if(o_state != c_state){

        M_dir = M_dir_control(o_state,c_state);

        if(M_dir == 1){
            encoder_count++;
        }
        else if(M_dir == 0){
            encoder_count--;
        }
    }
    o_state = c_state;
}



uint32 LPF(uint32 Y_fil_d,uint32 U,uint32 Ts,int cf)
{
    uint32 Y_fil=(1-Ts*cf)*Y_fil_d+Ts*cf*U;
    return Y_fil;
}


boolean M_dir_control(int o, int c){
    boolean dir = 1;
    // 1 -> 3 -> 2 -> 0 -> 1    정방향 = M_dir = 1

    // 2 -> 3 -> 1 -> 0 -> 2    역방향 = M_dir = 0

    if(c == 0){
        if(o == 0){
            dir = 1;          // default = 정방향 1
        }
        else if(o == 2){
            dir = 1;
        }
        else if(o == 1){
            dir = 0;
        }
    }
    else if(c == 1){
        if(o == 0){
            dir = 1;
        }
        else if(o == 3){
            dir = 0;
        }
    }
    else if(c == 2){
        if(o == 3){
            dir = 1;
        }
        else if(o == 0){
            dir = 0;
        }
    }
    else if(c == 1){
        if(o == 1){
            dir = 1;
        }
        else if(o == 2){
            dir = 0;
        }
    }
    return dir;
}


float32 make_PI_V(float32 w_in){
    float V;
    if(seconds<=4){
        encoder_count =0;
        V = 0;
    }
    else if(seconds>=4 && seconds<=41){
        error_w = w_in-w;
        error_w_int = error_w_int_old+(error_w)*0.001;
        error_w_int_old=error_w_int;

        if(error_w_int >10){
            error_w_int = 10;
        }

        V = (kp*error_w + ki*error_w_int);

        if(V > 11){
            V = 11;
        }
        else if(V<0){
            V = 0;
        }
    }
    else if(seconds>41){
        V=0;
    }

    return V;
}




