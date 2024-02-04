/** es-proc.c
 *
 * Copyright (c) 2021-2024 Leesoo Ahn <lsahn@ooseel.net>
 */

#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

#include "es-ctrl.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#ifndef HAVE_PROC_OPS
#define proc_open open
#define proc_write write
#define proc_read read
#define proc_lseek llseek
#define proc_release release
#endif

/* -- common ops -- */
static int common_proc_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);

    return 0;
}

static int common_proc_close(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);

    return 0;
}

/* -- rx_enable proc ops -- */
static ssize_t rx_enable_proc_read(struct file *file,
        char __user *buffer, size_t len, loff_t *off)
{
    struct esbpf_ctrl *ctrl;
    char buf[1];
    int val;

    if (*off) {
        *off = 0;
        return 0;
    }

    ctrl = pde_data(file_inode(file));
    if (!ctrl)
        return -ENOMEM;

    val = atomic_read(&ctrl->rx_enable);
    buf[0] = val + '0';

    len = min(len, sizeof(buf));
    if (copy_to_user(buffer, buf, len))
        return -EFAULT;
    *off += len;

    return len;
}

static ssize_t rx_enable_proc_write(struct file *file,
        const char __user *buffer, size_t len, loff_t *off)
{
    struct esbpf_ctrl *ctrl;
    char buf[1];
    int new_val, old_val, expected;

    if (copy_from_user(buf, buffer, sizeof(buf)))
        return -EFAULT;

    switch (buf[0]) {
    case '0': case '1':
        new_val = !!(buf[0] - '0');
        break;
    default:
        return -EINVAL;
    }

    ctrl = pde_data(file_inode(file));
    if (!ctrl)
        return -ENOMEM;

    old_val = atomic_read(&ctrl->rx_enable);
    do {
        expected = old_val;
        old_val = atomic_cmpxchg(&ctrl->rx_enable, expected, new_val);
    } while (old_val != expected);

    return len;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops
#else
static const struct file_operations
#endif
rx_enable_fops = {
    .proc_read     = rx_enable_proc_read,
    .proc_write    = rx_enable_proc_write,
    .proc_open     = common_proc_open,
    .proc_release  = common_proc_close,
};


/* -- rx_hooks proc ops -- */
static ssize_t rx_hooks_proc_read(struct file *file,
        char __user *buffer, size_t len, loff_t *off)
{
    return 0;
}

static ssize_t rx_hooks_proc_write(struct file *file,
        const char __user *buffer, size_t len, loff_t *off)
{
    int ret;
    struct sock_fprog fprog;
    struct esbpf_ctrl *ctrl;
    struct esbpf_filter *new_filt, *old_filt;

    if (len != sizeof(struct sock_fprog))
        return -EINVAL;

    if (copy_from_user(&fprog, buffer, sizeof(fprog)))
        return -EFAULT;

    ret = esbpf_create_filter(&new_filt, &fprog);
    if (ret)
        return ret;

    ctrl = pde_data(file_inode(file));
    if (!ctrl)
        return -ENOMEM;

    /* RCU replacement */
    old_filt = esbpf_ctrl_replace_hook(ctrl, new_filt);
    if (old_filt)
        call_rcu(&old_filt->rcu, esbpf_reclaim_filter);

    return len;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops
#else
static const struct file_operations
#endif
rx_hooks_fops = {
    .proc_read     = rx_hooks_proc_read,
    .proc_write    = rx_hooks_proc_write,
    .proc_open     = common_proc_open,
    .proc_release  = common_proc_close,
};

#if 0
static int rx_show_match_count(struct seq_file *s, void *unused)
{
    struct esbpf_ctrl *ctrl = s->private;

    /* TODO */
    (void*)ctrl;

    return 0;
}
#endif

int esbpf_proc_init(struct esbpf_ctrl *ctrl, const char *proc_name)
{
    struct proc_dir_entry *proc_root;
    int ret = -EPERM;

	if (!proc_name)
		return -EINVAL;

    proc_root = proc_mkdir(proc_name, NULL);
    if (!proc_root)
        return ret;

    if (!proc_create_data("rx_enable", 0644, proc_root,
                &rx_enable_fops, ctrl))
        goto err;

    if (!proc_create_data("rx_hooks", 0644, proc_root,
                &rx_hooks_fops, ctrl))
        goto err;

#if 0
    if (!proc_create_single_data("rx_match_count", 0444, proc_root,
                rx_show_match_count, ctrl))
        goto err;
#endif

    ctrl->proc_root = proc_root;

    return 0;

err:
    proc_remove(proc_root);
    return ret;
}

void esbpf_proc_release(struct esbpf_ctrl *ctrl)
{
    if (ctrl->proc_root) {
        proc_remove(ctrl->proc_root);
        ctrl->proc_root = NULL;
    }
}
