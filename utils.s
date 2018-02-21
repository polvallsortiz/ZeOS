# 1 "utils.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "utils.S"
# 1 "include/asm.h" 1
# 2 "utils.S" 2

.globl add2; .type add2, @function; .align 0; add2:
  push %ebp
  movl %esp,%ebp
  movl 8(%ebp),%eax
  movl 12(%ebp),%edx
  addl %edx,%eax
  pop %ebp
  ret
