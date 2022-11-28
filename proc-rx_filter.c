/** proc-rx_filter.c
 *
 * Copyright (c) 2021-2022 Leesoo Ahn <lsahn@ooseel.net>
 *
 * included at proc.c
 */

static void *
rx_filter_start(struct seq_file *f, loff_t *pos)
{
  return NULL;
}

static void *
rx_filter_next(struct seq_file *f, void *v, loff_t *pos)
{
  return NULL;
}

static void
rx_filter_stop(struct seq_file *f, void *v)
{
  /* Nothing to do */
}

static int
rx_filter_show(struct seq_file *f, void *v)
{
  return 0;
}

static const struct seq_operations rx_filter_ops = {
  .start = rx_filter_start,
  .next = rx_filter_next,
  .stop = rx_filter_stop,
  .show = rx_filter_show,
};

static int rx_filter_proc_open(struct inode *inode,
                               struct file *file)
{
  return seq_open(file, &rx_filter_ops);
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

