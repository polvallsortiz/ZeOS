/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <interrupt.h>

#include <system.h>

#define LECTURA 0
#define ESCRIPTURA 1

int MAX_BUFFER_SIZE = 64;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}


int ret_from_fork() {
  return 0;
}

int get_new_pid() {
  ++actual_pid;
  return actual_pid;
}

void update_proc_stats(unsigned long* stat, unsigned long* elapsed) {
  *stat += get_ticks() - *elapsed;
  *elapsed = get_ticks();
}

void user_system() {
  update_proc_stats(&(current()->proc_stats.user_ticks),&(current()->proc_stats.elapsed_total_ticks));
}

void system_user() {
  update_proc_stats(&(current()->proc_stats.system_ticks),&(current()->proc_stats.elapsed_total_ticks));
}

int sys_get_stats(int pid, struct stats* stat) {
  if(pid < 0) return -1;
  for(int tasks = 0; tasks < NR_TASKS; tasks++) {
    if(tasks[task].task.PID == pid) {
      task[tasks].task.proc_stats.remaining_ticks = actual_ticks;
      copy_to_user(&(task[tasks].task.proc_stats),stat,sizeof(struct stats));
      return 0;
    }
  }
  return -1;
}


int sys_fork()
{
  //CREATES A CHILD PROCESS
  int PID = -1; //BY DEFAULT RETURN A -1 if all OK WE WILL SET ANOTHER ONE
  if(list_empty(&freequeue)) return -1; //if empty return -1
  else {
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    struct task_struct *ts = list_head_to_task_struct(first_free);
    union task_union *taskun = (union task_union *)ts;
    copy_data(current(),taskun, sizeof(union task_union));
    allocate_DIR(ts);

    int frames[NUM_PAG_DATA];
    for(int frame = 0; frame < NUM_PAG_DATA; ++frame) {
      frames[frame] = alloc_frame();
      if(frames[frame] < 0) { //SINO POSIBLE ROLLBACK
        for(int free = 0; free < frame; ++free) free_frame(frames[free]);
        list_add_tail(first_free,&freequeue);
        return -1;
      }
    }

    page_table_entry *parent = get_PT(current());
    page_table_entry *child = get_PT(ts);

    for(int page = 0; page < NUM_PAG_DATA; page++) {
        set_ss_pag(child, PAG_LOG_INIT_DATA+page, frames[page]);
    }

    for(int page = 0; page < NUM_PAG_KERNEL; page++) {
      set_ss_pag(child, page, get_frame(parent, page));
    }

    for(int page = 0; page < NUM_PAG_CODE; page++) {
      set_ss_pag(child,PAG_LOG_INIT_CODE+page,get_frame(parent,PAG_LOG_INIT_CODE+page));
    }

    //Usamos paginas libres del padre de forma temporal
    //recorremos la tabla de paginas buscando una entry libre recorriendo Kernel, Codigo y Datos
    /*int free_entry = -1;
    for(int auxpag = PAG_LOG_INIT_DATA+NUM_PAG_DATA; auxpag < TOTAL_PAGES; auxpag++){
      if(!parent[auxpag].entry){
        free_entry = auxpag;
        break;
      }
    }
    if(free_entry == -1) return -1; //SINO HAY DEVOLVEMOS ERROR -1*/
    for(int it=NUM_PAG_KERNEL+NUM_PAG_CODE;it<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA;it++){
      set_ss_pag(parent,it+NUM_PAG_DATA,get_frame(child,it));   //Apuntamos de forma temporal a las del hijo

      copy_data((void *)(it<<12),(void *)((it+NUM_PAG_DATA)<<12), PAGE_SIZE); //Copiamos ya que tenemos al padre apuntando a las definitivas del hijo

      del_ss_pag(parent,it+NUM_PAG_DATA);  //Eliminamos el frame del padre hacia el hijo, para proteger su data
    }

    set_cr3(current()->dir_pages_baseAddr); //Flush de la TLB
    //ts.PID=new_pid();
    ts->PID=get_new_pid();
    set_quantum(ts,get_quantum(current()));


    ts->kernel_esp = taskun->stack[1024-19];  //Actualizamos el esp en su task_struct

    /*&((union task_union *)ts)->stack[1024-19] = 1234
      &((union task_union *)ts)->stack[1024-18] = &ret_from_fork*/

    int reg_ebp = (int) get_ebp();
    reg_ebp = reg_ebp - (int)current() + (int)taskun;
    taskun->task.kernel_esp = reg_ebp + sizeof(DWord);

    DWord temp_ebp = *(DWord*)reg_ebp;
    taskun->task.kernel_esp-=sizeof(DWord);
    *(DWord*)(taskun->task.kernel_esp)=(DWord)&ret_from_fork;
    taskun->task.kernel_esp-= sizeof(DWord);
    *(DWord*)(taskun->task.kernel_esp)=temp_ebp;

    /*taskun->stack[1024-19] = 1234;  //-19 porque por debajo tenemos &ret_from_fork,@handler,CTX.SFTW,CTX.HDW
    taskun->stack[1024-18] = &ret_from_fork;*/
    list_add_tail(&(ts->list),&readyqueue);
    ts->estat = ST_READY;
    init_stats(&ts->proc_stats);
    return ts->PID;
  }
}







void sys_exit()
{
    page_table_entry* actual = get_PT(current());
    for(int frame = 0; frame < NUM_PAG_DATA; frame++) {
      free_frame(get_frame(actual,PAG_LOG_INIT_DATA+frame));
      del_ss_pag(actual,PAG_LOG_INIT_DATA+frame);
    }
    current()->PID = -1;
    list_add_tail(&(current()->list),&freequeue);
    //update_process_state_rr(current(),&freequeue);
    sched_next();
}

int sys_write(int fd, char * buffer, int size) {
  int err1 = check_fd(fd,ESCRIPTURA);
  if(err1 != 0) return err1;
  if(buffer == NULL) return -14; /*EFAULT*/
  if(size < 0) return -22; /*EINVALL*/
  if(fd == 1) {
    int s = 0;
    char buf_aux[64];
    while(size > MAX_BUFFER_SIZE) {
      if(copy_from_user(buffer,buf_aux,MAX_BUFFER_SIZE) == -1) return -14;
      s += sys_write_console(buf_aux,MAX_BUFFER_SIZE);
      buffer += MAX_BUFFER_SIZE;
      size -= MAX_BUFFER_SIZE;
    }
    if(size != 0) {
      if(copy_from_user(buffer,buf_aux,size) == -1) return -14;
      s += sys_write_console(buf_aux,size);
    }
    return s;
  }
}

int sys_gettime() {
  return zeos_ticks;
}
