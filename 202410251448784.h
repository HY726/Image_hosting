////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxtypes.h
/// @brief CamX abstraction of common types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE PR002: Allow #ifdef
// NOWHINE FILE NC004c: Converting CSL header to C, we need typedef
// NOWHINE FILE GR017: Typedefing prohibited types in this file

#ifndef CAMXTYPES_H
#define CAMXTYPES_H

#include <stddef.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LINUX)
#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef int             BOOL;
typedef char            CHAR;
typedef uint8_t         BYTE;
typedef unsigned char   UCHAR;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef int             INT;
typedef unsigned int    UINT;
typedef float           FLOAT;

typedef uint8_t         UINT8;
typedef int8_t          INT8;
typedef uint16_t        UINT16;
typedef int16_t         INT16;
typedef uint32_t        UINT32;
typedef int32_t         INT32;
typedef uint64_t        UINT64;
typedef int64_t         INT64;

typedef max_align_t     MAXALIGN_T;
typedef size_t          SIZE_T;
typedef void            VOID;

typedef unsigned char           Word8u;
typedef short unsigned int      Word16u;
#ifndef COMDEF_H
typedef short signed int        Word16;
#endif // COMDEF_H
typedef unsigned int            Word32u;
typedef signed int              Word32;
typedef long long signed int    Word64;
typedef long long unsigned int  Word64u;

#if !defined (TRUE)
#define TRUE                1
#endif // !defined (TRUE)

#if !defined (FALSE)
#define FALSE               0
#endif // !defined (FALSE)

#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Windows Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined (_WINDOWS)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif // !defined(WIN32_LEAN_AND_MEAN)

#include <Windows.h>

#ifdef __cplusplus
#include <cstddef>
typedef std::max_align_t    MAXALIGN_T;
#endif // __cplusplus

#else

// #error Unsupported target

#endif // defined (_LINUX)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "camxreturntypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS Agnostic Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned int    BIT;
typedef double          DOUBLE;

typedef intptr_t        INTPTR_T;
typedef uintptr_t       UINTPTR_T;
typedef ptrdiff_t       PTRDIFF_T;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The type of the memory allocation. Must match ordering and size CamxMemTypeStrings.
typedef INT32               CamxMemType;
static const CamxMemType    CamxMemTypeAny      = 0;
/// @todo (CAMX-431): Add additional memory types here
// Increment the count below if adding additional memory types.
static const CamxMemType    CamxMemTypeCount    = 1;   ///< The number of memory types. Not to be used.

/// @brief The string name of the type of the memory allocation. Must match ordering and size of CamxMemType.
#if __GNUC__
static const CHAR* CamxMemTypeStrings[] __attribute__((unused)) =
#else // __GNUC__
static const CHAR* CamxMemTypeStrings[] =
#endif // _GNUC__
{
    "CamxMemTypeAny",   ///< Generic allocation with no associated type
    /// @todo (CAMX-431): Add additional memory types here
};

/// @brief Specifies the read/write protection for a buffer
struct CamxProtectionType
{
    BOOL read;  ///< Memory allocation is being read from
    BOOL write; ///< Memory allocation is being written to
};

static const CamxProtectionType CamxNoProtection    = { 0, 0 };
static const CamxProtectionType CamxReadOnly        = { 1, 0 };
static const CamxProtectionType CamxWriteOnly       = { 0, 1 };
static const CamxProtectionType CamxReadWrite       = { 1, 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log Codes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The verbosity tag for a given debug print message
typedef UINT32 CamxLog;
static const CamxLog CamxLogDebug                   = 0;            ///< Debug messages
static const CamxLog CamxLogError                   = 1;            ///< Error messages
static const CamxLog CamxLogWarning                 = 2;            ///< Warning messages
static const CamxLog CamxLogConfig                  = 3;            ///< Config messages
static const CamxLog CamxLogInfo                    = 4;            ///< Informational messages
static const CamxLog CamxLogVerbose                 = 5;            ///< Verbose messages
static const CamxLog CamxLogPerfInfo                = 6;            ///< Performance info message
static const CamxLog CamxLogPerfWarning             = 7;            ///< Performance warning message
static const CamxLog CamxLogDRQ                     = 8;            ///< DRQ logging
static const CamxLog CamxLogMeta                    = 9;            ///< Metadata logging
static const CamxLog CamxLogEntryExit               = 10;           ///< Entry/Exit messages
static const CamxLog CamxLogReqMap                  = 11;           ///< Request mapping messages
static const CamxLog CamxLogDump                    = 12;           ///< Recovery Dump messages
static const CamxLog CamxLogCoreCfg                 = 13;           ///< Core Config messages
static const CamxLog CamxLogMax                     = 14;           ///< Max verbosity levels

// The group tag for a given debug print message
typedef UINT64 CamxLogGroup;
static const CamxLogGroup CamxLogGroupAFD           = (static_cast<UINT64>(1) << 0);    ///< AFD
static const CamxLogGroup CamxLogGroupSensor        = (static_cast<UINT64>(1) << 1);    ///< Sensor
static const CamxLogGroup CamxLogGroupTracker       = (static_cast<UINT64>(1) << 2);    ///< Tracker
static const CamxLogGroup CamxLogGroupISP           = (static_cast<UINT64>(1) << 3);    ///< ISP
static const CamxLogGroup CamxLogGroupPProc         = (static_cast<UINT64>(1) << 4);    ///< Post Processor
static const CamxLogGroup CamxLogGroupMemMgr        = (static_cast<UINT64>(1) << 5);    ///< MemMgr
static const CamxLogGroup CamxLogGroupPower         = (static_cast<UINT64>(1) << 6);    ///< Power (clock, bandwidth related)
static const CamxLogGroup CamxLogGroupHAL           = (static_cast<UINT64>(1) << 7);    ///< HAL
static const CamxLogGroup CamxLogGroupJPEG          = (static_cast<UINT64>(1) << 8);    ///< JPEG
static const CamxLogGroup CamxLogGroupStats         = (static_cast<UINT64>(1) << 9);    ///< Stats
static const CamxLogGroup CamxLogGroupCSL           = (static_cast<UINT64>(1) << 10);   ///< CSL
static const CamxLogGroup CamxLogGroupApp           = (static_cast<UINT64>(1) << 11);   ///< Application
static const CamxLogGroup CamxLogGroupUtils         = (static_cast<UINT64>(1) << 12);   ///< Utilities
static const CamxLogGroup CamxLogGroupSync          = (static_cast<UINT64>(1) << 13);   ///< Sync
static const CamxLogGroup CamxLogGroupMemSpy        = (static_cast<UINT64>(1) << 14);   ///< MemSpy
static const CamxLogGroup CamxLogGroupFormat        = (static_cast<UINT64>(1) << 15);   ///< Format
static const CamxLogGroup CamxLogGroupCore          = (static_cast<UINT64>(1) << 16);   ///< Core
static const CamxLogGroup CamxLogGroupHWL           = (static_cast<UINT64>(1) << 17);   ///< HWL
static const CamxLogGroup CamxLogGroupChi           = (static_cast<UINT64>(1) << 18);   ///< CHI
static const CamxLogGroup CamxLogGroupDRQ           = (static_cast<UINT64>(1) << 19);   ///< DRQ
static const CamxLogGroup CamxLogGroupFD            = (static_cast<UINT64>(1) << 20);   ///< FD
static const CamxLogGroup CamxLogGroupIQMod         = (static_cast<UINT64>(1) << 21);   ///< IQ module
static const CamxLogGroup CamxLogGroupLRME          = (static_cast<UINT64>(1) << 22);   ///< LRME
static const CamxLogGroup CamxLogGroupCVP           = (static_cast<UINT64>(1) << 22);   ///< CVP
static const CamxLogGroup CamxLogGroupNCS           = (static_cast<UINT64>(1) << 23);   ///< NCS
static const CamxLogGroup CamxLogGroupMeta          = (static_cast<UINT64>(1) << 24);   ///< Metadata
static const CamxLogGroup CamxLogGroupAEC           = (static_cast<UINT64>(1) << 25);   ///< AEC
static const CamxLogGroup CamxLogGroupAWB           = (static_cast<UINT64>(1) << 26);   ///< AWB
static const CamxLogGroup CamxLogGroupAF            = (static_cast<UINT64>(1) << 27);   ///< AF
static const CamxLogGroup CamxLogGroupSWP           = (static_cast<UINT64>(1) << 28);   ///< SW Process
static const CamxLogGroup CamxLogGroupHist          = (static_cast<UINT64>(1) << 29);   ///< Histogram Process
static const CamxLogGroup CamxLogGroupBPS           = (static_cast<UINT64>(1) << 30);   ///< BPS
static const CamxLogGroup CamxLogGroupDebugData     = (static_cast<UINT64>(1) << 31);   ///< Debug-Data Framework -
                                                                                        ///  3A Debug-Data & Tuning-metadata
static const CamxLogGroup CamxLogGroupQSAT          = (static_cast<UINT64>(1) << 32);   ///< QSAT
static const CamxLogGroup CamxLogGroupQLL           = CamxLogGroupStats;                ///< QLL share for now
static const CamxLogGroup CamxLogGroupPSM           = (static_cast<UINT64>(1) << 33);   ///< PSM
static const CamxLogGroup CamxLogGroupStatsNN       = (static_cast<UINT64>(1) << 34);   ///< StatsNN
static const CamxLogGroup CamxLogGroupCRE           = (static_cast<UINT64>(1) << 35);   ///< CRE

/// @brief Trace enablement structure
struct CamxTraceInfo
{
    CamxLogGroup    groupsEnable;        ///< Tracing groups enable bits
    BOOL            traceErrorLogEnable; ///< Enable tracing for error logs
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Debug print settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DebugLogInfo
{
    CamxLogGroup    groupsEnable[CamxLogMax];               ///< Logging groups enable bits per log level
    FILE*           pDebugLogFile;                          ///< Debug log filehandle or NULL when disabled
    BOOL            systemLogEnable;                        ///< Global logging enable flag
    BOOL            enableAsciiLogging;                     ///< Global offline ascii logging enable
    BOOL            isUpdated;                              ///< Flag to indicate debugInfo has been updated
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Time structure
struct CamxTime
{
    UINT64  seconds;                    ///< Time in whole seconds
    UINT64  nanoSeconds;                ///< Fractional nanoseconds
};

/// @brief Date time structure
struct CamxDateTime
{
    UINT64  wholeseconds;               ///< System Time in whole seconds
    UINT64  microseconds;               ///< Microseconds 0-999999
    UINT32  seconds;                    ///< Seconds after the minute 0-61*
    UINT32  minutes;                    ///< Time in minutes 0-59
    UINT32  hours;                      ///< Time in hours 0-23
    UINT32  dayOfMonth;                 ///< Day of month 1-31
    UINT32  month;                      ///< Months since January 0-11, where 0 is January
    UINT32  year;                       ///< Year since 1900
    UINT32  weekday;                    ///< Day of week 0-6
    UINT32  yearday;                    ///< Day of year 0-365
    UINT32  dayLightSavingTimeFlag;     ///< Day light saving time flag
    INT64   gmtOffset;                  ///< Offset from GMT
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Miscellaneous common types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Rational number type
struct CamxRational
{
    INT32 numerator;    ///< Rational number numerator
    INT32 denominator;  ///< Rational number denominator
};

/// @brief dimension type
struct CamxDimension
{
    UINT32 width;    ///< width
    UINT32 height;   ///< height
};

/// @brief camx metadata types
enum CamxMetadataType
{
    InvalidType = 0,        ///< Invalid metadata type
    InputPoolType,          ///< Input pool metadata type
    ResultPoolType,         ///< Result pool metadata type
    UsecasePoolType,        ///< Usecase pool metadata type
    StaticPoolType,         ///< Static pool metadata type
    PortSpecificType,       ///< Port specific metadata type
    MaxMetadataType,        ///< Max metadata type
};

/// @brief StreamCropStatus
enum StreamCropStatus
{
    Disabled = 0,   ///< Disabled Status
    Enabled,        ///< Enabled Status
    LocalDisable,   ///< Disable StreamCrop on specific pipeline
    Uninitialized   ///< Uninitialized
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CAMXTYPES_H
