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
# 39 "entry.S"
      pushl %gs;
      pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
# 67 "entry.S"
      CALL print_debug_routine;





      outb %al, $0x20;


.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
      ;
      CALL print_debug_routine;
      movb $0x20, %al; CALL print_debug_routine2;;
      CALL keyboard_routine;

      popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %gs; popl %fs; popl %es; popl %ds;;
      iret;
