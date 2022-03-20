#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/queue.h>
#define MAX_TINFO (3)
#define STACK_BYTES (1 << 12UL)
//pack does not seem necessary since saved registers are on natural alignment boundaries
//#define PACK __attribute__((packed))
#define PACK

typedef enum thrstate{
    UNINIT,
    RUNNING,
    WAITING, //Unused since async waits are not implemented
    EXITED,
    MAXTSTATE
}thrstate_e;

typedef struct tinfo tinfo_t;
typedef void (*target_fn_t)(tinfo_t *t);

typedef struct reginfo
{
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t lr;
    uint64_t sp;
} reginfo_t;


typedef struct tinfo
{
    STAILQ_ENTRY(tinfo) link;
    reginfo_t saved_regs;
    uint8_t *stack_mem;
    uint8_t *stack_ptr;
    size_t thread_stack_size;
    target_fn_t f;
    uint64_t scheduled_counter;
    uint32_t id;
    uint32_t state;
} tinfo_t;


extern void tswitch(reginfo_t *old, reginfo_t *new, uint64_t *schedule_count, tinfo_t *target_thread_info);
void tinit();
bool tyield();
void tstart(target_fn_t f, size_t thread_stack_size);
void tend();

