/** helper.c
 *
 * Copyright (c) 2021 Leesoo Ahn <lsahn@ooseel.net>
 *
 * A set of helper components between ethernet driver and esBPF.
 */

#ifndef __ESBPF_HELPER_H__
#define __ESBPF_HELPER_H__

#include <linux/types.h>
#include <linux/spinlock.h>

struct esbpf_filter;

enum {
  ESBPF_OFF = 0,
  ESBPF_ON
};

struct esbpf_helper
{
  atomic_t rx_enable;
  struct esbpf_filter *rx_hooks;
  spinlock_t rx_hooks_lock;
};

static inline void
esbpf_helper_init(struct esbpf_helper *helper)
{
  helper->rx_enable = (atomic_t)ATOMIC_INIT(ESBPF_OFF);
  spin_lock_init(&helper->rx_hooks_lock);
}

#endif /* __ESBPF_HELPER_H__ */

