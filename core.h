/** core.h
 *
 * Linux Socket Filter Data Structures
 */
#ifndef __ESBPF_CORE_H__
#define __ESBPF_CORE_H__

#include <linux/atomic.h>
#include <linux/compat.h>
#include <linux/workqueue.h>

#include <uapi/linux/filter.h>

struct sk_buff;
struct sock;

struct esbpf_filter
{
  atomic_t        refcnt;
  atomic_t        match; /* match count for filter */
  unsigned int    len;	/* Number of filter blocks */
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

void esbpf_release_filter_rcu(struct rcu_head *rcu);

static inline void
esbpf_release_filter_raw(struct esbpf_filter *self)
{
  call_rcu(&self->rcu, esbpf_release_filter_rcu);
}

static inline void
esbpf_release_filter(struct esbpf_filter *self)
{
  if (atomic_dec_and_test(&self->refcnt))
    esbpf_release_filter_raw(self);
}

int esbpf_run_filter(struct esbpf_filter *self,
                     struct sk_buff *skb);
int esbpf_create_filter(struct esbpf_filter **self_out,
                        struct sock_fprog *fprog);
void esbpf_destroy_unattached_filter(struct esbpf_filter *self);
void esbpf_decode_filter(struct sock_filter *filt,
                         struct sock_filter *to);

#define SK_RUN_FILTER(FILTER, SKB) \
  (*FILTER->bpf_func)(SKB, FILTER->insns)

static inline int
esbpf_bpf_tell_extensions(void)
{
  return SKF_AD_MAX;
}

enum {
  BPF_S_RET_K = 1,
  BPF_S_RET_A,
  BPF_S_ALU_ADD_K,
  BPF_S_ALU_ADD_X,
  BPF_S_ALU_SUB_K,
  BPF_S_ALU_SUB_X,
  BPF_S_ALU_MUL_K,
  BPF_S_ALU_MUL_X,
  BPF_S_ALU_DIV_X,
  BPF_S_ALU_MOD_K,
  BPF_S_ALU_MOD_X,
  BPF_S_ALU_AND_K,
  BPF_S_ALU_AND_X,
  BPF_S_ALU_OR_K,
  BPF_S_ALU_OR_X,
  BPF_S_ALU_XOR_K,
  BPF_S_ALU_XOR_X,
  BPF_S_ALU_LSH_K,
  BPF_S_ALU_LSH_X,
  BPF_S_ALU_RSH_K,
  BPF_S_ALU_RSH_X,
  BPF_S_ALU_NEG,
  BPF_S_LD_W_ABS,
  BPF_S_LD_H_ABS,
  BPF_S_LD_B_ABS,
  BPF_S_LD_W_LEN,
  BPF_S_LD_W_IND,
  BPF_S_LD_H_IND,
  BPF_S_LD_B_IND,
  BPF_S_LD_IMM,
  BPF_S_LDX_W_LEN,
  BPF_S_LDX_B_MSH,
  BPF_S_LDX_IMM,
  BPF_S_MISC_TAX,
  BPF_S_MISC_TXA,
  BPF_S_ALU_DIV_K,
  BPF_S_LD_MEM,
  BPF_S_LDX_MEM,
  BPF_S_ST,
  BPF_S_STX,
  BPF_S_JMP_JA,
  BPF_S_JMP_JEQ_K,
  BPF_S_JMP_JEQ_X,
  BPF_S_JMP_JGE_K,
  BPF_S_JMP_JGE_X,
  BPF_S_JMP_JGT_K,
  BPF_S_JMP_JGT_X,
  BPF_S_JMP_JSET_K,
  BPF_S_JMP_JSET_X,
  /* Ancillary data */
  BPF_S_ANC_PROTOCOL,
  BPF_S_ANC_PKTTYPE,
  BPF_S_ANC_IFINDEX,
  BPF_S_ANC_NLATTR,
  BPF_S_ANC_NLATTR_NEST,
  BPF_S_ANC_MARK,
  BPF_S_ANC_QUEUE,
  BPF_S_ANC_HATYPE,
  BPF_S_ANC_RXHASH,
  BPF_S_ANC_CPU,
  BPF_S_ANC_ALU_XOR_X,
  BPF_S_ANC_SECCOMP_LD_W,
  BPF_S_ANC_VLAN_TAG,
  BPF_S_ANC_VLAN_TAG_PRESENT,
  BPF_S_ANC_PAY_OFFSET,
};

#endif /* __ESBPF_CORE_H__ */
