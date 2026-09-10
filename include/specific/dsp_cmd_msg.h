/**
 * dsp_cmd_msg.h
 *
 * History:
 *	2021/02/18 - [Zhaoyang Chen] created file
 *
 * Copyright (c) 2021 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef	__DSP_CMD_MSG_H__
#define	__DSP_CMD_MSG_H__

#include <linux/types.h>

#define ENABLE_IPROC

#if defined (AMBA_SOC_CV72)
#define PROJECT_CV7
#elif defined (AMBA_SOC_CV75)
#define PROJECT_CV7S
#elif defined(AMBA_SOC_N1)
#define PROJECT_CV6
#elif defined(AMBA_SOC_N1_655)
#define PROJECT_CV6S
#elif defined (AMBA_SOC_CV7)
#define PROJECT_CV5P
#define PROJECT_CV7
#elif defined (AMBA_SOC_CV5) || defined (AMBA_SOC_CV52)
#define PROJECT_CV5
#elif defined (AMBA_SOC_CV3AD685)
#define PROJECT_CV6A
#elif defined (AMBA_SOC_CV8)
#define PROJECT_CV8
#else
#error "Unsupported CHIP ID, please check sdk config"
#endif

//#define DSP_TYPE_DEFINED
#include "cmd_msg_dsp.h"
#include "cmd_msg_dsp_ipc_ext.h"
#include "idspdrv_cmd_msg.h"
#include "idspdrv_imgknl_if.h"

#define GET_DSP_CMD_CAT(x)		(((x)>>24)&0x7f)

#define DSP_VP_MSG_SIZE			(512)
#define VP_MSG_INFO_SIZE		(128)
#define DSP_VP_MSG_NUM			(128)

typedef enum {
	DAI_VDSP    = 0,
	DAI_VENC    = 1,
	DAI_VCAP    = 2,
	DAI_VIN     = 3,
	DAI_VPOSTP  = 4,
	DAI_VIN_PIP = 5,
	DAI_ORCVOUT = 6,
#if defined(AMBA_SOC_N1)
	DAI_NUM     = DAI_VIN_PIP + 1,
#elif defined(AMBA_SOC_CV8)
	DAI_NUM     = DAI_ORCVOUT + 1,
#else
	DAI_NUM     = DAI_VPOSTP + 1,
#endif
} DSP_AUDIT_INT;

typedef enum {
	DSP_INIT_MODE		= 0,
	DSP_IDLE_MODE		= 1,
	DSP_ENCODE_MODE		= 2,
	DSP_DECODE_MODE		= 3,
	DSP_DRAM_TEST_MODE	= 4,
} dsp_op_mode_t;

typedef enum {
	DSP_CMD_ARR_NUM = 8,

	MAX_NUM_CMD = 127,
	MAX_DEFAULT_CMD = 127,
	MAX_NUM_ENC_CMD = 64,
	MAX_DEFAULT_CMD_SIZE = (MAX_DEFAULT_CMD * DSP_CMD_SIZE),

	MAX_NUM_MSG = 64,
	NUM_MSG_CAT = 16,
} DSP_CMD_MSG_PARAM;

typedef enum {
	DSP_CHROMA_FORMAT_MONO = 0,
	DSP_CHROMA_FORMAT_YUV420 = 1,
	DSP_CHROMA_FORMAT_YUV422 = 2,
	DSP_CHROMA_FORMAT_YUV444 = 3,
} DSP_CHROMA_FORMAT;

typedef enum {
	DSP_RAW_FMT_FULL = 0,
	DSP_RAW_FMT_CMPR = 1,
	DSP_RAW_FMT_PACK_12BITS = 2,
	DSP_RAW_FMT_PACK_14BITS = 3,
	DSP_RAW_FMT_PACK_10BITS = 4,
	DSP_RAW_FMT_PACK_8BITS = 5,
} DSP_RAW_FORMAT;

/* for uv_nf_radius_max */
typedef enum {
	DSP_CHROMA_RADIUS_128 = 0,
	DSP_CHROMA_RADIUS_64 = 1,
	DSP_CHROMA_RADIUS_32 = 2,
} DSP_CHROMA_RADIUS;

typedef enum {
	DSP_USE_CORE_0 = 0x1,
	DSP_USE_CORE_1 = 0x2,
	DSP_USE_CORE_0_1 = 0x3,
} DSP_CORE_USAGE;

typedef enum {
	VDSP_DUAL_CORE_TEMPORAL = 0,
	VDSP_DUAL_CORE_SPATIAL = 1,
} VDSP_DUAL_CORE_MODE;

/* for pattern_demux_mode */
typedef enum {
	VIN_PAT_DEMUX_UNCMPR = 0,	/* pattern demux by the first u32 in uncompressed raw */
	VIN_PAT_DEMUX_AUX = 1,		/* pattern demux by aux data */
} VIN_PAT_DEMUX_MODE;

/// DSP VP message format
typedef struct {
  uint32_t  pay_load[DSP_VP_MSG_SIZE/4];
} dsp_vp_msg_t;

typedef dsp_msg_t	DSP_MSG;
typedef dsp_vp_msg_t	DSP_VP_MSG;
typedef dsp_cmd_t	DSP_CMD;
typedef struct {
	uint32_t cmd_code;
	uint8_t  vin_id;
	uint8_t  pay_load[DSP_CMD_SIZE - 5];
} DSP_VIN_CMD;

typedef enc_bits_info_t	BIT_STREAM_HDR;

typedef cmd_vout_osd_setup_t vout_osd_setup_t;
typedef cmd_vout_osd_buf_setup_t vout_osd_buf_setup_t;
typedef cmd_vout_default_img_setup_t vout_default_img_setup_t;
typedef cmd_vout_mixer_setup_t vout_mixer_setup_t;
typedef cmd_vout_display_setup_t vout_display_setup_t;
typedef cmd_vout_reset_t vout_reset_t;
typedef cmd_vout_display_csc_setup_t vout_display_csc_setup_t;
typedef cmd_vout_dve_setup_t vout_dve_setup_t;
typedef cmd_vout_osd_clut_setup_t vout_osd_clut_setup_t;
typedef cmd_vout_video_setup_t vout_video_setup_t;

#if defined(AMBA_SOC_CV8)
typedef enum {
	DSP_MSG_Q_GEN       = 0,		/* general msg */
	DSP_MSG_Q_VIN       = 1,		/* vin msg */
	DSP_MSG_Q_ASYNC     = 2,		/* async msg */
	DSP_MSG_Q_VOUT      = 3,		/* orcvout msg, only used for CV8 */
	DSP_MSG_Q_VPROC     = 4,		/* vproc msg */
	DSP_MSG_Q_IDSP_VP   = 5,		/* msg between idsp and VP */
	DSP_MSG_Q_VENC      = 6,		/* venc msg */
	DSP_MSG_Q_VIN_VP    = 7,		/* vin_vp msg */

	DSP_MSG_Q_VIN_PIP   = 0xff,		/* Not used, avoid compile issue */

	DSP_MSG_ACTIVE_NUM  = DSP_MSG_Q_VENC + 1,		/* Active MSG port num */
	DSP_MSG_TOTAL_NUM   = 8,		/* Total MSG port num */
} DSP_MSG_QUEUE_PARAM;

typedef enum {
	CMD_PORT_GEN        = 0,        /* general */
	CMD_PORT_VIN        = 1,        /* vin */
	CMD_PORT_ASYNC      = 2,        /* async */
	CMD_PORT_VOUT       = 3,        /* vout */
	CMD_PORT_VIN_0      = 4,        /* vin 0 */
	CMD_PORT_VIN_1      = 5,        /* vin 1 */
	CMD_PORT_VIN_2      = 6,        /* vin 2 */
	CMD_PORT_VIN_3      = 7,        /* vin 3 */
	CMD_PORT_VIN_4      = 8,        /* vin 4 */
	CMD_PORT_VIN_5      = 9,        /* vin 5 */
	CMD_PORT_VIN_6      = 10,       /* vin 6 */
	CMD_PORT_VIN_7      = 11,       /* vin 7 */
	CMD_PORT_VIN_8      = 12,       /* vin 8 */
	CMD_PORT_VIN_9      = 13,       /* vin 9 */
	CMD_PORT_VIN_10     = 14,       /* vin 10 */
	CMD_PORT_VIN_11     = 15,       /* vin 11 */
	CMD_PORT_VIN_12     = 16,       /* vin 12 */
	CMD_PORT_VIN_13     = 17,       /* vin 13 */
	CMD_PORT_VIN_14     = 18,       /* vin 14 */

	CMD_PORT_VIN_15     = 19,       /* vin 15 */
	CMD_PORT_VIN_16     = 20,       /* vin 16 */
	CMD_PORT_VIN_17     = 21,       /* vin 17 */
	CMD_PORT_VIN_18     = 22,       /* vin 18 */
	CMD_PORT_VIN_19     = 23,       /* vin 19 */
	CMD_PORT_VIN_20     = 24,       /* vin 20 */
	CMD_PORT_VIN_21     = 25,       /* vin 21 */
	CMD_PORT_VIN_22     = 26,       /* vin 22 */
	CMD_PORT_VIN_23     = 27,       /* vin 23 */
	CMD_PORT_VIN_24     = 28,       /* vin 24 */
	CMD_PORT_VIN_PIP    = 29,       /* Not used, only avoid compile issue */

	CMD_PORT_DSP_NUM    = 6,        /* Actual CMD port num between ARM and DSP */
	CMD_PORT_ACTIVE_NUM = CMD_PORT_VOUT + 1,         /* Total active CMD port num */
	CMD_PORT_TOTAL_NUM  = CMD_PORT_VIN_PIP + 1,       /* Total CMD port num including virtual CMD ports */
	CMD_PORT_VIN_FIRST  = CMD_PORT_VIN_0,
	CMD_PORT_VIN_LAST   = CMD_PORT_TOTAL_NUM,
} DSP_CMD_PORT_PARAM;

#else
typedef enum {
	DSP_MSG_Q_GEN       = 0,		/* general msg */
	DSP_MSG_Q_VIN       = 1,		/* vin msg */
	DSP_MSG_Q_ASYNC     = 2,		/* async msg */
	DSP_MSG_Q_VPROC     = 3,		/* vproc msg */
	DSP_MSG_Q_IDSP_VP   = 4,		/* msg between idsp and VP */
	DSP_MSG_Q_VENC      = 5,		/* venc msg */
	DSP_MSG_Q_VIN_PIP   = 6,		/* vin_pip msg */
	DSP_MSG_Q_VIN_VP    = 7,		/* vin_vp msg */

	DSP_MSG_Q_VOUT      = 0xff,		/* Not used, avoid compile issue */
#if defined(AMBA_SOC_N1)
	DSP_MSG_ACTIVE_NUM  = DSP_MSG_Q_VIN_PIP + 1,	/* Active MSG port num */
#else
	DSP_MSG_ACTIVE_NUM  = DSP_MSG_Q_VENC + 1,		/* Active MSG port num */
#endif
	DSP_MSG_TOTAL_NUM   = 8,		/* Total MSG port num */
} DSP_MSG_QUEUE_PARAM;

typedef enum {
	CMD_PORT_GEN        = 0,        /* general */
	CMD_PORT_VIN        = 1,        /* vin */
	CMD_PORT_ASYNC      = 2,        /* async */
	CMD_PORT_VIN_PIP    = 3,        /* vin pip */
	CMD_PORT_VIN_0      = 4,        /* vin 0 */
	CMD_PORT_VIN_1      = 5,        /* vin 1 */
	CMD_PORT_VIN_2      = 6,        /* vin 2 */
	CMD_PORT_VIN_3      = 7,        /* vin 3 */
	CMD_PORT_VIN_4      = 8,        /* vin 4 */
	CMD_PORT_VIN_5      = 9,        /* vin 5 */
	CMD_PORT_VIN_6      = 10,       /* vin 6 */
	CMD_PORT_VIN_7      = 11,       /* vin 7 */
	CMD_PORT_VIN_8      = 12,       /* vin 8 */
	CMD_PORT_VIN_9      = 13,       /* vin 9 */
	CMD_PORT_VIN_10     = 14,       /* vin 10 */
	CMD_PORT_VIN_11     = 15,       /* vin 11 */
	CMD_PORT_VIN_12     = 16,       /* vin 12 */
	CMD_PORT_VIN_13     = 17,       /* vin 13 */
	CMD_PORT_VIN_14     = 18,       /* vin 14 */

	CMD_PORT_VIN_15     = 19,       /* vin 15 */
	CMD_PORT_VIN_16     = 20,       /* vin 16 */
	CMD_PORT_VIN_17     = 21,       /* vin 17 */
	CMD_PORT_VIN_18     = 22,       /* vin 18 */
	CMD_PORT_VIN_19     = 23,       /* vin 19 */
	CMD_PORT_VIN_20     = 24,       /* vin 20 */
	CMD_PORT_VIN_21     = 25,       /* vin 21 */
	CMD_PORT_VIN_22     = 26,       /* vin 22 */
	CMD_PORT_VIN_23     = 27,       /* vin 23 */
	CMD_PORT_VIN_24     = 28,       /* vin 24 */
	CMD_PORT_VOUT       = 29,       /* Not used, only avoid compile issue */

	CMD_PORT_DSP_NUM    = 6,        /* Actual CMD port num between ARM and DSP */
#if defined(AMBA_SOC_N1)
	CMD_PORT_ACTIVE_NUM = CMD_PORT_VIN_PIP + 1,      /* Total active CMD port num */
#else
	CMD_PORT_ACTIVE_NUM = CMD_PORT_ASYNC + 1,        /* Total active CMD port num */
#endif
	CMD_PORT_TOTAL_NUM  = CMD_PORT_VOUT + 1,       /* Total CMD port num including virtual CMD ports */
	CMD_PORT_VIN_FIRST  = CMD_PORT_VIN_0,
	CMD_PORT_VIN_LAST   = CMD_PORT_TOTAL_NUM,
} DSP_CMD_PORT_PARAM;
#endif

// DSP 'header command', must be the first for each iteration
typedef struct {
	uint32_t cmd_code;
	uint32_t cmd_seq_num;
	uint32_t num_cmds;
	uint32_t cmd_block_crc;
	uint8_t pipeline_id; /* don't care in CV5; start from CV7, see @DSP_PIPELINE_ID */
	uint8_t reserved[3];
	uint32_t padding[DSP_CMD_SIZE / 4 - 5];
} DSP_HEADER_CMD;

typedef struct {
	uint32_t msg_code;
	uint32_t dsp_prof_id;
	uint32_t time_code;
	uint32_t prev_cmd_seq;
	uint32_t prev_num_cmds;
	uint32_t padding[DSP_MSG_SIZE / 4 - 5];
} DSP_STATUS_MSG;

/// VCAP "SETUP" modes
typedef enum VIN_RAW_CAPTURE_MODEtag {
	VIN_RAW_TIMER_MODE = 0,  ///< timer mode is the idle mode
	VIN_RAW_VIDEO_MODE = 1,  ///< video (recording) mode
	VIN_RAW_SELF_STOP_MODE = 2,
} VIN_RAW_CAPTURE_MODE;

typedef enum VCAP_SETUP_MODEtag {
	VCAP_TIMER_MODE			= 0,  ///< timer mode is the idle mode
	VCAP_VIDEO_MODE			= 1,  ///< video (recording) mode
	VCAP_BACKGROUND_MODE	= 2,
	VCAP_INIT_MODE			= 7,
} VCAP_SETUP_MODE;

typedef enum {
	DSP_VIN_ID_0 = 0,
	DSP_VIN_ID_1 = 1,
	DSP_VIN_ID_2 = 2,
	DSP_VIN_ID_3 = 3,
	DSP_VIN_ID_4 = 4,
	DSP_VIN_ID_5 = 5,
	DSP_VIN_ID_6 = 6,
	DSP_VIN_ID_7 = 7,
	DSP_VIN_ID_8 = 8,
	DSP_VIN_ID_9 = 9,
	DSP_VIN_ID_10 = 10,
	DSP_VIN_ID_11 = 11,
	DSP_VIN_ID_12 = 12,
	DSP_VIN_ID_13 = 13,
	DSP_VIN_ID_14 = 14,

	DSP_VIN_ID_15 = 15,
	DSP_VIN_ID_16 = 16,
	DSP_VIN_ID_17 = 17,
	DSP_VIN_ID_18 = 18,
	DSP_VIN_ID_19 = 19,
	DSP_VIN_ID_20 = 20,
	DSP_VIN_ID_21 = 21,
	DSP_VIN_ID_22 = 22,
	DSP_VIN_ID_23 = 23,
	DSP_VIN_ID_24 = 24,
#if defined(AMBA_SOC_N1)
	DSP_VIN_ID_NUM = DSP_VIN_ID_24 + 1,
#else
	DSP_VIN_ID_NUM = DSP_VIN_ID_14 + 1,
#endif
	DSP_VIN_ID_INVALID = 255,
	DSP_VIN_ID_FIRST = DSP_VIN_ID_0,
	DSP_VIN_ID_LAST = DSP_VIN_ID_NUM,
} DSP_VIN_ID;

typedef enum {
	DSP_FP_BIND_VIN_VPROC		= 0,	/* Bind VIN to IDSP */
	DSP_FP_BIND_VPROC_VENC		= 1,	/* Bind IDSP to Encoder */
	DSP_FP_BIND_VENC_ENG0		= 2,	/* Bind Encoder to CABAC */
	DSP_FP_BIND_VIN_VENC		= 3,	/* Bind VIN to Encoder */
	DSP_FP_BIND_DEC_POSTP		= 4,	/* Bind Decoder to Post-Processor */
	DSP_FP_BIND_EFM_VIN_VPROC	= 5,	/* Bind EFM VIN to IDSP */
	DSP_FP_BIND_EFM_VPROC_VENC	= 6,	/* Bind EFM IDSP to ENCODER */
	DSP_FB_BIND_CANVAS_VPROC	= 7,	/* Bind Canvas to IDSP */
} DSP_FP_BIND_SRC;

typedef enum {
	DSP_BUF_ALLOC_MODE_NEW = 0,
	DSP_BUF_ALLOC_MODE_APPEND = 1,
} DSP_BUF_ALLOC_MODE;

typedef enum {
	DSP_BUF_CTL_ALLOC_TYPE_DISTINCT_ADDR = 0,
	DSP_BUF_CTL_ALLOC_TYPE_START_ADDR = 1,
} DSP_BUF_CTL_ALLOCATION_TYPE;

typedef enum {
	DSP_BUF_CTL_ROTATE = 0,
	DSP_BUF_CTL_STALL = 1,
	DSP_BUF_CTL_DISABLE_DRAM = 2,
} DSP_BUF_CTL;

typedef enum {
	GO_MAP_SYNC_ERR_BIT = 0,
} C2Y_SYNC_FAIL_BIT;

typedef enum {
	EIS_WARP_SYNC_ERR_BIT = 2,
	IDSP_SYNC_ERR_BIT = EIS_WARP_SYNC_ERR_BIT,
} Y2Y_SYNC_FAIL_BIT;

typedef enum {
	OVERLAY_SYNC_ERR_BIT = 0,
	BLUR_SYNC_ERR_BIT = 1,
	YUV_REMAP_SYNC_ERR_BIT = 3,
	SOOTH_SYNC_ERR_BIT = 4,
	POSTP_SCALE_SYNC_ERR_BIT = 5,
} POSTP_SYNC_FAIL_BIT;

#if defined (AMBA_SOC_CV72) || defined(AMBA_SOC_CV75)
#define DSP_HEVC_STATE_REG	(0xFFED08008C)
#define DSP_HEVC_STATE_ON	(0xFFFFFFFF)
#define DSP_HEVC_STATE_OFF	(0xFFFFFFFB)
#elif defined(AMBA_SOC_N1)
#define DSP_HEVC_STATE_REG	(0xFFED08008C)
#define DSP_HEVC_STATE_ON	(0xFFFFFFFF)
#define DSP_HEVC_STATE_OFF	(0xFFFFFFF3)
#elif defined (AMBA_SOC_CV5) || defined (AMBA_SOC_CV52)
#define DSP_HEVC_STATE_REG	(0x20ED08008C)
#define DSP_HEVC_STATE_ON	(0xFFFFFFFF)
#define DSP_HEVC_STATE_OFF	(0xFFFF7F7F)
#elif defined(AMBA_SOC_N1_655) || defined(AMBA_SOC_CV3AD685)
#define DSP_HEVC_STATE_REG	(0x00000000)
#define DSP_HEVC_STATE_ON	(0x00000000)
#define DSP_HEVC_STATE_OFF	(0x00000000)
#elif defined(AMBA_SOC_CV7)
#define DSP_HEVC_STATE_REG	(0x20ED08008C)
#define DSP_HEVC_STATE_ON	(0xFFFFFFFF)
#define DSP_HEVC_STATE_OFF	(0xFFFFFFF3)
#elif defined(AMBA_SOC_CV8)
#define DSP_HEVC_STATE_REG	(0xFFED08008C)
#define DSP_HEVC_STATE_ON	(0xFFFFFFFF)
#define DSP_HEVC_STATE_OFF	(0xFFFFFFFB)
#else
#error "Unsupported CHIP ID, please check sdk config"
#endif

/* Helper: linear address from dsp_daddr_t; 32-bit: u32 value; 40-bit: lo + (hi<<32). (p) = pointer to dsp_daddr_t. */
#ifdef SUPPORT_DRAM_40BIT
#define DSP_DADDR_TO_U64(p) ((u64)((p)->lo) + ((u64)((p)->hi) << 32))
#define DSP_DADDR_SEG_ID(p) ((p)->seg_id)
#else
#define DSP_DADDR_TO_U64(p) ((u64)*(const u32 *)(p))
#define DSP_DADDR_SEG_ID(p) 0
#endif

#endif	// __DSP_CMD_MSG_H__
