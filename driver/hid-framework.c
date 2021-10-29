/*
 * HID driver for Framework Function (Fn) Keys (FRMW0001:00 32AC:0006)
 * 
 * Thanks to these resources:
 * - https://www.nirenjan.com/2020/linux-hid-driver/
 * - https://github.com/nirenjan/libx52/tree/master/kernel_module
 * - https://github.com/jaytohe/aerofnkeys/
 * - https://github.com/torvalds/linux/tree/master/drivers/hid
 * 
 * Sticking keys fixed:
 *  - Brightness Down
 *  - Brightness Up
 * 
 * Supported devices:
 *  - Framework Laptop
 */

#include <linux/module.h>
#include <linux/hid.h>
#include <linux/input.h>

#define VENDOR_ID_FRAMEWORK 0x32ac
#define DEVICE_ID_FRAMEWORK_KEYBOARD_I2C_KEYBOARD 0x0006

#define SEND_KEY(c, x) send_key(((report->field)[0])->hidinput->input, (c), (x));

static void send_key(struct input_dev *inputd, unsigned int keycode, unsigned int hexcode)
{
    //  We simulate an instaneous key press and release at the cost of the ability to handle long-press of a key.
    input_event(inputd, EV_MSC, MSC_SCAN, hexcode);
    input_event(inputd, EV_KEY, keycode, 1);
    input_sync(inputd);

    input_event(inputd, EV_MSC, MSC_SCAN, hexcode);
    input_event(inputd, EV_KEY, keycode, 0);
    input_sync(inputd);
}

static int framework_raw_event(struct hid_device *hdev,
                               struct hid_report *report,
                               u8 *data,
                               int size)
{
    if (size == 3)
    {
        switch (data[1])
        {
        case 0x70:
            SEND_KEY(KEY_BRIGHTNESSDOWN, 0xc0070u);
            return -1;
        case 0x6f:
            SEND_KEY(KEY_BRIGHTNESSUP, 0xc006fu);
            return -1;
        }
    }

    return 0;
}

static const struct hid_device_id framework_devices[] = {
    {HID_I2C_DEVICE(VENDOR_ID_FRAMEWORK, DEVICE_ID_FRAMEWORK_KEYBOARD_I2C_KEYBOARD)},
    {}};

MODULE_DEVICE_TABLE(hid, framework_devices);

static struct hid_driver framework_driver = {
    .name = "framework",
    .id_table = framework_devices,
    .raw_event = framework_raw_event,
};

module_hid_driver(framework_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Michael Wu");
MODULE_DESCRIPTION("HID driver for Framework laptop FRMW0001:00 32AC:0006");
