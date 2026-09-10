/*
 * led_api.h
 *
 * History:
 *    2022/11/08 - [Jian Cai] Create
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


#ifndef __LED_API_H__
#define __LED_API_H__
#include"iav_led_common.h"

typedef struct led_device {
	u32 ldev_id;

	const char *name;

	struct list_head list;
	struct led_ops *ops;
	struct vin_controller *vinc;

	void *privdata;
} led_t;

struct led_ops {
	int (*led_trigger)(led_t *led_data, u32 led_id);
	int (*led_enable)(led_t *led_data);
	int (*led_set_cfg)(led_t *led_data, struct leddev_cfg *led_param);
	int (*led_get_cfg)(led_t *led_data, struct leddev_cfg *led_param);
	int (*led_get_info)(struct leddev_info *led_info);
	int (*led_read)(led_t *led_data, struct leddev_reg *led_reg);
	int (*led_write)(led_t *led_data, struct leddev_reg *led_reg);
};

#endif

