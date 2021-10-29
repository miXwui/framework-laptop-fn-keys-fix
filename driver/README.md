# Kernel driver to fix Brightness Up / Down getting stuck for the Framework laptop

**Note: wasn't tested with secure boot enabled so, if on, this probably won't work without extra installation steps.**

Thanks to these resources:

- <https://www.nirenjan.com/2020/linux-hid-driver/>
- <https://github.com/nirenjan/libx52/tree/master/kernel_module>
- <https://github.com/jaytohe/aerofnkeys/>
- <https://github.com/torvalds/linux/tree/master/drivers/hid>

Custom HID Quirks Driver that replaces for the hid-generic driver the Framework laptop uses.

## tldr speedrun  

1. Clone this project
2. `cd` folder and run `sudo mv 70-keyboard-framework-fix.hwdb  /etc/udev/hwdb.d`
3. Run `sudo systemd-hwdb update && sudo udevadm trigger --verbose --sysname-match="event*"` (weird glitch where this may need to be run more than once)
4. Verify changes set with:
   `udevadm info  /sys/class/input/event2`

   ```console
   E: KEYBOARD_KEY_3b=!F1
   E: KEYBOARD_KEY_3c=!F2
   E: KEYBOARD_KEY_3d=!F3
   ... (rest of KEYBOARD_KEY_*=*)
   E: KEYBOARD_KEY_d2=!insert
   E: KEYBOARD_KEY_d3=!delete
   E: KEYBOARD_KEY_ed=!media
   ````


5. `cd` into it
6. run `make`
7. Install either:
   - only for the current kernel:  
   [Loading with `modprobe`](#loading-with-modprobe)  
   - to persist across kernel changes:  
   [Persisting Across Kernel Updates with DKMS](#persisting-across-kernel-updates-with-dkms)

## Details

By default, `sudo evtest`:

```console
No device specified, trying to scan all of /dev/input/event*
Available devices:
/dev/input/event0:  Lid Switch
/dev/input/event1:  Power Button
/dev/input/event2:  AT Translated Set 2 keyboard
/dev/input/event3:  FRMW0001:00 32AC:0006 Wireless Radio Control
/dev/input/event4:  FRMW0001:00 32AC:0006 Consumer Control
...
```

In `journalctl -b`

```console
input: FRMW0001:00 32AC:0006 Wireless Radio Control as /devices/pci0000:00/0000:00:15.1/i2c_designware.1/i2c-1/i2c-FRMW0001:00/0018:32AC:0006.0002/input/input3
input: FRMW0001:00 32AC:0006 Consumer Control as /devices/pci0000:00/0000:00:15.1/i2c_designware.1/i2c-1/i2c-FRMW0001:00/0018:32AC:0006.0002/input/input4
hid-generic 0018:32AC:0006.0002: input,hidraw1: I2C HID v1.00 Device [FRMW0001:00 32AC:0006] on i2c-FRMW0001:00
```

- `/dev/input/event3:  FRMW0001:00 32AC:0006 Wireless Radio Control` handles the brightness down/up keys
- `/dev/input/event4:  FRMW0001:00 32AC:0006 Consumer Control` handles the airplane mode key

With this driver, `/dev/input/event3` and `/dev/input/event4` will be combined into:

```console
/dev/input/event3:  FRMW0001:00 32AC:0006
```

This quirks driver intercepts the brightness down / up keys and then sends them with a release event so the key won't stick. However, this sacrifices the ability to handle a long-press.

All other events, such as the airplane mode key, go through normally.

## Building & Loading

1. Install the Linux headers for the currently running kernel.  

   e.g. on Fedora, make sure `kernel-devel` and `kernel-headers` are installed:  
   `sudo dnf in kernel-devel kernel-headers`

2. `cd` into the directory and run `make`. This will build the module from source. After successful build, you can load with `insmod` or `modprobe`. `modprobe` is recommended over `insmod`, and can be setup to persist across reboots. However, if you want to install and automatically remake/persist after kernel update, skip to [Persisting Across Kernel Updates with DKMS](#persisting-across-kernel-updates-with-dkms).

### Loading with With `insmod` (not recommended)

(in the built project directory)  

- To load: `sudo insmod hid-framework.ko`

   With a recent enough kernel, the driver should switch automatically. and the brightness keys should now not stick. However, this isn't permanent and will unload after boot.

- To unload: `sudo rmmod hid_framework`

### Loading with `modprobe`

(in the built project directory)  

Note: this will need to be reinstalled with a kernel update. See next section if you want to persist across kernels.

1. Install into the current /lib/modules/(kernel-version):  
   `sudo make install`
2. Load module manually:  
   `sudo modprobe hid_framework`
3. Brigtness keys should now not stick
4. To persist across boots, create this file:  
   `/etc/modules-load.d/hid-framework.conf` with a single line:

   ```text
   hid_framework
   ```

5. Reboot, then run `lsmod | grep framework` to check it worked:  

   ```console
   hid_framework  ...
   ```

Uninstall:  

1. `sudo modprobe -r hid_framework`  
2. `cd` into this project directory and run `sudo make uninstall`  
3. `sudo rm /etc/modules-load.d/hid-framework.conf`  

## Persisting Across Kernel Updates with DKMS

Using DKMS

1. Install DKMS  
`sudo dnf in dkms` (Fedora)

2. Create a directory `/usr/src/<module>-<module-version>/`  
   `sudo mkdir /usr/src/hid-framework-1.0`

3. `cd` into directory and clone this project (without the parent folder) into the directory:  
   `git clone origin-url .`

4. Add the `<module>/<module-version>` to the DKMS tree:  
   `sudo dkms add -m hid-framework -v 1.0`

   ```console
   Creating symlink /var/lib/dkms/hid-framework/1.0/source -> /usr/src/hid-framework-1.0
   ```

5. Compile the module, under DKMS control:
  `sudo dkms build -m hid-framework -v 1.0`

   ```console
   Kernel preparation unnecessary for this  kernel. Skipping...
 
   Building module:
   cleaning build area...
   make -j8 KERNELRELEASE=5.14.14-200.fc34. x86_64 all KVERSION=5.14.14-200.fc34. x86_64...
   cleaning build area...
   ```

6. Install the module, again under DKMS control:
   `sudo dkms install -m hid-framework -v 1.0`

   ```console
   hid-framework.ko.xz:
   Running module version sanity    ck.
   - Original module
      - No original module    sts within this kernel
   - Installation
      - Installing to /lib/   ules/5.14.14-200.fc34.  _64/extra/
   depmod....
   ```
  
7. Check status `dmks status`

   ```console
   hid-framework/1.0, 5.14.14-200.fc34.x86_64, x86_64: installed
   ```

Brightness keys should now not stick, and the driver should persist across kernel updates.

To uninstall:  
`sudo dkms uninstall -m hid-framework -v 1.0`

```console
Module hid-framework-1.0 for kernel 5. 14.14-200.fc34.x86_64 (x86_64).
Before uninstall, this module version  was ACTIVE on this kernel.

hid-framework.ko.xz:
- Uninstallation
   - Deleting from: /lib/modules/5.14. 14-200.fc34.x86_64/extra/
- Original module
   - No original module was found for  this module on this kernel.
   - Use the dkms install command to   reinstall any previous module   version.
```

`dkms status`

```console
hid-framework/1.0, 5.14.14-200.fc34.x86_64, x86_64: built
```

To remove:
`sudo dkms remove -m hid-framework -v 1.0`

```console
Module hid-framework-1.0 for kernel 5.14.14-200.fc34.x86_64 (x86_64).
This module version was INACTIVE for this kernel.
depmod....
Deleting module hid-framework-1.0 completely from the DKMS tree.
```

To remove for all kernels:  
`sudo dkms remove hid-framework/1.0 --all`

`hid-framework/version` will be gone from  `dkms status`.

Cleanup folder:  
`sudo rm /usr/src/hid-framework-1.0/`

Adapted from:  
<https://wiki.kubuntu.org/Kernel/Dev/DKMSPackaging>
<https://wiki.centos.org/HowTos/BuildingKernelModules#Building_a_kernel_module_using_Dynamic_Kernel_Module_Support_.28DKMS.29>
<https://docs.01.org/clearlinux/latest/guides/kernel/kernel-modules-dkms.html#build-kernel-module-without-an-existing-dkms-conf>

## Signing Kernel Modules for Secure Boot

I haven't tried this, feel free to open a issue/pull request if you get it to work:  
<https://docs.fedoraproject.org/en-US/fedora/rawhide/system-administrators-guide/kernel-module-driver-configuration/Working_with_Kernel_Modules/#sect-signing-kernel-modules-for-secure-boot>

## Troubleshooting

If any of these errors happen:

```console
sudo insmod hid-framework.ko
insmod: ERROR: could not insert module hid-framework.ko: Invalid module format
```

```console
sudo modprobe -v hid_framework
insmod /lib/modules/5.14.14-200.fc34.x86_64/kernel/drivers/hid/hid-framework.ko.xz 
modprobe: ERROR: could not insert 'hid_framework': Exec format error
```

dmesg logs:

```console
module: x86/modules: Skipping invalid relocation target, existing value is nonzero for type 1
```

Ensure you're building to the same running kernel version, can check with `modinfo`. If building for a another kernel version, start from scratch with a `make clean` / `make uninstall`. In my case though, seems like kernel-devel / kernel-headers corrupted, which I fixed with a reinstall: `sudo dnf reinstall kernel-devel kernel-headers`.
