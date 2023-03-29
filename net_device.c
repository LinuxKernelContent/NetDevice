#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *netdev;

static int my_netdev_open(struct net_device *dev)
{
    printk(KERN_INFO "my_netdev: device opened\n");
    netif_start_queue(dev);
    return 0;
}

static int my_netdev_release(struct net_device *dev)
{
    printk(KERN_INFO "my_netdev: device released\n");
    netif_stop_queue(dev);
    return 0;
}

static netdev_tx_t my_netdev_xmit(struct sk_buff *skb, struct net_device *dev)
{
    printk(KERN_INFO "my_netdev: packet transmitted\n");
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops my_netdev_ops = {
    .ndo_open = my_netdev_open,
    .ndo_stop = my_netdev_release,
    .ndo_start_xmit = my_netdev_xmit,
};

static int __init my_netdev_init(void)
{
    netdev = alloc_etherdev(0);
    if (!netdev) {
        printk(KERN_ERR "my_netdev: failed to allocate etherdev\n");
        return -ENOMEM;
    }

    netdev->netdev_ops = &my_netdev_ops;
    strcpy(netdev->name, "my_netdev");

    if (register_netdev(netdev)) {
        printk(KERN_ERR "my_netdev: failed to register netdev\n");
        free_netdev(netdev);
        return -EINVAL;
    }

    printk(KERN_INFO "my_netdev: driver loaded\n");
    return 0;
}

static void __exit my_netdev_exit(void)
{
    unregister_netdev(netdev);
    free_netdev(netdev);
    printk(KERN_INFO "my_netdev: driver unloaded\n");
}

module_init(my_netdev_init);
module_exit(my_netdev_exit);
MODULE_AUTHOR("Guy Davidi");
MODULE_LICENSE("GPL");
