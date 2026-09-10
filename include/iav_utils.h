/*
 * iav_util.h
 *
 * History:
 *	2008/1/25 - [Oliver Li] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
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


#ifndef __IAV_UTILS_H__
#define __IAV_UTILS_H__

#ifdef AMBA_AMYOC_BUILD
#include <ambvideo_conf.h>
#endif
#include <amba_arch_mem_lnx.h>
#include "msg_print.h"
#include "ambcma_api.h"

#define KB			(1024)
#define MB			(1024 * 1024)
#define GB			(1024 * 1024 * 1024)
#define KB_ALIGN(addr)		ALIGN(addr, KB)
#define MB_ALIGN(addr)		ALIGN(addr, MB)

/*
 * U32_BOUNDARY: 2^32 (0x100000000). Use for u32 range / wrap / modulo semantics
 * (e.g. frame number wrap, overflow check). Do not use for address comparison.
 *
 * ADDR_4GB_BOUNDARY: Same value; use for physical or DSP address comparison only
 * (e.g. below/above 4G, seg_id, DSP 32-bit address limit).
 */
#define U32_BOUNDARY		(4ULL * GB)
#define ADDR_4GB_BOUNDARY	U32_BOUNDARY

#define AUDIO_CLK_KHZ		(12288)
#define PTS_CLK				(90000)

#include <linux/delay.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <soc/ambarella/iav_helper.h>
#include <linux/spi/spi.h>
#include <soc/ambarella/spi.h>
#else
#include <plat/iav_helper.h>
#include <plat/spi.h>
#include <plat/fb.h>
#endif
#define clean_d_cache		ambcache_clean_range
#define invalidate_d_cache	ambcache_inv_range

/*
 * SET_DSP_DADDR_PHYS / SET_DSP_DADDR_PHYS_ADD
 *
 * PHYS_TO_DSP must be defined for the platform (see below). Branching is by AMBA_SOC_* only
 * (this header does not rely on SUPPORT_DRAM_40BIT from cmd headers).
 *
 * SET_DSP_DADDR_PHYS(ptr, phys)
 *   ptr:  dsp_daddr_t * — embedded field in a DSP cmd/msg, e.g. &dsp_cmd->x.
 *   phys: phys_addr_t (or compatible); stored once in __dsp_phys_ (see macro body).
 *
 * SET_DSP_DADDR_PHYS_ADD(ptr, phys, offset)
 *   ptr:    dsp_daddr_t *.
 *   phys:   physical base; must be evaluated once — use __dsp_phys_ because it is used in
 *           PHYS_TO_DSP() and in IAV_PART_DSP MMB range checks.
 *   offset: byte offset added after PHYS_TO_DSP(phys); cast to u32 inline (single use).
 *
 * CV7, CV8: dsp_daddr_t is struct (lo/hi/rsvc/seg_id). seg_id: linear < 4G -> 0; linear >= 4G and
 * phys inside IAV_PART_DSP mmb -> 1; else -> 16.
 * Other supported SOCs: dsp_daddr_t is u32, assign *(ptr).
 * New chip: add an explicit #elif here or you get #error.
 */
#if defined(AMBA_SOC_CV5) || defined(AMBA_SOC_CV52) || defined(AMBA_SOC_CV72) || \
	defined(AMBA_SOC_CV75) || defined(AMBA_SOC_N1) || defined(AMBA_SOC_N1_655) || \
	defined(AMBA_SOC_CV3AD685)
#define SET_DSP_DADDR_PHYS(ptr, phys) \
		SET_DSP_DADDR_PHYS_ADD((ptr), (phys), 0)

#define SET_DSP_DADDR_PHYS_ADD(ptr, phys, offset) \
		do { *(ptr) = (u32)(PHYS_TO_DSP(phys) + (u32)(offset)); } while (0)
#else
#define SET_DSP_DADDR_PHYS(ptr, phys) \
		SET_DSP_DADDR_PHYS_ADD((ptr), (phys), 0)

#define SET_DSP_DADDR_PHYS_ADD(ptr, phys, offset)				\
		do {								\
			phys_addr_t __dsp_phys_ = (phys);			\
			u64 __dsp_addr = (u64)PHYS_TO_DSP(__dsp_phys_) +	\
				(u32)(offset);					\
			struct ambcma_mmb *__dsp_mmb_ = ambarella_query_mmb(IAV_PART_DSP); \
										\
			(ptr)->lo = (u32)__dsp_addr;				\
			(ptr)->hi = (u8)(__dsp_addr >> 32); 		\
			(ptr)->rsvc = 0;					\
			if (__dsp_addr < ADDR_4GB_BOUNDARY) {			\
				(ptr)->seg_id = 0;				\
			} else if (__dsp_addr >= ADDR_4GB_BOUNDARY &&		\
					__dsp_mmb_ &&				\
					__dsp_phys_ >= __dsp_mmb_->phys_addr &&	\
					__dsp_phys_ < __dsp_mmb_->phys_addr + __dsp_mmb_->size) { \
				(ptr)->seg_id = 1;				\
			} else {						\
				(ptr)->seg_id = 16; 			\
			}							\
		} while (0)
#endif

/* CV7, CV8 support 40-bit dram for both DSP, ARM and VP.
 *     IDSP_PRIVATE_START, IDSP_PRIVATE_END can be cross 4G.
 *     IDSP_SHARED_START, IDSP_SHARED_END are within range (0, 4G).
 * CV72 / CV75 / N1 / N1-655 only support 40-bit dram for ARM and VP, while DSP is still 32-bit.
 *     IDSP_PRIVATE_START, IDSP_PRIVATE_END are within range (0, 4G).
 *     IDSP_SHARED_START, IDSP_SHARED_END are within range (0, 4G)
 * CV5 only supports 40-bit dram for ARM, while DSP and VP is still 32-bit, so DSP and VP has to
 * to be squeezed within the 4G range, to avoid memory shortage issue, ATT is enabled.
 */
typedef union {
	phys_addr_t phys_addr;
#if defined(AMBA_SOC_CV5) || defined(AMBA_SOC_CV52) || defined(AMBA_SOC_CV72) || \
	defined(AMBA_SOC_CV75) || defined(AMBA_SOC_N1) || defined(AMBA_SOC_N1_655) || \
	defined(AMBA_SOC_CV3AD685)
	u32 dsp_addr;
#else
	u64 dsp_addr;
#endif
} amba_addr_t;

/* CV5x use below memory remap for DSP and ARM */
#if defined(AMBA_SOC_CV5) || defined(AMBA_SOC_CV52)
/*
 * Take amba_addr_t as a glue layer between dsp_addr and phys_addr.
 * 1.When iav enc modules try to download addresses of configurations, IK etc to dsp,
 *   amba_addr_t become the glue layer between iav enc modules and dsp cmd download layer
 * 2.Used by buffer_cap queue buffer which aims at stitching producer(irq message[dsp_addr])
 *   and consumer(buffer query[phys_addr]).
 *
 *   +-----------------------+           +-----------------------------+
 *   |    iav enc modules    |           |       dequeue buffer        |
 *   |    (phys_addr)        |           |    (comsumer phys_addr)     |
 *   +-----------------------+           +-----------------------------+
 *              |                                      ^
 *              v                                      |
 *   +-----------------------+           +-----------------------------+
 *   |   address_glue_layer  |           |     address_glue_layer      |
 *   |    (amba_addr_t)      |           | (queue buffer amba_addr_t)  |
 *   +-----------------------+           +-----------------------------+
 *              |                                      ^
 *              v                                      |
 *   +-----------------------+           +-----------------------------+
 *   |dsp cmd download layer |           |    dsp irq msg procedure    |
 *   |    (dsp_addr)         |           |          (dsp_addr)         |
 *   +-----------------------+           +-----------------------------+
 *           Figure 1                               Figure 2
 */

/*
 * PHYS_TO_DSP(phys): physical (or host) address -> DSP linear address for cmd/msg payloads.
 * Return type is always u64 so the same expression works with dsp_daddr_t (u32 or 40-bit),
 * arithmetic, and CV7 full 40-bit range without widening/narrowing surprises.
 *
 * - CV5/CV52 (ATT): inverse of amba_dsp_to_phys(); when cma_pool.start_addr[IAV_CMA_POOL]
 *   is 4GiB, physical addresses at/above 4GiB map to DSP as (phys - 4GiB). Implemented
 *   in amba_phys_to_dsp(unsigned long) in ambcma_drv.c (64-bit kernel: unsigned long is 64-bit).
 * - CV7: identity; parameter may exceed 4GiB; no runtime check in the macro.
 * - Other (CV72/CV75/N1/...): identity; caller must keep phys in 4GiB range; no check in the macro.
 */
#define PHYS_TO_DSP(addr)	((u64)amba_phys_to_dsp((unsigned long)(addr)))
/* When the total DRAM size >= 8GB, DSP memory 0 ~ 4GB is remapped to Physical space 4GB ~ 8GB.
 * Else, the DSP memory addr is equal to Physical memory addr.
 */

/*
 *                                       +----------------------------------------------------------------+
 *                                       |                                                                |
 *                                       |        +--------------------------------------------------+    |
 *                                       |        |                                                  |    |
 *                                       |        +---------------------->IDSP_RAM_START=4GB         |    |
 *                                       |        |                 +---->4GB+IDSP_PRIVATE_SIZE_MB   |    |
 *                                       V        V                 |                                |    |
 *      +--------------------------------+--------+-----------------+---------------------------+    |    |
 *      |                                |        |                 |                           |    |    |
 *      +--------------------------------+--------+-----------------+---------------------------+    |    |
 *                                                ^                 ^                                |    |
 *                                                |                 |                                |    |
 *  +---------------------------------------------+                 |                                |    |
 *  |                        +--------------------------------------+                                |    |
 *  |                        |                                                                       |    |
 *  |      Mapped by ATT     |                                                                       |    |
 *  |                        |                                                                       |    |
 *  +----------------------- +------------+--------+                                                 |    |
 *  |                        |            |        |    DSP Memory layout(0~-4GB)                    |    |
 *  +----------------------- +------------+--------+                                                 |    |
 *  |<-IDSP_PRIVATE_SIZE_MB->|            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        +-------->IDSP_SHARED_END(4GB)--------------------+    |
 *  |                        |            |              Mapped by ATT                                    |
 *  |                        |            +----------------->IDSP_SHARED_START(4GB-IDSP_SHARE_SIZE_MB)----+
 *  |                        |
 *  |                        +------------------------------> IDSP_PRIVATE_END(0MB + IDSP_PRIVATE_SIZE_MB)
 *  |
 *  +------------------------------------------------------->IDSP_PRIVATE_START(0MB)
 *
 *
 * Due to the 32bit bus width, DSP and GDMA of archv6 can only access 0~4GB memory size. Here we use ATT to mmap
 * physical memory range[IDSP_RAM_START(4GB), IDSP_RAM_START(4GB) + IDSP_PRIVATE_SIZE_MB] to IDSP private memory
 * [IDSP_PRIVATE_START(0) ~ IDSP_PRIVATE_END(0+IDSP_PRIVATE_SIZE_MB)].On the contrary, We use ATT to mmap physical memory
 * range[IDSP_SHARED_START, IDSP_SHARED_END] directly to the same private dsp memory area[IDSP_SHARED_START, IDSP_SHARED_END]
 * So when DSP report a private IDSP_PRIVATE AREA address which is always smaller than IDSP_SHARED_START, converting DSP
 * private address to physical memory become a must step.
 *
 * And please note ATT is only supported on CV5x platform. It is not supported on CV7x / N1-655 / N1.
 */
/*
 *                  Physical memory
 *  +-----------------+-------+---------------+
 *  |                 |       |               |
 *  +-----------------+-------+---------------+
 *  |                 |       |               |
 *  |                 |       |               |
 *  |                 |       |               |
 *  |                 |       |               +---------------------------------------------------------------->DRAM_END
 *  |                 |       |               |                                                         ^
 *  |                 |       |               +----------------->IDSP_PRIVATE_END                       |
 *  |                 |       |                       ^                                                 |
 *  |                 |       |                       |                                                 |
 *  |                 |       |                 IDSP_PRIVATE_SIZE_MB                                    |
 *  |                 |       |                       |                                                 |
 *  |                 |       |                       V                                                 |
 *  |                 |       +-------------------------------> IDSP_PRIVATE_START                  DRAM_SIZE <= 4GB
 *  |                 |       |                                                                         |
 *  |                 |       +------------------------------->IDSP_SHARED_END                          |
 *  |                 |                              ^                                                  |
 *  |                 |                              |                                                  |
 *  |                 |                         IDSP_SHARED_SIZE_MB                                     |
 *  |                 |                              |                                                  |
 *  |                 |                              v                                                  |
 *  |                 +--------------------------------------->IDSP_SHARED_START                        V
 *  +--------------------------------------------------------------------------------------------------------->DRAM_START
 *
 */
#define DSP_TO_PHYS(addr)	((phys_addr_t)amba_dsp_to_phys((unsigned long)(addr)))

#elif defined(AMBA_SOC_CV7) || defined(AMBA_SOC_CV8)
/* CV7: 40-bit DSP; phys may exceed 4GiB; DSP linear == phys (no ATT). */
#define PHYS_TO_DSP(addr)	((u64)(addr))
#define DSP_TO_PHYS(addr)	((phys_addr_t)(addr))

#else
/* CV72/CV75/N1/N1-655: 32-bit DSP, identity; phys must stay within 4GiB (by design). */
#define PHYS_TO_DSP(addr)	((u64)(addr))
#define DSP_TO_PHYS(addr)	((phys_addr_t)(addr))
#endif

#ifndef DRV_PRINT
#ifdef CONFIG_PRINT_THRU_KMSG_MODULE
#define DRV_PRINT	print_drv
#else
#define DRV_PRINT	printk
#endif
#endif

#define amba_inc_idx_safe(write_index, max_item)		\
({		\
	wmb();		\
	(write_index) = ((((write_index) + 1) < (max_item)) ? ((write_index) + 1) : 0);		\
})

#define amba_dec_idx_safe(write_index, max_item)		\
({		\
	wmb();		\
	(write_index) = ((((write_index) - 1) >= 0) ? ((write_index) - 1) : ((max_item) - 1));		\
})

#define wake_event_after_signal(event, ms_time_out)		\
({		\
	int rval = -1, loop;		\
	iav_debug("Wake up by the signal.\n");		\
	loop = ms_time_out / HALF_SECOND_IN_MS;		\
	do {		\
		if (!loop) {		\
			break;		\
		}		\
		msleep(HALF_SECOND_IN_MS);		\
		if (event) {		\
			rval = 1;		\
			break;		\
		}		\
	} while (--loop);		\
	if (rval < 0) {		\
		iav_error("Failed to switch condition!\n");		\
	} else {		\
		iav_debug("Successfully switch condition!\n");		\
	}		\
	rval;		\
})

#define wake_event_after_signal_interval(event, ms_time_out, interval)		\
({		\
	int rval = -1, loop;		\
	iav_debug("Wake up by the signal.\n");		\
	loop = ms_time_out / interval;		\
	do {		\
		if (!loop) {		\
			break;		\
		}		\
		msleep(interval);		\
		if (event) {		\
			rval = 1;		\
			break;		\
		}		\
	} while (--loop);		\
	if (rval < 0) {		\
		iav_error("Failed to switch condition!\n");		\
	} else {		\
		iav_debug("Successfully switch condition!\n");		\
	}		\
	rval;		\
})

#define get_dsp_work_state(iav_state)		\
({		\
	u32 _dsp_work_state = DSP_WORK_STATE_IDLE;	\
	switch (iav_state) {	\
	case IAV_STATE_INIT:	\
		_dsp_work_state = DSP_WORK_STATE_INIT;	\
		break;	\
	case IAV_STATE_IDLE:	\
		_dsp_work_state = DSP_WORK_STATE_IDLE;	\
		break;	\
	case IAV_STATE_PREVIEW: \
		_dsp_work_state = DSP_WORK_STATE_PREVIEW;	\
		break;	\
	case IAV_STATE_ENCODING:	\
		_dsp_work_state = DSP_WORK_STATE_ENCODING;	\
		break;	\
	case IAV_STATE_DECODING:	\
		_dsp_work_state = DSP_WORK_STATE_DECODING;	\
		break;	\
	case IAV_STATE_EXITING_PREVIEW: \
		_dsp_work_state = DSP_WORK_STATE_EXITING_PREVIEW;	\
		break;	\
	default:	\
		iav_warn("Invalid iav state: %d.\n", iav_state);	\
		break;	\
	}	\
	_dsp_work_state;	\
})

#define get_iav_state(dsp_work_state)		\
({		\
	u32 _iav_state = IAV_STATE_IDLE; \
	switch (dsp_work_state) {	\
	case DSP_WORK_STATE_INIT:	\
		_iav_state = IAV_STATE_INIT; \
		break;	\
	case DSP_WORK_STATE_IDLE:	\
		_iav_state = IAV_STATE_IDLE; \
		break;	\
	case DSP_WORK_STATE_PREVIEW:	\
		_iav_state = IAV_STATE_PREVIEW;	\
		break;	\
	case DSP_WORK_STATE_ENCODING:	\
		_iav_state = IAV_STATE_ENCODING; \
		break;	\
	case DSP_WORK_STATE_DECODING:	\
		_iav_state = IAV_STATE_DECODING; \
		break;	\
	case DSP_WORK_STATE_EXITING_PREVIEW:	\
	case DSP_WORK_STATE_EXITING_PREVIEW_3A_STOPPED: \
		_iav_state = IAV_STATE_EXITING_PREVIEW;	\
		break;	\
	default:	\
		iav_warn("Invalid dsp_work_state: %d.\n", dsp_work_state);	\
		break;	\
	}	\
	_iav_state;	\
})

#define INVALID_FRAME_BUF_ID				(0xFFFFFFFF)

#define is_valid_dsp_addr(dsp_addr, base, limit)	\
({	\
	u8 _valid = ((dsp_addr) != 0) && ((dsp_addr) != 0xdeadbeef);	\
	if (_valid && (base) && (limit)) {	\
		_valid = ((dsp_addr) >= (base)) && ((dsp_addr) < (limit));	\
	}	\
	_valid;	\
})

/* NOTE: Since debug levels higher than KERN_DEBUG output msg through UART
   which blocks irq reaction time too long, this will affect DSP cmd transaction
   because DSP requires ARM to react it's interrupt as soon as possible in order
   to prevent DSP cmd loss. Here we change all levels to KERN_DEBUG. */

#if 0 // for debug usage
#define iav_trace()				DRV_PRINT(KERN_INFO ">>>> %s(%d)\n", __func__, __LINE__)
#define iav_debug(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_printk(str, arg...)	DRV_PRINT(KERN_INFO "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_error(str, arg...)	DRV_PRINT(KERN_ERR "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_warn(str, arg...)	DRV_PRINT(KERN_WARNING "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_info(str...)	DRV_PRINT(KERN_INFO str)
#else
#define iav_trace()				DRV_PRINT(KERN_DEBUG ">>>> %s(%d)\n", __func__, __LINE__)
#define iav_debug(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_printk(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_error(str, arg...)	DRV_PRINT(KERN_DEBUG "#iav_error# %s(%d): "str, __func__, __LINE__, ##arg)
#define iav_warn(str, arg...)	DRV_PRINT(KERN_DEBUG "#iav_warn# %s(%d): "str, __func__, __LINE__, ##arg)
#define iav_info(str...)	DRV_PRINT(KERN_DEBUG str)
#endif

#ifndef CONFIG_AMBARELLA_VIN_DEBUG
#define vin_debug(format, arg...)
#else
#define vin_debug(str, arg...)	iav_debug("VIN: "str, ##arg)
#endif
#define vin_printk(str, arg...)	iav_printk(str, ##arg)
#define vin_error(str, arg...)	iav_error("VIN: "str, ##arg)
#define vin_warn(str, arg...)	iav_warn("VIN: "str, ##arg)
#define vin_info(str, arg...)	iav_info("VIN: "str, ##arg)

#define vout_debug(str, arg...)	iav_debug("VOUT: "str, ##arg)
#define vout_error(str, arg...)	iav_error("VOUT: "str, ##arg)
#define vout_warn(str, arg...)	iav_warn("VOUT: "str, ##arg)
#define vout_info(str, arg...)	iav_info("VOUT: "str, ##arg)

#define led_debug(str, arg...) iav_debug("LED: "str, ##arg)
#define led_error(str, arg...) iav_error("LED: "str, ##arg)
#define led_warn(str, arg...)  iav_warn("LED: "str, ##arg)
#define led_info(str, arg...)  iav_info("LED: "str, ##arg)

#endif	// UTIL_H

