/*
 * dsplog_api.h
 *
 * History:
 *    2016/09/23 - [Ming Wen] Create
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


#ifndef __DSPLOG_API_H__
#define __DSPLOG_API_H__

struct dsplog_ops {
	int (*suspend)(void);
	int (*resume)(void);
	int (*reset_capture)(u8);
};

int iav_register_dsplog_ops(struct dsplog_ops *ops);

#endif	// __DSPLOG_API_H__

