#include <asm.h>
#include <errno.h>

ENTRY(write)
      movl -4(%ebp), %ebx
      movl -8(%ebp),%ecx
      movl -12(%ebp),%edx
      movl $4,%eax
      int 0x80
      cmpl $0,%eax
      jge fin
      movl $0, %ebx
      subl %eax,%ebx
      movl %ebx,$errno
      movl $-1,%eax
      j fin
      fin:
      ret
