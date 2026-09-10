/*
 * dsplog_drv.c
 *
 * History:
 *	2013/09/30 - [Louis Sun] create new dsplog driver
 *
 * Copyright (C) 2016  Ambarella, Inc.
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


#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/i2c.h>
#include <iav_devnum.h>
#include <dsplog_api.h>

#include "iav_utils.h"

#include "dsplog_priv.h"

/* Now separate dsplog driver from dsp driver.
    DSP driver is kernel mode only driver used by IAV driver.
    DSP log driver supports DSP logging and it is not called by dsp driver.
    DSP log driver is optional and only used for debugging
   */


MODULE_AUTHOR("Louis Sun <lysun@ambarella.com>");
MODULE_DESCRIPTION("Ambarella dsplog driver");
MODULE_LICENSE("GPL v2");

amba_dsplog_controller_t    g_dsplog_controller;

static const char *amba_dsplog_name = "dsplog";
static struct cdev amba_dsplog_cdev;
static struct class *amba_dsplog_class;
int clean_dsplog_memory = 1; /* Set it into 0 in fastboot case */
module_param(clean_dsplog_memory, int, 0644);

int enable_debug_msg_output; /* 0 disable, 1 enable */
module_param(enable_debug_msg_output, int, 0644);

static ssize_t amba_dsplog_read(struct file *filp,
	char __user *buffer, size_t count, loff_t *offp)
{
	int readout_size = 0;
	amba_dsplog_context_t *context = NULL;

	context = filp->private_data;
	if (!context || !context->dsplog_priv_data)
		return -EINVAL;
	// Since the instance for each dsp has been set to single.
	dsplog_lock();
	readout_size = dsplog_read(buffer, count, context);
	dsplog_unlock();

	return readout_size;
}

DEFINE_MUTEX(amba_dsplog_mutex);

void dsplog_lock(void)
{
	mutex_lock(&amba_dsplog_mutex);
}

void dsplog_unlock(void)
{
	mutex_unlock(&amba_dsplog_mutex);
}

static int dsplog_set_dsp_core_id(struct file *filp, unsigned long arg)
{
	int rval = 0;
	u32 dsp_id = ~0;
	void *prv_data = NULL;
	amba_dsplog_context_t *context = filp->private_data;

	if (copy_from_user((void *) &dsp_id, (const void *) arg, sizeof(u32))) {
		rval = -EFAULT;
		goto DSPLOG_SET_DSP_CORE_ID_EXIT;
	}
	prv_data = dsplog_register_log_cap(filp, dsp_id);
	if (!prv_data) {
		rval = -EFAULT;
		goto DSPLOG_SET_DSP_CORE_ID_EXIT;
	}

	context->dsplog_priv_data = prv_data;
	if (enable_debug_msg_output) {
		DRV_PRINT(KERN_DEBUG " DSP log private data for dspId[%d] assigned successfully.\n",
			dsp_id);
	}

DSPLOG_SET_DSP_CORE_ID_EXIT:
	return rval;
}

static long amba_dsplog_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rval = 0;
	amba_dsplog_context_t *context = NULL;
	struct dsplog_mem_info mem_info = {0};

	dsplog_lock();
	context = filp->private_data;
	switch (cmd) {
	case AMBA_IOC_DSPLOG_START_CAPUTRE:
		rval = dsplog_start_cap(filp, &context->dsplog_priv_data);
		break;

	case AMBA_IOC_DSPLOG_STOP_CAPTURE:
		rval = dsplog_stop_cap(context->dsplog_priv_data);
		break;
	case AMBA_IOC_DSPLOG_SET_DSP_CORE:
		rval = dsplog_set_dsp_core_id(filp, arg);
		break;
	case AMBA_IOC_DSPLOG_SET_LOG_LEVEL:
		rval = dsplog_set_level((int)arg);
		break;

	case AMBA_IOC_DSPLOG_GET_LOG_LEVEL: {
		int level = 0;

		rval = dsplog_get_level(&level);
		if (rval == 0 && copy_to_user((void __user *)arg, &level, sizeof(level)))
			rval = -EFAULT;
		break;
	}

	case AMBA_IOC_DSPLOG_PARSE_LOG:
		rval = dsplog_parse((int)arg);
		break;

	case AMBA_IOC_DSPLOG_GET_MEMORY_INFO:
		rval = dsplog_get_memory_info(context->dsplog_priv_data, &mem_info);
		if (rval == 0 && copy_to_user((void __user *)arg, &mem_info, sizeof(mem_info)))
			rval = -EFAULT;
		break;

	case AMBA_IOC_DSPLOG_DUMP_MEMORY_SNAPSHOT:
		context->dump_mem_snapshot = (u8)arg;
		break;

	default:
		rval = -ENOIOCTLCMD;
		break;
	}
	dsplog_unlock();

	return rval;
}

static int amba_dsplog_open(struct inode *inode, struct file *filp)
{
	amba_dsplog_context_t *context;

	context = kzalloc(sizeof(amba_dsplog_context_t), GFP_KERNEL);
	if (context == NULL) {
		return -ENOMEM;
	}
	context->file = filp;
	context->mutex = &amba_dsplog_mutex;
	context->controller = &g_dsplog_controller;
	filp->private_data = context;

	return 0;
}

static int amba_dsplog_release(struct inode *inode, struct file *filp)
{
	dsplog_reset_log_cap_state(filp);
	kfree(filp->private_data);
	return 0;
}

static struct dsplog_ops amba_dsplog_ops = {
#ifdef CONFIG_PM
	.suspend = dsplog_suspend,
	.resume = dsplog_resume,
#endif
#ifdef AMBA_DSP_ARCH_V6
	.reset_capture = dsplog_reset_cap,
#endif
};

static struct file_operations amba_dsplog_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = amba_dsplog_ioctl,
	.open = amba_dsplog_open,
	.release = amba_dsplog_release,
	.read = amba_dsplog_read,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
static char *amba_dsplog_devnode(struct device *dev, umode_t *mode)
#else
static char *amba_dsplog_devnode(const struct device *dev, umode_t *mode)
#endif
{
	if (!mode) {
		return NULL;
	}
	*mode = 0666;

	return NULL;
}

static int __init amba_dsplog_create_dev(int numdev,
	const char *name, struct cdev *cdev, struct file_operations *fops)
{
	int rval = 0;
	struct device *amba_dsplog_device = NULL;
	dev_t amba_dsplog_dev_id;

	rval = alloc_chrdev_region(&amba_dsplog_dev_id, 0, numdev, name);
	if (rval) {
		DRV_PRINT(KERN_ERR "failed to get dev region for %s.\n", name);
		return rval;
	}

	cdev_init(cdev, fops);
	cdev->owner = THIS_MODULE;
	rval = cdev_add(cdev, amba_dsplog_dev_id, 1);
	if (rval) {
		DRV_PRINT(KERN_ERR "cdev_add failed for %s, error = %d.\n", name, rval);
		unregister_chrdev_region(amba_dsplog_dev_id, 1);
		goto AMAB_DSPLOG_CREATE_DEV_EXIT;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
	amba_dsplog_class = class_create(THIS_MODULE, name);
#else
	amba_dsplog_class = class_create(name);
#endif
	if (IS_ERR(amba_dsplog_class)) {
		rval = PTR_ERR(amba_dsplog_class);
		DRV_PRINT(KERN_ERR "class_create failed for %s: %d\n", name, rval);
		cdev_del(cdev);
		unregister_chrdev_region(cdev->dev, 1);
		goto AMAB_DSPLOG_CREATE_DEV_EXIT;
	}

	amba_dsplog_class->devnode = amba_dsplog_devnode;
	amba_dsplog_device = device_create(amba_dsplog_class, NULL, cdev->dev, NULL, "%s", name);
	if (IS_ERR(amba_dsplog_device)) {
		rval = PTR_ERR(amba_dsplog_device);
		DRV_PRINT(KERN_ERR "device_create failed for %s: %d\n", name, rval);
		class_destroy(amba_dsplog_class);
		cdev_del(cdev);
		unregister_chrdev_region(cdev->dev, 1);
		goto AMAB_DSPLOG_CREATE_DEV_EXIT;
	}

	DRV_PRINT(KERN_DEBUG "%s dev init done, dev_id = %d:%d.\n",
		name, MAJOR(cdev->dev), MINOR(cdev->dev));

AMAB_DSPLOG_CREATE_DEV_EXIT:
	return rval;
}

static void amba_dsplog_remove_dev(void)
{
	device_destroy(amba_dsplog_class, amba_dsplog_cdev.dev);
	class_destroy(amba_dsplog_class);
	cdev_del(&amba_dsplog_cdev);
	unregister_chrdev_region(amba_dsplog_cdev.dev, 1);

	return;
}

static int __init amba_dsplog_init(void)
{
	int rval = 0;

	if (amba_dsplog_create_dev(1, amba_dsplog_name,
		&amba_dsplog_cdev, &amba_dsplog_fops) < 0) {
		rval = -EFAULT;
		goto AMBA_DSPLOG_INIT_EXIT;
	}

	if (dsplog_init() < 0) {
		iav_error("dsplog_init fails!\n");
		rval = -EFAULT;
		goto AMBA_DSPLOG_INIT_ERR;
	}

	if (iav_register_dsplog_ops(&amba_dsplog_ops) < 0) {
		iav_error("iav_register_dsplog_ops fails!\n");
		rval = -EFAULT;
		goto AMBA_DSPLOG_INIT_ERR;
	}

	goto AMBA_DSPLOG_INIT_EXIT;

AMBA_DSPLOG_INIT_ERR:
	amba_dsplog_remove_dev();
AMBA_DSPLOG_INIT_EXIT:
	return rval;
}

static void __exit amba_dsplog_exit(void)
{
	dsplog_deinit(&g_dsplog_controller);
	amba_dsplog_remove_dev();
}

module_init(amba_dsplog_init);
module_exit(amba_dsplog_exit);
