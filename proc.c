/** proc.c
 *
 * Copyright (c) 2021 Leesoo Ahn <lsahn@ooseel.net>
 *
 * procfs class for esbpf under a ethernet driver.
 */

#include <linux/module.h>
#include <linux/seq_file.h>

#include "core.h"
#include "proc.h"
#include "helper.h"

static void *
filter_seq_start(struct seq_file *f, loff_t *pos)
{
	return NULL;
}

static void *
filter_seq_next(struct seq_file *f, void *v, loff_t *pos)
{
	return NULL;
}

static void
filter_seq_stop(struct seq_file *f, void *v)
{
	/* Nothing to do */
}

static int
filter_seq_show(struct seq_file *f, void *v)
{
	return 0;
}

static const struct seq_operations filter_seq_ops = {
	.start = filter_seq_start,
	.next = filter_seq_next,
	.stop = filter_seq_stop,
	.show = filter_seq_show,
};

static int rx_filter_proc_open(struct inode *inode,
                               struct file *file)
{
	return seq_open(file, &filter_seq_ops);
}

static ssize_t
rx_filter_proc_write(struct file *file,
                     const char *buffer,
                     size_t len,
                     loff_t *off)
{
  struct sock_fprog fprog;
  struct esbpf_helper *helper;
  struct esbpf_filter *new_filt, *old_filt;
  int ret = -EFAULT;

  if (len != sizeof(struct sock_fprog))
    return ret;

  if (copy_from_user(&fprog, buffer, sizeof(fprog)))
    return ret;

  ret = esbpf_create_filter(&new_filt, &fprog);
  if (ret)
    return ret;

  helper = pde_data(file_inode(file));

  /* replace */
  spin_lock(&helper->rx_filter_lock);
  old_filt = rcu_dereference_protected(helper->rx_filter,
                  lockdep_is_held(&helper->rx_filter_lock));
  rcu_assign_pointer(helper->rx_filter, new_filt);
  spin_unlock(&helper->rx_filter_lock);

  if (old_filt)
    esbpf_release_filter(old_filt);

  return len;
}

static const struct proc_ops rx_filter_fops = {
  .proc_open     = rx_filter_proc_open,
  .proc_write    = rx_filter_proc_write,
  .proc_lseek    = seq_lseek,
  .proc_release  = seq_release,
};

static int
rx_show_match_count(struct seq_file *s, void *unused)
{
  struct esbpf_helper *helper = s->private;
  int match_cnt = -1;

  if (helper) {
    struct esbpf_filter *filt; 
    rcu_read_lock();
    filt = rcu_dereference(helper->rx_filter);
    if (filt)
      match_cnt = atomic_read(&filt->match);
    rcu_read_unlock();
  }

  seq_printf(s, "%d", match_cnt);

  return 0;
}

static struct proc_dir_entry *esb_root;

int
esbpf_proc_init(struct proc_dir_entry *root,
                struct esbpf_helper *hdata)
{
  int ret = -EPERM;

  esb_root = proc_mkdir("esbpf", root);
  if (!esb_root)
    return ret;

  if (!proc_create_data("rx_filter", 0644, esb_root,
                        &rx_filter_fops, hdata))
    goto err;

  if (!proc_create_single_data("rx_match_count", 0, esb_root,
                               rx_show_match_count, hdata))
    goto err;

  return 0;

err:
  proc_remove(esb_root);
  esb_root = NULL;
  return ret;
}

void
esbpf_proc_exit(void)
{
  if (esb_root) {
    proc_remove(esb_root);
    esb_root = NULL;
  }
}

