/*
 * amba_imgproc.h
 *
 * History:
 *    2021/05/06 - [Zhaoyang Chen] Create
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

#ifndef __AMBA_IMGPROC_H__
#define __AMBA_IMGPROC_H__

#include "ambas_imgproc_arch.h"

#if defined(AMBA_SOC_N1)
#define MAX_VINC_NUM			(32)
#elif defined(AMBA_SOC_N1_655)
#define MAX_VINC_NUM			(30)
#else
#define MAX_VINC_NUM			(26)
#endif
#define MAX_CHAN_NUM			(CONFIG_AMBARELLA_MAX_CHANNEL_NUM)
#define MAX_AF_STAT_NUM			(24 * 16)
#define MAX_PTS_BUF_NUM			(16)

enum {
	STATIS_RGB = 0,
	STATIS_CFA,
	STATIS_VIN_MAIN,
	STATIS_VIN_HDR,
	STATIS_VIN_HDR_2,
	STATIS_ITEM_NUM,
	STATIS_ITEM_FIRST = STATIS_RGB,
	STATIS_ITEM_LAST = STATIS_ITEM_NUM,
};

struct imgproc_vin_pts_info {
	u64 ik_id;
	u64 sof_pts[MAX_EXPOSURE_NUM];
	u64 eof_pts[MAX_EXPOSURE_NUM];
	u64 iav_issue_pts;
};

struct iav_imgproc_vin_info {
	u8 *vcap_state;
	u32 chan_map;
	u8 vsrc_num;
	u8 mipi_vc_hdr : 1;
	u8 hdr_expo_num : 3;
	u8 reserved0 : 4;
	u8 reserved1[2];
	u32 full_batch_chan_mask;  //chan mask if batch cmd q full in vin

	struct imgproc_vin_pts_info vin_pts[MAX_PTS_BUF_NUM];
	u32 sof_wr_index;
	u32 eof_wr_index;
};

struct iav_imgproc_info {
	u32 *iav_state;
	u64 img_rsv_size;
	u32 chan_num : 8;
	u32 reserved : 24;
	u32 reserved1;
	u8 instant_update[MAX_VINC_NUM];
	unsigned long img_config_offset;
	u32 *dec_active_chan;
	struct iav_imgproc_vin_info vin_info[MAX_VINC_NUM];
	struct semaphore *dec_sem;
	u32 chan_idsp_fps_hp[MAX_CHAN_NUM];
};

struct imgproc_chan_af_statistic {
	u8 af_data_valid;
	u8 slice_num;
	u8 reserved[2];
	u32 hw_pts;
	u64 mono_pts;

	struct af_stat af_stat[MAX_SLICE_FOR_STAT][MAX_AF_STAT_NUM];
};

struct imgproc_af_statistic {
	u32 channel_map : 8;
	u32 is_2nd_stats : 1;	/* Use the second set of statistics, only valid for AISP. */
	u32 reserved : 23;

	struct imgproc_chan_af_statistic af_data[CONFIG_AMBARELLA_MAX_CHANNEL_NUM];
};

struct imgproc_chan_reinit_info {
	u32 vcap_mode_flags : 8;
	u32 reserved0 : 4;
	u32 vsrc_region_num : 4;
	u32 vsrc_region_map : 14;
	u32 ca_warp_enable : 1;
	u32 is_dec : 1;
	u32 vfov_id : 8;
	u32 img_stats_src_chan : 8;
	u32 eis_delay_count : 8;
	u32 extra_cached_batch_cmd_cnt : 3;
	u32 reserved1 : 5;
	u32 statis_sync_enable : 1;
	u32 statis_sync_delay_ms : 8;
	u32 report_pts_info_enable : 1;
	u32 aisp_second_stats_enable : 1;
	u32 aisp_img_stats_low_delay_enable : 1;
	u32 reserved2 : 20;
	u32 hrtimer_expire_us;
};

struct imgproc_reinit_info {
	u32 chan_num : 8;
	u32 encode_mode : 8;
	u32 hwtimer_enabled : 1;
	u32 img_statis_zero_copy_enable : 1;
	u32 reserved : 14;
	u32 chan_map;
	u64 img_rsv_size;

	struct dsp_device *dsp;
	struct imgproc_chan_reinit_info chan_info[MAX_CHAN_NUM];
	unsigned long statis_fifo_phy_base_addr;
};

typedef enum {
	IMG_CUSTOM_OPS_LED = 0,
	IMG_CUSTOM_OPS_EXTRA_RAW = 1,
	IMG_CUSTOM_OPS_NUM = 2,
} IMG_CUSTOM_OPS_TYPE;

struct amba_imgproc_statis_ops {
	int (*ctrl)(void *data);
	void *data;
};

struct amb_dsp_cmd;

int amba_imgproc_msg(msg_vproc_aaa_status_t *msg);
int amba_imgproc_debug(iproc_debug_info_t *iproc_debug_info);
int amba_imgproc_reinit(struct imgproc_reinit_info *reinit_info);
int amba_imgproc_cmd(struct iav_imgproc_info *info, unsigned int cmd, unsigned long arg);
int amba_imgproc_apply_idsp_cfg(struct iav_imgproc_info *info, u32 channel_map);
int amba_imgproc_wait_idsp_cfg_idle(struct iav_imgproc_info *info, u32 channel_map);
u8 amba_imgproc_fill_vin_batch_cmd_set(struct iav_imgproc_info *context, u8 fov_num, u8 vin_id, u8 chan_id, u8 is_empty);
int amba_imgproc_fill_idsp_cfg_dsp_cmd(struct iav_imgproc_info *context, u8 vin_id, u8 chan_id, idsp_config_t *idsp_cfg);
int amba_imgproc_prepare_hdr_dsp_cmd(struct iav_imgproc_info *context, u8 vinc_id, struct amb_dsp_cmd *cmd, u8 is_mipi_vc_src);
void amba_imgproc_prepare_vin_batch_cmd(struct iav_imgproc_info *context, u8 vin_id, u8 fov_num, void *cmd);
	/* Append a DSP cmd into the current chan batch. */
int amba_imgproc_append_chan_batch_cmd(struct iav_imgproc_info *context, u8 vin_id,
	u8 chan_id, const void *dsp_cmd, u32 cmd_size);
int amba_imgproc_commit_efr_vin_batch(struct iav_imgproc_info *context, u8 vin_id,
	u8 fov_num, u32 *batch_cmd_set_phys, u8 *batch_cmd_num);
int amba_imgproc_issue_idsp_cfg(struct iav_imgproc_info *info, u32 vinc_map);
u32 amba_imgproc_is_idsp_cfg_applying(u8 vinc_id);
void amba_imgproc_clear_idsp_cfg_apply_flag(u8 vinc_id);
int amba_imgproc_set_idsp_cfg(struct iav_imgproc_info *context, idsp_config_t *idsp_config);
void amba_imgproc_clear_idsp_cfg(struct iav_imgproc_info *context, u32 chan_id);
int amba_imgproc_set_idsp_delay_info(struct iav_imgproc_info *context, idsp_delay_info_t *idsp_delay_info);
idsp_config_t *amba_imgproc_get_idsp_cfg(struct iav_imgproc_info *context, u32 chan_id);
int amba_imgproc_get_chan_batch_offset(struct iav_imgproc_info *context, u8 chan_id, u32 delay, u32 *offset, u8 total_cmd_num);
int amba_imgproc_inc_chan_batch_cmd_num(u8 chan_id, u8 num, u32 delay);
u32 amba_imgproc_is_vin_batch_applying(struct iav_imgproc_info *context, u8 vinc_id);
int amba_imgproc_update_ik_cfg_id(msg_iproc_status_t *msg, msg_vproc_aaa_status_t *aaa_msg);
int amba_imgproc_update_of_nn_id(u32 chan_id, u32 of_nn_id);
int amba_imgproc_update_batch_id(msg_iproc_status_t *msg);
void amba_imgproc_update_vin_debug_info(struct iav_imgproc_info *context, u32 vin_id, u32 vin_debug_addr, u32 cap_seq_no);

int amba_imgproc_get_latest_af_statistics(struct imgproc_af_statistic *af_statistic);
int amba_imgproc_set_stats_report_rate(struct iav_imgproc_info *context, struct img_statistics_report_rate *report_rate);
void amba_imgproc_wakeup_statis_wq(void);
void amba_imgproc_wakeup_statis_wq_clear(void);
int amba_imgproc_get_stats_report_rate(struct iav_imgproc_info *context, struct img_statistics_report_rate *report_rate);
void amba_imgproc_trigger_latest_statistics(struct iav_imgproc_info *context, u8 vin_id);

int img_register_custom_ops(u8 ops_type, void *ops, void *private_data);
void amba_imgproc_register_statis_ops(struct amba_imgproc_statis_ops *statis_ops);
void amba_imgproc_unregister_statis_ops(void);
struct img_idsp_debug_setup *amba_imgproc_get_idsp_debug_setup(struct iav_imgproc_info *context, u32 chan_id);
int amb_imgproc_issue_vin_switch_idsp_cmds(struct iav_imgproc_info *context, u8 chan_id, u8 vin_id);
void img_register_rw_sem_ops(void *lock, void *unlock, void *private_data);
int img_register_mono_pts_ops(void *ops, void *private_data);

int amba_imgproc_record_idsp_cfg(struct iav_imgproc_info *context, idsp_config_t *idsp_config);
#ifdef CONFIG_PM
int amba_imgproc_suspend(struct iav_imgproc_info *info);
int amba_imgproc_resume(struct iav_imgproc_info *info);
#endif

#endif	// __AMBA_IMGPROC_H__

