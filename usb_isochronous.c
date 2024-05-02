#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

static int usb_isoc_probe(struct usb_interface *interface,
const struct usb_device_id *id)
{
    printk(KERN_INFO "USB isochronous driver example: probe\n");
    return 0;
}

static void usb_isoc_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "USB isochronous driver example: disconnect\n");
}

static struct usb_device_id usb_isoc_table[] = {
    { USB_INTERFACE_INFO(USB_CLASS_AUDIO, 1, 0) },
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usb_isoc_table);

static struct usb_driver usb_isoc_driver = {
    .name = "usb-isoc-example",
    .probe = usb_isoc_probe,
    .disconnect = usb_isoc_disconnect,
    .id_table = usb_isoc_table,
};

static int __init usb_isoc_init(void)
{
    int result;
    result = usb_register(&usb_isoc_driver);
    if (result)
        printk(KERN_ERR "USB isochronous driver example: register failed\n");
    else
        printk(KERN_INFO "USB isochronous driver example: registered\n");
    return result;
}

static void __exit usb_isoc_exit(void)
{
    usb_deregister(&usb_isoc_driver);
    printk(KERN_INFO "USB isochronous driver example: deregistered\n");
}

module_init(usb_isoc_init);
module_exit(usb_isoc_exit);

MODULE_LICENSE("GPL");
