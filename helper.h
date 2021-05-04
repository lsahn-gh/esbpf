/** helper.c
 *
 * Copyright (c) 2021 Leesoo Ahn <lsahn@ooseel.net>
 *
 * A set of helper components between ethernet driver and esBPF.
 */

#ifndef __ESBPF_HELPER_H__
#define __ESBPF_HELPER_H__

#include <linux/spinlock.h>

struct esbpf_filter;

struct esbpf_helper
{
  struct esbpf_filter *rx_filter;
  spinlock_t rx_filter_lock;
};

static inline void
esbpf_helper_init(struct esbpf_helper *helper)
{
  spin_lock_init(&helper->rx_filter_lock);
}

#endif /* __ESBPF_HELPER_H__ */

