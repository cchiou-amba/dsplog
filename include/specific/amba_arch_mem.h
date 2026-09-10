/*
 * amba_arch_mem.h
 *
 * History:
 *	2021/05/26 - [Zhaoyang Chen] created file
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

#ifndef __AMBA_ARCH_MEM_H__
#define __AMBA_ARCH_MEM_H__

#ifdef AMBA_AMYOC_BUILD
#include <ambvideo_conf.h>
#endif
#if defined(AMBA_SOC_CV5)
#include <dsp/cv5/dsp_cv5_ucode_mem.h>
#elif defined(AMBA_SOC_CV52)
#include <dsp/cv52/dsp_cv52_ucode_mem.h>
#elif defined(AMBA_SOC_CV72)
#include <dsp/cv72/dsp_cv72_ucode_mem.h>
#elif defined(AMBA_SOC_CV75)
#include <dsp/cv75/dsp_cv75_ucode_mem.h>
#elif defined(AMBA_SOC_N1)
#include <dsp/n1/dsp_n1_ucode_mem.h>
#elif defined(AMBA_SOC_CV3AD685) /* Fixme, just to pass compile */
#include <dsp/n1/dsp_n1_ucode_mem.h>
#elif defined(AMBA_SOC_N1_655)
#include <dsp/n1_655/dsp_n1_655_ucode_mem.h>
#elif defined(AMBA_SOC_CV7)
#include <dsp/cv7/dsp_cv7_ucode_mem.h>
#elif defined(AMBA_SOC_CV8)
#include <dsp/cv8/dsp_cv8_ucode_mem.h>
#else
#error "Unsupported CHIP ID, please check sdk config"
#endif

/**
 * IAV / DSP MEMORY Layout:
 *
 *       +----------------------+ <--- IDSP_RAM_START
 *       | DSP_BSB_SIZE         |
 *       +----------------------+
 *       | DSP_INT_BSB_SIZE     |
 *       +----------------------+
 *       | DSP_IAVRSVD_SIZE     |
 *       +----------------------+
 *       | DSP_BUFFER_SIZE      |
 *       +----------------------+
 *       | DSP_CMD_BUF_SIZE     |
 *       +----------------------+
 *       | DSP_MSG_BUF_SIZE     |
 *       +----------------------+
 *       | DSP_BSH_SIZE         |
 *       +----------------------+
 *       | DSP_LOG_SIZE         |
 *       +----------------------+
 *       | DSP_UCODE_SIZE       |
 *       +----------------------+
 *
 */

/*
 * Note:
 *   1. "start" means the first byte of physical address.
 *   2. "base" means the first byte of virtual address.
 *   3. DSP_INT_BSB_SIZE, DSP_BSB_SIZE ,DSP_IAVRSVD_SIZE are specified by menuconfig.
 *   4. DSP_IAVRSVD_SIZE is the size of memory reserved for IAV drivers.
 *   5. DSP_BSH_SIZE is the size of memory for storing BIT_STREAM_HDR.
 */

/* IAV_MARGIN_SIZE is the memory margin size for IAV driver excluding IMG/BSB.
 * It is used to decide the reserved memory for IAV driver.
 * Normally below condition should be satisfied.
 * IAV_MARGIN_SIZE + IAV_DRAM_IMG + IAV_DRAM_BSB + IAV_DRAM_INT_BSB >= IAV_DRAM_MAX
 */


#ifndef DBGBUS_BASE
#if defined(AMBA_SOC_CV72) || defined(AMBA_SOC_CV75) || defined(AMBA_SOC_N1) || defined(AMBA_SOC_N1_655) || defined(AMBA_SOC_CV8)
#define DBGBUS_BASE			(0xFFED000000)
#else /* CV5x and CV7 */
#define DBGBUS_BASE			(0x20ED000000)
#endif
#endif

#if defined(AMBA_SOC_CV8)

#ifndef VIN_BASE_OFFSET
#define VIN_BASE_OFFSET		(0x1F0000)
#endif

#ifndef SECTION_SEL_REG_OFFSET
#define SECTION_SEL_REG_OFFSET	(0x1F8000)
#endif

#ifndef SECTION_RST_REG_OFFSET
#define SECTION_RST_REG_OFFSET	(0x1F8180)
#endif

#else

#ifndef VIN_BASE_OFFSET
#define VIN_BASE_OFFSET		(0x1C0000)
#endif

#ifndef SECTION_SEL_REG_OFFSET
#define SECTION_SEL_REG_OFFSET	(0x1E8000)
#endif

#ifndef SECTION_RST_REG_OFFSET
#define SECTION_RST_REG_OFFSET	(0x1E8180)
#endif

#endif /* AMBA_SOC_CV8 */

#ifndef IAV_ROI_NUM_FOR_IPB_FRAMES
#define IAV_ROI_NUM_FOR_IPB_FRAMES		1
#endif

/* DSP buffer size */
#ifndef AMBCMA_DEFAULT_DSP_BUFFER_SIZE
#define AMBCMA_DEFAULT_DSP_BUFFER_SIZE		(0xF000000)
#endif

#ifndef IAV_MEM_INTRA_PB_SIZE
#define IAV_MEM_INTRA_PB_SIZE	0
#endif

#ifndef IAV_MEM_USR_SIZE
#define IAV_MEM_USR_SIZE		0
#endif

#ifndef IAV_MEM_MV_SIZE
#define IAV_MEM_MV_SIZE		0
#endif

#ifndef IAV_MV_STREAM_NUM
#define IAV_MV_STREAM_NUM	0
#endif

#ifndef CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM
#define CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM	0
#endif

#ifndef IAV_MEM_OVERLAY_SIZE
#define IAV_MEM_OVERLAY_SIZE			0
#endif

#ifndef IAV_MEM_CANVAS_OVERLAY_SIZE
#define IAV_MEM_CANVAS_OVERLAY_SIZE			0
#endif

#ifndef IAV_MEM_BLUR_SIZE
#define IAV_MEM_BLUR_SIZE				0
#endif

#ifndef IAV_MEM_SMOOTH_BLUR_SIZE
#define IAV_MEM_SMOOTH_BLUR_SIZE		0
#endif

#ifndef IAV_MEM_IMG_SBP_SIZE
#define IAV_MEM_IMG_SBP_SIZE			0
#endif

#ifndef IAV_MEM_IMG_NN_BF_SIZE
#define IAV_MEM_IMG_NN_BF_SIZE	0
#endif

#if defined(AMBA_SOC_CV8)
#define IAV_MEM_IMG_IK_CFG_SIZE_PER_CHANNEL	(0x1400000)
#else
#define IAV_MEM_IMG_IK_CFG_SIZE_PER_CHANNEL	(0x800000)
#endif

#ifndef IAV_MEM_IMG_IK_CFG_SIZE
#define IAV_MEM_IMG_IK_CFG_SIZE		(IAV_MEM_IMG_IK_CFG_SIZE_PER_CHANNEL * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)
#endif

#if (IAV_MEM_IMG_IK_CFG_SIZE == 0)
#undef IAV_MEM_IMG_IK_CFG_SIZE
#define IAV_MEM_IMG_IK_CFG_SIZE		(IAV_MEM_IMG_IK_CFG_SIZE_PER_CHANNEL * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)
#endif

#if !defined(IAV_MEM_IMG_RSVD_SIZE) || (IAV_MEM_IMG_RSVD_SIZE == 0)
#undef IAV_MEM_IMG_RSVD_SIZE
#if defined(AMBA_SOC_N1_655)
#define IAV_MEM_IMG_RSVD_SIZE		(1792 << 10)
#else
#define IAV_MEM_IMG_RSVD_SIZE		(768 << 10)
#endif
#endif

#ifndef IAV_MEM_ROI_MATRIX_SIZE
#define IAV_MEM_ROI_MATRIX_SIZE		0
#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_WARP_MEM
#define CONFIG_AMBARELLA_IAV_DRAM_WARP_MEM		0
#endif

#ifndef DSP_INT_BSB_SIZE
#define DSP_INT_BSB_SIZE	0
#endif

#ifndef DSP_BSB_SIZE
#define DSP_BSB_SIZE		0
#endif

#ifndef DSP_CV_SHARE_BUF_SIZE
#define DSP_CV_SHARE_BUF_SIZE	0x100000
#endif

#ifndef IAV_EXTRA_RAW_SIZE
#define IAV_EXTRA_RAW_SIZE		0
#endif

#ifndef CONFIG_AMBARELLA_MAX_VIN_WIDTH
#define CONFIG_AMBARELLA_MAX_VIN_WIDTH		(4096)
#endif

#ifndef CONFIG_AMBARELLA_MAX_VIN_HEIGHT
#define CONFIG_AMBARELLA_MAX_VIN_HEIGHT		(4096)
#endif


#ifndef DSP_FASTDATA_SIZE
#define DSP_FASTDATA_SIZE			0
#endif

#ifndef DSP_FASTAUDIO_SIZE
#define DSP_FASTAUDIO_SIZE			0
#endif

#ifndef IAV_MEM_AISP_SIZE
#define IAV_MEM_AISP_SIZE			0
#endif

#ifndef IAV_MEM_IMG_NN_BUF_SIZE
#define IAV_MEM_IMG_NN_BUF_SIZE		0
#endif

#ifndef IAV_MEM_YUV_REMAP_SIZE
#define IAV_MEM_YUV_REMAP_SIZE		0
#endif

#ifndef IAV_MEM_ARB_BLEND_ALPHA_SIZE
#define IAV_MEM_ARB_BLEND_ALPHA_SIZE		0
#endif

#ifndef IAV_MEM_OF_NN_SIZE
#define IAV_MEM_OF_NN_SIZE		0
#endif

#ifndef IAV_MEM_TML_FUSION_SIZE
#define IAV_MEM_TML_FUSION_SIZE		0
#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_DECODE_ONLY

#define	IAV_MASK_RSVD_SIZE	(((7 << 20) + (512 << 10)) * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)

#define	IAV_PIC_STAT_RSVD_SIZE	((64 << 10) * CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM)

#ifndef CONFIG_AMBARELLA_IAV_ROI_PBG

#if (IAV_ROI_NUM_FOR_IPB_FRAMES == 1)
#define IAV_MARGIN_SIZE		((16 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 2)
#define IAV_MARGIN_SIZE		((21 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 3)
#define IAV_MARGIN_SIZE		((26 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 0)
#define IAV_MARGIN_SIZE		((13 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#endif

#else

#if (IAV_ROI_NUM_FOR_IPB_FRAMES == 1)
#define IAV_MARGIN_SIZE		((19 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 2)
#define IAV_MARGIN_SIZE		((24 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 3)
#define IAV_MARGIN_SIZE		((29 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 0)
#define IAV_MARGIN_SIZE		((16 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#endif

#endif

#else

#define IAV_MARGIN_SIZE		0

#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_DECODE_ONLY
#define IK_DEF_BIN_SIZE			(256 << 10)
#define IK_BUF_SIZE				(IAV_MEM_IMG_IK_CFG_SIZE)
#define DSP_SBP_SIZE			(IAV_MEM_IMG_SBP_SIZE)
#define DSP_NN_BF_SIZE			(IAV_MEM_IMG_NN_BF_SIZE)
#define IAV_IMGRSVD_SIZE		(IAV_MEM_IMG_RSVD_SIZE)

#define IK_DEF_BIN_OFFSET		(0)
#define IK_BUF_OFFSET			(IK_DEF_BIN_OFFSET + IK_DEF_BIN_SIZE)
#define IK_DSP_SBP_OFFSET		(IK_BUF_OFFSET + IK_BUF_SIZE)
#define IK_DSP_NN_BF_OFFSET		(IK_DSP_SBP_OFFSET + IAV_MEM_IMG_SBP_SIZE)
#define IK_IAV_IMGRSVD_OFFSET	(IK_DSP_NN_BF_OFFSET + DSP_NN_BF_SIZE)

/**
* IAV_PART_IMG Layout:
*       +--------------------+
*       | IK_DEF_BIN_SIZE    |
*       +--------------------+
*       | IK_BUF_SIZE        |
*       +--------------------+
*       | DSP_SBP_SIZE       |
*       +--------------------+
*       | DSP_NN_BF_SIZE     |
*       +--------------------+
*       | IAV_IMGRSVD_SIZE   |
*       +--------------------+
*/
#define DSP_IMG_SIZE			(IK_DEF_BIN_SIZE + IK_BUF_SIZE + DSP_SBP_SIZE + DSP_NN_BF_SIZE)
#define TOTAL_IMG_SIZE			(DSP_IMG_SIZE + IAV_IMGRSVD_SIZE)

#ifndef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE		0x0A00000
#endif

#if (DSP_IAVRSVD_SIZE < (TOTAL_IMG_SIZE + IAV_MARGIN_SIZE))
#undef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE	(TOTAL_IMG_SIZE + IAV_MARGIN_SIZE)
//#  error "Reserved IAV driver memory size must be larger than 16MB."
#endif

#else

#define TOTAL_IMG_SIZE			0
#undef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE		0

#endif

#ifdef CONFIG_AMBARELLA_DSP_LOG_SIZE
#define DSP_LOG_SIZE			CONFIG_AMBARELLA_DSP_LOG_SIZE
#else
#define DSP_LOG_SIZE			(8 << 20)
#endif
#define DSP_BSH_SIZE			(16 << 10)

/* MSG size is 128 bytes on CV5x and 256 bytes on later chips, total is 2048 MSGs
 * for each port.
 * There are 6 MSG ports in total, and one MSG bank for each port for backup:
 * 128 * 2048 * 2 = 512 KB on CV5x, 256 * 2048 * 2 = 1024 KB on later chips.
 */
#define DSP_PORT_MSG_NUM				(2048)

#if defined(AMBA_SOC_CV5) || defined(AMBA_SOC_CV52)
#define DSP_PORT_MSG_SIZE		(64 << 12)
#else
#define DSP_PORT_MSG_SIZE		(128 << 12)
#endif
/* Note: DSP_MSG_Q_IDSP_VP and DSP_MSG_Q_VIN_VP are not used, so subtract 2 here */
#define DSP_MSG_BUF_SIZE_PART	(DSP_PORT_MSG_SIZE * (DSP_MSG_TOTAL_NUM - 2))
#define DSP_MSG_BUF_SIZE	(DSP_MSG_BUF_SIZE_PART * 2)
#define DSP_MSG_BUF_INFO_SIZE	(sizeof(dsp_msg_fifo_cb_t) * (DSP_MSG_TOTAL_NUM - 2))

/* CMD size is 128 bytes on CV5x and 256 bytes on later chips, total is 127 CMD + header for each port.
 * For the ASYNC & VIN cmd port: 128 * 128 * 3 = 48 KB on CV5x and 256 * 128 * 3 = 96KB on later chips.
 */
#if defined(AMBA_SOC_CV5) || defined(AMBA_SOC_CV52)
#define DSP_PORT_CMD_SIZE		(16 << 10)
#else
#define DSP_PORT_CMD_SIZE		(32 << 10)
#endif
#define DSP_CMD_BUF_SIZE		(DSP_PORT_CMD_SIZE * CMD_PORT_ACTIVE_NUM)

/* Default CMD size is 128 bytes, total is 127 CMD + header. */
#define DSP_DEF_CMD_BUF_SIZE		(DSP_PORT_CMD_SIZE)

/* layout of IAV_PART_DSP_RSV */
#define DSP_RSV_BUF_SIZE					(DSP_CMD_BUF_SIZE + DSP_MSG_BUF_SIZE + \
											DSP_MSG_BUF_INFO_SIZE)
#define DSP_CMD_BUF_OFFSET					(0)
#define DSP_MSG_BUF_OFFSET					(DSP_CMD_BUF_OFFSET + DSP_CMD_BUF_SIZE)
#define DSP_MSG_BUF_INFO_OFFSET				(DSP_MSG_BUF_OFFSET + DSP_MSG_BUF_SIZE)

#endif	// __AMBA_ARCH_MEM_H__

