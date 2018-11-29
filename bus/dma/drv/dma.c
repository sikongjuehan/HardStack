/*
 * Copyright (C) 2017 buddy.zhang@aliyun.com
 *
 * dma device driver demo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 */
#include <linux/types.h>
#include <linux/string.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/dmaengine.h>
 
#include <linux/kernel.h>
#include <linux/uaccess.h>


#define DRIVER_NAME			"axidma"
#define AXIDMA_IOC_MAGIC		'A'
#define AXIDMA_IOCGETCHN		_IO(AXIDMA_IOC_MAGIC, 0)
#define AXIDMA_IOCCFGANDSTART		_IO(AXIDMA_IOC_MAGIC, 1)
#define AXIDMA_IOCGETSTATUS		_IO(AXIDMA_IOC_MAGIC, 2)
#define AXIDMA_IOCRELEASECHN		_IO(AXIDMA_IOC_MAGIC, 3)
 
#define AXI_DMA_MAX_CHANS		8
 
#define DMA_CHN_UNUSED			0
#define DMA_CHN_USED			1
 
struct axidma_chncfg {
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int len;
	unsigned char chn_num;
	unsigned char status;
	unsigned char reserve[2];
	unsigned int reserve2;
};
 
struct axidma_chns {
	struct dma_chan *dma_chan;
	unsigned char used;
#define DMA_STATUS_UNFINISHED	0
#define DMA_STATUS_FINISHED		1
	unsigned char status;
	unsigned char reserve[2];
};
 
struct axidma_chns channels[AXI_DMA_MAX_CHANS];
 
static int axidma_open(struct inode *inode, struct file *file)
{
    printk("Open: do nothing\n");
    return 0;
}
 
static int axidma_release(struct inode *inode, struct file *file)
{
    printk("Release: do nothing\n");
    return 0;
}
 
static ssize_t axidma_write(struct file *file, const char __user *data, size_t len, loff_t *ppos)
{
    printk("Write: do nothing\n");
    return 0;
}
 
static void dma_complete_func(void *status)
{
    *(char *)status = DMA_STATUS_FINISHED;
    printk("dma_complete!\n");
}
 
static long axidma_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct dma_device *dma_dev;
    struct dma_async_tx_descriptor *tx = NULL;
    dma_cap_mask_t mask;
    dma_cookie_t cookie;
    enum dma_ctrl_flags flags;
 
    struct axidma_chncfg chncfg;
    int ret = -1;
    int i;
 
    memset(&chncfg, 0, sizeof(struct axidma_chncfg));
    switch(cmd) {
    case AXIDMA_IOCGETCHN:
        for(i = 0; i < AXI_DMA_MAX_CHANS; i++) {
            if(DMA_CHN_UNUSED == channels[i].used)
                break;				
        }
        if (AXI_DMA_MAX_CHANS == i) {
            printk("Get dma chn failed, because no idle channel\n");
            goto error;
        } else {
            channels[i].used   = DMA_CHN_USED;
            channels[i].status = DMA_STATUS_UNFINISHED;
            chncfg.chn_num     = i;
            chncfg.status      = DMA_STATUS_UNFINISHED;
        }
 
        dma_cap_zero(mask);
        dma_cap_set(DMA_MEMCPY, mask); 
        channels[i].dma_chan = dma_request_channel(mask, NULL, NULL);
        if(!channels[i].dma_chan) {
            printk("dma request channel failed\n");
            channels[i].used = DMA_CHN_UNUSED;
            goto error;
        }
 
        ret = copy_to_user((void __user *)arg, &chncfg, 
                                    sizeof(struct axidma_chncfg));
        if(ret) {
            printk("Copy to user failed\n");
            goto error;
        }
        break;
    case AXIDMA_IOCCFGANDSTART:
        ret = copy_from_user(&chncfg, (void __user *)arg, 
                       sizeof(struct axidma_chncfg));
        if(ret) {
            printk("Copy from user failed\n");
            goto error;
        }
 
        if((chncfg.chn_num >= AXI_DMA_MAX_CHANS) || 
                      (!channels[chncfg.chn_num].dma_chan)) {
            printk("chn_num[%d] is invalid\n", chncfg.chn_num);
            goto error;
        }
 
        dma_dev = channels[chncfg.chn_num].dma_chan->device;
        flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
        tx = dma_dev->device_prep_dma_memcpy(channels[chncfg.chn_num].dma_chan,
                       chncfg.dst_addr, chncfg.src_addr, chncfg.len, flags);
        if(!tx) {
            printk("Failed to prepare DMA memcpy\n");
            goto error;
        }
        tx->callback = dma_complete_func;
        channels[chncfg.chn_num].status = DMA_STATUS_UNFINISHED;
        tx->callback_param = &channels[chncfg.chn_num].status;
        cookie =  tx->tx_submit(tx);
        if(dma_submit_error(cookie)) {
            printk("Failed to dma tx_submit\n");
            goto error;
        }
        dma_async_issue_pending(channels[chncfg.chn_num].dma_chan);
        break;
    case AXIDMA_IOCGETSTATUS:
        ret = copy_from_user(&chncfg, (void __user *)arg, 
                             sizeof(struct axidma_chncfg));
        if(ret) {
            printk("Copy from user failed\n");
            goto error;
        }
        if(chncfg.chn_num >= AXI_DMA_MAX_CHANS) {
            printk("chn_num[%d] is invalid\n", chncfg.chn_num);
            goto error;
        }
        chncfg.status = channels[chncfg.chn_num].status;
        ret = copy_to_user((void __user *)arg, &chncfg, 
                         sizeof(struct axidma_chncfg));
        if(ret) {
            printk("Copy to user failed\n");
            goto error;
        }
        break;
    case AXIDMA_IOCRELEASECHN:
        ret = copy_from_user(&chncfg, (void __user *)arg, 
                  sizeof(struct axidma_chncfg));
        if(ret) {
            printk("Copy from user failed\n");
            goto error;
        }
        if((chncfg.chn_num >= AXI_DMA_MAX_CHANS) || 
                      (!channels[chncfg.chn_num].dma_chan)) {
            printk("chn_num[%d] is invalid\n", chncfg.chn_num);
            goto error;
        }
 
        dma_release_channel(channels[chncfg.chn_num].dma_chan);
        channels[chncfg.chn_num].used = DMA_CHN_UNUSED;
        channels[chncfg.chn_num].status = DMA_STATUS_UNFINISHED;
        break;
    default:
        printk("Don't support cmd [%d]\n", cmd);
        break;
    }
    return 0;
error:
    return -EFAULT;
}
 
/*
 *    Kernel Interfaces
 */
 
static struct file_operations axidma_fops = {
    .owner          = THIS_MODULE,
    .llseek         = no_llseek,
    .write          = axidma_write,
    .unlocked_ioctl = axidma_unlocked_ioctl,
    .open           = axidma_open,
    .release        = axidma_release,
};
 
static struct miscdevice axidma_miscdev = {
    .minor       = MISC_DYNAMIC_MINOR,
    .name        = DRIVER_NAME,
    .fops        = &axidma_fops,
};
 
static int __init axidma_init(void)
{
    int ret = 0;
 
    ret = misc_register(&axidma_miscdev);
    if(ret) {
        printk (KERN_ERR "cannot register miscdev (err=%d)\n", ret);
		return ret;
    }
    memset(&channels, 0, sizeof(channels));
 
    return 0;
}
device_initcall(axidma_init);
