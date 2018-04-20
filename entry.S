/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es


#define RESTORE_ALL \
      popl %ebx;  \
      popl %ecx;  \
      popl %edx;  \
      popl %esi;  \
      popl %edi;  \
      popl %ebp;  \
      popl %eax;  \
      popl %ds;   \
      popl %es;   \
      popl %fs;   \
      popl %gs

#define EOI \
      movb $0x20, %al; \
      outb %al, $0x20;


ENTRY (keyboard_handler)
      SAVE_ALL
      movl %eax,%ebx
      CALL user_system
      movl %ebx,%eax
      EOI
      CALL keyboard_routine
      movl %eax,%ebx
      CALL system_user
      movl %ebx,%eax
      RESTORE_ALL
      iret

ENTRY (clock_handler)
      SAVE_ALL
      movl %eax,%ebx
      CALL user_system
      movl %ebx,%eax
      EOI
      CALL clock_routine
      movl %eax,%ebx
      CALL system_user
      movl %ebx,%eax
      RESTORE_ALL
      iret

ENTRY (system_call_handler)
      SAVE_ALL
      movl %eax,%ebx
      CALL user_system
      movl %ebx,%eax
      cmpl $0, %eax
      jl err
      cmpl $MAX_SYSCALL, %eax
      jg err
      call *sys_call_table(,%eax,0x04)
      jmp fin
      err:
      movl $-38,%eax
      fin:
      movl %eax, 0x18(%esp)
      movl %eax,%ebx
      CALL system_user
      movl %ebx,%eax
      RESTORE_ALL
      iret

ENTRY (task_switch)
      pushl %ebp
      movl %esp,%ebp
      pushl %esi;
      pushl %edi;
      pushl %ebx;

      movl 8(%ebp),%ecx
      pushl %ecx

      call inner_task_switch

      popl %ecx
      popl %ebx
      popl %edi
      popl %esi
      popl %ebp;
      ret;


ENTRY (finalize_task_switch)
      movl 4(%esp),%ecx
      movl %ebp,(%ecx)
      movl 8(%esp),%esp
      popl %ebp
      ret

ENTRY (get_ebp)
      movl %ebp,%eax
      ret
