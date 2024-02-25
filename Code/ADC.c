
/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "ADC.h"
#include "IfxVadc.h"
#include "IfxVadc_Adc.h"
#include "AppScheduling.h"
#include "DRIVER_STM.h"
#include "IfxPort.h"
#include "IfxPort_PinMap.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
//ADC 모듈변수 선언
typedef struct
{
        IfxVadc_Adc vadc; /*VADC handle */
        IfxVadc_Adc_Group adcGroup;

}App_VadcAutoScan;

/*Static Function Prototype */

//변수 선언 및 채널 선언
static void DrvAdc_Group0Init(void);
static void DrvAdc_Group0ChannelInit(uint8 param_ChNum);

/*Variables */

//ADC 초기화 함수 선언
App_VadcAutoScan g_VadcAutoScan;
IfxVadc_Adc_Channel adc0Channel[ADC_GROUP0_MAX];
SensorAdcRaw stSensorAdcRaw; //ADC Raw 값 받을 변수
void DrvAdcInit(void)
{
    DrvAdc_Group0Init();
}

static void DrvAdc_Group0Init(void)
{
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    IfxVadc_Adc_initModule(&g_VadcAutoScan.vadc, &adcConfig);

    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcAutoScan.vadc);

    adcGroupConfig.groupId = IfxVadc_GroupId_0;
    adcGroupConfig.master = adcGroupConfig.groupId;

    adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;

    adcGroupConfig.scanRequest.autoscanEnabled = TRUE;

    adcGroupConfig.scanRequest.triggerConfig.gatingMode = 1;

    IfxVadc_Adc_initGroup(&g_VadcAutoScan.adcGroup, &adcGroupConfig);

    DrvAdc_Group0ChannelInit(ADC_GROUP0_CH1);
    DrvAdc_Group0ChannelInit(ADC_GROUP0_CH2);
    DrvAdc_Group0ChannelInit(ADC_GROUP0_CH3);

    IfxVadc_Adc_startScan(&g_VadcAutoScan.adcGroup);
}
static void DrvAdc_Group0ChannelInit(uint8 param_ChNum)
{
    IfxVadc_Adc_ChannelConfig adcChannelConfigInfo;
    uint32 ulTemp = ((uint32)1u << param_ChNum);

    IfxVadc_Adc_initChannelConfig(&adcChannelConfigInfo,&g_VadcAutoScan.adcGroup);

    adcChannelConfigInfo.channelId           = (IfxVadc_ChannelId)(param_ChNum);
    adcChannelConfigInfo.resultRegister      = (IfxVadc_ChannelResult)(param_ChNum);

    IfxVadc_Adc_initChannel(&adc0Channel[param_ChNum], &adcChannelConfigInfo);

    IfxVadc_Adc_setScan(&g_VadcAutoScan.adcGroup, ulTemp, ulTemp);
}
void DrvAdc_GetAdcRawGroup0(void)
{
    Ifx_VADC_RES conversionResult;
    conversionResult = IfxVadc_Adc_getResult(&adc0Channel[ADC_GROUP0_CH1]);
    stSensorAdcRaw.UlSSense1_Raw = conversionResult.B.RESULT;
    conversionResult = IfxVadc_Adc_getResult(&adc0Channel[ADC_GROUP0_CH2]);
    stSensorAdcRaw.UlSSense2_Raw = conversionResult.B.RESULT;
    conversionResult = IfxVadc_Adc_getResult(&adc0Channel[ADC_GROUP0_CH3]);
    stSensorAdcRaw.UlSSense3_Raw = conversionResult.B.RESULT;

    IfxVadc_Adc_startScan(&g_VadcAutoScan.adcGroup);
}
