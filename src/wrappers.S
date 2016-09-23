.code64 
.global handler_wrappers

.macro PUSH_REGISTERS
    pushq %rax 
    pushq %rbx 
    pushq %rcx 
    pushq %rdx 
    pushq %rsi 
    pushq %rdi 
    pushq %rbp 
    pushq %r8  
    pushq %r9 
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13 
    pushq %r14
    pushq %r15
.endm

.macro POP_REGISTERS
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9 
    popq %r8  
    popq %rbp 
    popq %rdi 
    popq %rsi 
    popq %rdx 
    popq %rcx 
    popq %rbx 
    popq %rax 
.endm

.globl interrupt_handler

.macro WRAPPER N 
wrapper\N: 
    PUSH_REGISTERS
    movq $\N, %rdi
    cld
    call interrupt_handler
    POP_REGISTERS
    iretq
.endm

.macro ERROR_WRAPPER N
wrapper\N:
    add $8, %rsp 

    PUSH_REGISTERS
    movq $\N, %rdi    
    cld
    call interrupt_handler
    POP_REGISTERS
    iretq
.endm

WRAPPER 0
WRAPPER 1 
WRAPPER 2
WRAPPER 3
WRAPPER 4
WRAPPER 5
WRAPPER 6
WRAPPER 7
ERROR_WRAPPER 8
WRAPPER 9
ERROR_WRAPPER 10
ERROR_WRAPPER 11
ERROR_WRAPPER 12
ERROR_WRAPPER 13
ERROR_WRAPPER 14
WRAPPER 15
WRAPPER 16
ERROR_WRAPPER 17
WRAPPER 18
WRAPPER 19
WRAPPER 20
WRAPPER 21
WRAPPER 22
WRAPPER 23
WRAPPER 24
WRAPPER 25
WRAPPER 26
WRAPPER 27
WRAPPER 28
WRAPPER 29
WRAPPER 30
WRAPPER 31
WRAPPER 32
WRAPPER 33
WRAPPER 34
WRAPPER 35
WRAPPER 36
WRAPPER 37
WRAPPER 38
WRAPPER 39
WRAPPER 40
WRAPPER 41
WRAPPER 42
WRAPPER 43
WRAPPER 44
WRAPPER 45
WRAPPER 46
WRAPPER 47

handler_wrappers:
    .quad wrapper0
    .quad wrapper1 
    .quad wrapper2
    .quad wrapper3
    .quad wrapper4
    .quad wrapper5
    .quad wrapper6
    .quad wrapper7
    .quad wrapper8
    .quad wrapper9
    .quad wrapper10
    .quad wrapper11
    .quad wrapper12
    .quad wrapper13
    .quad wrapper14
    .quad wrapper15
    .quad wrapper16
    .quad wrapper17
    .quad wrapper18
    .quad wrapper19
    .quad wrapper20
    .quad wrapper21
    .quad wrapper22
    .quad wrapper23
    .quad wrapper24
    .quad wrapper25
    .quad wrapper26
    .quad wrapper27
    .quad wrapper28
    .quad wrapper29
    .quad wrapper30
    .quad wrapper31
    .quad wrapper32
    .quad wrapper33
    .quad wrapper34
    .quad wrapper35
    .quad wrapper36
    .quad wrapper37
    .quad wrapper38
    .quad wrapper39
    .quad wrapper40
    .quad wrapper41
    .quad wrapper42
    .quad wrapper43
    .quad wrapper44
    .quad wrapper45
    .quad wrapper46
    .quad wrapper47

