# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2

.globl write; .type write, @function; .align 0; write:
      pushl %ebp
      movl %esp,%ebp
      movl 8(%ebp), %ebx
      movl 12(%ebp),%ecx
      movl 16(%ebp),%edx
      movl $4,%eax
      int $0x80
      cmpl $0,%eax
      jge fin
      movl $0, %ebx
      subl %eax,%ebx
      movl %ebx,errno
      movl $-1,%eax
      jmp fin
      fin:
      movl %ebp,%esp
      popl %ebp
      ret
