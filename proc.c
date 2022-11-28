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

#include "proc-rx_enable.c"
static const struct proc_ops rx_enable_fops = {
  .proc_open     = rx_enable_proc_open,
  .proc_write    = rx_enable_proc_write,
  .proc_lseek    = seq_lseek,
  .proc_release  = seq_release,
};

#include "proc-rx_filter.c"
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

  if (!proc_create_data("rx_enable", 0644, esb_root,
                        &rx_enable_fops, hdata))
    goto err;

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

