#include <libc.h>
#include <io.h>
#include <utils.h>
#include <sched.h>

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
    switch(errno) {
      case 9:
        write(1,"\nEBADF : Bad file number",sizeof("\nEBADF : Bad file number"));
        break;
      case 13:
        write(1,"\nEACCES : Permission denied",sizeof("\nEACCES : Permission denied"));
        break;
      case 38:
         write(1,"\nENOSYS : Function not implemented",sizeof("\nENOSYS : Function not implemented"));
        break;
      case 14:
        write(1,"\nEFAULT : Bad address",sizeof("\nEFAULT : Bad address"));
        break;
      case 22:
        write(1,"\nEINVAL : Invalid Argument", sizeof("\nEINVAL : Invalid Argument"));
        break;
    }
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
     int pid = fork();
     if(pid != 0) {
         exit();
     }
     else {
         int pid2 = getpid();
         char num3[10];
         itoa(pid2, num3);
         write(1, "\n SOC EL FILL I NOMES QUEDO JO AMB PID: ", sizeof("\n SOC EL FILL I NOMES QUEDO JO AMB PID: "));
         write(1, num3, strlen(num3));
     }
     struct stats s;
     get_stats(3,&s);
     int stat = (int)s.elapsed_total_ticks;
     char num4[10];
     itoa(stat,num4);
     write(1, "\n STAT DE ELAPSED TOTAL TICKS : ", sizeof("\n STAT DE ELAPSED TOTAL TICKS : "));
     write(1,num4,strlen(num4));
     exit();
     while(1);
     return aux;
}
