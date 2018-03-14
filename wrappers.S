#include <asm.h>

ENTRY(write)
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
