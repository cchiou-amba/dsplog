/*
 * dsplog_timer.c
 *
 * History:
 *	2013/09/27 - [Louis Sun] created file
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
#include <linux/rculist.h>
#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/version.h>

#include "iav_utils.h"
#include "dsplog_priv.h"
#include "linux/kern_levels.h"
#include "linux/list.h"

typedef struct idsp_printf_s {
	u32 seq_num;		/**< Sequence number */
	u32 dsp_core;		/**< 0 - DSP, 1 - MEMD */
	u32 format_addr;	/**< Address (offset) to find '%s' arg */
	u32 arg1;		/**< 1st var. arg */
	u32 arg2;		/**< 2nd var. arg */
	u32 arg3;		/**< 3rd var. arg */
	u32 arg4;		/**< 4th var. arg */
	u32 arg5;		/**< 5th var. arg */
} idsp_printf_t;

#define DSPLOG_ENTRY_SIZE       sizeof(idsp_printf_t)

//increase DSP printk buffer size to 2MB to keep more data
#if defined(AMBA_DSP_ARCH_V6)
#if defined(AMBA_SOC_N1) || defined(AMBA_SOC_N1_655) || defined(AMBA_SOC_CV8)
#define UCODE_CORE_NUM 5
#else
#define UCODE_CORE_NUM 4
#endif
#define SEQ_NUMBER_THREAD_HOLD 500
#else
#define UCODE_CORE_NUM 1
#define SEQ_NUMBER_THREAD_HOLD 1
#endif
//increase DSP printk buffer size to 2MB to keep more data
#define DSP_PRINTK_BUF_SIZE	(2 << 20)

//when buffer data is beyond DUMP_THRESHOLD, it outputs
#define DSP_PRINTK_BUF_DUMP_THRESHOLD_SIZE      (512 << 10)
#define DSP_LOG_TIMER_PERIOD        (msecs_to_jiffies(6))


//Ring buffer (Cache) is used to store the DSP log data, to cache the DSP log to a bigger buffer,
//in order to avoid data loss if user process is unable to read out the DSP log immediately
//especially when CPU is very busy.
//Because of this reason, DSP log should not use kernel thread to read dsp log buffer.
//and this program uses kernel timer to copy log data from "dsp log buffer" into "ring buffer"

typedef struct dsp_log_ringbuf_s {
	u8 *start;
	u8 *end;
	u8 *read_ptr;
	u8 *write_ptr;
	int full;
	atomic_t num_reader;
	struct completion fill_compl;
} dsp_log_ringbuf_t;

struct dsp_log_timer_cap_info {
	//DSP log buffer is usually a Fixed size ( most case it's 128KB) buffer, with fixed physical address
	//used to do log data exchange between DSP and ARM.
	int active;
	u32 dsplog_buf_size;
	u32 curr_seq_num;
	int seq_num_check_enable; //0: disable  1: enable
	int dsplog_state;		  //0: not started  1: started
	int dsplog_read_finish;	  //0: not finished 1: finished
	u32 dsp_suspended;
	u32 seq_num_wrapped; // 1: sequence number wrap around, 0: doesn't
	u32 dsp_id;
	int debug_msg_enable;
	dsp_log_ringbuf_t ringbuf;
	struct list_head node;
	u8 *dsppf_base;
	idsp_printf_t *dsppf_current;
	pid_t pid;
	void *filp;
};

struct list_head G_dsplog_read_queue;
/* User Kernel Timer to implement periodical dsp log copy,
 * The granularity for Kernel Timer for A5s is 10ms.
 */
#include <linux/timer.h>
typedef int (*TIMER_CALLBACK) (void *timer_data);
struct timer_control_s {
	struct timer_list  timer;
	unsigned long data;
	int timer_initialized;
	TIMER_CALLBACK timer_callback;
};

static struct timer_control_s G_timer_control = {
	.timer_initialized = 0,
	.timer_callback = NULL,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
static void kernel_timer_callback(unsigned long data)
#else
static void kernel_timer_callback(struct timer_list *t)
#endif
{
	TIMER_CALLBACK cb = G_timer_control.timer_callback;

	if (cb) {
		//DRV_PRINT(KERN_DEBUG "user callback\n");
		cb((void *)G_timer_control.data);
	}

	//auto restart
	mod_timer(&G_timer_control.timer, jiffies + DSP_LOG_TIMER_PERIOD);
}

static int dsp_printk_init_timer(void *p_user_callback, unsigned long data)
{
#define CPU_1 (1)
#define CPU_0 (0)
	if (G_timer_control.timer_initialized) {
		DRV_PRINT(KERN_DEBUG "timer already init, skip.\n");
	} else {
		G_timer_control.timer_callback = (TIMER_CALLBACK)p_user_callback;
		G_timer_control.timer.expires = jiffies + DSP_LOG_TIMER_PERIOD;
		G_timer_control.data = (unsigned long) data;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
		G_timer_control.timer.function = &kernel_timer_callback;
		init_timer(&G_timer_control.timer);
		add_timer(&G_timer_control.timer);
#else
	#if defined(AMBA_DSP_ARCH_V6)
		timer_setup(&G_timer_control.timer, kernel_timer_callback, TIMER_PINNED);
		/* Setup the timer to the cpu1 avoid affect the iav irq.*/
		if (cpu_possible(CPU_1) && cpu_online(CPU_1)) {
			add_timer_on(&G_timer_control.timer, CPU_1);
		} else {
			DRV_PRINT(KERN_DEBUG "add timer on cpu_0, as cpu_1 is not available.\n");
			add_timer_on(&G_timer_control.timer, CPU_0);
		}
	#else
		timer_setup(&G_timer_control.timer, kernel_timer_callback, 0);
		add_timer(&G_timer_control.timer);
	#endif
#endif
		G_timer_control.timer_initialized = 1;
	}

	return 0;
}

static int dsp_printk_deinit_timer(void)
{
	if (G_timer_control.timer_initialized) {
		del_timer_sync(&G_timer_control.timer);
		G_timer_control.timer_callback = NULL;
		G_timer_control.timer_initialized = 0;
	}
	return 0;
}

//buffer is user allocated mem buffer,  size is max read bytes.
//return value of dsp_read_log is actual output.
//actual output should be > 0 and <=  MIN ( ring buffer size,  max_size of user buffer)
int dsplog_read(char __user *buffer, size_t max_size, amba_dsplog_context_t *context)
{
	u8 *cur_write_ptr;
	u8 *cur_read_ptr;
	int readout_size = 0;
	int cur_log_size;
	int retv = 0;
	int rval = 0;
	struct dsp_log_timer_cap_info *cap_info = NULL;

	if (!context) {
		rval = -EINVAL;
		goto DSPLOG_READ_EXIT;
	}

	cap_info = (struct dsp_log_timer_cap_info *)context->dsplog_priv_data;
	if (!cap_info) {
		rval = -EINVAL;
		goto DSPLOG_READ_EXIT;
	}

	if (context->dump_mem_snapshot) {
		if (max_size != cap_info->dsplog_buf_size) {
			DRV_PRINT(KERN_DEBUG
				"dsp_core_id[%u]: read size 0x%zx != core buffer size 0x%x\n",
				cap_info->dsp_id, max_size, cap_info->dsplog_buf_size);
			rval = -EINVAL;
			goto DSPLOG_READ_EXIT;
		}

		retv = copy_to_user(buffer, cap_info->dsppf_base, max_size);
		if (retv) {
			iav_error("dsplog_read: Failed to dump DSP log memory snapshot.\n");
			rval = -EFAULT;
			goto DSPLOG_READ_EXIT;
		}

		context->dump_mem_snapshot = 0;
		rval = max_size;
		goto DSPLOG_READ_EXIT;
	}

	//check size, must be multiple of DSPLOG_ENTRY_SIZE, and size > 128KB
	if ((max_size % DSPLOG_ENTRY_SIZE) || (max_size < DSP_PRINTK_BUF_SIZE)) {
		DRV_PRINT(KERN_DEBUG"dsp_printk: please read dsp log with a buffer "
			"over %d size  and size is multiple of 32\n", DSP_PRINTK_BUF_SIZE);
		rval = -EINVAL;
		goto DSPLOG_READ_EXIT;
	}

	if (cap_info->dsplog_read_finish) {
		DRV_PRINT(KERN_DEBUG
			"dsp_core_id[%u]: dsp_printk: log has stopped and read finished, no more logs.\n",
			cap_info->dsp_id);
		rval = 0;
		goto DSPLOG_READ_EXIT;
	}

	if (cap_info->dsplog_state != 0) {
		if (!atomic_inc_and_test(&cap_info->ringbuf.num_reader)) {
			dsplog_unlock();
			wait_for_completion_interruptible(&cap_info->ringbuf.fill_compl);
			dsplog_lock();
		}
	}

	cur_write_ptr = cap_info->ringbuf.write_ptr;
	cur_read_ptr = cap_info->ringbuf.read_ptr;
	cur_log_size = cur_write_ptr - cur_read_ptr;
	if (cur_log_size < 0) {
		cur_log_size += DSP_PRINTK_BUF_SIZE;
	}

	if (cur_write_ptr < cur_read_ptr) {
		//copy 2 segments
		retv = copy_to_user(buffer, cur_read_ptr, cap_info->ringbuf.end - cur_read_ptr);
		if (retv) {
			iav_error("dsplog_read: Failed to read out DSP log.\n");
			rval = -EFAULT;
			goto DSPLOG_READ_EXIT;
		}
		readout_size += (cap_info->ringbuf.end - cur_read_ptr);
		retv = copy_to_user(buffer + readout_size, cap_info->ringbuf.start,
			cur_write_ptr - cap_info->ringbuf.start);
		if (retv) {
			iav_error("dsplog_read: Failed to read out DSP log.\n");
			rval = -EFAULT;
			goto DSPLOG_READ_EXIT;
		}
		readout_size += (cur_write_ptr - cap_info->ringbuf.start);
	} else if (cur_write_ptr > cur_read_ptr) {
		//copy 1 segment
		retv = copy_to_user(buffer, cur_read_ptr, cur_write_ptr - cur_read_ptr);
		if (retv) {
			iav_error("dsplog_read: Failed to read out DSP log.\n");
			rval = -EFAULT;
			goto DSPLOG_READ_EXIT;
		}
		readout_size += (cur_write_ptr - cur_read_ptr);
	}
	//read out all, so change the read ptr position, note that
	//amb_timer_cap_info->G_ringbuf.write_ptr may still move ahead during readout, so here, we
	//only set amb_timer_cap_info->G_ringbuf.read_ptr to saved write ptr position
	cap_info->ringbuf.read_ptr = cur_write_ptr;
	cap_info->ringbuf.full = 0;

	if (cur_log_size < DSP_PRINTK_BUF_DUMP_THRESHOLD_SIZE) {
		DRV_PRINT(KERN_DEBUG "dsp_core_id[%u]: dsp_printk: log(%d) may have stopped.\n",
			cap_info->dsp_id, cur_log_size);
	}
	if (cap_info->dsplog_state == 0) {
		cap_info->dsplog_read_finish = 1;
	}
	if (cap_info->dsplog_read_finish) {
		DRV_PRINT(KERN_DEBUG "dsp_core_id[%u]: last flush data size is %d\n", cap_info->dsp_id,
			readout_size);
	}

	rval = readout_size;

DSPLOG_READ_EXIT:
	return rval;
}

static int init_buffer(struct dsp_log_timer_cap_info *cap_info)
{
	cap_info->ringbuf.start = kzalloc(DSP_PRINTK_BUF_SIZE, GFP_KERNEL);
	if (cap_info->ringbuf.start == NULL) {
		DRV_PRINT(KERN_DEBUG"Init DSP printk buffer error, insufficient kernel mem\n");
		return -ENOMEM;
	}

	cap_info->ringbuf.read_ptr = cap_info->ringbuf.start;
	cap_info->ringbuf.write_ptr = cap_info->ringbuf.start;
	cap_info->ringbuf.end = cap_info->ringbuf.start + DSP_PRINTK_BUF_SIZE;
	init_completion(&cap_info->ringbuf.fill_compl);
	atomic_set(&cap_info->ringbuf.num_reader, 0);

	return 0;
}

static inline void dsp_log_add_to_ring_buffer(struct dsp_log_timer_cap_info *cap_info)
{
	int cur_log_size;

	//when ring buffer full, discard new record
	if (likely(!cap_info->ringbuf.full)) {
		memcpy(cap_info->ringbuf.write_ptr, cap_info->dsppf_current, DSPLOG_ENTRY_SIZE);
		//move ring buffer write pointer and handle wrap around
		cap_info->ringbuf.write_ptr += DSPLOG_ENTRY_SIZE;
		if (cap_info->ringbuf.write_ptr >= cap_info->ringbuf.end) {
			cap_info->ringbuf.write_ptr = cap_info->ringbuf.start;
		}

		//if ring buffer is full  (write ptr will NEVER catch read ptr)
		if (unlikely(((cap_info->ringbuf.write_ptr == cap_info->ringbuf.end - DSPLOG_ENTRY_SIZE) &&
						(cap_info->ringbuf.read_ptr == cap_info->ringbuf.start)) ||
				(cap_info->ringbuf.write_ptr + DSPLOG_ENTRY_SIZE == cap_info->ringbuf.read_ptr))) {
			DRV_PRINT(KERN_DEBUG "dsp_printk: SLOW start 0x%p, end0x%p, write0x%p, read 0x%p\n",
				cap_info->ringbuf.start, cap_info->ringbuf.end, cap_info->ringbuf.write_ptr,
				cap_info->ringbuf.read_ptr);
			cap_info->ringbuf.full = 1;
		}
	}

	if (atomic_read(&cap_info->ringbuf.num_reader)) {
		if (!cap_info->ringbuf.full) {
			cur_log_size = cap_info->ringbuf.write_ptr - cap_info->ringbuf.read_ptr;
			if (cur_log_size < 0) {
				cur_log_size += DSP_PRINTK_BUF_SIZE;
			}
			if (cur_log_size > DSP_PRINTK_BUF_DUMP_THRESHOLD_SIZE) {
				if (atomic_dec_and_test(&cap_info->ringbuf.num_reader)) {
					complete(&cap_info->ringbuf.fill_compl);
				}
			}
		} else {
			if (atomic_dec_and_test(&cap_info->ringbuf.num_reader)) {
				complete(&cap_info->ringbuf.fill_compl);
			}
		}
	}
}

static int dsp_printk_timer(void *arg)
{
	u32 prev_seq_num = 0, cap_info_num = 0, i = 0, rm_cap_info = 0;
	struct dsp_log_timer_cap_info *temp_cap_info = NULL, *cap_info[UCODE_CORE_NUM] = {};

	list_for_each_entry_rcu(temp_cap_info, &G_dsplog_read_queue, node) {
		if (cap_info_num >= UCODE_CORE_NUM) {
			DRV_PRINT(KERN_ERR "dsplog: cap_info array overflow, skipping extra entries\n");
			break;
		}
		cap_info[cap_info_num] = temp_cap_info;
		cap_info_num++;
	}
	while (cap_info_num) {
		/* Read every cap_info every time for balance.*/
		for (i = 0; i < cap_info_num; i++) {
			temp_cap_info = cap_info[i];
			// when dsp suspends, stop dumping dsp log
			if (!temp_cap_info->dsp_suspended) {
				if (((temp_cap_info->dsppf_current->seq_num < temp_cap_info->curr_seq_num) &&
					(temp_cap_info->curr_seq_num != -1) &&
					(temp_cap_info->curr_seq_num - temp_cap_info->dsppf_current->seq_num < 0xF0000000))) {
					if (temp_cap_info->debug_msg_enable) {
						DRV_PRINT(KERN_DEBUG "dsp_printk: current seq num %u smaller than last %u.\n",
							temp_cap_info->dsppf_current->seq_num, temp_cap_info->curr_seq_num);
					}
					rm_cap_info = 1;
					break;
				}
				if((temp_cap_info->curr_seq_num > 0) &&
					(temp_cap_info->dsppf_current->seq_num > temp_cap_info->curr_seq_num) &&
					(temp_cap_info->dsppf_current->seq_num - temp_cap_info->curr_seq_num > 0xF000000)) {
					if (temp_cap_info->debug_msg_enable) {
						DRV_PRINT(KERN_DEBUG "[%d]dsp_printk: current seq num %u smaller than last %u.\n",
							temp_cap_info->dsp_id, temp_cap_info->dsppf_current->seq_num, temp_cap_info->curr_seq_num);
					}
					temp_cap_info->curr_seq_num = temp_cap_info->dsppf_current->seq_num;
					rm_cap_info = 1;
					break;
				}
				if (temp_cap_info->curr_seq_num == -1) {
					temp_cap_info->seq_num_wrapped = 1;
				}
				prev_seq_num = temp_cap_info->curr_seq_num;
				temp_cap_info->curr_seq_num = temp_cap_info->dsppf_current->seq_num;

				if (temp_cap_info->seq_num_check_enable) {
					if ((prev_seq_num != 0) &&
						(temp_cap_info->curr_seq_num - prev_seq_num > SEQ_NUMBER_THREAD_HOLD)) {
						DRV_PRINT(KERN_DEBUG "dsp_core_id[%u]: dsp_printk: log buffer overwritten, "
							"curr_seq_num = 0x%X, prev prev_seq_num = 0x%X\n",
							temp_cap_info->dsp_id, temp_cap_info->curr_seq_num, prev_seq_num);
					}
				}

				if ((temp_cap_info->curr_seq_num == 0) && (temp_cap_info->seq_num_wrapped == 0) &&
					(temp_cap_info->dsppf_current->format_addr == 0)) {
					if (temp_cap_info->debug_msg_enable) {
						DRV_PRINT(KERN_DEBUG "dsp_printk: Zero sequence num\n");
					}
					rm_cap_info = 1;
					break;
				}
				dsp_log_add_to_ring_buffer(temp_cap_info);
				//advance read pointer and also handle wrap around
				temp_cap_info->dsppf_current++;
				if ((u8 *) temp_cap_info->dsppf_current >=
					temp_cap_info->dsppf_base + temp_cap_info->dsplog_buf_size) {
					temp_cap_info->dsppf_current = (idsp_printf_t *) temp_cap_info->dsppf_base;
				}
			} else {
				rm_cap_info = 1;
				break;
			}
		}
		/* Remove the no data cap_info. */
		if (rm_cap_info) {
			for (i = i + 1; i < cap_info_num; i++) {
				cap_info[i - 1] = cap_info[i];
			}
			cap_info_num--;
			rm_cap_info = 0;
		}
	}

	return 0;
}

static struct dsp_log_timer_cap_info *alloc_log_cap_instance(void *filp, u32 dsp_id)
{
	struct dsp_log_timer_cap_info *cap_info = NULL;
	struct task_struct *cur_thread = get_current();
	u8 *dsppf_base = NULL;
	u32 dsppf_size = 0;

	cap_info = (struct dsp_log_timer_cap_info *)kzalloc(
		sizeof(struct dsp_log_timer_cap_info), GFP_KERNEL);
	if (!cap_info) {
		DRV_PRINT(KERN_ERR "[%s:%d] kzalloc failed for amb timer cap info\n",
			__func__, __LINE__);
		goto ALLOC_LOG_CAP_INSTANCE_EXIT;
	}

	cap_info->dsp_id = dsp_id;
	cap_info->debug_msg_enable = enable_debug_msg_output;
	cap_info->pid = cur_thread->pid;
	cap_info->seq_num_check_enable = (UCODE_CORE_NUM <= 1);
	cap_info->active = 1; // mark this instance as occupied
	cap_info->filp = filp;

	dsp_init_logbuf(&dsppf_base, &dsppf_size);
	cap_info->dsplog_buf_size = dsppf_size / UCODE_CORE_NUM;
	cap_info->dsppf_base = dsppf_base + (dsppf_size / UCODE_CORE_NUM) * dsp_id;

	cap_info->dsppf_current = (idsp_printf_t *)cap_info->dsppf_base;

	if (init_buffer(cap_info) < 0) {
		dsp_deinit_logbuf(cap_info->dsppf_base, cap_info->dsplog_buf_size);
		kfree(cap_info);
		cap_info = NULL;
		goto ALLOC_LOG_CAP_INSTANCE_EXIT;
	}

	list_add_tail_rcu(&cap_info->node, &G_dsplog_read_queue);
	DRV_PRINT(KERN_DEBUG "dsplog_printk: new instance of dsp num[%d] appended\n", dsp_id);

ALLOC_LOG_CAP_INSTANCE_EXIT:
	return cap_info;
}

void *dsplog_register_log_cap(void *filp, u32 dsp_id)
{
	struct dsp_log_timer_cap_info *_cap_info = NULL, *cap_info = NULL;
	struct task_struct *cur_thread = get_current();

	if (dsp_id >= UCODE_CORE_NUM) {
		DRV_PRINT(KERN_ERR "dsplog_printk: dsp_core_id[%d] must be < total core num[%d]!!!\n",
			dsp_id, UCODE_CORE_NUM);
		goto DSPLOG_REGISTER_LOG_CAP_EXIT;
	}

	dsp_printk_deinit_timer();
	list_for_each_entry(_cap_info, &G_dsplog_read_queue, node) {
		if (_cap_info->dsp_id == dsp_id) {
			if (_cap_info->active) {
				DRV_PRINT(KERN_ERR "dsplog_printk: dsp_core_id[%u] currently occupied "
					"by thread[%d]\n", dsp_id, _cap_info->pid);
				goto DSPLOG_REGISTER_LOG_CAP_EXIT;
			} else {
				cap_info = _cap_info;
				DRV_PRINT(KERN_DEBUG "dsplog_printk: dsp_core_id[%u] instance found\n"
					"change owner from pid [%d] to [%d]\n", dsp_id, cap_info->pid, cur_thread->pid);
				cap_info->pid = cur_thread->pid;
				cap_info->active = 1;
				cap_info->filp = filp;
				break;
			}
		}
	}

	if (cap_info == NULL) {
		cap_info = alloc_log_cap_instance(filp, dsp_id);
	}

DSPLOG_REGISTER_LOG_CAP_EXIT:
	return cap_info;
}

static void dsplog_unregister_log_cap(struct dsp_log_timer_cap_info *cap_info)
{
	DRV_PRINT(KERN_DEBUG "dsp_core_id[%u]: dsplog: list deinit ring buffer.\n", cap_info->dsp_id);

	if (cap_info->ringbuf.start) {
		kfree(cap_info->ringbuf.start);
		cap_info->ringbuf.start = NULL;
	}
	dsp_deinit_logbuf(cap_info->dsppf_base, cap_info->dsplog_buf_size);
	list_del(&cap_info->node);
	kfree(cap_info);

	return;
}

void dsplog_reset_log_cap_state(void *filp)
{
	struct dsp_log_timer_cap_info *cap_info = NULL;

	list_for_each_entry(cap_info, &G_dsplog_read_queue, node) {
		if ((cap_info->filp == filp) && cap_info->active) {
			cap_info->filp = NULL;
			cap_info->active = 0;
		}
	}

	return;
}

//platform independant DSP printk init
int dsplog_init(void)
{
	u8 *base = NULL;
	u32 size = 0;

	dsp_init_logbuf(&base, &size);
	if (clean_dsplog_memory) {
		memset(base, 0, size);
	}
	//call DSP driver's function to init the dsplog
	INIT_LIST_HEAD(&G_dsplog_read_queue);
	return 0;
}

int dsplog_deinit(amba_dsplog_controller_t *controller)
{
	struct dsp_log_timer_cap_info *cap_info = NULL, *next = NULL;

	DRV_PRINT(KERN_DEBUG "dsplog: deinit timer.\n");
	dsp_printk_deinit_timer();

	list_for_each_entry_safe(cap_info, next, &G_dsplog_read_queue, node) {
		dsplog_unregister_log_cap(cap_info);
	}
	return 0;
}

int dsplog_start_cap(void *filp, void **priv)
{
	struct dsp_log_timer_cap_info *cap_info = NULL;
	int i = 0, rval = 0;

	if (*priv == NULL) {
		// FIXME:
		/*this is the compatible case with old dsplog version,
		 since old chip only have one idsp core, register reg dsp core 0*/
		if (UCODE_CORE_NUM == 1) {
			*priv = dsplog_register_log_cap(filp, 0);
		} else {
			rval = -EFAULT;
			goto DSPLOG_START_CAP_EXIT;
		}
	}

	if (*priv == NULL) {
		rval = -EFAULT;
		goto DSPLOG_START_CAP_EXIT;
	}

	cap_info = *priv;
	cap_info->ringbuf.read_ptr = cap_info->ringbuf.write_ptr;
	cap_info->ringbuf.full = 0;
	atomic_set(&cap_info->ringbuf.num_reader, 0);
	cap_info->curr_seq_num = 0;
	cap_info->dsplog_state = 1;		  //started
	cap_info->dsplog_read_finish = 0; //clear finish state

	list_for_each_entry(cap_info, &G_dsplog_read_queue, node) {
		i++;
	}
	/*Start timer when all dsp core becomes registered*/
	if (i == UCODE_CORE_NUM) {
		dsp_printk_init_timer(dsp_printk_timer, 0);   //make the timer to issue for each HZ
	}

DSPLOG_START_CAP_EXIT:
	return rval;
}

int dsplog_stop_cap(void *priv)
{

	struct dsp_log_timer_cap_info *cap_info = NULL;

	if (priv == NULL) {
		return -1;
	}
	cap_info = priv;
	dsp_printk_deinit_timer();
	if (cap_info) {
		DRV_PRINT(KERN_DEBUG "dsplog_stop_cap flushes ring buf\n");
		atomic_set(&cap_info->ringbuf.num_reader, 0);
		cap_info->dsplog_state = 0;
		complete(&cap_info->ringbuf.fill_compl);
		cap_info->active = 0;
		cap_info->filp = NULL;
	}

	return 0;
}

int dsplog_set_level(int level)
{
	return 0;
}

int dsplog_get_level(int *level)
{
	return 0;
}

int dsplog_parse(int arg)
{
	return 0;
}

int dsplog_get_memory_info(void *priv, struct dsplog_mem_info *info)
{
	int rval = 0;
	struct dsp_log_timer_cap_info *cap_info = (struct dsp_log_timer_cap_info *)priv;

	if (!info || !cap_info) {
		rval = -EINVAL;
		goto DSPLOG_GET_MEMORY_INFO_EXIT;
	}

	info->dsp_id = cap_info->dsp_id;
	info->size = cap_info->dsplog_buf_size;

	if (enable_debug_msg_output) {
		DRV_PRINT(KERN_DEBUG
			"dsplog mem: dsp_id=%u size=0x%x\n", info->dsp_id, info->size);
	}

DSPLOG_GET_MEMORY_INFO_EXIT:
	return rval;
}

#ifdef CONFIG_PM
int dsplog_suspend(void)
{

	struct dsp_log_timer_cap_info *cap_info = NULL;

	list_for_each_entry(cap_info, &G_dsplog_read_queue, node) {
		cap_info->dsp_suspended = 1;
		dsp_init_logbuf(&cap_info->dsppf_base, &cap_info->dsplog_buf_size);

		// clear dsp log buffer
		cap_info->dsppf_current = (idsp_printf_t *) cap_info->dsppf_base;
		cap_info->curr_seq_num = 0;
		cap_info->seq_num_wrapped = 0;
		if (cap_info->dsppf_base) {
			memset(cap_info->dsppf_base, 0, cap_info->dsplog_buf_size);
		}
	}

	return 0;
}

int dsplog_resume(void)
{
	struct dsp_log_timer_cap_info *cap_info = NULL;

	list_for_each_entry(cap_info, &G_dsplog_read_queue, node) {
		dsp_init_logbuf(&cap_info->dsppf_base, &cap_info->dsplog_buf_size);

		cap_info->dsp_suspended = 0;
	}
	return 0;
}
#endif

#ifdef AMBA_DSP_ARCH_V6
/* Called when recovering from DSP hang, DSP will write DSP log from buffer start. */
int dsplog_reset_cap(u8 data)
{
	struct dsp_log_timer_cap_info *cap_info = NULL;

	(void)data;
	dsplog_lock();

	/* stop timer */
	if (G_timer_control.timer_initialized) {
		del_timer_sync(&G_timer_control.timer);
	}

	/* reset cap_info */
	list_for_each_entry(cap_info, &G_dsplog_read_queue, node) {
		cap_info->dsppf_current = (idsp_printf_t *)cap_info->dsppf_base;
		cap_info->curr_seq_num = 0;
		cap_info->seq_num_wrapped = 0;
		if (cap_info->dsppf_current) {
			memset(cap_info->dsppf_current, 0, cap_info->dsplog_buf_size);
		}
	}

	/* start timer */
	if (G_timer_control.timer_initialized) {
		mod_timer(&G_timer_control.timer, jiffies + DSP_LOG_TIMER_PERIOD);
	}

	dsplog_unlock();

	return 0;
}
#endif
