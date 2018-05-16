#include "include/asm.h"

ENTRY (add2)
  push %ebp
  movl %esp,%ebp
  movl 8(%ebp),%eax
  movl 12(%ebp),%edx
  addl %edx,%eax
  pop %ebp
  ret
