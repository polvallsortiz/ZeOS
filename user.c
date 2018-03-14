#include <libc.h>

char buff[24];

int pid;

long inner(long n) {
  int i;
  long suma;
  suma = 0;
  for(i=0; i<n; i++) suma = suma + i;
  return
  suma;
}

long outer(long n) {
  int i;
  long acum;
  acum = 0;
  for(i=0; i<n; i++) acum = acum + inner(i);
  return acum;
}

int add(int par1,int par2){
  return par1+par2;
}

void perror() {
    char buff[];
    switch(errno) {
      case 9:
        write(1,"EBADF : Bad file number",sizeof("EBADF : Bad file number"));
        break;
      case 13:
        buff = "EACCES : Permission denied";
        break;
      case 38:
        buff = "ENOSYS : Function not implemented";
        break;
      case 14:
        buff = "EFAULT : Bad address";
        break;
      case 22:
        buff = "EINVAL : Invalid Argument";
        break;
    }
    write(1,buff,sizeof(buff));
}

int __attribute__ ((__section__(".text.main")))
main()
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

     long count, acum;
     count = 75;
     acum = 0;
     acum = outer(count);
     int aux = add2(2,2);
     if(write(1,"\nHola soc un write",-1) < 0) perror();
     while(1);
     return aux;
}
