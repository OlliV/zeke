/**
 *******************************************************************************
 * @file    vmmap.c
 * @author  Olli Vanhoja
 * @brief   Process vm stats.
 * @section LICENSE
 * Copyright (c) 2019 Olli Vanhoja <olli.vanhoja@alumni.helsinki.fi>
 * Copyright (c) 2017 Olli Vanhoja <olli.vanhoja@cs.helsinki.fi>
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/proc.h>
#include <sys/sysctl.h>
#include <sysexits.h>

static size_t pid_vmmap(struct kinfo_vmentry ** vmmap, pid_t pid)
{
    int mib[5];

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = pid;
    mib[4] = KERN_PROC_VMMAP;

    for (int i = 0; i < 3; i++) {
        size_t size = 0;
        struct kinfo_vmentry * map;


        if (sysctl(mib, num_elem(mib), NULL, &size, 0, 0)) {
            return 0;
        }

        map = malloc(size);
        if (!map)
            return 0;

        if (sysctl(mib, num_elem(mib), map, &size, 0, 0)) {
            free(map);
            continue;
        }
        *vmmap = map;
        return size / sizeof(struct kinfo_vmentry);
    }
    return 0;
}

int main(int argc, char * argv[], char * envp[])
{
    pid_t pid;
    struct kinfo_vmentry * vmmap;
    struct kinfo_vmentry * entry;
    size_t n;

    if (argc < 2 || sscanf(argv[1], "%d", &pid) != 1) {
        fprintf(stderr, "usage: %s PID\n", argv[0]);

        return EX_USAGE;
    }

    printf("PID: %d\n", pid);
    n = pid_vmmap(&vmmap, pid);
    if (n == 0) {
        perror("Failed to get vmmap for the process");
        return EX_NOINPUT;
    }

    printf("START      END        PADDR      FLAGS     UAP\n");
    entry = vmmap;
    for (size_t i = 0; i < n; i++) {
        printf("0x%08x 0x%08x 0x%08x 0x%07x %s\n",
               entry->reg_start,
               entry->reg_end,
               entry->paddr,
               entry->flags,
               entry->uap);
        entry++;
    }

    free(vmmap);
    return EX_OK;
}
