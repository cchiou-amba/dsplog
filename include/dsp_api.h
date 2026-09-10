/*
 * dsp_api.h
 *
 * History:
 *	2015/07/12 - [Jian Tang] created file
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


#ifndef _DSP_API_H
#define _DSP_API_H

#include <linux/list.h>
#include <dsp_cmd_msg.h>

#define USE_INSTANT_CMD	(0)

#define DSP_VOUT_NUM	(2)
#define DSP_VOUT_NUM_MAX	(3)

typedef unsigned int	dev_t;

enum {
	DSP_CMD_FLAG_NORMAL = 0,
	DSP_CMD_FLAG_HIGH_PRIOR = 1,
	DSP_CMD_FLAG_BLOCK = 2,
	DSP_CMD_FLAG_INSTANT = 3,
	DSP_CMD_FLAG_BATCH = 4,

	DSP_CMD_FLAG_NUM,
	DSP_CMD_FLAG_FIRST = 0,
	DSP_CMD_FLAG_LAST = DSP_CMD_FLAG_NUM,
};

enum {
	DSP_CMD_TYPE_NORMAL = 0,
	DSP_CMD_TYPE_VIN = 1,
	DSP_CMD_TYPE_VOUT = 2,
	DSP_CMD_TYPE_PREV = 3,
	DSP_CMD_TYPE_ENC = 4,
	DSP_CMD_TYPE_IMG = 5,
	DSP_CMD_TYPE_DEC = 6,
	DSP_CMD_TYPE_VDSP_PWI = 7,

	DSP_CMD_TYPE_NUM,
	DSP_CMD_TYPE_FIRST = 0,
	DSP_CMD_TYPE_LAST = DSP_CMD_TYPE_NUM,
};

enum {
	DSP_CLK_TYPE_IDSP = 0,
	DSP_CLK_TYPE_HEVC = 1,

	DSP_CLK_TYPE_NUM,
	DSP_CLK_TYPE_FIRST = 0,
	DSP_CLK_TYPE_LAST = DSP_CLK_TYPE_NUM,
};

enum dsp_audit_type {
	DSP_AUDIT_TYPE_ISR_DURATION = 0,
	DSP_AUDIT_TYPE_ISR_INTERVAL = 1,
	DSP_AUDIT_TYPE_ISR_DSP_HEALTH = 2,
};

enum {
	DSP_RESET_TYPE_NONE = 0,
	DSP_RESET_TYPE_SAFETY_PROFILE = 1,
	DSP_RESET_TYPE_RESET_ORC = 2,
	DSP_RESET_TYPE_IN_HALT = 3,
	DSP_RESET_TYPE_NUM,
};

struct amb_dsp_cmd {
	DSP_CMD dsp_cmd;
	struct list_head head;
	struct list_head node;
	u32 cmd_type;
	u32 flag;
	u32 keep_latest :1;
	u32 reserved :31;
};

struct dsp_vout_profile_params {
	u16 support_rotate : 1;
	u16 osd_depth : 2;
	u16 back_pressure_margin : 2;
	u16 reserved0 : 11;
	u16 reserved1;

	u16 max_width_video;
	u16 max_width_osd;
};

struct dsp_boot_params {
	u8 vout_profile;
	u8 osd_profile[DSP_VOUT_NUM_MAX];
	u8 vout_profile_flag : 1;
	u8 vout_osd_profile_flag : 1;
	u8 dsp_private_dram_flag : 1;
	u8 dsp_max_fb_num_flag : 1;
	u8 vout_underflow_prevention_flag : 1;			/*!< Flag to config underflow prevention */
	u8 support_vout_rotate_flag : 1;			/*!< Flag to config whether support VOUT rotate */
	u8 vout_back_pressure_margin_flag : 1;			/*!< Flag to config VOUT back pressure margin */
	u8 max_dram_par_num_flag : 1;
	u8 dsp_max_fb_num;
	u8 vout_underflow_prevention : 1;			/*!< Flag to enable underflow prevention */
	u8 vout0_support_rotate : 1;				/*!< Flag to support VOUT0 rotate */
	u8 vout1_support_rotate : 1;				/*!< Flag to support VOUT1 rotate */
	u8 vout0_back_pressure_margin : 2;				/*!< Specify IDSP VOUT0 back pressure margin */
	u8 vout1_back_pressure_margin : 2;				/*!< Specify IDSP VOUT1 back pressure margin */
	u8 sync_first_vout_frame_mode_flag : 1;		/*!< Flag to config sync first vout frame mode */
	u8 max_dram_par_num;
	u32 dsp_private_dram_addr;
	u32 dsp_private_dram_size;
	u32 sync_first_vout_frame_mode : 3;		/*!< Specify sync first vout frame mode */
	u32 reserved0 : 29;
	struct dsp_vout_profile_params vout_profile_cfg[DSP_VOUT_NUM_MAX];	/*!< Specify vout_profile uese by VOUT_PROFILE_SETUP_CMD */
};

struct dsp_device *ambarella_request_dsp(void);

void dsp_issue_cmd(void *cmd, u32 size, u8 is_vdsp_pwi);
void dsp_issue_delay_cmd(void *cmd, u32 size, u32 delay);
void dsp_issue_img_cmd(void *cmd, u32 size);
void dsp_cmd_print(void *dsp_cmd);
u8 dsp_is_ucode_loaded(void);

struct dsp_device {
	char name[32];
	struct device *dev;
	dev_t dev_id;

	unsigned long virtual_vin_map;
	u32 bsb_start;
	u32 buffer_start;
	u32 buffer_size;
	u32 dump_cmd_num : 8;
	u32 dump_cmd_flag : 1;
	u32 reserved1 : 23;

	u32 do_dump_cmd;
	u32 dump_cmd_pid;
	void *dump_cmd_addr;
	u32 dump_cmd_amount : 8;
	u32 dump_cmd_capacity : 8;
	u32 dump_cmd_overflow : 1;
	u32 dump_cmd_reserved : 15;
	u32 chan_map;

	void (*msg_callback[NUM_MSG_CAT])(void *data, DSP_MSG *msg);
	void *msg_data[NUM_MSG_CAT];
	void (*enc_callback)(void *data, DSP_MSG *msg);
	void *enc_data;
	void (*vcap_callback)(void *data, DSP_MSG *msg);
	void *vcap_data;

	void (*irq_sync_callback)(void *data);
	void *vdsp_data;
	void *vin_data;
	void *vin_pip_data; /* It is for N1 only */
	void (*vpostp_callback)(void *data);
	void *vpostp_data;

	void (*iav_sysfs_monitor_update_error_msg)(void *error_msg);

	int (*set_op_mode)(struct dsp_device *dsp_dev, u32 op_mode,
		struct amb_dsp_cmd *cmd, u32 no_wait, u32 *dsp_params);
	int (*set_enc_sub_mode)(struct dsp_device *dsp_dev, u32 enc_mode,
		struct amb_dsp_cmd *cmd, u32 no_wait, u8 force);
	int (*set_chan_enc_sub_mode)(struct dsp_device *dsp_dev, u8 chan_id,
		u32 enc_mode, struct amb_dsp_cmd *first, u32 no_wait);
	struct amb_dsp_cmd *(*get_cmd)(struct dsp_device *dsp_dev, u32 flag);
	struct amb_dsp_cmd *(*get_multi_cmds)(struct dsp_device *dsp_dev,
		int num, u32 flag);
	void (*put_cmd)(struct dsp_device *dsp_dev, struct amb_dsp_cmd *cmd, u32 delay);
	void (*print_cmd)(void *cmd);
	void (*print_cmd_to_buf)(void *cmd, void *parsed_result);
	void (*release_cmd)(struct dsp_device *dsp_dev, struct amb_dsp_cmd *cmd);
	void (*set_vin_port)(struct dsp_device *dsp_dev, u8 enable);
	int (*get_chip_id)(struct dsp_device *dsp_dev, u32 *dsp_chip_id, u32 *chip);
	int (*set_debug_chip_id)(struct dsp_device *dsp_dev, u32 dsp_chip_id);
	int (*get_chip_wafer_id)(struct dsp_device *dsp_dev, u64 *wafer_id);
	int (*wait_vcap)(struct dsp_device *dsp_dev, u32 count);
	dsp_init_data_t *(*get_dsp_init_data)(struct dsp_device *dsp_dev);
	int (*set_audit)(struct dsp_device *dsp_dev, u32 cmd, u32 type, unsigned long audit_addr);
	int (*get_audit)(struct dsp_device *dsp_dev, u32 cmd, u32 type, unsigned long audit_addr);
	int (*suspend)(struct dsp_device *dsp_dev);
	int (*resume)(struct dsp_device *dsp_dev);
	int (*set_clock_state)(u32 clk_type, u32 enable);
	int (*reset)(struct dsp_device *dsp_dev, u8 reset_type);
	int (*query_unique_id)(struct dsp_device *dsp_dev, u8 *unique_id);
	int (*set_skip_irq_cnt)(struct dsp_device *dsp_dev, u8 port, u32 cnt);
	int (*wait_idle)(struct dsp_device *dsp_dev, u32 op_mode);
	int (*enable_dsp_recover)(struct dsp_device *dsp_dev, u32 enable);
	int (*is_dsp_asserted)(struct dsp_device *dsp_dev);
	void (*dsp_health_callback)(void *data, u32 health_status);
	void *dsp_health_data;
	u64 (*get_async_cmd_counter)(struct dsp_device *dsp_dev);
	unsigned long (*get_ucode_default_bin_offset)(struct dsp_device *dsp_dev);
};

#endif	// _DSP_API_H

