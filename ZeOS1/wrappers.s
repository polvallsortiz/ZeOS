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

.globl gettime; .type gettime, @function; .align 0; gettime:
      pushl %ebp
      movl %esp, %ebp
      movl $10, %eax
      int $0x80
      ret

.globl getpid; .type getpid, @function; .align 0; getpid:
      pushl %ebp
      movl %esp,%ebp
      movl $20,%eax
      int $0x80
      popl %ebp
      ret

.globl fork; .type fork, @function; .align 0; fork:
      pushl %ebp
      movl %esp,%ebp
      movl $2,%eax
      int $0x80
      popl %ebp
      ret

.globl exit; .type exit, @function; .align 0; exit:
      pushl %ebp
      movl %esp,%ebp
      movl $1,%eax
      int $0x80
      popl %ebp
      ret

.globl get_stats; .type get_stats, @function; .align 0; get_stats:
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
