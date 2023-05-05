#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PCI driver for MAC address");

#define MYDRIVER_IOC_MAGIC 'm'
#define MYDRIVER_GET_MAC_ADDR _IOR(MYDRIVER_IOC_MAGIC, 1, char*)

struct mydriver_priv {
    struct pci_dev *pdev;
    struct net_device *netdev;
};

static dev_t dev_num;
static struct cdev *mydev;
static struct pci_device_id mydriver_pci_tbl[] = {
    { PCI_DEVICE(0x1af4, 0x1041) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, mydriver_pci_tbl);

static int mydriver_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    u16 vendor, device;
    u8 mac_addr[6];
    struct net_device *net_dev;
    struct mydriver_priv *priv;
    printk(KERN_INFO "mydriver: probe\n");
    vendor = pdev->vendor;
    device = pdev->device;
    printk(KERN_INFO "mydriver: vendor=0x%04x, device=0x%04x\n", vendor, device);
    err = pci_enable_device(pdev);
    if (err < 0) {
        printk(KERN_ERR "mydriver: Failed to enable device\n");
        return err;
    }
    pci_read_config_byte(pdev, PCI_CLASS_NETWORK_ETHERNET, &pdev->class);
    pci_read_config_byte(pdev, PCI_REVISION_ID, &pdev->revision);
    net_dev = pci_alloc_net(pdev, sizeof(struct mydriver_priv));
    if (!net_dev) {
        printk(KERN_ERR "mydriver: Failed to allocate net device\n");
        pci_disable_device(pdev);
        return -ENOMEM;
    }
    priv = netdev_priv(net_dev);
    priv->pdev = pdev;
    priv->netdev = net_dev;
    pci_read_config_byte(pdev, PCI_CLASS_NETWORK_ETHERNET, &pdev->class);
    pci_read_config_byte(pdev, PCI_REVISION_ID, &pdev->revision);
    err = register_netdev(net_dev);
    if (err < 0) {
        printk(KERN_ERR "mydriver: Failed to register net device\n");
        pci_free_net(net_dev);
        return err;
    }
    printk(KERN_INFO "mydriver: MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", 
           net_dev->dev_addr[0], net_dev->dev_addr[1], net_dev->dev_addr[2], 
           net_dev->dev_addr[3], net_dev->dev_addr[4], net_dev->dev_addr[5]);
    return 0;
}

static void mydriver_remove(struct pci_dev *pdev)
{
    struct mydriver_priv *priv = pci_get_drvdata(pdev);
    struct net_device *netdev = priv->netdev;
    printk(KERN_INFO "mydriver: remove\n");
    unregister_netdev(netdev);
    pci_free_net(pdev);
    pci_disable_device(pdev);
}

static struct pci_driver mydriver_pci_driver = {
    .name = "mydriver",
    .id_table = mydriver_pci_tbl,
    .probe = mydriver_probe,
    .remove = mydriver_remove,
};

static int mydriver_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydriver: open\n");
    return 0;
}

static int mydriver_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydriver: release\n");
    return 0;
}

static long mydriver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mydriver_priv *priv = container_of(file->private_data, struct mydriver_priv, netdev->priv);
    char *mac_addr;
    if (cmd != MYDRIVER_GET_MAC_ADDR) {
        return -ENOTTY;
    }
    mac_addr = kmalloc(6, GFP_KERNEL);
    if (!mac_addr) {
        return -ENOMEM;
    }
    memcpy(mac_addr, priv->netdev->dev_addr, 6);
    if (copy_to_user((void *)arg, mac_addr, 6)) {
        kfree(mac_addr);
        return -EFAULT;
    }
    kfree(mac_addr);
    return 0;
}

static const struct file_operations mydriver_fops = {
    .owner = THIS_MODULE,
    .open = mydriver_open,
    .release = mydriver_release,
    .unlocked_ioctl = mydriver_ioctl,
};

static int __init mydriver_init(void)
{
    int err;
    printk(KERN_INFO "mydriver: Initializing\n");
    err = alloc_chrdev_region(&dev_num, 0, 1, "mydriver");
    if (err < 0) {
        printk(KERN_ERR "mydriver: Failed to allocate device number\n");
        return err;
    }
    mydev = cdev_alloc();
    if (!mydev) {
        printk(KERN_ERR "mydriver: Failed to allocate cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }
    mydev->owner = THIS_MODULE;
    mydev->ops = &mydriver_fops;
    err = cdev_add(mydev, dev_num, 1);
    if (err < 0) {
        printk(KERN_ERR "mydriver: Failed to add cdev\n");
        cdev_del(mydev);
        unregister_chrdev_region(dev_num, 1);
        return err;
    }
    err = pci_register_driver(&mydriver_pci_driver);
    if (err < 0) {
        printk(KERN_ERR "mydriver: Failed to register PCI driver\n");
        cdev_del(mydev);
        unregister_chrdev_region(dev_num, 1);
        return err;
    }
    return 0;
}

static void __exit mydriver_exit(void)
{
    printk(KERN_INFO "mydriver: Exiting\n");
    pci_unregister_driver(&mydriver_pci_driver);
    cdev_del(mydev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(mydriver_init);
module_exit(mydriver_exit);

