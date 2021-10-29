MODULE_NAME := hid-framework

obj-m := hid-framework.o
KVERSION := $(shell uname -r)
MODDESTDIR := /lib/modules/$(KVERSION)/kernel/drivers/hid/

all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

install:
	xz -kf9 hid-framework.ko
	install -p -m 644 $(MODULE_NAME).ko.xz $(MODDESTDIR)
	/sbin/depmod -a ${KVERSION}

uninstall:
	rm -f $(MODDESTDIR)/$(MODULE_NAME).ko.xz
	/sbin/depmod -a ${KVERSION}