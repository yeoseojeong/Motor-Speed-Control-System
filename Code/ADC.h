
/*
 * DRVADC.h
 *
 *  Created on: 2024. 2. 20.
 *      Author: yeonhee0207
 */

#ifndef ADC_H_
#define ADC_H_

#include "Ifx_Types.h"

typedef enum
{
    ADC_GROUP0_CH0 = 0u,
    ADC_GROUP0_CH1,
    ADC_GROUP0_CH2,
    ADC_GROUP0_CH3,
    ADC_GROUP0_CH4,
    ADC_GROUP0_CH5,
    ADC_GROUP0_CH6,
    ADC_GROUP0_CH7,
    ADC_GROUP0_MAX
}ADC_GROUP0;

typedef struct
{
        uint32 UlSSense1_Raw;
        uint32 UlSSense2_Raw;
        uint32 UlSSense3_Raw;

}SensorAdcRaw;


extern SensorAdcRaw stSensorAdcRaw;


extern void DrvAdcInit(void);
extern void DrvAdc_GetAdcRawGroup0(void);

#endif /* DRVADC_H_ */
