
#ifndef GTMTOMPWMHL_H_
#define GTMTOMPWMHL_H_ 1

#include "Ifx_Types.h"
#include "SysSe/Bsp/Bsp.h"
#include "Gtm/Tom/PwmHl/IfxGtm_Tom_PwmHl.h"


#include "Configurationisr_.h"

typedef struct
{

        struct
        {
                float32         gtmFreq;
                float32         gtmGclkFreq;
                float32         gtmCmuClk0Freq;
                Ifx_TimerValue  timerValue;

        }info;

        float32 tOn[3];

        struct
        {
                IfxGtm_Tom_Timer    timer;
                IfxGtm_Tom_PwmHl    pwm;
        }drivers;

        struct
        {
                uint32 counter;

        }isrCounter;
}App_GtmTomPwmHl;


IFX_EXTERN App_GtmTomPwmHl g_GtmTomPwmHl;

IFX_EXTERN void GtmTomPwmHl_init(void);
IFX_EXTERN void GtmTomPwmHl_run(void);


#endif /* GTMTOMPWMHL_H_ */
