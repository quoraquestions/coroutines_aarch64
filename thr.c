#include "thr.h"

tinfo_t tinfo[MAX_TINFO];

STAILQ_HEAD(, tinfo) tlists[MAXTSTATE];

void tinit()
{
    tinfo_t *curt;
    memset(tinfo, '\0', sizeof(tinfo));
    for (size_t i = 0; i < MAXTSTATE; i++)
    {
        STAILQ_INIT(&tlists[i]);
    }

    for (int i = 0; i < MAX_TINFO; i++)
    {
        STAILQ_INSERT_TAIL(&tlists[UNINIT], &tinfo[i], link);
    }
    curt = STAILQ_FIRST(&tlists[UNINIT]);
    STAILQ_REMOVE_HEAD(&tlists[UNINIT], link);
    curt->state = RUNNING;
    curt->id = 0;
    STAILQ_INSERT_TAIL(&tlists[RUNNING], curt, link);
}


void tstart(target_fn_t f, size_t thread_stack_size)
{
    tinfo_t *pt = NULL;
    static uint32_t id = 1;
    pt = STAILQ_FIRST(&tlists[UNINIT]);
    thread_stack_size = thread_stack_size ? thread_stack_size : STACK_BYTES;
    assert(pt && thread_stack_size);
    STAILQ_REMOVE_HEAD(&tlists[UNINIT], link);

    pt->id = id++;
    pt->f = f;
    pt->state = RUNNING;

    pt->stack_mem = (uint8_t *)calloc(thread_stack_size , sizeof(uint8_t));
    pt->stack_ptr =  pt->stack_mem + thread_stack_size - 16;
    pt->thread_stack_size = thread_stack_size;
    memset(pt->stack_mem, 0xaa, thread_stack_size);

    pt->saved_regs.lr = (uint64_t)f;
    pt->saved_regs.sp = (uint64_t)pt->stack_ptr;
    pt->saved_regs.x29 = 0;
    pt->scheduled_counter = 0;
    STAILQ_INSERT_TAIL(&tlists[RUNNING], pt, link);
}

bool tyield()
{
    tinfo_t *tnew = NULL;
    tinfo_t *told = NULL;
    {
        thrstate_e s = RUNNING;
        told = STAILQ_FIRST(&tlists[RUNNING]);
        STAILQ_REMOVE_HEAD(&tlists[RUNNING], link);
        if (told->state == EXITED)
        {
            s = EXITED;
        }
        STAILQ_INSERT_TAIL(&tlists[s], told, link);
    }

    tnew = STAILQ_FIRST(&tlists[RUNNING]);


    if ((!tnew) || (tnew == told))
    {
        return false;
    }
    else
    {
        printf("Switching from %u to %u\n", told->id, tnew->id);
        tswitch(&told->saved_regs, &tnew->saved_regs, &tnew->scheduled_counter, tnew);
    }
    return true;
}

void tend()
{
    tinfo_t *cur;
    cur = STAILQ_FIRST(&tlists[RUNNING]);
    cur->state = EXITED;
    tyield();
}

void sample_coroutine(tinfo_t *ti)
{
    int j = 0;
    for (j = 0; j <= 300; j++)
    {
        printf("thread:%d count:%d\n", ti->id, j);
        ti->scheduled_counter = (uint64_t)-1;
        tyield();
    }
    tend();
}

void twait()
{
    while(tyield())
    {
#if defined(CHECK_STACK_USAGE)
        for (tinfo_t *pt = tinfo + 1; pt < tinfo +  MAX_TINFO; pt++)
        {
            size_t j = 0;

            if (!pt->stack_mem)
            {
                continue;
            }

            while (pt->stack_mem[j++] == 0xaa)
                ;
            printf("Max Used stack :%zu for id :%d\n", pt->thread_stack_size - j, pt->id);
        }
#endif
    }
    printf("Exit yield from main, no remaining coroutines\n");
}

int main()
{
    tinit();
    tstart(sample_coroutine, 0);
    tstart(sample_coroutine, 0);
    twait();
    printf("Done\n");
    return 0;
}
