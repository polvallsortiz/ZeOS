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


int sys_fork()
{
  //CREATES A CHILD PROCESS
  int PID = -1; //BY DEFAULT RETURN A -1 if all OK WE WILL SET ANOTHER ONE
  if(list_empty(&freequeue)) return -1; //if empty return -1
  else {
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    struct task_struct *ts = list_head_to_task_struct(first_free);
    copy_data((union task_union *)current(),(union task_union *)ts, sizeof(union task_union));
    allocate_DIR(ts);

    int frames[NUM_PAG_DATA];
    for(int frame = 0; frame < NUM_PAG_DATA; ++frame) {
      frames[frame] = alloc_frame();
      if(frames[frame] < 0) { //SINO POSIBLE ROLLBACK
        for(int free = 0; free < frame; ++free) free_frame(frames[free]);
        return -1;
      }
    }

    page_table_entry *parent = get_PT(current());
    page_table_entry *child = get_PT(ts);

    int totalpages = NUM_PAG_KERNEL + NUM_PAG_CODE;
    for(int page = 0; page < totalpages; page++) {
      set_ss_pag(child, page, get_frame(parent, page));
    }

    //Usamos paginas libres del padre de forma temporal
    int free_entry = -1;
    for(int auxpag = PAG_LOG_INIT_DATA+NUM_PAG_DATA; auxpag < TOTAL_PAGES; auxpag++){
      if(!parent[auxpag].entry){
        free_entry = auxpag;
        break;
      }
    }
    if(free_entry == -1) return -1; //SINO HAY DEVOLVEMOS ERROR -1
    for(int it=0;it<NUM_PAG_DATA;it++){
      set_ss_pag(child,it+PAG_LOG_INIT_DATA,frames[it]);  //Al hijo les asignamos permanentemente
      set_ss_pag(parent,free_entry,frames[it]);   //Apuntamos de forma temporal a las del hijo

      copy_data((void *)((PAG_LOG_INIT_DATA+it)*PAGE_SIZE),(void *)(free_entry*PAGE_SIZE), PAGE_SIZE); //Copiamos ya que tenemos al padre apuntando a las definitivas del hijo

      del_ss_pag(parent,free_entry);  //Eliminamos el frame del padre hacia el hijo, para proteger su data

      set_cr3(current()->dir_pages_baseAddr); //Flush de la TLB
    }
    //ts.PID=new_pid();
    ts->PID=get_new_pid();


    union task_union *taskun;
    taskun = (union task_union *)ts;

    ts->kernel_esp = taskun->stack[1024-19];  //Actualizamos el esp en su task_struct

    /*&((union task_union *)ts)->stack[1024-19] = 1234
      &((union task_union *)ts)->stack[1024-18] = &ret_from_fork*/

    taskun->stack[1024-19] = 1234;  //feka
    taskun->stack[1024-18] = &ret_from_fork;
    list_add(&(ts->list),&readyqueue);
    return ts->PID;
  }
}

void sys_exit()
{
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
