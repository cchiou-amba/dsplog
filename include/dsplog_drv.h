/*
 * dsplog_drv.h
 *
 * History:
 *	2013/09/30 - [Louis Sun] created file
 *
 * Copyright (c) 2026 Ambarella International LP
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
 */
#ifndef __DSP_LOG_DRV_H__
#define __DSP_LOG_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif


#define DSPLOG_IOC_MAGIC			'p'


enum DSPLOG_IOC_ENUM {
	IOC_DSPLOG_START_CAPUTRE = 0,
	IOC_DSPLOG_STOP_CAPTURE    =  1,
	IOC_DSPLOG_SET_LOG_LEVEL   =  2,
	IOC_DSPLOG_GET_LOG_LEVEL   = 3,
	IOC_DSPLOG_PARSE_LOG   = 4,
	IOC_DSPLOG_SET_DSP   = 5,
	IOC_DSPLOG_GET_MEMORY_INFO = 6,
	IOC_DSPLOG_DUMP_MEMORY_SNAPSHOT = 7,
};

struct dsplog_mem_info {
	unsigned int dsp_id;	/*!< current DSP core id */
	unsigned int size;	/*!< size of this core's log region */
};

//IOCTLs  for driver DSPLOG
#define AMBA_IOC_DSPLOG_START_CAPUTRE	_IO(DSPLOG_IOC_MAGIC, IOC_DSPLOG_START_CAPUTRE)
#define AMBA_IOC_DSPLOG_STOP_CAPTURE	_IO(DSPLOG_IOC_MAGIC, IOC_DSPLOG_STOP_CAPTURE)
#define AMBA_IOC_DSPLOG_SET_LOG_LEVEL	_IOW(DSPLOG_IOC_MAGIC, IOC_DSPLOG_SET_LOG_LEVEL, int)
#define AMBA_IOC_DSPLOG_GET_LOG_LEVEL	_IOR(DSPLOG_IOC_MAGIC, IOC_DSPLOG_GET_LOG_LEVEL, int *)
#define AMBA_IOC_DSPLOG_PARSE_LOG	_IOW(DSPLOG_IOC_MAGIC, IOC_DSPLOG_PARSE_LOG, int )
#define AMBA_IOC_DSPLOG_SET_DSP_CORE	_IOW(DSPLOG_IOC_MAGIC, IOC_DSPLOG_SET_DSP, int *)
#define AMBA_IOC_DSPLOG_GET_MEMORY_INFO	_IOR(DSPLOG_IOC_MAGIC, IOC_DSPLOG_GET_MEMORY_INFO, struct dsplog_mem_info *)
#define AMBA_IOC_DSPLOG_DUMP_MEMORY_SNAPSHOT	_IOW(DSPLOG_IOC_MAGIC, IOC_DSPLOG_DUMP_MEMORY_SNAPSHOT, int)

//int read(int fd,  unsigned char * buffer,   unsigned int  max_size);

#ifdef __cplusplus
}
#endif

#endif   //__DSP_LOG_DRV_H__
