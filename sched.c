/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <schedperf.h>

struct task_struct *idle_task;

int actual_pid = 0;
int actual_ticks = 0;


/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct list_head freequeue;

struct list_head readyqueue;


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return (struct task_struct*)((int)l&0xfffff000);
}

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
    printk("INIT_IDLE");
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    idle_task = list_head_to_task_struct(first_free);
    idle_task->PID = 0;
    init_stats(&idle_task->proc_stats);
    ++actual_pid;
    allocate_DIR(idle_task);
    union task_union *taskun;
    taskun = (union task_union *)idle_task;
    taskun->stack[1023] = &cpu_idle;
    taskun->stack[1022] = 1234;
    idle_task->kernel_esp=&(taskun->stack[1022]);
    //list_del(first_free);
    printk("INIT_IDLE FINISHED");
}

void init_task1(void)
{
    printk("A INIT_TASK1 ");
    struct list_head *first_free = list_first(&freequeue);
    list_del(first_free);
    struct task_struct *task1 = list_head_to_task_struct(first_free);
    task1->PID = 1;
    set_quantum(task1,100);
    actual_ticks = 100;
    task1->estat = ST_RUN;
    init_stats(&task1->proc_stats);
    ++actual_pid;
    allocate_DIR(task1);
    set_user_pages(task1);
    tss.esp0 = &(((union task_union *)task1)->stack[1024]);
    set_cr3(task1->dir_pages_baseAddr);
    printk("INIT_TASK1 FINISHED");
}


void init_sched(){
    printk("INIT_SCHED");
    initialize_freequeue();
    initialize_readyqueue();
    init_sched_policy();
    printk("FINISHED INIT_SCHED");
}

void init_stats(struct stats *status) {
    status->user_ticks = 0;
    status->system_ticks = 0;
    status->blocked_ticks = 0;
    status->ready_ticks = 0;
    status->elapsed_total_ticks = get_ticks();
    status->total_trans = 0; /* Number of times the process has got the CPU: READY->RUN transitions */
    status->remaining_ticks = get_ticks();
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
        list_add(&(task[i].task.list), &freequeue);
    }
}

void initialize_readyqueue() {
    INIT_LIST_HEAD(&readyqueue);
}

/*
void task_switch(union task_union*t) {
    save_registers();
    inner_task_switch(t);
    recover_registers();
}
*/
/*void inner_task_switch(union task_union*t) {
    //pushebp();
    set_cr3(t->task.dir_pages_baseAddr);
    tss.esp0 = &(t->stack[1024]);
    save_registers();
    int *esp = current()->kernel_esp;
    int *t_esp = t->task.kernel_esp;
    finalize_task_switch(esp,t_esp);
}*/

void inner_task_switch(union task_union *t)
{
    page_table_entry *dir = get_DIR(&t->task);
    tss.esp0=&(t->stack[1024]);
    set_cr3(dir);
    actual_ticks = get_quantum(t);
    finalize_task_switch(&current()->kernel_esp,t->task.kernel_esp);
}

void update_sched_data_rr() {
    --actual_ticks;
}

int needs_sched_rr(void) {
    return (actual_ticks == 0);
}

void update_process_state_rr(struct task_struct * t, struct list_head *dest) {
    if(t->estat != ST_RUN) list_del(&t->list);
    if(dest != NULL) {
        list_add_tail(&t->list, dest);
        if(dest == &readyqueue) {
            t->estat = ST_READY;
            /*STAT PROCS
            current()->proc_stats->system_ticks += get_ticks() - current()->proc_stats->elapsed_total_ticks;
            current()->proc_stats->elapsed_total_ticks = get_ticks;*/
            update_proc_stats(&(t->proc_stats.system_ticks),&(t->proc_stats.elapsed_total_ticks));
        }
        //if(dest == &freequeue) t->estat = ST_BLOCKED;
    }
    else t->estat = ST_RUN;
}

void sched_next_rr(void) {
    struct list_head *first_free;
    struct task_struct * ts;
    union task_union * taskun;
    if(!list_empty(&readyqueue)) {
        first_free = list_first(&readyqueue);
        list_del(first_free);
        ts = list_head_to_task_struct(first_free);
        taskun = (union task_union *)ts;
        actual_ticks = get_quantum(ts);
        ts->estat = ST_RUN;
        /*STAT PROCS
        current()->proc_stats->system_ticks += get_ticks() - current()->proc_stats->elapsed_total_ticks;
        current()->proc_stats->elapsed_total_ticks = get_ticks;*/
        update_proc_stats(&(current()->proc_stats.system_ticks),&(current()->proc_stats.elapsed_total_ticks));
        /*STAT PROCS
        ts->proc_stats->ready_ticks += get_ticks() - ts->proc_stats->elapsed_total_ticks;
        ts->proc_stats->elapsed_total_ticks = get_ticks;*/
        update_proc_stats(&(ts->proc_stats.ready_ticks),&(ts->proc_stats.elapsed_total_ticks));
        ts->proc_stats.total_trans++;
        task_switch(taskun);
    }
    else if(current() != idle_task) {
        actual_ticks = get_quantum(idle_task);
        idle_task->estat = ST_RUN;
        /*STAT PROCS
        current()->proc_stats->system_ticks += get_ticks() - current()->proc_stats->elapsed_total_ticks;
        current()->proc_stats->elapsed_total_ticks = get_ticks;*/
        update_proc_stats(&(current()->proc_stats.system_ticks),&(current()->proc_stats.elapsed_total_ticks));
        /*STAT PROCS
        ts->proc_stats->ready_ticks += get_ticks() - current()->proc_stats->elapsed_total_ticks;
        ts->proc_stats->elapsed_total_ticks = get_ticks;*/
        update_proc_stats(&(ts->proc_stats.ready_ticks),&(ts->proc_stats.elapsed_total_ticks));
        ts->proc_stats.total_trans++;
        task_switch((union task_union *)idle_task);
    }
}

int get_quantum(struct task_struct *t) {
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
    t->quantum = new_quantum;
}

void schedule() {
    update_sched_data();
    if(needs_sched()) {
        update_process_state(current(),&readyqueue);
        sched_next();
    }
}

/*void init_sched_policy() {
    sched_next = sched_next_fcfs;
    update_process_state = update_process_state_fcfs;
    needs_sched = needs_sched_fcfs;
    update_sched_data = update_sched_data_fcfs;
}*/

struct stats * get_task_stats(struct task_struct *t) {
     return &(t->proc_stats);
}

struct list_head *get_task_list(struct task_struct *t) {
    return &(t->list);
}

void block_process(struct list_head *block_queue) {
    struct task_struct *t = current();
    struct  stats *st = get_task_stats(t);

    update_process_state(t,block_queue);
    st->system_ticks = get_ticks()-st->elapsed_total_ticks;
    st->elapsed_total_ticks = get_ticks();
    sched_next();
}

void unblock_process(struct task_struct *blocked) {
    struct  stats *st = get_task_stats(blocked);
    struct list_head *l = get_task_list(blocked);

    update_process_state(blocked,&readyqueue);
    st->blocked_ticks += (get_ticks()-st->elapsed_total_ticks);
    st->elapsed_total_ticks = get_ticks();
    if(needs_sched()) {
        update_process_state(current(),&readyqueue);
        sched_next();
    }

}

