/*
 * vout_api.h
 *
 * History:
 *    2009/05/13 - [Anthony Ginger] Create
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


#ifndef __VOUT_API_H__
#define __VOUT_API_H__

#include <linux/types.h>

#define AMBA_VOUTDEV_MAX_NUM		32

/* following enum are all hw depenedent, so don't change the order */
enum {
	VOUT_FIXED_FORMAT_NONE		= 0,
	VOUT_FIXED_FORMAT_480I60,
	VOUT_FIXED_FORMAT_480P60,
	VOUT_FIXED_FORMAT_576I50,
	VOUT_FIXED_FORMAT_576P50,
	VOUT_FIXED_FORMAT_720P60,
	VOUT_FIXED_FORMAT_720P50,
	VOUT_FIXED_FORMAT_1080I60,
	VOUT_FIXED_FORMAT_1080I50,
	VOUT_FIXED_FORMAT_1080I48,
	VOUT_FIXED_FORMAT_1080P60,
	VOUT_FIXED_FORMAT_1080P50,
	VOUT_FIXED_FORMAT_1080P24,
};

enum {
	VOUT_LCD_COLOR_SEQ_RGB = 0,
	VOUT_LCD_COLOR_SEQ_RBG,
	VOUT_LCD_COLOR_SEQ_GRB,
	VOUT_LCD_COLOR_SEQ_GBR,
	VOUT_LCD_COLOR_SEQ_BRG,
	VOUT_LCD_COLOR_SEQ_BGR,

	VOUT_LCD_COLOR_SEQ_RGRG = 0,
	VOUT_LCD_COLOR_SEQ_GRGR = 2,
	VOUT_LCD_COLOR_SEQ_GBGB = 3,
	VOUT_LCD_COLOR_SEQ_BGBR = 5,
};

enum {
	VOUT_CLOCK_EDGE_RISING = 0,
	VOUT_CLOCK_EDGE_FALLING,
};

enum {
	VOUT_OSD_MODE_RGBVYU565 = 0,
	VOUT_OSD_MODE_BGRUYV565,
	VOUT_OSD_MODE_AYUV4444,
	VOUT_OSD_MODE_RGBA4444,
	VOUT_OSD_MODE_BGRA4444,
	VOUT_OSD_MODE_ABGR4444,
	VOUT_OSD_MODE_ARGB4444,
	VOUT_OSD_MODE_AYUV1555,
	VOUT_OSD_MODE_XYUV1555,
	VOUT_OSD_MODE_RGBA5551,
	VOUT_OSD_MODE_BGRA5551,
	VOUT_OSD_MODE_ABGR1555,
	VOUT_OSD_MODE_ARGB1555,
	VOUT_OSD_MODE_AYUV8888 = 27,
	VOUT_OSD_MODE_RGBA8888,
	VOUT_OSD_MODE_BGRA8888,
	VOUT_OSD_MODE_ABGR8888,
	VOUT_OSD_MODE_ARGB8888,
};

/*
 * Configuration for vout_timing_t::mipi_burst_mode
 */
enum {
	VOUT_MIPI_NON_BURST_MODE_WITH_SYNC_PULSES = 0,
	VOUT_MIPI_NON_BURST_MODE_WITH_SYNC_EVENTS,
	VOUT_MIPI_BURST_MODE,
};

struct vout_ops;

/* please refer to CEA-861-F for video timing */
typedef struct {
	u32 mode;
	u32 fixed_format;
	u32 pixel_freq;
	u32 vfreq;		/* frame_rate(fps) = vfreq / 2 if interlaced */
	u16 interlaced;
	u16 width;		/* Hactive, double colocked for some formats */
	u16 height;		/* Vactive */
	u16 htotal;
	u16 vtotal;		/* top field vtotal + bottom field vtotal if interlaced */
	u16 hpol;		/* Hsync polarity, dve is different than CEA-861-F */
	u16 vpol;		/* Vsync polarity, dve is different than CEA-861-F */
	u16 vic;		/* video information code, refer to CEA-861-F, used by HDMI */
	u32 hsync_start;	/* should be 0, different than CEA-861-F */
	u32 hsync_end;		/* Hsync for hdmi */
	/* 1 for bt656 and dve, different than CEA-861-F */
	u32 hactive_start;	/* Hsync+Hback for hdmi */
	/* dve and lcd are different than CEA-861-F */
	u32 hactive_end;	/* hactive_start+Hactive-1 for tv, but lcd dependency for lcd */
	u32 vsync_start;	/* should be 0 */
	u32 vsync_end;		/* vsync width */
	u32 vactive_start;	/* Vsync+Vback for tv, but lcd dependency for lcd */
	u32 vactive_end;	/* vactive_start+Vactive-1 for tv, but lcd dependency for lcd */
	u32 fpd_orderreverse;
	u32 fpd_4thlane_en;
	u32 fpd_msb_select;
	u32 mipi_lane_num;
	u32 mipi_eotp_en;
	u32 mipi_sync_end_en;
	u32 mipi_dsi_hbp_wc;
	u32 mipi_dsi_sync_wc;
	u32 mipi_dsi_hfp_wc;
	u32 mipi_dsi_vblank_wc;
	unsigned long long data_rate_per_lane;
	u32 mipi_output_mode;
	u32 hvldpol;
	u32 vsync_start_col;
	u32 vsync_end_col;
	u32 cvbs_blacklevel : 8;
	u32 cvbs_blanklevel : 8;
	u32 cvbs_synclevel : 8;
	u32 cvbs_ygain : 2;
	u32 cvbs_cgain : 2;
	u32 reserved_cvbs : 4;
	u32 mipi_cont_clk_mode : 1;
	u32 mipi_tx_ibctrl : 3;
	u32 mipi_tx_vcmset : 3;
	u32 mipi_tx_pib : 4;
	u32 mipi_tx_term : 4;
	/* config_mipi_by_timing:
	 * 1: tx_ibctrl/tx_vcmset/tx_pib/tx_term will be configured by timing parameters
	 * 0: tx_ibctrl/tx_vcmset/tx_pib/tx_term will be configured somewhere else
	 */
	u32 config_mipi_by_timing : 1;
	u32 mipi_burst_mode : 2;
	u32 reserved_mipi : 14;
} vout_timing_t;

#define DSI_REG_DELAY_FLAG 0xFFFFFFFF
#define DSI_REG_TABLE_END 0xFFFFFFFE

#define MIPI_DSI_CMD_EXT_PARAM_SIZE	128
#define MIPI_DSI_CMD_ACK_SIZE 12

typedef struct {
	u32 command_id;
	u32 size;
	u8 param[MIPI_DSI_CMD_EXT_PARAM_SIZE];
} vout_dsi_reg_t;

typedef struct vout_device {
	int vsink_id;
	u32 voutc_id;
	const char *name;
	struct list_head list;

	u32 type;
	vout_timing_t *avail;	/* available format and timing */
	u32 num_avail;
	u32 fixed_pclk;
	u32 hdmi_controller_version;	/* HDMI only */

	u32 output_mode : 5;	/* data mode for LCD, while color space for HDMI */
	u32 color_seq_even : 3;	/* LCD only */
	u32 color_seq_odd : 3;	/* LCD only */
	u32 clk_edge : 1;	/* LCD only */
	u32 overscan : 1;	/* HDMI only */
	u32 mipi_lane_num : 3;	/* MIPI only */
	u32 bta_en : 1;
	u32 hdmi_deep_color_mode : 2;	/* HDMI only: 0: 8bpc; 1: 10bpc; 2/3: reserved */
	u32 reserved0 : 13;

	void __iomem *mipi_cmd_reg;/* MIPI only */
	void __iomem *digital_opmod_reg;/* Used for mipi dsi command */
	struct vout_ops *ops;
	vout_timing_t *current_timing;
	struct clk *vout_clk;

	unsigned long priv[0];
} voutd_t;

#define VOUT_CEA_VIC_NUM		128
#define VOUT_NATIVE_TIMING_NUM		10

struct vout_edid_vic {
	u8 vic;
	u8 yuv420_support;
};

struct vout_edid_sink {
	const struct vout_edid_vic *cea_vic;
	const vout_timing_t *native_timings;
	/* ambhdmi only: HDMI VIC block from VSDB */
	bool (*is_hdmi_vic)(voutd_t *voutd, u16 vic);
};

struct vout_ops {
	vout_timing_t *(*auto_timing)(voutd_t *voutd);
	int (*check_timing)(voutd_t *voutd, vout_timing_t *timing);
	/*
	 * Return true if timing may appear in the DRM connector mode list.
	 * When unset, all voutd->avail timings are exposed.
	 * When set but sink has no valid EDID, return true for every timing.
	 */
	bool (*mode_supported)(voutd_t *voutd, vout_timing_t *timing);
	int (*set_timing)(voutd_t *voutd, vout_timing_t *timing);
	void (*set_infoframe)(voutd_t *voutd, vout_timing_t *timing);
	int (*dsi_gen_short_write)(voutd_t *voutd, u8 after_frame, u8 commandid, u8 size, u8 data);
	int (*dsi_gen_long_write)(voutd_t *voutd, u8 after_frame, u8 commandid, u16 size, u8 *data);
	int (*dsi_dcs_short_write)(voutd_t *voutd, u8 after_frame, u8 commandid, u8 size, u8 data);
	int (*dsi_dcs_long_write)(voutd_t *voutd, u8 after_frame, u8 commandid, u16 size, u8 *data);
	int (*dsi_dcs_read)(voutd_t *voutd, u8 after_frame, u8 commandid, u16 size, u8 *data);
	int (*dsi_gen_read)(voutd_t *voutd, u8 after_frame, u8 cmd1, u8 cmd2, u16 size, u8 *data);
	int (*dsi_set_max_ret_pkt_size)(voutd_t *voutd, u8 after_frame, u16 size);
	int (*register_notifier)(struct notifier_block *nb);
	int (*unregister_notifier)(struct notifier_block *nb);
	int (*dev_reset)(voutd_t *voutd);
	int (*dev_disable)(voutd_t *voutd);
};

int vout_register_device(struct vout_device *voutd, struct vout_ops *ops);
int vout_unregister_device(struct vout_device *voutd);
int vout_auto_start(struct vout_device *voutd);

vout_timing_t *vout_find_timing_by_timing(voutd_t *voutd, vout_timing_t *timing);
vout_timing_t *vout_find_timing_by_vic(voutd_t *voutd, u32 vic);
vout_timing_t *vout_find_timing_by_vic_vfreq(voutd_t *voutd, u32 vic, u32 vfreq);
bool vout_edid_timing_supported(voutd_t *voutd, vout_timing_t *timing,
				const struct vout_edid_sink *sink);

/* HDMI 2.1 FRL link configuration */
enum vout_hdmi_frl_rate {
	VOUT_HDMI_FRL_RATE_NONE = 0,
	VOUT_HDMI_FRL_RATE_3L3G = 1,
	VOUT_HDMI_FRL_RATE_3L6G,
	VOUT_HDMI_FRL_RATE_4L6G,
	VOUT_HDMI_FRL_RATE_4L8G,
	VOUT_HDMI_FRL_RATE_4L10G,
	VOUT_HDMI_FRL_RATE_4L12G,
};

#define VOUT_HDMI_FRL_RATE_MAX			VOUT_HDMI_FRL_RATE_4L12G

u64 vout_hdmi_get_frl_required_rate(voutd_t *voutd, vout_timing_t *timing);
void vout_hdmi_get_frl_cap(u8 frl_rate, u8 *lanes, u8 *rate_per_lane);
int vout_hdmi_select_frl_config(voutd_t *voutd, vout_timing_t *timing,
	u8 max_frl_rate, u8 *frl_rate, u8 *frl_lanes, u8 *rate_per_lane);

/*===========================================================================*/

#define AMBA_VOUT_CLUT_SIZE		(256 * 4)

enum amba_video_source_status {
	AMBA_VIDEO_SOURCE_STATUS_IDLE = 0,
	AMBA_VIDEO_SOURCE_STATUS_RUNNING = 1,
	AMBA_VIDEO_SOURCE_STATUS_SUSPENDED = 2,
};

struct amba_video_source_scale_analog_info {
	u16 y_coeff;
	u16 pb_coeff;
	u16 pr_coeff;
	u16 y_cost;
	u16 pb_cost;
	u16 pr_cost;
};

struct amba_video_source_display_info {
	u8 enable_video;
	u8 enable_osd0;
	u8 enable_osd1;
	u8 enable_cursor;
};

struct amba_video_source_info {
	u32 enabled;
};

struct amba_video_source_osd_info {
	u32 osd_id;
	u32 gblend;
	u16 width;
	u16 height;
	u16 offset_x;
	u16 offset_y;
	u16 zoom_x;
	u16 zoom_y;
};

struct amba_video_source_osd_clut_info {
	u8 *pclut_table;
	u8 *pblend_table;
};

struct amba_video_source_clock_setup {
	u32 src;
	u32 freq_hz;
};

struct amba_vout_window_info {
	u16 start_x;
	u16 start_y;
	u16 end_x;
	u16 end_y;
	u16 width;
	u16 field_reverse;
};

struct amba_vout_hv_sync_info {
	u16 hsync_start;
	u16 hsync_end;
	u16 vtsync_start;
	u16 vtsync_end;
	u16 vbsync_start;
	u16 vbsync_end;

	u16 vtsync_start_row;
	u16 vtsync_start_col;
	u16 vtsync_end_row;
	u16 vtsync_end_col;
	u16 vbsync_start_row;
	u16 vbsync_start_col;
	u16 vbsync_end_row;
	u16 vbsync_end_col;

	enum amba_vout_sink_type sink_type;
};

enum amba_vout_hvld_type {
	AMBA_VOUT_HVLD_POL_LOW	= 0,
	AMBA_VOUT_HVLD_POL_HIGH,
};

struct amba_vout_hvld_sync_info {
	enum amba_vout_hvld_type hvld_type;
};

struct amba_vout_hv_size_info {
	u16 hsize;
	u16 vtsize;	//vsize for progressive
	u16 vbsize;
};


enum amba_video_source_cmd {
	AMBA_VIDEO_SOURCE_IDLE = 30000,
	AMBA_VIDEO_SOURCE_UPDATE_IAV_INFO,

	AMBA_VIDEO_SOURCE_RESET = 30100,
	AMBA_VIDEO_SOURCE_UPDATE_VOUT_SETUP,
	AMBA_VIDEO_SOURCE_RUN,
	AMBA_VIDEO_SOURCE_SUSPEND,
	AMBA_VIDEO_SOURCE_RESUME,
	AMBA_VIDEO_SOURCE_SUSPEND_TOSS,
	AMBA_VIDEO_SOURCE_RESUME_TOSS,
	AMBA_VIDEO_SOURCE_HALT,

	AMBA_VIDEO_SOURCE_REGISTER_SINK = 30200,
	AMBA_VIDEO_SOURCE_UNREGISTER_SINK,
	AMBA_VIDEO_SOURCE_REGISTER_IRQ_CALLBACK,
	AMBA_VIDEO_SOURCE_GET_SINK_NUM,
	AMBA_VIDEO_SOURCE_REGISTER_AR_NOTIFIER,
	AMBA_VIDEO_SOURCE_REPORT_SINK_EVENT,

	AMBA_VIDEO_SOURCE_GET_CONFIG = 31000,
	AMBA_VIDEO_SOURCE_GET_OSD,
	AMBA_VIDEO_SOURCE_GET_ACTIVE_WIN,
	AMBA_VIDEO_SOURCE_GET_VOUT_SETUP,
	AMBA_VIDEO_SOURCE_GET_DVE,
	AMBA_VIDEO_SOURCE_GET_VIDEO_AR,

	AMBA_VIDEO_SOURCE_SET_CONFIG = 32000,
	AMBA_VIDEO_SOURCE_SET_VIDEO_SIZE,
	AMBA_VIDEO_SOURCE_SET_OSD,
	AMBA_VIDEO_SOURCE_SET_OSD_CLUT,
	AMBA_VIDEO_SOURCE_SET_BG_COLOR,
	AMBA_VIDEO_SOURCE_SET_LCD,
	AMBA_VIDEO_SOURCE_SET_ACTIVE_WIN,
	AMBA_VIDEO_SOURCE_SET_HV,
	AMBA_VIDEO_SOURCE_SET_HVSYNC,
	AMBA_VIDEO_SOURCE_SET_HVLD,
	AMBA_VIDEO_SOURCE_SET_SCALE_SD_ANALOG_OUT,
	AMBA_VIDEO_SOURCE_SET_VBI,
	AMBA_VIDEO_SOURCE_SET_VIDEO_INFO,
	AMBA_VIDEO_SOURCE_SET_VOUT_SETUP,
	AMBA_VIDEO_SOURCE_SET_CSC,
	AMBA_VIDEO_SOURCE_SET_CSC_DYNAMICALLY,
	AMBA_VIDEO_SOURCE_SET_DVE,
	AMBA_VIDEO_SOURCE_SET_CLOCK_SETUP,
	AMBA_VIDEO_SOURCE_SET_OSD_BUFFER,
	AMBA_VIDEO_SOURCE_SET_DISPLAY_INPUT,
	AMBA_VIDEO_SOURCE_SET_VIDEO_AR,
	AMBA_VIDEO_SOURCE_SET_MIXER_CSC,

	AMBA_VIDEO_SOURCE_INIT_HDMI = 33000,
	AMBA_VIDEO_SOURCE_INIT_ANALOG,
	AMBA_VIDEO_SOURCE_INIT_DIGITAL,
	AMBA_VIDEO_SOURCE_HDMI_GET_NATIVE_MODE,
};
#define AMBA_VIDEO_SOURCE_FORCE_RESET		(1 << 0)
#define AMBA_VIDEO_SOURCE_UPDATE_MIXER_SETUP	(1 << 1)
#define AMBA_VIDEO_SOURCE_UPDATE_VIDEO_SETUP	(1 << 2)
#define AMBA_VIDEO_SOURCE_UPDATE_DISPLAY_SETUP	(1 << 3)
#define AMBA_VIDEO_SOURCE_UPDATE_OSD_SETUP	(1 << 4)
#define AMBA_VIDEO_SOURCE_UPDATE_CSC_SETUP	(1 << 5)

enum amba_video_sink_cmd {
	AMBA_VIDEO_SINK_IDLE = 40000,

	AMBA_VIDEO_SINK_RESET = 40100,
	AMBA_VIDEO_SINK_SUSPEND,
	AMBA_VIDEO_SINK_RESUME,
	AMBA_VIDEO_SINK_GET_SOURCE_ID,
	AMBA_VIDEO_SINK_GET_INFO,

	AMBA_VIDEO_SINK_GET_MODE = 41000,

	AMBA_VIDEO_SINK_SET_MODE = 42000,
};

enum amba_video_source_csc_path_info {
	AMBA_VIDEO_SOURCE_CSC_DIGITAL = 0,
	AMBA_VIDEO_SOURCE_CSC_ANALOG = 1,
	AMBA_VIDEO_SOURCE_CSC_HDMI = 2,
};

enum amba_video_source_csc_mode_info {
	AMBA_VIDEO_SOURCE_CSC_YUVSD2YUVHD	= 0,	/* YUV601 -> YUV709 */
	AMBA_VIDEO_SOURCE_CSC_YUVSD2YUVSD	= 1,	/* YUV601 -> YUV601 */
	AMBA_VIDEO_SOURCE_CSC_YUVSD2RGB		= 2,	/* YUV601 -> RGB    */
	AMBA_VIDEO_SOURCE_CSC_YUVHD2YUVSD	= 3,	/* YUV709 -> YUV601 */
	AMBA_VIDEO_SOURCE_CSC_YUVHD2YUVHD	= 4,	/* YUV709 -> YUV709 */
	AMBA_VIDEO_SOURCE_CSC_YUVHD2RGB		= 5,	/* YUV709 -> RGB    */
	AMBA_VIDEO_SOURCE_CSC_RGB2RGB		= 6,	/* RGB    -> RGB */
	AMBA_VIDEO_SOURCE_CSC_RGB2YUV		= 7,	/* RGB    -> YUV */
	AMBA_VIDEO_SOURCE_CSC_RGB2YUV_12BITS	= 8,	/* RGB    -> YUV_12bits */

	AMBA_VIDEO_SOURCE_CSC_ANALOG_SD		= 0,
	AMBA_VIDEO_SOURCE_CSC_ANALOG_HD		= 1,
};

enum amba_video_source_csc_clamp_info {
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_ANALOG_HD_FULL		= 0,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_ANALOG_SD_FULL		= 1,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_HD_FULL		= 2,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_SD_FULL		= 3,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_ANALOG_HD_CLAMP		= 4,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_ANALOG_SD_CLAMP		= 5,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_HD_CLAMP	= 6,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_SD_CLAMP	= 7,
	AMBA_VIDEO_SOURCE_CSC_DATARANGE_HDMI_YCBCR422_CLAMP	= 8,

	AMBA_VIDEO_SOURCE_CSC_ANALOG_CLAMP_SD			= 0,
	AMBA_VIDEO_SOURCE_CSC_ANALOG_CLAMP_HD			= 1,
	AMBA_VIDEO_SOURCE_CSC_ANALOG_CLAMP_SD_NTSC		= 2,
	AMBA_VIDEO_SOURCE_CSC_ANALOG_CLAMP_SD_PAL		= 3,
};

enum amba_vout_interlace_enable {
	INTERLACE_DISABLE		= 0,
	INTERLACE_ENABLE		= 1,
};

enum amba_vout_digital_hvld_polarity {
	HVLD_ASSERTED_LOW		= 0,
	HVLD_ASSERTED_HIGH		= 1,
};

enum amba_vout_digital_vsync_polarity {
	VSYNC_ASSERTED_LOW		= 0,
	VSYNC_ASSERTED_HIGH		= 1,
};

enum amba_vout_digital_hsync_polarity {
	HSYNC_ASSERTED_LOW		= 0,
	HSYNC_ASSERTED_HIGH		= 1,
};

enum amba_vout_digital_fpd_msb_select {
	JEIDA					= 0,
	VESA					= 1,
};

struct amba_video_source_csc_info {
	enum amba_video_source_csc_path_info path;
	enum amba_video_source_csc_mode_info mode;
	enum amba_video_source_csc_clamp_info clamp;
};

typedef void irq_callback_t(void);
typedef void (*vout_ar_notifier_t)(u8 video_src, u8 ar);


#define MIPI_DSI_DATA_TYPE_DCS_SHORTWRITE0          0x05    /* Short write, 0 parameter */
#define MIPI_DSI_DATA_TYPE_DCS_SHORTWRITE1          0x15    /* Short write, 1 parameter */
#define MIPI_DSI_DATA_TYPE_DCS_READ                 0x06    /* Read */
#define MIPI_DSI_DATA_TYPE_GENERIC_SHORTWRITE0      0x03    /* Generic short write, 0 parameter */
#define MIPI_DSI_DATA_TYPE_GENERIC_SHORTWRITE1      0x13    /* Generic short write, 1 parameter */
#define MIPI_DSI_DATA_TYPE_GENERIC_SHORTWRITE2      0x23    /* Generic short write, 2 parameter */
#define MIPI_DSI_DATA_TYPE_GENERIC_LONGWRITE        0x29    /* Generic long write,  */
#define MIPI_DSI_DATA_TYPE_DCS_LONGWRITE            0x39    /* Long write */
#define MIPI_DSI_DATA_TYPE_GENERIC_READ             0x04    /* Generic read , 0 parameter */
#define MIPI_DSI_DATA_TYPE_GENERIC_READ_1           0x14    /* Generic read , 1 parameter */
#define MIPI_DSI_DATA_TYPE_GENERIC_READ_2           0x24    /* Generic read , 2 parameter */
#define MIPI_DSI_DATA_TYPE_SET_MAX_RET_PKT_SIZE     0x37    /* Set Max Return Packet Size */
#define MIPI_DSI_COMMAND_WRITEACK_NOERR 0x84
#define MIPI_DSI_COMMAND_WRITEACK_ERR 0x87
#define MIPI_DSI_COMMAND_READACK 0x87
#define MIPI_DSI_DCS_LONG_READ_RESPONSE 0x1c
#define MIPI_DSI_DCS_SHORT_READ_1BYTE_RESPONSE 0x21
#define MIPI_DSI_DCS_SHORT_READ_2BYTES_RESPONSE 0x22
#define MIPI_DSI_GEN_SHORT_READ_1BYTE_RESPONSE 0x11
#define MIPI_DSI_GEN_SHORT_READ_2BYTES_RESPONSE 0x12
#define MIPI_DSI_GEN_LONG_READ_RESPONSE 0x1a

// mipi dsi cmd reg offset
#define MIPI_DSI_CMD_HEADER_REG			0x00000000
#define MIPI_DSI_CMD_PARAM0_REG			0x00000004
#define MIPI_DSI_CMD_PARAM4_REG			0x00000024
#define MIPI_DSI_CMD_LEGACY_PARAM_WORDS		4
#define MIPI_DSI_CMD_EXT_PARAM_WORDS		32
#define MIPI_DSI_CMD_LEGACY_PARAM_SIZE		(MIPI_DSI_CMD_LEGACY_PARAM_WORDS * 4)
#define MIPI_DSI_CMD_PARAM_REG(n)		(((n) < 4) ? \
						 (MIPI_DSI_CMD_PARAM0_REG + ((n) * 4)) : \
						 (MIPI_DSI_CMD_PARAM4_REG + (((n) - 4) * 4)))
#define MIPI_DSI_CMD_CTRL_REG			0x00000014
#define MIPI_DSI_RX_DATA_0			0x00000018
#define MIPI_DSI_RX_DATA_1			0x0000001C
#define MIPI_DSI_RX_DATA_2			0x00000020
#define MIPI_DSI_CMD_PARAM1_REG			MIPI_DSI_CMD_PARAM_REG(1)
#define MIPI_DSI_CMD_PARAM2_REG			MIPI_DSI_CMD_PARAM_REG(2)
#define MIPI_DSI_CMD_PARAM3_REG			MIPI_DSI_CMD_PARAM_REG(3)

typedef enum _AMBA_MIPI_DCS_CMD_e_ {
	/* User Command Set: 00h to AFh */
	AMBA_MIPI_DCS_CMD_NOP                       = 0x00,
	AMBA_MIPI_DCS_CMD_SOFT_RESET                = 0x01,
	AMBA_MIPI_DCS_CMD_GET_RED_CHANNEL           = 0x06,
	AMBA_MIPI_DCS_CMD_GET_GREEN_CHANNEL         = 0x07,
	AMBA_MIPI_DCS_CMD_GET_BLUE_CHANNEL          = 0x08,
	AMBA_MIPI_DCS_CMD_GET_POWER_MODE            = 0x0A,
	AMBA_MIPI_DCS_CMD_GET_ADDRESS_MODE          = 0x0B,
	AMBA_MIPI_DCS_CMD_GET_PIXEL_FORMAT          = 0x0C,
	AMBA_MIPI_DCS_CMD_GET_DISPLAY_MODE          = 0x0D,
	AMBA_MIPI_DCS_CMD_GET_SIGNAL_MODE           = 0x0E,
	AMBA_MIPI_DCS_CMD_GET_DIAGNOSTIC_RESULT     = 0x0F,
	AMBA_MIPI_DCS_CMD_ENTER_SLEEP_MODE          = 0x10,
	AMBA_MIPI_DCS_CMD_EXIT_SLEEP_MODE           = 0x11,
	AMBA_MIPI_DCS_CMD_ENTER_PARTIAL_MODE        = 0x12,
	AMBA_MIPI_DCS_CMD_ENTER_NORMAL_MODE         = 0x13,
	AMBA_MIPI_DCS_CMD_EXIT_INVERT_MODE          = 0x20,
	AMBA_MIPI_DCS_CMD_ENTER_INVERT_MODE         = 0x21,
	AMBA_MIPI_DCS_CMD_SET_GAMMA_CURVE           = 0x26,
	AMBA_MIPI_DCS_CMD_SET_DISPLAY_OFF           = 0x28,
	AMBA_MIPI_DCS_CMD_SET_DISPLAY_ON            = 0x29,
	AMBA_MIPI_DCS_CMD_SET_COLUMN_ADDRESS        = 0x2A,
	AMBA_MIPI_DCS_CMD_SET_PAGE_ADDRESS          = 0x2B,
	AMBA_MIPI_DCS_CMD_WRITE_MEMORY_START        = 0x2C,
	AMBA_MIPI_DCS_CMD_WRITE_LUT                 = 0x2D,
	AMBA_MIPI_DCS_CMD_READ_MEMORY_START         = 0x2E,
	AMBA_MIPI_DCS_CMD_SET_PARTIAL_ROWS          = 0x30,
	AMBA_MIPI_DCS_CMD_SET_PARTIAL_COLUMNS       = 0x31,
	AMBA_MIPI_DCS_CMD_SET_SCROLL_AREA           = 0x33,
	AMBA_MIPI_DCS_CMD_SET_TEAR_OFF              = 0x34,
	AMBA_MIPI_DCS_CMD_SET_TEAR_ON               = 0x35,
	AMBA_MIPI_DCS_CMD_SET_ADDRESS_MODE          = 0x36,
	AMBA_MIPI_DCS_CMD_SET_SCROLL_START          = 0x37,
	AMBA_MIPI_DCS_CMD_EXIT_IDLE_MODE            = 0x38,
	AMBA_MIPI_DCS_CMD_ENTER_IDLE_MODE           = 0x39,
	AMBA_MIPI_DCS_CMD_SET_PIXEL_FORMAT          = 0x3A,
	AMBA_MIPI_DCS_CMD_WRITE_MEMORY_CONTINUE     = 0x3C,
	AMBA_MIPI_DCS_CMD_READ_MEMORY_CONTINUE      = 0x3E,
	AMBA_MIPI_DCS_CMD_SET_TEAR_SCANLINE         = 0x44,
	AMBA_MIPI_DCS_CMD_GET_SCANLINE              = 0x45,
	AMBA_MIPI_DCS_CMD_SET_DISPLAY_BRIGHTNESS    = 0x51,
	AMBA_MIPI_DCS_CMD_GET_DISPLAY_BRIGHTNESS    = 0x52,
	AMBA_MIPI_DCS_CMD_WRITE_CTRL_DISPLAY        = 0x53,
	AMBA_MIPI_DCS_CMD_READ_DDB_START            = 0xA1,
	AMBA_MIPI_DCS_CMD_READ_DDB_CONTINUE         = 0xA8,

	/* Manufacturer Command Set: B0h to FFh */
} AMBA_MIPI_DCS_CMD_e;


typedef union {
	struct {
		u32 param_1 : 8;
		u32 param_0 : 8;
		u32 data_type : 6;
		u32 reserved : 10;
	} s;
	u32 data;
} mipi_dsi_cmd_short_pkt_header;

typedef union {
	struct {
		u32 wc_1 : 8;
		u32 wc_0 : 8;
		u32 data_type : 6;
		u32 reserved : 2;
		u32 param_0 : 8;
	} s;
	u32 data;
} mipi_dsi_cmd_long_pkt_header;

/* ========================================================================== */
static inline int amba_vout_pm(u32 pmval)
{
	return 0;
}

#endif

