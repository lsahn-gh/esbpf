/** proc-rx_enable.c
 *
 * Copyright (c) 2021-2022 Leesoo Ahn <lsahn@ooseel.net>
 *
 * included at proc.c
 */

static void *
rx_enable_start(struct seq_file *f, loff_t *pos)
{
  return NULL;
}

static void *
rx_enable_next(struct seq_file *f, void *v, loff_t *pos)
{
  return NULL;
}

static void
rx_enable_stop(struct seq_file *f, void *v)
{
  /* Nothing to do */
}

static int
rx_enable_show(struct seq_file *f, void *v)
{
  return 0;
}

static const struct seq_operations rx_enable_ops = {
  .start = rx_enable_start,
  .next = rx_enable_next,
  .stop = rx_enable_stop,
  .show = rx_enable_show,
};

static int rx_enable_proc_open(struct inode *inode,
                               struct file *file)
{
  return seq_open(file, &rx_enable_ops);
}

static ssize_t
rx_enable_proc_write(struct file *file,
                     const char *buffer,
                     size_t len,
                     loff_t *off)
{
  struct esbpf_helper *priv;
  char buf[1];
  int new_val, old_val;

  if (copy_from_user(buf, buffer, sizeof(buf)))
    goto err;

  switch (buf[0]) {
    case '0': case '1':
      new_val = buf[0] - '0';
      break;
    default:
      goto err;
  }

  priv = pde_data(file_inode(file));
  if (!priv)
    return -ENOMEM;

  old_val = atomic_read_acquire(&priv->rx_enable);
  if (old_val != new_val)
    atomic_xchg(&priv->rx_enable, new_val);

  return len;

err:
  return -EINVAL;
}

