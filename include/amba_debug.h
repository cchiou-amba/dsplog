/*
 * amba_debug.h
 *
 * History:
 *    2008/04/10 - [Anthony Ginger] Create
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

#ifndef __AMBA_DEBUG_H
#define __AMBA_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#define AMBA_DEBUG_IOC_MAGIC		'd'

#define AMBA_DEBUG_IOC_GET_DEBUG_FLAG		_IOR(AMBA_DEBUG_IOC_MAGIC, 1, int *)
#define AMBA_DEBUG_IOC_SET_DEBUG_FLAG		_IOW(AMBA_DEBUG_IOC_MAGIC, 1, int *)

struct amba_vin_test_gpio {
	u32 id;
	u32 data;
};
#define AMBA_DEBUG_IOC_GET_GPIO			_IOR(AMBA_DEBUG_IOC_MAGIC, 203, struct amba_vin_test_gpio *)
#define AMBA_DEBUG_IOC_SET_GPIO			_IOW(AMBA_DEBUG_IOC_MAGIC, 203, struct amba_vin_test_gpio *)

#ifdef __cplusplus
}
#endif

#endif	//AMBA_DEBUG_IOC_MAGIC
