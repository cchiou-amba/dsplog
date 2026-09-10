/* iav_fb_struct.h
 *
 * History:
 *    2023/05/10 - [JingYang Qiu] created file
 *
 * Copyright (c) 2018 Ambarella, Inc.
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

#ifndef __IAV_FB_STRUCT_H__
#define __IAV_FB_STRUCT_H__

enum iav_fb_bin_id {
	IAV_FB_VIN_VIDEO_FORMAT = 0,
	IAV_FB_VIN_DSP_CONFIG = 1,
	IAV_FB_ENC_CONFIG = 2,
	IAV_FB_SRCBUF_CONFIG = 3,
	IAV_FB_VOUT0_SETUP = 4,
	IAV_FB_VOUT1_SETUP = 5,
	IAV_FB_DSP_RSV_MEM = 6,

	IAV_FB_DATA_MAX_NUM,
	IAV_FB_DATA_FIRST = IAV_FB_VIN_VIDEO_FORMAT,
	IAV_FB_DATA_LAST = IAV_FB_DATA_MAX_NUM,
};

struct iav_fb_aeb {
	/* AWB */
	unsigned int r_gain;
	unsigned int b_gain;
	/* AE */
	unsigned int d_gain;
	u16 shutter_row;
	u16 shutter_row_short;
	u32 agc_index : 10;
	u32 reserve1: 6;
	u32 extra_gain : 16;
	u16 auto_knee;
	u16 reserved;
};

struct iav_fb_node {
	unsigned int offset;
	unsigned int size;
};

#define FB_MAGIC_NUM		(0x46425644)
struct iav_fb_hdr {  /* 128 * sizeof(u32)  */
	//W0
	unsigned int dsp_status;

	//W1
	unsigned int magic_num;

	//W2
	unsigned int header_size : 16;
	unsigned int reserved : 8;
	unsigned int active_vin_num : 4;
	unsigned int rtos_log_level : 4;

	//W3
	unsigned int light_value;

	//W4
	unsigned int bin_offset;

	//w5
	u32 chan_num : 8;					// total channel num
	u32 canvas_num : 8; 				// total canvas num
	u32 vinc_num : 8;					// total vinc num
	u32 vsrc_num : 8;					// total vsrc num

	//w6
	u32 max_stream_num : 8;					// max stream num
	u32 reserved0 : 24;

	//w7~11
	u32 stream_bitmap;
	u32 vinc_bitmap;
	u32 vsrc_bitmap;
	u32 chan_bitmap;
	u32 canvas_bitmap;

	//w12~35
	struct iav_fb_aeb pre_aeb[4];	// CONFIG_AMBARELLA_MAX_CHANNEL_NUM

	//w36~47
	u32 reserved1[12];


	//W48 ~ w48 + 63
	struct iav_fb_node bin[32];  //8 * 32

	//W112~ W112 + 15
	u32 reserve2[16];
}__attribute__((packed));


#endif	// __iav_fb_struct_h__

