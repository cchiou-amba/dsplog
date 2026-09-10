/*
 * ambnl_api.h
 *
 * History:
 *    2018/07/06 - [Zhaoyang Chen] Create
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

#ifndef __AMBNL_API_H
#define __AMBNL_API_H

struct ambnl_wq {
	u32 condition;
	u32 reserved;
	struct list_head node;
	wait_queue_head_t *wq;
};

int ambnl_register_defined_module(enum AMBA_NL_MODULE module);
int ambnl_unregister_defined_module(enum AMBA_NL_MODULE module);
int ambnl_register_module(u8 *module_name, u8 *module_id);
int ambnl_unregister_module(u8 module_id);
void ambnl_clear_req_info(int port, u32 msg_id);
int is_ambnl_ready(int port);
int ambnl_send_msg(int port, u32 msg_id, u16 payload_size, void *payload);
struct ambnl_wq *ambnl_get_ambnl_wq(void);
int ambnl_send_req(int port, u32 msg_id, u16 payload_size, void *payload, struct ambnl_wq *nl_wq);
int ambnl_report_error_msg(u32 msg_id);

#endif //__AMBNL_API_H

