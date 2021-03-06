/******************************************************************************
 *
 * Copyright 2010, Dream Chip Technologies GmbH. All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Dream Chip Technologies GmbH, Steinriede 10, 30827 Garbsen / Berenbostel,
 * Germany
 *
 *****************************************************************************/
#ifndef __AEC_H__
#define __AEC_H__

/**
 * @file aec.h
 *
 * @brief
 *
 *****************************************************************************/
/**
 * @page module_name_page Module Name
 * Describe here what this module does.
 *
 * For a detailed list of functions and implementation detail refer to:
 * - @ref module_name
 *
 * @defgroup AECM Auto white Balance Module
 * @{
 *
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <ebase/types.h>
#include <ebase/trace.h>
#include <ebase/builtins.h>
#include <ebase/dct_assert.h>
#include <common/return_codes.h>
#include <common/misc.h>
#include <common/cam_types.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************/
/**
 * @brief   The number of mean and hist.
 */
/*****************************************************************************/
#define CIFISP_AE_MEAN_MAX 25
#define CIFISP_HIST_BIN_N_MAX 16

typedef enum AecMeasuringMode_e
{
    AEC_MEASURING_MODE_INVALID    = 0,    /**< invalid histogram measuring mode   */
    AEC_MEASURING_MODE_1          = 1,    /**< Y = (R + G + B) x (85/256)         */
    AEC_MEASURING_MODE_2          = 2,    /**< Y = 16 + 0.25R + 0.5G + 0.1094B    */
    AEC_MEASURING_MODE_MAX,    
} AecMeasuringMode_t;

/*****************************************************************************/
/**
 *          AecDampingMode_t
 *
 * @brief   mode type of AEC Damping
 *
 */
/*****************************************************************************/
typedef enum AecDampingMode_e
{
	AEC_DAMPING_MODE_INVALID        = 0,        /* invalid (only used for initialization) */
	AEC_DAMPING_MODE_STILL_IMAGE    = 1,        /* damping mode still image */
	AEC_DAMPING_MODE_VIDEO          = 2,        /* damping mode video */
	AEC_DAMPING_MODE_MAX
} AecDampingMode_t;


/*****************************************************************************/
/**
 *          AecEcmMode_t
 *
 * @brief   mode type of AEC Exposure Conversion
 *
 */
/*****************************************************************************/
typedef enum AecEcmMode_e
{
	AEC_EXPOSURE_CONVERSION_INVALID = 0,        /* invalid (only used for initialization) */
	AEC_EXPOSURE_CONVERSION_LINEAR  = 1,         /* Exposure Conversion uses a linear function (eq. 38) */
	AEC_EXPOSURE_CONVERSION_MAX
} AecEcmMode_t;


/*****************************************************************************/
/**
 *          AecEcmFlickerPeriod_t
 *
 * @brief   flicker period types for the AEC algorithm
 *
 */
/*****************************************************************************/
typedef enum AecEcmFlickerPeriod_e
{
	AEC_EXPOSURE_CONVERSION_FLICKER_OFF   = 0x00,
	AEC_EXPOSURE_CONVERSION_FLICKER_100HZ = 0x01,
	AEC_EXPOSURE_CONVERSION_FLICKER_120HZ = 0x02
} AecEcmFlickerPeriod_t;

/*****************************************************************************/
/**
 *          AecConfig_t
 *
 * @brief   AEC Module configuration structure; used for re-configuration as well
 *
 *****************************************************************************/
typedef struct AecConfig_s
{

	Cam5x5UCharMatrix_t     		GridWeights;
	CamerIcIspHistMode_t	HistMode;

	float                   		SetPoint;                   /**< set point to hit by the ae control system */
	float                   		ClmTolerance;
	float                   		DampOverStill;              /**< damping coefficient for still image mode */
	float                   		DampUnderStill;             /**< damping coefficient for still image mode */
	float                   		DampOverVideo;              /**< damping coefficient for video mode */
	float                   		DampUnderVideo;             /**< damping coefficient for video mode */
	Cam6x1FloatMatrix_t     		EcmTimeDot;
	Cam6x1FloatMatrix_t     		EcmGainDot;
	AecDampingMode_t        		DampingMode;            	/**< damping mode */
	AecSemMode_t            		SemMode;                	/**< scene evaluation mode */
	AecEcmFlickerPeriod_t   		EcmFlickerSelect;      		/**< flicker period selection */
	uint8_t                         StepSize;
	float                           GainFactor;
	float                           GainBias;
	float                           LinePeriodsPerField;
	float                           PixelClockFreqMHZ;
	float                           PixelPeriodsPerLine;
	/* gain range */
	#define AEC_GAIN_RANG_MAX     (4 * 7)
	float               GainRange[AEC_GAIN_RANG_MAX];
	float               TimeFactor[4];
} AecConfig_t;

/*****************************************************************************/
/**
 *          Aec_stat_t
 *
 * @brief   AEC Module Hardware statistics structure
 *
 *****************************************************************************/
typedef struct AecStat_s {
	unsigned char  exp_mean[CIFISP_AE_MEAN_MAX];
	unsigned short hist_bins[CIFISP_HIST_BIN_N_MAX];
}AecStat_t;


struct AecDyCfg
{
	AecEcmFlickerPeriod_t  flicker;
	struct Cam_Win win;
};

/*****************************************************************************/
/**
*		  AecResult_t
 * @brief   Aec_Result.
 */
/*****************************************************************************/
typedef struct AecResult_s
{
	float coarse_integration_time;
	float analog_gain_code_global;
	int regIntegrationTime;
	int regGain;
	float PixelClockFreqMHZ;
	float PixelPeriodsPerLine;

	AecMeasuringMode_t meas_mode;
	struct Cam_Win meas_win;
	unsigned int actives;
	unsigned char GridWeights[CIFISP_AE_MEAN_MAX];
	uint8_t stepSize;
    	CamerIcIspHistMode_t HistMode;	
	float gainFactor;
	float gainBias;
} AecResult_t;

/*****************************************************************************/
/**
 * @brief   This function init the AEC instance.
 *
 * @param   AecConfig
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 * @retval  RET_OUTOFRANGE
 *
 *****************************************************************************/
RESULT AecInit
(
	AecConfig_t *AecConfig	
);

RESULT AecUpdateConfig
(
    AecConfig_t         *pConfig
);

RESULT AecStart
(
);

RESULT AecStop
(
);

/*****************************************************************************/
/**
 * @brief   This function single run the AEC instance.
 *
 * @param   Aec_stat_t
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 * @retval  RET_OUTOFRANGE
 *
 *****************************************************************************/
RESULT AecRun
(
	AecStat_t *ae_stat,
	AecResult_t *AecResult
);

/*****************************************************************************/
/**
 * @brief   This function get the AEC result.
 *
 * @param   
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 * @retval  RET_OUTOFRANGE
 *
 *****************************************************************************/
RESULT AecGetResults
(
	AecResult_t *AecResult 
);


/*****************************************************************************/
/**
 * @brief   This function release the AEC instance.
 *
 * @param   AecConfig
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 * @retval  RET_OUTOFRANGE
 *
 *****************************************************************************/
RESULT AecRelease
(
);


#ifdef __cplusplus
}
#endif

/* @} AECM */


#endif /* __AEC_H__*/
