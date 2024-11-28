////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "camxsensordriverapi.h"
#include <android/log.h>
// NOWHINE ENTIRE FILE

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/* Controlling HCG and LCG linear modes
 * by default is enabled HCG linear mode */
// #define ENABLE_LINEAR_LCG_MODE

#define MIN_HCG_DGAIN (512)  // 2.0x
#define MIN_LCG_DGAIN (280)  // 1.09375x
#define MIN_LCG_GAIN (256 * 3)  //3.0x
#define PS5280_MIN_DGAIN (1)
#define PS5280_MIN_DGAIN_VAL (1024)

#define MIN_EXPOSURE_LINES             (1)

#define MIN_ANALOG_GAIN (1.0000)
#define MAX_ANALOG_GAIN (128)

#define PS5280_DBG(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, "PS5280",fmt, ##args)

double gainValues[] = {
    1.0000,  1.0625,  1.1250,  1.1875,  1.2500,  1.3125,  1.3750,  1.4375,  1.5000,  1.5625,  1.6250,  1.6875,  1.7500,  1.8125,  1.8750,  1.9375,
    2.0000,  2.1250,  2.2500,  2.3750,  2.5000,  2.6250,  2.7500,  2.8750,  3.0000,  3.1250,  3.2500,  3.3750,  3.5000,  3.6250,  3.7500,  3.8750,
    4.0000,  4.2500,  4.5000,  4.7500,  5.0000,  5.2500,  5.5000,  5.7500,  6.0000,  6.2500,  6.5000,  6.7500,  7.0000,  7.2500,  7.5000,  7.7500,
    8.0000,  8.5000,  9.0000,  9.5000,  10.0000, 10.5000, 11.0000, 11.5000, 12.0000, 12.5000, 13.0000, 13.5000, 14.0000, 14.5000, 15.0000, 15.5000,
    16.0000, 17.0000, 18.0000, 19.0000, 20.0000, 21.0000, 22.0000, 23.0000, 24.0000, 25.0000, 26.0000, 27.0000, 28.0000, 29.0000, 30.0000, 31.0000,
    32.0000, 34.0000, 36.0000, 38.0000, 40.0000, 42.0000, 44.0000, 46.0000, 48.0000, 50.0000, 52.0000, 54.0000, 56.0000, 58.0000, 60.0000, 62.0000,
    64.0000, 68.0000, 72.0000, 76.0000, 80.0000, 84.0000, 88.0000, 92.0000, 96.0000, 100.0000, 104.0000, 108.0000, 112.0000, 116.0000, 120.0000, 124.0000,
    128.0000
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain, SensorExposureInfo* pExposureInfo) {
    UINT regGain = 0;
    DOUBLE analogRealGain;
    int i;
    PS5280_DBG("RealToRegisterGain: Input realGain: %lf.", realGain);
    if (realGain < MIN_ANALOG_GAIN) {
      analogRealGain = MIN_ANALOG_GAIN;
    } else if (realGain > MAX_ANALOG_GAIN) {
      analogRealGain = MAX_ANALOG_GAIN;
    } else {
      analogRealGain = realGain;
    }
    PS5280_DBG("RealToRegisterGain: Adjusted analogRealGain: %lf", analogRealGain);

    for (i = 0; i < sizeof(gainValues) / sizeof(gainValues[0]); i++) {
        if (analogRealGain < gainValues[i] + 0.0625) {
            break;
        }
        regGain++;
    }
    if (i == sizeof(gainValues) / sizeof(gainValues[0])) {
        regGain = 80; // If the value is greater than the last element
    }
    pExposureInfo->analogRealGain = gainValues[i > 0 ? i - 1 : 0];

    PS5280_DBG("RealToRegisterGain: Selected regGain: %d, analogRealGain: %lf", regGain, pExposureInfo->analogRealGain);

    return regGain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain) {
    DOUBLE realGain;

    PS5280_DBG("RegisterToRealGain: Input registerGain: %d", registerGain);
    if (registerGain < 0x100){
        registerGain = 0x100;
    } else if (registerGain > (8 * 256) -1) {
        registerGain = (8 * 256) -1;
    }
    realGain = (DOUBLE) registerGain / 256.0;
    PS5280_DBG("RegisterToRealGain: Calculated realGain: %lf", realGain);
    return realGain;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
    SensorExposureInfo*          pExposureInfo,
    SensorCalculateExposureData* pCalculateExposureData)
{
    if (NULL == pExposureInfo || NULL == pCalculateExposureData)
    {
        PS5280_DBG("CalculateExposure: Invalid parameters provided.");
        return FALSE;
    }
    PS5280_DBG("CalculateExposure: Input realGain: %lf, lineCount: %d", 
               pCalculateExposureData->realGain, pCalculateExposureData->lineCount);

    UINT digitalGain = 0;
    pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain, pExposureInfo);
    pExposureInfo->digitalRegisterGain = PS5280_MIN_DGAIN_VAL;
    pExposureInfo->digitalRealGain  = PS5280_MIN_DGAIN;    
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain / (pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;

    PS5280_DBG("CalculateExposure: analogRegisterGain: %d, digitalRegisterGain: %d, ISPDigitalGain: %lf, lineCount: %d",
               pExposureInfo->analogRegisterGain, pExposureInfo->digitalRegisterGain, 
               pExposureInfo->ISPDigitalGain, pExposureInfo->lineCount);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillExposureSettings(
    RegSettingsInfo*        pRegSettingsInfo,
    SensorFillExposureData* pExposureData) {
    UINT32 index = 0;
    UINT16 regCount = 0;

    if ((NULL == pRegSettingsInfo) || (NULL == pExposureData))
    {
        PS5280_DBG("FillExposureSettings: Invalid parameters provided.");
        return FALSE;
    }

    PS5280_DBG("FillExposureSettings: frameLengthLines: %d, lineCount: %d, analogRegisterGain: %d",
               pExposureData->frameLengthLines, pExposureData->lineCount, pExposureData->analogRegisterGain);

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount;
         index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regDataType;
        pRegSettingsInfo->regSetting[regCount].delayUs =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].delayUs;
        PS5280_DBG("GroupHoldOn: registerAddr: 0x%X, registerData: 0x%X",
                   pRegSettingsInfo->regSetting[regCount].registerAddr,
                   pRegSettingsInfo->regSetting[regCount].registerData);
        regCount++;
    }

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->frameLengthLinesAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->frameLengthLines & 0xFF00) >> 8;
    PS5280_DBG("FrameLengthLines: registerAddr: 0x%X, registerData: 0x%X",
               pRegSettingsInfo->regSetting[regCount].registerAddr,
               pRegSettingsInfo->regSetting[regCount].registerData);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = (pExposureData->pRegInfo->frameLengthLinesAddr + 1);
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->frameLengthLines & 0xFF);
    PS5280_DBG("FrameLengthLines Low Byte: registerAddr: 0x%X, registerData: 0x%X",
               pRegSettingsInfo->regSetting[regCount].registerAddr,
               pRegSettingsInfo->regSetting[regCount].registerData);
    regCount++;

    if (pExposureData->lineCount < MIN_EXPOSURE_LINES) {
        pExposureData->lineCount = MIN_EXPOSURE_LINES;
    }
    else if (pExposureData->lineCount > (pExposureData->frameLengthLines - 2)) {
        pExposureData->lineCount = pExposureData->frameLengthLines - 2;
    }

    pExposureData->lineCount = pExposureData->frameLengthLines - pExposureData->lineCount;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->coarseIntgTimeAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->lineCount & 0xFF00) >> 8;
    PS5280_DBG("CoarseIntgTime High Byte: registerAddr: 0x%X, registerData: 0x%X",
               pRegSettingsInfo->regSetting[regCount].registerAddr,
               pRegSettingsInfo->regSetting[regCount].registerData);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = (pExposureData->pRegInfo->coarseIntgTimeAddr + 1);
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->lineCount & 0xFF);
    PS5280_DBG("CoarseIntgTime Low Byte: registerAddr: 0x%X, registerData: 0x%X",
               pRegSettingsInfo->regSetting[regCount].registerAddr,
               pRegSettingsInfo->regSetting[regCount].registerData);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->analogRegisterGain & 0xFF);
    PS5280_DBG("GlobalAnalogGain: registerAddr: 0x%X, registerData: 0x%X",
               pRegSettingsInfo->regSetting[regCount].registerAddr,
               pRegSettingsInfo->regSetting[regCount].registerData);
    regCount++;


    for (index = 0; index < regCount; index++)
    {
        PS5280_DBG("RegSetting[%d]: registerAddr: 0x%X, registerData: 0x%X",
                   index,
                   pRegSettingsInfo->regSetting[index].registerAddr,
                   pRegSettingsInfo->regSetting[index].registerData);
        pRegSettingsInfo->regSetting[index].regAddrType  =
            I2CRegAddressDataTypeByte;
        pRegSettingsInfo->regSetting[index].regDataType  =
            I2CRegAddressDataTypeByte;
        pRegSettingsInfo->regSetting[index].delayUs      =
            0;
    }

    PS5280_DBG("FillExposureSettings: frameLengthLines: %d, lineCount: %d, analogRegisterGain: %d",
               pExposureData->frameLengthLines, pExposureData->lineCount, pExposureData->analogRegisterGain);

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOffSettings.regSettingCount;
         index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regDataType;
        PS5280_DBG("GroupHoldOff: registerAddr: 0x%X, registerData: 0x%X",
                   pRegSettingsInfo->regSetting[regCount].registerAddr,
                   pRegSettingsInfo->regSetting[regCount].registerData);
        regCount++;
    }

    PS5280_DBG("FillExposureSettings: Updated lineCount: %d", pExposureData->lineCount);
    pRegSettingsInfo->regSettingCount = regCount;

    if (MAX_REG_SETTINGS <= regCount)
    {
        PS5280_DBG("FillExposureSettings: Exceeded max register settings count.");
        return FALSE;
    }
    else
    {
        PS5280_DBG("FillExposureSettings: Successfully filled exposure settings.");
        return TRUE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetSensorLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI) {
    pSensorLibraryAPI->majorVersion          = 1;
    pSensorLibraryAPI->minorVersion          = 0;
    pSensorLibraryAPI->pCalculateExposure    = CalculateExposure;
    pSensorLibraryAPI->pFillExposureSettings = FillExposureSettings;
}

#ifdef __cplusplus
}   // CamX Namespace
#endif   // __cplusplus