#include <libc.h>
#include <io.h>

char buff[24];

int pid;


int cpu_intensiu() {
    int res = 0;
    for(int i = 0; i < 10000000; ++i) {
        res += i*200;
    }
    return res;
}

int io_intensiu() {
    char buff[10];
    int res = read(0,buff,1000);
    return res;
}

int mix_intensiu() {
    int res = 0;
    for(int i = 0; i < 5000000; ++i) {
        res += i*200;
    }
    char buff[10];
    res += read(0,buff,500);
    return res;
}

void print_stats(int pid) {
    struct stats s;
    get_stats(pid,&s);
    int user = (int)s.user_ticks;
    int blocked = (int)s.blocked_ticks;
    int ready = (int)s.ready_ticks;
    int system = (int)s.system_ticks;
    char pidn[100];
    char num1[100];
    char num2[100];
    char num3[100];
    char num4[100];
    itoa(pid,pidn);
    itoa(user,num1);
    itoa(blocked,num2);
    itoa(ready,num3);
    itoa(system,num4);
    write(1, "\n[PID,USER,BLOCKED,READY,SYSTEM]: ", sizeof("\n[PID,USER,BLOCKED,READY,SYSTEM]: "));
    write(1,pidn,strlen(pidn));
    write(1,",", sizeof(","));
    write(1,num1,strlen(num1));
    write(1,",", sizeof(","));
    write(1,num2,strlen(num2));
    write(1,",", sizeof(","));
    write(1,num3,strlen(num3));
    write(1,",", sizeof(","));
    write(1,num4,strlen(num4));
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  set_sched_policy(1); //0 ROUND ROBIN     1 FCFS
  int PID = fork();
  int PID2;
  int PID3;
  if(PID != 0) PID2 = fork();
  if(PID2 != 0) PID3 = fork();
  int pidactual = getpid();
  int res2;
  if(pidactual != 1) {
      res2 += mix_intensiu();
      print_stats(pidactual);
      if(pidactual == 1003) print_stats(0);
      exit();
  }
  exit();
  while(1) { }
  return res2;
}
