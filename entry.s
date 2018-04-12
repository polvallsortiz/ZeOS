# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 73 "entry.S"
.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
      movb $0x20, %al; outb %al, $0x20;
      CALL keyboard_routine
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
      movb $0x20, %al; outb %al, $0x20;
      CALL clock_routine
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
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
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl save_registers; .type save_registers, @function; .align 0; save_registers:
      pushl %ebp
      movl %esp,%ebp
      pushl %esi
      pushl %edi
      pushl %ebx
      movl %ebp,%esp
      popl %ebp
      ret

.globl recover_registers; .type recover_registers, @function; .align 0; recover_registers:
      pushl %ebp
      movl %esp,%ebp
      popl %ebx
      popl %edi
      popl %esi
      movl %ebp,%esp
      popl %ebp
      ret

.globl inner_task_switch; .type inner_task_switch, @function; .align 0; inner_task_switch:
      pushl %ebp
      movl %esp,%ebp
      movl 8(%ebp),%esi
      and $0xfffff000,%esi
      movl %ebp,%cr3
      movl 8(%ebp),%ecx
      pushl %ecx
      call puttts
      popl %ecx
      call current
      movl 8(%eax),%ebp
      movl 8(%esi),%esp
      movl %ebp,%esp
      popl %ebp
      ret
