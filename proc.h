/** proc.h
 *
 * Copyright (c) 2021 Leesoo Ahn <lsahn@ooseel.net>
 *
 * procfs class for esbpf under a ethernet driver.
 */

#ifndef __ESBPF_PROC_H__
#define __ESBPF_PROC_H__

#include <linux/proc_fs.h>

struct esbpf_helper;

int esbpf_proc_init(struct proc_dir_entry *root,
                    struct esbpf_helper *hdata);
void esbpf_proc_exit(void);

#endif /* __ESBPF_PROC_H__ */
