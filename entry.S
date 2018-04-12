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
      EOI
      CALL keyboard_routine
      RESTORE_ALL
      iret

ENTRY (clock_handler)
      SAVE_ALL
      EOI
      CALL clock_routine
      RESTORE_ALL
      iret

ENTRY (system_call_handler)
      SAVE_ALL
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
      RESTORE_ALL
      iret

ENTRY (save_registers)
      pushl %ebp
      movl %esp,%ebp
      pushl %esi
      pushl %edi
      pushl %ebx
      movl %ebp,%esp
      popl %ebp
      ret

ENTRY (recover_registers)
      pushl %ebp
      movl %esp,%ebp
      popl %ebx
      popl %edi
      popl %esi
      movl %ebp,%esp
      popl %ebp
      ret

ENTRY (inner_task_switch)
      pushl %ebp
      movl %esp,%ebp
      movl 8(%ebp),%esi         //t to ebx
      and $0xfffff000,%esi      //and to get to the task_struct
      movl 4(%esi),%cr3         //cr3 = new->dir
      movl 8(%ebp),%ecx         //ecx = taskunion
      pushl %ecx                //parameter for tss
      call puttts               //tss esp0=&((task_union*)t)->stack[1024]
      popl %ecx
      call current              //eax = current task struct
      movl 8(%eax),%ebp         //current()->kernel_esp=ebp
      movl 8(%esi),%esp
      movl %ebp,%esp
      popl %ebp
      ret
