        .text
        .global tswitch
        //two args r0 old r1 new register info
tswitch:

        stp x19, x20, [x0], #0x10
        stp x21, x22, [x0], #0x10
        stp x23, x24, [x0], #0x10
        stp x25, x26, [x0], #0x10
        stp x27, x28, [x0], #0x10
        stp x29, x30, [x0], #0x10
        mov x29, sp
        str x29, [x0]
        tst x29, #0xf
        bne _trap

        ldr  x4, [x2]
        cbnz x4, _notfirst_yield
        mov  x0, x3
_notfirst_yield:

        adds x3, x4, #1
        csel x3, x4, x3, eq
        str  x3, [x2]

        ldr x29, [x1, #0x60]
        mov sp, x29
        tst x29, #0xf


        bne _trap

        ldp x19, x20, [x1], #0x10
        ldp x21, x22, [x1], #0x10
        ldp x23, x24, [x1], #0x10
        ldp x25, x26, [x1], #0x10
        ldp x27, x28, [x1], #0x10
        ldp x29, x30, [x1], #0x10
        b _end
_trap:  svc #0xdead
_end:
        ret

