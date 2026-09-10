/*
 * dsplog_priv.h
 *
 * History:
 *	2012/12/25 - [Rongrong Cao] created file
 *
 * Copyright (C) 2016  Ambarella, Inc.
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

#ifndef __DSP_LOG_PRIV_H__
#define  __DSP_LOG_PRIV_H__

#include <dsplog_drv.h>

typedef struct amba_dsplog_controller_s {
    int reserved;
} amba_dsplog_controller_t;

typedef struct amba_dsplog_context_s {
	void	*file;
	struct mutex	*mutex;
	amba_dsplog_controller_t *controller;
	void *dsplog_priv_data;
	u32 dump_mem_snapshot : 1;
	u32 reserved : 31;
} amba_dsplog_context_t;

extern int clean_dsplog_memory;
extern int enable_debug_msg_output;
int dsplog_init(void);
void *dsplog_register_log_cap(void *filp, u32 dsp_id);
void dsplog_reset_log_cap_state(void *filp);
int dsplog_deinit(amba_dsplog_controller_t *controller);
int dsplog_start_cap(void *filp, void **priv);
int dsplog_stop_cap(void *priv);
int dsplog_set_level(int level);
int dsplog_get_level(int *level);
int dsplog_parse(int arg);
int dsplog_read(char __user *buffer, size_t max_size, amba_dsplog_context_t *context);
int dsplog_get_memory_info(void *priv, struct dsplog_mem_info *info);


#ifdef CONFIG_PM
int dsplog_suspend(void);
int dsplog_resume(void);
#endif

#ifdef AMBA_DSP_ARCH_V6
int dsplog_reset_cap(u8 data);
#endif

void dsplog_lock(void);
void dsplog_unlock(void);

//extern function provided by DSP driver
int dsp_init_logbuf(u8 **print_buffer, u32 *buffer_size);
int dsp_deinit_logbuf(u8 *print_buffer, u32 buffer_size);

#endif // __DSP_LOG_PRIV_H__

