////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"
// NOWHINE ENTIRE FILE
#include <android/log.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define ps5280_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "ps5280",fmt, ##args)
#define MINVALUE(Value1, Value2) (Value1 > Value2 ? Value2 : Value1)
#define MAXVALUE(Value1, Value2) (Value1 < Value2 ? Value2 : Value1)
#define EVEN(Value) ((0 != (Value % 2)) ? Value-1 : Value)

#define SHDR_MIN_LINE_COUNT 8    // Minimum line count for Long, Middle or Short frame in SHDR mode
#define CIT_MARGIN  22      // Time between shutter time of short exposure frame and readout time of long exposure frame
#define BRL_FIXED_VALUE 39
              // Minimum Verical Blanking + Fixed Value (Defined on Page 113 in "ynos ps5280_DOL-HDR_QuickReference_v2.0.0.pdf")

static const UINT   ps5280MinRegisterGain        = 0;                                     ///< Minimum analog register gain
static const UINT   ps5280MaxRegisterGain        = 978;                                   ///< Maximum analog register gain
static const DOUBLE ps5280MinRealGain            = (1024.0f / (1024.0f - ps5280MinRegisterGain));
                                                                                          ///< Minimum analog real gain (1X)
static const DOUBLE ps5280MaxRealGain            = (1024.0f / (1024.0f - ps5280MaxRegisterGain));
                                                                                          ///< Maximum analog real gain (22.26X)
static const UINT   ps5280MinDigitalRegisterGain = 256;
static const UINT   ps5280MaxDigitalRegisterGain = 256;
static const FLOAT  ps5280DigitalGainDecimator   = 256.0f;
static const DOUBLE ps5280MinDigitalRealGain     = (ps5280MinDigitalRegisterGain / ps5280DigitalGainDecimator);
                                                                                           ///< Minimum digital real gain (1X)
static const DOUBLE ps5280MaxDigitalRealGain     = (ps5280MaxDigitalRegisterGain / ps5280DigitalGainDecimator);
                                                                                           ///< Maximum digital real gain (4X)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{

    if(ps5280MaxRegisterGain < registerGain)
    {
        registerGain = ps5280MaxRegisterGain;
    }

    return (1024.0f / (1024.0f - registerGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    UINT32 regGain = 0;

    if (realGain < 1.0)
    {
        realGain = 1.0;
    }
    else if (realGain > ps5280MaxRealGain)
    {
        realGain = ps5280MaxRealGain;
    }

    regGain = static_cast<UINT32>(1024.0 - (1024.0 / realGain));
    return regGain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDigitalGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 CalculateDigitalGain(
    FLOAT realGain,
    FLOAT realAnalogGain)
{
    FLOAT digitalRealGain = realGain / realAnalogGain;

    if (ps5280MaxDigitalRealGain < digitalRealGain)
    {
        digitalRealGain = ps5280MaxDigitalRealGain;
    }
    else if (ps5280MinDigitalRealGain > digitalRealGain)
    {
        digitalRealGain = ps5280MinDigitalRealGain;
    }

    return static_cast<UINT>(digitalRealGain * ps5280DigitalGainDecimator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateExposure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
    SensorExposureInfo*          pExposureInfo,
    SensorCalculateExposureData* pCalculateExposureData)
{

    INT     CITLong          = 0;   // Coarse Integration Time for Long Exposure Frame
    INT     CITShort         = 0;   // Coarse Integration Time for Short Exposure Frame
    INT     frameLengthLines = 0;
    INT     BRL              = 0;   // Basic Readout Line  Fixed + vertical Active pixel
    INT     maxCITLong       = 0;   // Maximum Coarse Integration Time for Long Exposure Frame
    INT     maxCITLong2      = 0;   // Maximum Coarse Integration Time for Long Exposure Frame
    INT     maxCITShort      = 0;   // Maximum Coarse Integration Time for Short Exposure Frame

    if (NULL == pExposureInfo || NULL == pCalculateExposureData)
    {
        return FALSE;
    }

    pExposureInfo->analogRegisterGain       = RealToRegisterGain(pCalculateExposureData->realGain);
    pExposureInfo->analogRealGain           = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
    pExposureInfo->digitalRegisterGain      = CalculateDigitalGain(pCalculateExposureData->realGain,
                                                                    pExposureInfo->analogRealGain);
    pExposureInfo->digitalRealGain          = static_cast<FLOAT>(pExposureInfo->digitalRegisterGain) /
                                                                    ps5280DigitalGainDecimator;
    pExposureInfo->ISPDigitalGain           =
        pCalculateExposureData->realGain     /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount                = pCalculateExposureData->lineCount;

    pExposureInfo->shortRegisterGain        = RealToRegisterGain(pCalculateExposureData->shortRealGain);
    pExposureInfo->shortAnalogRealGain      = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->shortRegisterGain));
    pExposureInfo->shortDigitalRegisterGain = CalculateDigitalGain(pCalculateExposureData->shortRealGain,
                                                                    pExposureInfo->shortAnalogRealGain);
    pExposureInfo->shortDigitalRealGain     = static_cast<FLOAT>(pExposureInfo->shortDigitalRegisterGain) /
                                                                   ps5280DigitalGainDecimator;
    pExposureInfo->shortISPDigitalGain      =
        pCalculateExposureData->shortRealGain / (pExposureInfo->shortAnalogRealGain * pExposureInfo->shortDigitalRealGain);
    pExposureInfo->shortLinecount           = pCalculateExposureData->shortLinecount;

    if (TRUE == pCalculateExposureData->applyShortExposure && FALSE == pCalculateExposureData->applyMiddleExposure &&
        SensorResolutionMode::SHDR == pCalculateExposureData->sensorResolutionMode)
    {
        CITLong          = EVEN(pCalculateExposureData->lineCount);
        CITShort         = EVEN(pCalculateExposureData->shortLinecount);
        frameLengthLines = (INT) pCalculateExposureData->frameLengthLines;
        BRL              = BRL_FIXED_VALUE + pCalculateExposureData->frameHeight;
        maxCITLong       = EVEN((frameLengthLines - (2 * CIT_MARGIN) - 8));
        maxCITShort      = EVEN((frameLengthLines - BRL - CIT_MARGIN - 2));

        // CITLong must range between SHDR_MIN_LINE_COUNT and maxCITLong
        CITLong = MAXVALUE(CITLong, SHDR_MIN_LINE_COUNT);
        CITLong = MINVALUE(CITLong, maxCITLong);

        // CITShort must range between SHDR_MIN_LINE_COUNT and maxCITShort
        CITShort = MAXVALUE(CITShort, SHDR_MIN_LINE_COUNT);
        CITShort = MINVALUE(CITShort, maxCITShort);

        // Ensure CITLong > CITShort and their sum doesn't exceed FLL
        maxCITLong2 = frameLengthLines - CITShort - (2 * CIT_MARGIN);
        CITLong = MAXVALUE(CITLong, CITShort);
        CITLong = MINVALUE(CITLong, maxCITLong2);

        pExposureInfo->lineCount      = CITLong;
        pExposureInfo->shortLinecount = CITShort;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillExposureSettings(
    RegSettingsInfo*        pRegSettingsInfo,
    SensorFillExposureData* pExposureData)
{
    UINT16  i                = 0;
    UINT32  index            = 0;
    UINT16  regCount         = 0;
    INT     CITLong          = 0;   // Coarse Integration Time for Long Exposure Frame
    INT     CITShort         = 0;   // Coarse Integration Time for Short Exposure Frame
    INT     frameOffset      = 0;   // Gap between start of Long Exposure and Short Exposure Frame
    INT     frameLengthLines = 0;
    INT     BRL              = 0;   // Basic Readout Line  Fixed + vertical Active pixel
    INT     maxFrameOffset1  = 0;   // Maximum Gap between start of Long Exposure and Short Exposure Frame
    INT     maxFrameOffset2  = 0;   // Maximum Gap between start of Long Exposure and Short Exposure Frame

    if ((NULL == pRegSettingsInfo) || (NULL == pExposureData))
    {
        return FALSE;
    }

    CITLong          = pExposureData->lineCount;
    CITShort         = pExposureData->shortLineCount;

    if (TRUE == pExposureData->applyShortExposure && FALSE == pExposureData->applyMiddleExposure &&
        SensorResolutionMode::SHDR == pExposureData->sensorResolutionMode)
    {
        frameLengthLines = (INT) pExposureData->frameLengthLines;
        BRL              = BRL_FIXED_VALUE + pExposureData->frameHeight;
        frameOffset      = 0;

        //Calculation of FrameOffset
        frameOffset     = EVEN((CITShort + CIT_MARGIN));
        maxFrameOffset1 = EVEN((frameLengthLines - BRL));
        maxFrameOffset2 = EVEN((frameLengthLines - CITLong - CIT_MARGIN));

        frameOffset = MINVALUE(frameOffset, maxFrameOffset1);
        frameOffset = MINVALUE(frameOffset, maxFrameOffset2);

        ps5280_DBG("Applied To Sensor: Long LC:%d, Short LC:%d, FrameOffset:%d", CITLong,
            CITShort, frameOffset);
    }

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount; index++)
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
        regCount++;
    }

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->frameLengthLinesAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->frameLengthLines & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->frameLengthLinesAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->frameLengthLines & 0xFF);
    regCount++;

    //Assigning values to registers for Long Exposure Frame
    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->coarseIntgTimeAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (CITLong & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData = (CITLong & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->globalAnalogGainAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->analogRegisterGain & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->GlobalDigitalGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->digitalRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->GlobalDigitalGainAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->digitalRegisterGain & 0xFF);
    regCount++;

    if(TRUE == pExposureData->applyShortExposure)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortCoarseIntgTimeAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (CITShort & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortCoarseIntgTimeAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (CITShort & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortGlobalAnalogGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortGlobalAnalogGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortGlobalDigitalGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortDigitalRegisterGain & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->shortGlobalDigitalGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortDigitalRegisterGain & 0xFF);
        regCount++;

        if(SensorResolutionMode::SHDR == pExposureData->sensorResolutionMode)
        {
            pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->frameOffsetSettings.shortFrameOffsetAddr;
            pRegSettingsInfo->regSetting[regCount].registerData = (frameOffset & 0xFF00) >> 8;
            regCount++;

            pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->frameOffsetSettings.shortFrameOffsetAddr + 1;
            pRegSettingsInfo->regSetting[regCount].registerData = (frameOffset & 0xFF);
            regCount++;
        }
    }

    for (index = 0; (pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index) < regCount; index++)
    {
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regAddrType =
            I2CRegAddressDataTypeWord;
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regDataType =
            I2CRegAddressDataTypeByte;
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].delayUs =
            0;
    }

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOffSettings.regSettingCount; index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regDataType;
        pRegSettingsInfo->regSetting[regCount].delayUs      =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].delayUs;
        regCount++;
    }

    pRegSettingsInfo->regSettingCount = regCount;

    for ( i = 0; i < regCount; i++)
    {
        ps5280_DBG("RegAddr: 0x%X, RegData: 0x%X, RegAddrType: %d, RegDataType: %d",
                pRegSettingsInfo->regSetting[i].registerAddr,
                pRegSettingsInfo->regSetting[i].registerData,
                pRegSettingsInfo->regSetting[i].regAddrType,
                pRegSettingsInfo->regSetting[i].regDataType);
    }

    regCount = 1;

    pRegSettingsInfo->regSettingCount = regCount;

    ps5280_DBG("Register Settings: %d", regCount);

    pRegSettingsInfo->regSetting[0].registerAddr = 0xEF;
    pRegSettingsInfo->regSetting[0].registerData = 0x00;
    pRegSettingsInfo->regSetting[0].regAddrType = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[0].regDataType = I2CRegAddressDataTypeByte;

    for ( i = 0; i < regCount; i++)
    {
        ps5280_DBG("RegAddr: 0x%X, RegData: 0x%X, RegAddrType: %d, RegDataType: %d",
                pRegSettingsInfo->regSetting[i].registerAddr,
                pRegSettingsInfo->regSetting[i].registerData,
                pRegSettingsInfo->regSetting[i].regAddrType,
                pRegSettingsInfo->regSetting[i].regDataType);
    }

    if (MAX_REG_SETTINGS <= regCount)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetSensorLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI)
{
    pSensorLibraryAPI->majorVersion          = 1;
    pSensorLibraryAPI->minorVersion          = 0;
    pSensorLibraryAPI->pCalculateExposure    = CalculateExposure;
    pSensorLibraryAPI->pFillExposureSettings = FillExposureSettings;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
