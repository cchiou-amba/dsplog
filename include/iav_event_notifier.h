/*
 * iav_event_notifier.h
 *
 * History:
 *	2021/02/03 - [Xiaopan Zhan] Created file
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

#ifndef __IAV_EVENT_NOTIFIER_H__
#define __IAV_EVENT_NOTIFIER_H__

/* Event info type for @IAV_EVENT_NOTIFIER_EIS_RAW_INFO. */
struct iav_event_raw_info {
	u32 hw_pts;
	u32 sw_pts;
	u32 vsrc_map;
	u32 chan_map;
	u8 vinc_id;
	u8 reserved2[3];
	u32 sof_pts;
	u32 eof_pts;
	u32 vin_fps_hp;
	u32 reserved3[6]; /* Align structure to 64B. */
	u64 mono_pts;
};

typedef enum {
	IAV_EVENT_NOTIFIER_EIS_RAW_INFO = 0, /*!< 0, event notifier type for reporting EIS raw info. */
	IAV_EVENT_NOTIFIER_NUM = 1, /*!< 1, total number of event notifier. */
	IAV_EVENT_NOTIFIER_FIRST = IAV_EVENT_NOTIFIER_EIS_RAW_INFO, /*!< 0, IAV_EVENT_NOTIFIER_EIS_RAW_INFO. */
	IAV_EVENT_NOTIFIER_LAST = IAV_EVENT_NOTIFIER_NUM, /*!< 1, IAV_EVENT_NOTIFIER_NUM. */
} IAV_EVENT_NOTIFIER_TYPE;

/**
 * @brief: Common event callback for report different event info to private drivers use.
 *     1: IAV_EVENT_NOTIFIER_EIS_RAW_INFO: called in ISR bottom half, cannot be blocked.
 * @param info: event info for private drivers, should be converted to different event info type accordingly.
 *     1: IAV_EVENT_NOTIFIER_EIS_RAW_INFO: (struct iav_event_raw_info *)
 * @param private_data: for use by the owner of the @sa iav_event_cb.
 * @return: 0:success, failure: negative error code.
 */
typedef int (*iav_event_cb)(void *info, void *private_data);

/* For private drivers use only */
struct iav_event_notifier_obj {
	iav_event_cb callback;
	void *private_data;
};

extern struct iav_event_notifier_obj event_notifier[IAV_EVENT_NOTIFIER_NUM];

/* APIs exported for other kernel modules */
int iav_register_notifier(u8 notifier_type, iav_event_cb callback, void *private_data);
int iav_unregister_notifier(u8 notifier_type);

/* APIs for IAV driver only */
struct iav_event_notifier_obj *iav_get_event_notifier(u8 notifier_type);

#endif	// __IAV_EVENT_NOTIFIER_H__

