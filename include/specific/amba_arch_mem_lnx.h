/*
 * amba_arch_mem_lnx.h
 *
 * History:
 *	2015/07/21 - [Jian Tang] created file
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

#ifndef __AMBA_ARCH_MEM_LNX_H__
#define __AMBA_ARCH_MEM_LNX_H__

#include <linux/mm.h>
#include <linux/io.h>

#ifdef _LP64
#define amba_ioremap(addr, size)	ioremap((resource_size_t)addr, size)
// NOTE: for any reserved memory remapping, pls use this kernel API.
#define amba_memremap(addr, size)	memremap((resource_size_t)addr, size, MEMREMAP_WC)
#else
#define amba_ioremap(addr, size)	ioremap(addr, size)
#define amba_memremap(addr, size)	ioremap(addr, size)
#endif

#define ONE_SECONDS			(msecs_to_jiffies(1000))
#define TWO_SECONDS			(msecs_to_jiffies(2000))
#define FIVE_SECONDS		(msecs_to_jiffies(5000))
#define TEN_SECONDS			(msecs_to_jiffies(10000))
#define FIFTEEN_SECONDS		(msecs_to_jiffies(15000))

#endif	// __AMBA_ARCH_MEM_LNX_H__

