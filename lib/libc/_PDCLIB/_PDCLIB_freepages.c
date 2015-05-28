/*
 * _PDCLIB_freepages( void *, size_t )
 *
 * This file is part of the Public Domain C Library (PDCLib).
 * Permission is granted to use, modify, and / or redistribute at will.
 */

#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <sys/_PDCLIB_glue.h>

void _PDCLIB_freepages(void * p, size_t n)
{
    munmap(p, n * _PDCLIB_MALLOC_PAGESIZE);
}
