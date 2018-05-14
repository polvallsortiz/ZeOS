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
      movl %eax,%ebx
      CALL user_system
      movl %ebx,%eax
      movb $0x20, %al; outb %al, $0x20;
      CALL keyboard_routine
      movl %eax,%ebx
      CALL system_user
      movl %ebx,%eax
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
      movl %eax,%ebx
      CALL user_system
      movl %ebx,%eax
      movb $0x20, %al; outb %al, $0x20;
      CALL clock_routine
      movl %eax,%ebx
      CALL system_user
      movl %ebx,%eax
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
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
      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
      iret

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
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


.globl finalize_task_switch; .type finalize_task_switch, @function; .align 0; finalize_task_switch:
      movl 4(%esp),%ecx
      movl %ebp,(%ecx)
      movl 8(%esp),%esp
      popl %ebp
      ret

.globl get_ebp; .type get_ebp, @function; .align 0; get_ebp:
      movl %ebp,%eax
      ret
