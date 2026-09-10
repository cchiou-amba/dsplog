# SPDX-License-Identifier: GPL-2.0
#
# Ambarella DSPLOG Out-of-Tree Kernel Driver Makefile

KDIR ?= $(CURDIR)/../eve-kernel
ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

KBUILD_EXTRA_SYMBOLS ?= $(wildcard $(CURDIR)/../ambvideo/dsp_v6/Module.symvers)

.PHONY: all modules clean install

all modules:
	$(MAKE) -C $(KDIR) M=$(CURDIR) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
		KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" \
		modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

install:
	$(MAKE) -C $(KDIR) M=$(CURDIR) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
		modules_install
