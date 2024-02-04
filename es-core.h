/** es-core.h
 *
 * Copyright (c) 2021-2024 Leesoo Ahn <lsahn@ooseel.net>
 */
#ifndef __ESBPF_CORE_H__
#define __ESBPF_CORE_H__

#include <linux/atomic.h>
#include <linux/compat.h>
#include <linux/workqueue.h>

#include <uapi/linux/filter.h>

struct sk_buff;
struct sock;

struct esbpf_filter {
    atomic_t        match; /* match count for filter */
    unsigned int    nr_blks;	/* Number of filter blocks */
    struct rcu_head rcu;
    unsigned int    (*bpf_func)(const struct sk_buff *skb,
                                const struct sock_filter *filter);
    union {
        struct sock_filter  insns[0];
        struct work_struct	work;
    };
};

static inline unsigned int
esbpf_get_filter_size(unsigned int proglen)
{
    return max(sizeof(struct esbpf_filter),
               offsetof(struct esbpf_filter, insns[proglen]));
}

static inline void
esbpf_release_filter(struct esbpf_filter *self)
{
    kfree(self);
}

static inline void
esbpf_reclaim_filter(struct rcu_head *rp)
{
    struct esbpf_filter *filt = container_of(rp, struct esbpf_filter, rcu);

    esbpf_release_filter(filt);
}

int esbpf_exec_filter(struct esbpf_filter *self, struct sk_buff *skb);
int esbpf_create_filter(struct esbpf_filter **self_out,
                        struct sock_fprog *fprog);
void esbpf_decode_filter(struct sock_filter *filt, struct sock_filter *to);

static inline int
esbpf_bpf_tell_extensions(void)
{
    return SKF_AD_MAX;
}

#endif /* __ESBPF_CORE_H__ */
