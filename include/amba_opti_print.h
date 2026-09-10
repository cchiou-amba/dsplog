/*
 * amba_opti_print.h
 *
 * History:
 *	2024/08/28 - [Qiankun Li] created file
 *
 * Copyright (c) 2022 Ambarella, Inc.
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

#ifndef __AMBA_OPTI_PRINT__H__
#define __AMBA_OPTI_PRINT__H__

#ifdef AMBA_AMYOC_BUILD
#include <ambvideo_conf.h>
#endif
#define AMBA_OPTI_PRINT_POOL_MAX_NUM	8
#define AMBA_OPTI_PRINT_TEXT_LENGTH	224

struct amba_opti_print_ctx {
	struct list_head free_list;
	struct list_head print_list;
	void *pool[AMBA_OPTI_PRINT_POOL_MAX_NUM];
	u32 pool_num : 4;
	u32 reserved : 28;
	spinlock_t lock;
	u32 opti_print_bitmap;
	u32 opti_print_bitmap_prev;
	struct proc_dir_entry *opti_print_entry;
	struct platform_device *dev;
};

enum {
	AMBA_OPTI_PRINT_ERR_STRM_SYNC_OVERLAY = 0,
	AMBA_OPTI_PRINT_ERR_STRM_SYNC_BLUR = 1,
	AMBA_OPTI_PRINT_ERR_STRM_SYNC_REMAP = 2,
	AMBA_OPTI_PRINT_ERR_CHAN_SYNC_EIS = 3,
	AMBA_OPTI_PRINT_ERR_CHAN_GET_AAA_STATIS = 4,
	AMBA_OPTI_PRINT_ERR_CHAN_IDSP_SYNC = 5,
	AMBA_OPTI_PRINT_ERR_CHAN_SYNC_GO_MAP = 6,
	AMBA_OPTI_PRINT_ERR_CHAN_SYNC_OF_NN = 7,
	AMBA_OPTI_PRINT_ERR_CANVAS_SYNC_POSTP_SCALE = 8,
	AMBA_OPTI_PRINT_ERR_NUM = 9,
};

enum {
	AMBA_OPTI_PRINT_VIN = 0,
	AMBA_OPTI_PRINT_CHAN = 1,
	AMBA_OPTI_PRINT_STREAM = 2,

	AMBA_OPTI_PRINT_NUM = 3,
	AMBA_OPTI_PRINT_FIRST = AMBA_OPTI_PRINT_VIN,
	AMBA_OPTI_PRINT_LAST = AMBA_OPTI_PRINT_NUM,
};

struct amba_opti_print_error_log_item {
	struct list_head node;
	u32 err_id;
	u32 err_count;
	u32 pts;
	char log[AMBA_OPTI_PRINT_TEXT_LENGTH];
};

extern void amba_opti_print_reset(void);
extern void amba_opti_print_log(int type, int err_id, const char *func, int line, const char *fmt, ...);
#define amba_opti_print(type, error_id, format, arg...) \
	do { \
		amba_opti_print_log(type, error_id, __func__, __LINE__, format, ##arg); \
	} while (0)

#endif	// __AMBA_OPTI_PRINT__H__
