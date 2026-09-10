/*
 * vin_api.h
 *
 * History:
 *    2008/01/18 - [Anthony Ginger] Create
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


#ifndef __VIN_PRI_H
#define __VIN_PRI_H

#include <linux/list.h>
#include <linux/semaphore.h>
#include <iav_ioctl.h>
#include <vin_sensors.h>

typedef enum {
	AMBA_VIN_INPUT_FORMAT_RGB_RAW = 0,
	AMBA_VIN_INPUT_FORMAT_YUV_422_INTLC = 1,
	AMBA_VIN_INPUT_FORMAT_YUV_422_PROG = 2,
	AMBA_VIN_INPUT_FORMAT_DRAM_INTLC = 3,
	AMBA_VIN_INPUT_FORMAT_DRAM_PROG = 4,
	AMBA_VIN_INPUT_FORMAT_YUV_420_PROG = 5,
} AMBA_VIN_INPUT_FORMAT;

typedef enum {
	AMBA_VIN_BRG_SEN_USE_I2C = 0,
	AMBA_VIN_BRG_SEN_USE_SPI = 1,
} AMBA_VIN_BRG_SEN_CTL;

typedef enum {
	AMBA_VIN_BRG_CHAN_SINGLE = 0,
	AMBA_VIN_BRG_CHAN_ALL = 1,
} AMBA_VIN_BRG_CHAN_CTL;

typedef enum {
	AMBA_VIN_INPUT_FRAME_TYPE_T1 = 0,
	AMBA_VIN_INPUT_FRAME_TYPE_T2 = 1,
} AMBA_VIN_INPUT_FRAME_TYPE;

#define VIN_BRG_I2C_M_IGNORE_NAK (1U << 31)
#define VIN_BRG_PWRST_IO_NUM	(4)
#define VIN_RESET_MIPI_CLK (1U << 31)

struct vin_device_config {
	u8 interface_type;
	u8 video_format;
	u8 bit_resolution;
	u8 sensor_id;

	u8 bayer_pattern;
	u8 input_format;
	u8 readout_mode;
	u8 yuv_pixel_order;

	u8 sync_mode;
	u8 input_mode;
	u8 line_reorder;
	u8 reserved0;
	u32 delayed_vs_dly;
	union {
		/* for serial lvds */
		struct {
			u32 lane_number : 16;
			u32 sync_code_style : 16;
			u32 lane_mux_0 : 16;
			u32 lane_mux_1 : 16;
			u32 lane_mux_2 : 16;
			u32 lane_mux_3 : 16;
		} slvds_cfg;
		/* for parallel lvds */
		struct {
			u32 lane_number : 16;
			u32 sync_code_style : 16;
			u32 data_edge : 1;
			u32 data_rate : 1;
			u32 emb_sync_loc : 2;
			u32 a8_mode : 1;
			u32 hw_specific : 2;
			u32 reserved1 : 25;
			u32 reserved2;
		} plvds_cfg;
		/* for parallel lvcmos */
		struct {
			u32 paralle_sync_type : 8;
			u32 sync_code_style : 8;
			u32 vs_hs_polarity : 8;
			u32 data_edge : 8;
			u32 reserved1;
			u32 reserved2;
		} plvcmos_cfg;
		/* for mipi */
		struct {
			u32 lane_number : 8;
			u32 bit_rate : 8;
			u32 data_type : 8;
			u32 clk_mode : 8;
			u32 lane_mux_0 : 16;
			u32 lane_mux_1 : 16;
			u32 vc_pattern : 8;
			u32 phy_type : 1;
			u32 ed_dt_pat : 8;
			u32 ed_dt_mask : 8;
			u32 vc_2nd_bits : 5;
			u32 reserved : 2;
		} mipi_cfg;
		/* for slvs-ec */
		struct {
			u32 lane_number : 8;
			u32 compression : 8;
			u32 act_width : 16;
			u32 dual_link : 1;
			u32 ecc_mode : 2;
			u32 baud_rate : 2;
			u32 reserved1 : 27;
			u32 reserved2;
		} slvsec_cfg;
	};
	/* for hdr sensor */
	struct {
		struct {
			u32 x : 16;
			u32 y : 16;
			u32 width : 16;
			u32 height : 16;
		} act_win;

		u32 split_width : 16;
		union {
			u32 num_splits : 16;
			u32 expo_num_m1 : 16;
		};
		u32 expo_offset_2nd : 16;
		u32 expo_offset_3rd : 16;
		u32 expo_offset_4th : 16;
		u32 reserved : 16;
	} hdr_cfg;
	/* for capture window */
	struct {
		u32 x : 16;
		u32 y : 16;
		u32 width : 16;
		u32 height : 16;
	} cap_win;
	/* for aux/meta data window */
	struct {
		u32 width : 16;
		u32 height : 16;
	} aux_win;
};

#define SHT_AGC_ARRAY_NUM	8
struct vin_wdr_cfg_array_s {
	struct vindev_wdr_gp_s shutter_gps[SHT_AGC_ARRAY_NUM];
	struct vindev_wdr_gp_s again_gps[SHT_AGC_ARRAY_NUM];
	struct vindev_wdr_gp_s dgain_gps[SHT_AGC_ARRAY_NUM];
	struct vindev_wdr_gp_s rhs_gps[SHT_AGC_ARRAY_NUM];
	u32 update_flag[SHT_AGC_ARRAY_NUM];
	u32 cur_idx;
};
struct vin_linear_cfg_array_s {
	u32 shutters[VINDEV_REG_CONTEXT_NUM][SHT_AGC_ARRAY_NUM];
	u32 agains[VINDEV_REG_CONTEXT_NUM][SHT_AGC_ARRAY_NUM];
	u32 update_flag[VINDEV_REG_CONTEXT_NUM][SHT_AGC_ARRAY_NUM];
	u32 cur_idx;
};

struct vin_apply_sht_agc {
	struct task_struct *kthread;
	atomic_t is_sht_agc_applying;
	wait_queue_head_t sht_agc_wq;
};

struct vin_sht_agc_gp_info {
	// wdr cfg work queue params
	struct {
		struct vindev_wdr_gp_s shutter_gp;
		struct vindev_wdr_gp_s again_gp;
		struct vindev_wdr_gp_s dgain_gp;
		struct vindev_wdr_gp_s rhs_gp;
		u32 update_flag;
		struct vin_wdr_cfg_array_s wdr_cfg_array;
	};
	// linear mode cfg work queue params
	struct {
		u32 shutter[VINDEV_REG_CONTEXT_NUM];
		u32 again[VINDEV_REG_CONTEXT_NUM];
		u32 linear_update_flag[VINDEV_REG_CONTEXT_NUM];
		struct vin_linear_cfg_array_s linear_cfg_array;
	};

	u32 wait_sof;
	struct task_struct *kthread;
	u32 wake_flag;
	struct vin_apply_sht_agc apply_sht_agc_ctx;
};

#define MAX_SHT_AGC_QUEUE_NUM	9
struct vindev_wdr_cfg_queue_s {
	struct vindev_wdr_gp_s queue[MAX_SHT_AGC_QUEUE_NUM];
	u32 delay[MAX_SHT_AGC_QUEUE_NUM];
	u16 head;
	u16 tail;
};

struct vindev_wdr_sht_agc_queue_s {
	struct vindev_wdr_cfg_queue_s shutter_gps;
	struct vindev_wdr_cfg_queue_s again_idx_gps;
	struct vindev_wdr_cfg_queue_s dgain_idx_gps;
	struct vindev_wdr_cfg_queue_s rhs_gps;
	u32 shutter_delays[VINDEV_SHT_AGC_PARAM_NUM]; // unit: vsync
	u32 again_delays[VINDEV_SHT_AGC_PARAM_NUM]; // unit: vsync
	u32 dgain_delays[VINDEV_SHT_AGC_PARAM_NUM]; // unit: vsync
	u32 rhs_delays[VINDEV_SHT_AGC_PARAM_NUM]; // unit: vsync
};

struct vindev_linear_cfg_queue_s {
	u32 queue[MAX_SHT_AGC_QUEUE_NUM];
	u16 head;
	u16 tail;
};

struct vindev_linear_sht_agc_queue_s {
	struct vindev_linear_cfg_queue_s shutters[VINDEV_REG_CONTEXT_NUM];
	struct vindev_linear_cfg_queue_s agains[VINDEV_REG_CONTEXT_NUM];
	u32 shutter_delay; // unit: vsync
	u32 again_delay; // unit: vsync
};

struct vindev_vsrc_ctx_s {
	u32 switch_supported : 1;
	u32 switch_enabled : 1;
	u32 num : 3;
	u32 cur_ctx : 3;
	u32 frame_cnt_locked : 1;
	u32 reset_frame_cnt : 1;
	u32 ctx0_frame_cnt_bit0 : 1;
	u32 exit_kthread : 1;
	u32 sensor_internal_switch : 1;
	u32 reserved0 : 19;
	struct task_struct *kthread;
};

struct vin_device {
	int vsrc_id;
	const char *name;
	struct device *dev;
	u32 intf_id;
	u32 dev_type;
	u32 sub_type;
	u32 sensor_id;
	u32 pixel_size;

	struct vin_ops *ops;
	struct list_head list;

	int default_mode;
	int default_hdr_mode;
	int default_dg_mode;
	int frame_rate;	/* q9 format */
	int framerate_hp;	/* high precision format */
	int shutter_time; /* vsrc_ctx: 0 */
	int shutter_time_ext[VINDEV_REG_CONTEXT_NUM - 1]; /* vsrc_ctx: 1 ~ n */
	int agc_db; /* vsrc_ctx: 0 */
	int agc_db_ext[VINDEV_REG_CONTEXT_NUM - 1]; /* vsrc_ctx: 1 ~ n */
	int agc_db_max;
	int agc_db_min;
	int agc_db_step;
	int wdr_again_idx_min;
	int wdr_again_idx_max;
	int wdr_dgain_idx_min;
	int wdr_dgain_idx_max;
	struct vindev_wdr_sht_agc_queue_s pending_wdr_cfg;
	struct vindev_wdr_sht_agc_queue_s applying_wdr_cfg;
	struct vindev_linear_sht_agc_queue_s pending_linear_cfg;
	struct vindev_linear_sht_agc_queue_s applying_linear_cfg;
	struct work_struct set_mode_work;
	struct vindev_mode pending_video_mode;
	wait_queue_head_t set_mode_wq;
	int set_mode_rval;
	int rhs_frame_delay; /* frame delay relative to shutter */

	struct vin_video_format *formats;
	u32 num_formats;
	struct vin_video_format *cur_format;
	struct vin_video_format *cached_format;
	struct vin_video_pll *plls;
	u32 num_plls;
	struct vin_video_pll *cur_pll;
	struct vin_precise_fps *p_fps;
	u32 num_p_fps;
	u32 pre_video_mode;
	u32 pre_hdr_mode;
	u32 pre_bits;
	u32 pre_max_fps;
	struct vin_sht_agc_gp_info wdr_gp;
	bool reset_for_mode_switch;
	bool shutter_agc_check;
	bool pwr_seq_rst_first;
	bool power_on_flag;
	bool skip_reg_update;
	bool dummy_out_support;
	u32 sync_bind_src;
	u32 chan_num;
	u32 reset_time_ms;
	u32 mirror_pattern;
	u32 bayer_pattern;
	struct vindev_vsrc_ctx_s vsrc_ctx;
	u8 vsrc_status : 4;	/* For S5L & CV2x & CV5x & CV72 & N1, it's used as HW status, @sa amba_vsrc_status.*/
	u8 vsrc_op : 2;	/* For Cv2x & CV5x & CV72 & N1, it's used as OP by marking device as enabled or disabled to
			support entering preivew with broken sensors. 0(Default): mark as enabled, 1: mark as
			disabled, @sa amba_vsrc_op.*/
	u8 vsrc_broken_skip : 1;	/* This is a flag used to skip setting sensor mode when vdev hw is broken
			and vsrc_op is enabled, 0(Default): marked as not-skipped, 1: marked as skipped. */
	u8 is_trigger_mode : 1; /* This is a flag used to indicate sensor trigger mode, 0: not use external trigger, 1: use external trigger */
	u8 flash_enable : 1;
	u8 is_virtual : 1;
	u8 is_brg : 1;
	u8 is_roi_supported : 1;
	u8 reserved1 : 4;
	u8 reserved2[2];
	unsigned long priv[0] __aligned(sizeof(void *));
};

struct vin_reg_8_8 {
	u8 addr;
	u8 data;
};

struct vin_reg_16_8 {
	u16 addr;
	u8 data;
};

struct vin_reg_16_16 {
	u16 addr;
	u16 data;
};

struct vin_video_pll {
	u32 mode;
	u32 clk_si;	/* output to sensor */
	u32 pixelclk;	/* input from sensor */
};

struct vin_precise_fps {
	int fps;
	int video_mode;
	int pll_idx;
};

/* for slave sensor */
struct vin_master_sync {
	u32 hsync_period;
	u32 hsync_width : 16;
	u32 hsync_offset : 16;
	u32 vsync_period;
	u32 vsync_width : 16;
	u32 vsync_offset : 16;
	u32 vs_hs_polarity : 16;
	u32 vs_dly_id : 16;
	u32 vs_dly : 16;
	u32 id : 16;
};

/* Sync to amboot/include/dsp/s2l_cmd_msg.h and s3l_cmd_msg.h*/
struct vin_video_format {
	u32 video_mode;
	u32 device_mode;
	u32 pll_idx;	/* clock index */
	u16 width;	/* image horizontal size in unit of pixel */
	u16 height;	/* image vertical size in unit of line */

	u16 def_start_x;
	u16 def_start_y;
	u16 def_width;
	u16 def_height;
	u8 format;
	u8 type;
	u8 bits;
	u8 ratio;
	u8 mirror_pattern;
	u8 bayer_pattern;
	u8 hdr_mode;
	u8 readout_mode;
	u32 line_time;
	u32 vb_time;
	u32 dlyvs_lines;

	u32 max_fps;
	int default_fps;
	int default_agc;
	int default_shutter_time;
	int default_bayer_pattern;

	/* hdr mode related */
	u16 act_start_x;
	u16 act_start_y;
	u16 act_width;
	u16 act_height;
	u16 hdr_long_offset;
	u16 hdr_short1_offset;
	u16 hdr_short2_offset;
	u16 hdr_short3_offset;
	u16 dual_gain_mode;
	u8 gp_ctrl_used;
	u8 xenon_flash_enable;

	/* bridge mode related */
	u16 brg_start_x;
	u16 brg_start_y;
	u16 brg_width;
	u16 brg_height;
	u8 brg_compress_ratio;
};

/* for vin bridge */
struct vin_brg_config {
	u8 intf_type;
	u8 dev_addr;
	u8 direct_map_addr;
	u8 reg_addr_width;
	u8 reg_data_width;
};

struct vin_brg_info {
	const char *name;
	u8 intf_type;
	u8 vout_lane;
	u8 max_chan;
	u16 max_width;
	u16 max_height;
};

struct vin_ops {
	int (*init_device)(struct vin_device *vdev);
	int (*suspend)(struct vin_device *vdev);
	int (*resume)(struct vin_device *vdev);
	int (*set_format)(struct vin_device *vdev, struct vin_video_format *format);
	int (*set_frame_rate)(struct vin_device *vdev, int fps);
	int (*set_agc_index)(struct vin_device *vdev, int idx);
	int (*set_pll)(struct vin_device *vdev, int pll_idx);
	int (*set_mirror_mode)(struct vin_device *vdev, struct vindev_mirror *args);
	int (*set_stream_mode)(struct vin_device *vdev, u32 stream_mode);
	int (*get_eis_info)(struct vin_device *vdev, struct vindev_eisinfo *args);
	int (*read_reg)(struct vin_device *vdev, u32 reg, u32 *data);
	int (*write_reg)(struct vin_device *vdev, u32 reg, u32 data);
	int (*shutter2row)(struct vin_device *vdev, u32 *shutter_time);
	int (*set_shutter_row)(struct vin_device *vdev, u32 shutter_row);
	int (*set_dgain_ratio)(struct vin_device *vdev, struct vindev_dgain_ratio *args);
	int (*get_dgain_ratio)(struct vin_device *vdev, struct vindev_dgain_ratio *args);
	int (*set_hold_mode)(struct vin_device *vdev, u32 hold_mode);
	int (*get_chip_status)(struct vin_device *vdev, struct vindev_chip_status *args);
	int (*get_aaa_info)(struct vin_device *vdev, struct vindev_aaa_info *args);
	int (*set_low_light_mode)(struct vin_device *vdev, u32 ll_mode);
	int (*set_tp_mode)(struct vin_device *vdev, u32 tp_mode);
	int (*set_pwr_mode)(struct vin_device *vdev, u32 pwr_mode);
	int (*set_gpio_ctrl)(struct vin_device *vdev, u32 ctrl_id, u32 value);
	int (*set_trig_mode)(struct vin_device *vdev, u32 value);
	int (*set_roi_win)(struct vin_device *vdev, struct vindev_roi_info *args);

	/* WDR control */
	int (*set_wdr_again_idx_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);
	int (*get_wdr_again_idx_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);

	int (*set_wdr_dgain_idx_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);
	int (*get_wdr_dgain_idx_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);

	int (*set_wdr_shutter_row_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);
	int (*get_wdr_shutter_row_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);

	int (*set_wdr_max_middle_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);
	int (*get_wdr_max_middle_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);

	int (*wdr_shutter2row)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);
	int (*set_wdr_shutter_agc_gp)(struct vin_device *vdev, struct vindev_wdr_gp_s *shutter_gp, struct vindev_wdr_gp_s *agc_gp, int frame_type);

	/* return dynamic rhs & offset */
	int (*wdr_shutter2offset)(struct vin_device *vdev, struct vindev_wdr_gp_s *args);

	int (*aaa_compensation)(struct vin_device *vdev, struct vindev_wdr_gp_info *args);

	/* For decoder, run-time report video mode */
	int (*get_format)(struct vin_device *vdev);

	/* for sensors with multiple contexts */
	int (*switch_vsrc_ctx)(struct vin_device *vdev);
	int (*set_agc_index_ctx)(struct vin_device *vdev, struct vindev_agc *vsrc_agc);
	int (*set_shutter_row_ctx)(struct vin_device *vdev, struct vindev_shutter *vsrc_shutter);
	int (*get_frame_cnt)(struct vin_device *vdev, u32 *frame_cnt);
};

struct vin_brg_chan_id {
	u32 load_id : 8;
	u32 active_id : 8;
	u32 broken_id : 8;
	u32 dbg_id : 8;
};

struct vin_brg_chan_dev {
	struct vin_device *ch_dev;
	u8 ch_id;
	u8 ch_index;
};

struct vin_brg_pll_info {
	struct vin_video_pll *brg_pll;
	struct vin_video_pll *sen_pll;
	int cur_pll_index;
};

struct vin_brg_sensor_ctrl {
	struct vin_ops *ops;
	bool reset_for_mode_switch;
	u8 intf_type;
	u8 dev_addr;
	u8 direct_map_addr;
	u8 reg_addr_w_byte;
	u8 reg_data_w_byte;
	u8 bus_id;
};

struct vin_brg_vout_info {
	u32 data_rate;
	u32 interface_type : 8;
	u32 output_format : 8;
	u32 lane_num : 8;
	u32 bits : 8;
	u32 vc_pattern : 4;
	u32 phy_type : 1;
	u32 reserved : 27;
};

struct vin_brg_map_gpio {
	int des;
	int ser;
	bool dir;
};

struct vin_brg_io_info {
	int ser_rst_gpios[2];
	int ser_pwr_gpios[2];
	int des_pwr_gpios[2];
	struct vin_brg_map_gpio map_gpios[6];
};

struct vin_brg_remap_addr_info {
	u32 ser;
	u32 sen;
	u32 global;
};

struct vin_brg_vdev_info {
	struct vin_device *vdev;
	struct vin_ops *ops;
	struct vin_video_format *formats;
	struct vin_video_pll *plls;
	u32 num_formats;
	u32 num_plls;
};

struct vin_brg_device {
	int id;
	const char *name;
	u32 intf_id;
	u32 intf_id_2nd;
	u32 bus_addr;
	u32 ch_num : 8;
	u32 base_index : 8;
	u32 exit_flag : 8;
	u32 probe_flag : 8;
	u32 ch_priv_size;
	u32 master_clk;
	u32 reset_time_ms;
	u32 data_rate_mhz;
	bool use_vin_rst;
	bool use_vin_crop;
	bool use_sen_clk;
	bool skip_reg_update;
	bool skip_hw_rst;
	bool gpio_request_flag;
	bool dummy_out_support;
	bool dual_port;
	bool set_vin_first;
	bool use_vc;
	int pwr_gpio[VIN_BRG_PWRST_IO_NUM];
	int rst_gpio[VIN_BRG_PWRST_IO_NUM];
	int irq_gpio[2];
	int irq_num[2];
	char irq_name[2][32];
	u8 rst_gpio_active[VIN_BRG_PWRST_IO_NUM];
	u8 pwr_gpio_active[VIN_BRG_PWRST_IO_NUM];
	struct list_head list;
	struct vin_brg_ops *ops;
	struct vin_brg_chan_id ch_id;
	struct vin_brg_chan_dev ch_list[4];
	struct vin_brg_sensor_ctrl sensor_ctrl;
	struct vin_brg_pll_info pll_info;
	struct vin_brg_vout_info vout_info;
	struct vin_brg_io_info io_info;
	struct vin_brg_remap_addr_info remap_addr;
	struct vin_device_config *vin_cfg;
	struct task_struct	*kthread;
	struct work_struct	 init_work;
	struct vin_brg_vdev_info vdev_info;

	unsigned long priv[0];
};

struct vin_brg_ops {
	int (*brg_write_reg)(struct vin_brg_device *ambrg, u32 chip_id, u32 subaddr, u32 data);
	int (*brg_read_reg)(struct vin_brg_device *ambrg, u32 chip_id, u32 subaddr, u32 *data);
	int (*brg_set_sensor_clk)(struct vin_brg_device *ambrg, u32 chip_id, int pll_idx);
	int (*brg_config)(struct vin_brg_device *ambrg, u32 chip_id, struct vin_video_format *format);
	int (*brg_confirm_stream)(struct vin_brg_device *ambrg, u32 chip_id);
	int (*brg_monitor_task)(struct vin_brg_device *ambrg);
	int (*brg_write_chan_reg)(struct vin_brg_device *ambrg, u32 chip_id, u32 addr, u32 data, u32 size);
	int (*brg_read_chan_reg)(struct vin_brg_device *ambrg, u32 chip_id, u32 addr, u32 *data);
	int (*brg_hw_init_pre)(struct vin_brg_device *ambrg, u32 chip_id);
	int (*brg_hw_init_post)(struct vin_brg_device *ambrg, u32 chip_id);
	int (*brg_irq_callback)(struct vin_brg_device *ambrg);
	int (*brg_get_status)(struct vin_brg_device *ambrg, u32 chip_id, struct vindev_chip_status *status);
	int (*brg_set_gpio_ctrl)(struct vin_brg_device *ambrg, u32 chip_id, u32 ctrl_id, u32 value);
	int (*brg_write_chan_buf)(struct vin_brg_device *ambrg, u32 chip_id, u8 *buf, u32 size);
	/* For decoder, run-time report video mode */
	int (*brg_get_format)(struct vin_brg_device *ambrg, u32 chip_id, struct vin_device *vdev);

	void (*brg_set_clk_si)(struct vin_brg_device *ambrg, u32 chip_id, u32 clk_si);
	void (*brg_vin_config_pre)(struct vin_brg_device *ambrg, u32 chip_id);
	void (*brg_vin_config_post)(struct vin_brg_device *ambrg, u32 chip_id);
	void (*brg_vout_config)(struct vin_brg_device *ambrg, u32 chip_id, struct vin_video_format *format);
	void (*brg_set_stream)(struct vin_brg_device *ambrg, u32 chip_id, u32 stream_mode);
};
/* ========================================================================== */
void ambarella_vin_mipi_phy_reset(void);
void ambarella_vin_mipi_phy_enable(u8 lanes);
int ambarella_set_vin_config(struct vin_device *vdev, struct vin_device_config *cfg);
int ambarella_set_vin_master_sync(struct vin_device *vdev,
		struct vin_master_sync *master_cfg, bool by_dbg_bus);
int ambarella_stop_vin_master_sync(struct vin_device *vdev);
int ambarella_vin_vsync_delay(struct vin_device *vdev, u32 vsync_delay);
int ambarella_vin_eof_delay(struct vin_device *vdev, u32 eof_delay);
int ambarella_vin_trigger_delay(struct vin_device *vdev, u32 trigger_delay);
int ambarella_vin_set_pending_sht_agc(struct vin_device *vdev, struct vindev_wdr_gp_info *sht_agc_gp);
int ambarella_vin_set_sht_agc_delay_info(struct vin_device *vdev, struct vindev_sht_agc_seq *sht_agc_delay_info);
void ambarella_vin_apply_pending_sht_agc(struct vin_device *vdev);
int ambarella_vin_wait_sht_agc_idle(struct vin_device *vdev);
void ambarella_vin_add_precise_fps(struct vin_device *vdev,
		struct vin_precise_fps *p_fps, u32 num_p_fps);
struct vin_device *ambarella_vin_create_device(const char *name,
		u32 sensor_id, u32 priv_size);
void ambarella_vin_free_device(struct vin_device *vdev);
int ambarella_vin_register_device(struct vin_device *vdev, struct vin_ops *ops,
		struct vin_video_format *formats, u32 num_formats,
		struct vin_video_pll *plls, u32 num_plls);
int ambarella_vin_unregister_device(struct vin_device *vdev);
int ambarella_vin_hw_rst(struct vin_device *vdev);
int amba_brg_write_reg(struct vin_device *vdev, u32 subaddr, u32 data, u32 size, u8 global);
int amba_brg_write_buf(struct vin_device *vdev, u8 *buf, u32 size, u8 global);
int amba_brg_read_reg(struct vin_device *vdev, u32 subaddr, u32 *data);
int amba_brg_register_device(struct vin_device *vdev, struct vin_ops *ops,
		struct vin_video_format *formats, u32 num_formats,
		struct vin_video_pll *plls, u32 num_plls, u32 priv_size,
		struct vin_brg_config *brg_cfg);
int amba_brg_unregister_device(const char *name);
int amba_brg_set_vin_config(struct vin_device *vdev, struct vin_device_config *cfg);
int amba_brg_query_info(u32 brg_id, struct vin_brg_info *brg_info);
int amba_register_vin_brg(struct vin_brg_device *ambrg, struct vin_brg_ops *ops);
int amba_unregister_vin_brg(const char *name);
int ambarella_vin_update_active_device(struct vin_device *vdev);
int ambarella_vin_repeat_frame(u8 vinc_id, u8 thru_reg);

#endif //__VIN_PRI_H

