/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

struct task_struct *idle_task;

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct list_head freequeue;

struct list_head readyqueue;

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    idle_task = list_entry(first_free,struct task_struct, list);
    idle_task->PID = 0;
    allocate_DIR(idle_task);
    union task_union *taskun;
    taskun = (union task_union *)idle_task;
    taskun->stack[1023] = &cpu_idle;
    taskun->stack[1022] = 1234;
    idle_task->kernel_esp=taskun->stack[1022];
    list_del(first_free);
    printk("HOLA\n");
}

void init_task1(void)
{
    printk("A INIT_TASK1\n");
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    struct task_struct *task1 = list_entry(first_free,struct task_struct, list);
    task1->PID = 1;
    allocate_DIR(task1);
    set_user_pages(task1);
    page_table_entry * directory_task1 = get_DIR(task1);
    set_cr3(directory_task1);
    printk("A FINAL INIT_TASK1\n");

}


void init_sched(){
    initialize_freequeue();
    initialize_readyqueue();
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void initialize_freequeue() {
    INIT_LIST_HEAD(&freequeue);
    int i = 0;
    for(i = 0; i < NR_TASKS; ++i) {
        union task_union actual = task[i];
        list_add(&(actual.task.list), &freequeue);
    }
}

void initialize_readyqueue() {
    INIT_LIST_HEAD(&readyqueue);
}

