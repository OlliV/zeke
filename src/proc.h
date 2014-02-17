/**
 *******************************************************************************
 * @file    proc.h
 * @author  Olli Vanhoja
 * @brief   Kernel process management header file.
 * @section LICENSE
 * Copyright (c) 2013, 2014 Olli Vanhoja <olli.vanhoja@cs.helsinki.fi>
 * Copyright (c) 2014 Joni Hauhia <joni.hauhia@cs.helsinki.fi>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */

/** @addtogroup Process
  * @{
  */

#pragma once
#ifndef PROC_H
#define PROC_H

#include <autoconf.h>

#ifndef KERNEL_INTERNAL
#define KERNEL_INTERNAL
#endif

#include <kernel.h>
#include <sched.h> /* Needed for threadInfo_t and threading functions */
#include <hal/mmu.h>
#include <klocks.h>
#include <sys/resource.h>

/**
 * Process Control Block or Process Descriptor Structure
 */

#define PROC_RUNNING    0
#define PROC_RUNNABLE   1       /* Can be woken up, ready to run */
#define PROC_WAITING    2       /* Can't be woken up */
#define PROC_ZOMBIE     4       
#define PROC_STOPPED    8

#define PROC_NAME_LEN   10 /*TODO Remove after tests! */


typedef struct {
    pid_t pid;
    char name[PROC_NAME_LEN]; /* process name */
    long state; /* 0 - running, >0 stopped */
    long priority; /* We might want to prioritize prosesses too */
    long counter; /* Counter for process running time */
    unsigned long blocked; /* bitmap of masked signals */
    int exit_code, exit_signal;
    uid_t uid, euid, suid, fsuid;
    gid_t gid, egid, sgid, fsgid;
    unsigned long timeout; /* Used to kill processes with absolute timeout */
    long utime, stime, cutime, cstime, start_time; /* For performance statistics */
    struct rlimit {
        rlim_t rlim_cur;
        rlim_t rlim_max;
    } rlim; /* hard and soft limit for filesize TODO: own struct or just pointer */
    /* open file information */
    struct vnode * cwd; /* current working dir */
    struct file * files;

    struct tty_struct *tty; /* NULL if no tty */

    /* memory management info */

    struct mm_struct {
        void * brk;
        void * brk_start;
        void * brk_stop;
        mmu_pagetable_t pptable;    /*!< Process master page table. */
        mmu_region_t * regions;   /*!< Memory regions of a process.
                                     *   [0] = code
                                     *   [1] = stack
                                     *   [2] = heap/data
                                     *   [n] = allocs
                                     */
        int nr_regions;             /* Number of regions allocated. */
    } mm;
    
/* note: main_thread already has a linked list of child threads
     *      - file_t fd's
     *      - tty
     *      - etc.
     */

    /**
     * Process inheritance; Parent and child thread pointers.
     * inh : Parent and child process relations
     * ----------------------------------------
     * + first_child is a parent process attribute containing address to a first
     *   child of the parent process
     * + parent is a child process attribute containing address of the parent
     *   process of the child thread
     * + next_child is a child thread attribute containing address of a next
     *   child node of the common parent thread
     */
    struct inh {
        void * parent;      /*!< Parent thread. */
        void * first_child; /*!< Link to the first child thread. */
        void * next_child;  /*!< Next child of the common parent. */
    } inh;

    threadInfo_t * main_thread; /*!< Main thread of this process. */
    /* signal handlers */
    sigs_t sigs;                /*!< Signals. */    
} proc_info_t;

int maxproc;
extern volatile pid_t current_process_id;
extern volatile proc_info_t * curproc;

pid_t proc_fork(pid_t pid);
int proc_kill(void);
int proc_replace(pid_t pid, void * image, size_t size);
void proc_thread_removed(pid_t pid, pthread_t thread_id);
proc_info_t * proc_get_struct(pid_t pid);
mmu_pagetable_t * pr_get_pptable(pid_t pid);

#endif /* PROC_H */

/**
  * @}
  */

