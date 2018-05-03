#include <libc.h>
#include <io.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  set_sched_policy(0);
  int PID = fork();
  if(PID == 0) {    //HIJO PROC 2
      int res = 0;
      for(int i = 0; i < 1000000000; ++i) {
          res += i;
      }
      int pid2 = getpid();
      char num3[10];
      itoa(pid2, num3);
      write(1,"\nSOC EL FILL AMB PID: ", sizeof("\nSOC EL FILL AMB PID: "));
      write(1, num3, strlen(num3));
  }
  else {
      char buff[10];
      read(0,buff,10000);

  }
  while(1) { }
}
