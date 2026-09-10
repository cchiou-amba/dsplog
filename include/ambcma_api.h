/*
 * ambcma_api.h
 *
 * History:
 *	2018/03/07 - [Ming Wen] created file
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


#ifndef _AMBCMA_API_H
#define _AMBCMA_API_H

#include <basetypes.h>
#include <linux/of_reserved_mem.h>

#include <linux/version.h>
#include <linux/module.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
MODULE_IMPORT_NS(DMA_BUF);
#endif

#define AMBCMA_NAME_LEN						(32)
#define AMBCMA_MAX_CMA_BLOCK_SIZE			(0x80000000)
#define AMBCMA_MAX_SUB_BLOCK_NUM			(2)
#define AMBCMA_MAX_SUB_BLOCK_SIZE			(0x80000000)

/* For most mmbs, their default sizes are not cared outside.
   So we choose all mmb default sizes in ambcma driver. */
#define AMBCMA_DEFAULT_MMB_SIZE				(0xFFFFFFFF)
#define AMBCMA_INVALID_BUF_ID				(0xFFFFFFFF)
#define AMBCMA_DEFAULT_MMB_ATTR				(0xFFFFFFFF)

#define ANONYMOUS_MMB_TAG					(0xFF)
#define ANONYMOUS_MMB_NAME					"ANONYMOUS_MMB"
#define CUSTOM_MMB_NAME						"CUSTOM_MMB"
#define generate_anon_buf_id(mmb_id)		(((mmb_id) << 8) | ANONYMOUS_MMB_TAG)
#define is_anon_mmb(buf_id)					(((buf_id) & 0xFF) == ANONYMOUS_MMB_TAG)
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
#define CMA_POOL_MEM_ALIGN					(1 << ((MAX_ORDER + 1) + 10))
#else
#define CMA_POOL_MEM_ALIGN					(1 << ((MAX_PAGE_ORDER + 1) + 10))
#endif

/* MMB attributes */
typedef enum {
	AMBCMA_MMB_USR_CANNOT_FREE      = BIT(0),	/* cannot be freed by user space */
	AMBCMA_MMB_USR_CANNOT_MAP       = BIT(1),	/* cannot be mapped by user space */
	AMBCMA_MMB_CANNOT_IN_PREALLOC   = BIT(2),	/* cannot be merged in IAV_PART_PREALLOC */
	AMBCMA_MMB_CACHE_ENABLED        = BIT(3),	/* enable cache */
	AMBCMA_MMB_BIND_DMA_BUF_OBJ     = BIT(4),	/* bind dma-buf:fd */
	AMBCMA_MMB_FREE_BY_DMA_BUF      = BIT(5),	/* be freed automatically when dma-buf:fd is closed */
	AMBCMA_MMB_ACCESS_BY_ADV        = BIT(6),	/* accessible by ARM, DSP and VP */
	AMBCMA_MMB_ACCESS_BY_AD         = BIT(7),	/* accessible by ARM and DSP */
	AMBCMA_MMB_DIRECT_IO            = BIT(8),	/* accessible by ARM and DSP, access as direct io region */
	AMBCMA_MMB_CROSS_4GB            = BIT(9),	/* cross 4GB */
} AMBCMA_MMB_ATTRIBUTE;

struct ambcma_driver_version {
	char description[AMBCMA_NAME_LEN];	/* driver description */
	int major;	/* major version */
	int minor;	/* minor version */
	int patch;	/* patch version */
	u32 mod_time;	/* last modification time of the driver */
};

struct release_data {
	void *arg0;
	void *arg1;
	void *arg2;
};

struct amb_bind_dmabuf {
	struct ambcma_mmb *mmb;
	struct device *ambcma_dev;
	size_t size;
	u32 offset;
	u32 reserved0;
	void (*release_cb)(struct release_data *release_data);
	struct release_data release_data;
};

struct sub_block_info {
	u32 block_num : 3;
	u32 reserved : 29;
	size_t size[AMBCMA_MAX_SUB_BLOCK_NUM];
	void *virt_addr[AMBCMA_MAX_SUB_BLOCK_NUM];
	phys_addr_t phys_addr[AMBCMA_MAX_SUB_BLOCK_NUM];
};

struct ambcma_mmb {
	u32 id;
	u32 buf_id;
	char name[AMBCMA_NAME_LEN];
	struct list_head list;
	struct list_head usr_list;

	phys_addr_t phys_addr;
	void *virt_addr;
	size_t size;
	u16 attr;
	u16 is_sub_mmb : 1; // only used for sub mmb of prealloc mmb
	u16 use_multi_blocks  : 1; // only used for mmb size > 2GB
	u16 reserved : 14;

	u32 user_map_ref_cnt;

	struct sub_block_info sub_blocks; // only valid when use_multi_blocks is 1
	struct list_head sub_mmb_list; // only used for prealloc mmb
	struct list_head free_sub_mmb_list; // only used for prealloc mmb
	size_t free_size; // only used for prealloc mmb

	struct list_head dmabuf_obj_list;
	struct ambarella_cma *cma; /* the cma pool this mmb belongs to */
	struct reserved_mem rmem;
};

enum cma_pool_num {
	IAV_CMA_POOL = 0,
	CV_CMA_POOL = 1,
	IAV_CMA_POOL_SHARED = 2,
	CV_CMA_POOL_SHARED = 3,
	CMA_POOL_NUM,
};

enum cma_type_num {
	IAV_CMA_PRIVATE = 0,
	IAV_CMA_SHARE = 1,
	IAV_CMA_TYPE_NUM = 2,
};

struct cma_pool_info {
	phys_addr_t start_addr[CMA_POOL_NUM];
	size_t size[CMA_POOL_NUM];
};

struct ambcma_mmb *ambarella_cma_alloc(size_t size, u32 buf_id, u32 attribute);
struct ambcma_mmb *ambarella_anon_cma_alloc(size_t size, u32 attribute, void *filp);
int ambarella_cma_clear_mmb(void *filp);
int ambarella_cma_register_anon_mmb(u32 buf_id, void *filp);
int ambarella_cma_unregister_anon_mmb(u32 buf_id, void *filp);
/*
 * Sometimes, user app just maps a sub region of mmb. So phys_addr and
 * map_size are used to assign the sub region.
 */
int ambarella_cma_map(struct vm_area_struct *vma, u32 buf_id,
	phys_addr_t phys_addr, size_t map_size, void *filp);

void ambarella_get_mc_mask_buf_info(u32 *pitch, u32 *height);
u32 ambarella_get_mv_stream_num(void);
void ambarella_get_max_vin_win(u32 *width, u32 *height);
u64 ambarella_get_img_rsv_buf_size(void);

int ambarella_anon_cma_alloc_fd(size_t size, u32 attribute);
int ambarella_bind_dma_buf(u32 buf_id, size_t offset, size_t size,
	void (*release_cb)(struct release_data *release_data), struct release_data *release_data);
int ambarella_cma_free(u32 buf_id);
struct ambcma_mmb *ambarella_query_mmb(u32 buf_id);
u32 ambarella_query_buf_id(phys_addr_t phys_addr);
size_t ambarella_query_mmb_default_size(u32 buf_id);

int ambarella_cma_invalidate_cache(u32 buf_id, size_t offset, size_t size);
int ambarella_cma_check_buf_boundary(u32 buf_id, size_t offset, size_t size);
int ambarella_cma_clean_cache(u32 buf_id, size_t offset, size_t size);

unsigned long amba_dsp_to_phys(unsigned long addr);
unsigned long amba_phys_to_dsp(unsigned long addr);
u32 ambarella_get_mv_stream_num(void);
bool ambarella_ucode_buf_compact_enabled(void);

#endif	// _AMBCMA_API_H

