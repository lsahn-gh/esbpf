/** es-ctrl.h
 *
 * Copyright (c) 2021-2024 Leesoo Ahn <lsahn@ooseel.net>
 */

#ifndef __ESBPF_CTRL_H__
#define __ESBPF_CTRL_H__

#include <linux/types.h>
#include <linux/spinlock.h>

#include "es-core.h"

struct esbpf_filter;

enum {
    ESBPF_OFF = 0,
    ESBPF_ON
};

struct esbpf_ctrl {
    atomic_t rx_enable;
    struct proc_dir_entry *proc_root;
    spinlock_t rcu_lock;
    struct esbpf_filter *rx_hooks;
};

static inline void esbpf_ctrl_init(struct esbpf_ctrl *self)
{
    self->rx_enable = (atomic_t)ATOMIC_INIT(ESBPF_OFF);
    self->proc_root = NULL;
    spin_lock_init(&self->rcu_lock);
    self->rx_hooks = NULL;
}

static inline struct esbpf_filter *
esbpf_ctrl_replace_hook(struct esbpf_ctrl *self, struct esbpf_filter *new)
{
    struct esbpf_filter *old;

    spin_lock(&self->rcu_lock);
    old = rcu_replace_pointer(self->rx_hooks, new,
                    lockdep_is_held(&self->rcu_lock));
    spin_unlock(&self->rcu_lock);

    return old;
}

static inline void esbpf_ctrl_release(struct esbpf_ctrl *self)
{
    int new_val = 0, old_val;
    struct esbpf_filter *new_filt = NULL, *old_filt;

    old_val = atomic_read(&self->rx_enable);
    atomic_cmpxchg_acquire(&self->rx_enable, old_val, new_val);

    old_filt = esbpf_ctrl_replace_hook(self, new_filt);
    if (old_filt)
        call_rcu(&old_filt->rcu, esbpf_reclaim_filter);
}

/* -- proc APIs -- */
int esbpf_proc_init(struct esbpf_ctrl *ctrl, const char *proc_name);
void esbpf_proc_release(struct esbpf_ctrl *ctrl);

#endif /* __ESBPF_CTRL_H__ */
