/*
 * iav_v4l2_api.h
 *
 * History:
 *    2025/11/28 - [Long Li] created file
 *
 * Copyright (c) 2025 Ambarella, Inc.
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

#ifndef __IAV_V4L2_API_H__
#define __IAV_V4L2_API_H__

#include "vin_api.h"
#include "iav_vout_common.h"

int v4l2_vin_get_global_vin_info(struct vin_global_info *info);
struct vin_device *v4l2_vin_get_vdev(u32 vsrc_id);
int v4l2_vin_dev_get_video_info(struct vin_device *vdev, struct vindev_video_info *video_info);
int v4l2_vin_dev_set_video_mode(struct vin_device *vdev, struct vindev_mode *mode);
int v4l2_vin_dev_get_video_mode(struct vin_device *vdev, struct vindev_mode *mode);
int v4l2_vin_dev_set_dev_info(struct vin_device *vdev, struct vindev_devinfo *devinfo);
int v4l2_vin_dev_get_dev_info(struct vin_device *vdev, struct vindev_devinfo *devinfo);
int v4l2_vin_dev_get_frame_rate(struct vin_device *vdev, struct vindev_fps *vsrc_fps);
int v4l2_vin_dev_set_frame_rate(struct vin_device *vdev, struct vindev_fps *vsrc_fps);
int v4l2_vin_dev_get_shutter_time(struct vin_device *vdev, struct vindev_shutter *vsrc_shutter);
int v4l2_vin_dev_set_shutter_time(struct vin_device *vdev, struct vindev_shutter *vsrc_shutter);
int v4l2_vin_dev_get_agc_db(struct vin_device *vdev, struct vindev_agc *vsrc_agc);
int v4l2_vin_dev_set_agc_db(struct vin_device *vdev, struct vindev_agc *vsrc_agc);
int v4l2_vin_dev_set_agc_shutter(struct vin_device *vdev, struct vindev_agc_shutter *agc_shutter);
int v4l2_vin_set_global_vin_mode(struct vindev_mode_group *mode_group);
int v4l2_iav_get_iav_state(void);
int v4l2_iav_reset(void);
int v4l2_iav_goto_idle(struct iav_idle_params *idle_params);
int v4l2_iav_enable_preview(struct iav_preview_params *prev_params);
int v4l2_iav_get_vout_num(void);
int v4l2_vout_get_params(struct vout_params *params);
int v4l2_vout_set_params(struct vout_params *params);
int v4l2_vout_set_mode(struct voutdev_format *format);
int v4l2_vout_switch_video(struct vout_onoff *onoff);
int v4l2_vout_halt(int vout_id);
int v4l2_iav_g_system_resource(struct iav_system_resource *resource);
int v4l2_iav_s_system_resource(struct iav_system_resource *resource);
/**
 * Validate a staged system resource (encode_mode + fields) without applying.
 * Safe in any IAV state. Does not mutate the caller's resource.
 */
int v4l2_iav_check_system_resource(struct iav_system_resource *resource);
int v4l2_iav_g_chan_cfg(struct iav_chan_cfg *chan_cfg);
int v4l2_iav_s_chan_cfg(struct iav_chan_cfg *chan_cfg);
int v4l2_iav_g_warp_ctrl(struct iav_warp_ctrl *warp_ctrl_cfg);
int v4l2_iav_c_warp_ctrl(struct iav_warp_ctrl *warp_ctrl_cfg);
int v4l2_iav_a_warp_ctrl(struct iav_apply_flag *warp_apply);
int v4l2_iav_g_canvas_cfg(struct iav_canvas_cfg *canvas_cfg);
int v4l2_iav_s_canvas_cfg(struct iav_canvas_cfg *canvas_cfg);
int v4l2_iav_g_stream_resource(struct iav_stream_resource *stream_resource);
int v4l2_iav_s_stream_resource(struct iav_stream_resource *stream_resource);
int v4l2_iav_g_pyramid_cfg(struct iav_pyramid_cfg *cfg);
int v4l2_iav_s_pyramid_cfg(struct iav_pyramid_cfg *cfg);
int v4l2_iav_get_vproc(struct iav_video_proc *vproc);
int v4l2_iav_cfg_vproc(struct iav_video_proc *vproc);
int v4l2_iav_set_vcap_cfg(struct iav_vcap_cfg *cfg);
int v4l2_iav_query_info(struct iav_queryinfo *info);
int v4l2_iav_g_stream_cfg(struct iav_stream_cfg *cfg);
int v4l2_iav_s_stream_cfg(struct iav_stream_cfg *cfg);
int v4l2_iav_s_h264_cfg(struct iav_h26x_cfg *cfg);
int v4l2_iav_g_h264_cfg(struct iav_h26x_cfg *h264);
int v4l2_iav_g_h265_cfg(struct iav_h26x_cfg *h265);
int v4l2_iav_s_h265_cfg(struct iav_h26x_cfg *cfg);
int v4l2_iav_g_mjpeg_cfg(struct iav_mjpeg_cfg *mjpeg);
int v4l2_iav_s_mjpeg_cfg(struct iav_mjpeg_cfg *mjpeg);
int v4l2_iav_start_encode(unsigned long stream_map);
int v4l2_iav_stop_encode(unsigned long stream_map);
int v4l2_iav_abort_encode(unsigned long stream_map);
int v4l2_iav_g_query_mem(struct iav_querymem *query_mem_ptr);
int v4l2_iav_gdma_mem_copy(struct iav_gdma_copy *param);
int v4l2_iav_g_query_desc(struct file *iav_filp, struct iav_querydesc *desc);
int v4l2_iav_get_filp(struct file **iav_filp);
void v4l2_iav_release_filp(struct file *iav_filp);
int v4l2_iav_q_capability(struct iav_capability *param);

#endif /* __IAV_V4L2_API_H__ */
