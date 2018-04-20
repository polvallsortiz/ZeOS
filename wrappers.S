#include <asm.h>

ENTRY(write)
      pushl %ebp
      movl %esp,%ebp
      pushl %ebx
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
      popl %ebx
      movl %ebp,%esp
      popl %ebp
      ret

ENTRY(gettime)
      pushl %ebp
      movl %esp, %ebp
      movl $10, %eax
      int $0x80
      ret

ENTRY(getpid)
      pushl %ebp
      movl %esp,%ebp
      movl $20,%eax
      int $0x80
      popl %ebp
      ret

ENTRY(fork)
      pushl %ebp
      movl %esp,%ebp
      movl $2,%eax
      int $0x80
      popl %ebp
      ret

ENTRY(exit)
      pushl %ebp
      movl %esp,%ebp
      movl $1,%eax
      int $0x80
      popl %ebp
      ret

ENTRY(get_stats)
     pushl %ebp
     movl %esp,%ebp
     pushl %ebx
     movl $35,%eax
     movl 8(%ebp),%ebx
     movl 12(%ebp),%ecx
     int $0x80
     popl %ebx
     popl %ebp
     ret