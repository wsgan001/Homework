#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include "fixed-point.h"
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
static struct list ready_list;

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
static struct list all_list;




//mlfq,记住要初始化

int64_t load_avg;

//mlfq


/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;

/* Stack frame for kernel_thread(). */
struct kernel_thread_frame 
  {
    void *eip;                  /* Return address. */
    thread_func *function;      /* Function to call. */
    void *aux;                  /* Auxiliary data for function. */
  };

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread *running_thread (void);
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static bool is_thread (struct thread *) UNUSED;
static void *alloc_frame (struct thread *, size_t size);
static void schedule (void);
void thread_schedule_tail (struct thread *prev);
static tid_t allocate_tid (void);

/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void) 
{
  ASSERT (intr_get_level () == INTR_OFF);

  lock_init (&tid_lock);
  list_init (&ready_list);
  list_init (&all_list);

  /* Set up a thread structure for the running thread. */
  initial_thread = running_thread ();
  init_thread (initial_thread, "main", PRI_DEFAULT);
  initial_thread->status = THREAD_RUNNING;
  initial_thread->tid = allocate_tid ();
  
  
  load_avg=0;
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) 
{
  /* Create the idle thread. */
  struct semaphore idle_started;
  sema_init (&idle_started, 0);
  thread_create ("idle", PRI_MIN, idle, &idle_started);

  /* Start preemptive thread scheduling. */
  intr_enable ();

  /* Wait for the idle thread to initialize idle_thread. */
  sema_down (&idle_started);
}

/*计算recent_cpu*/
//recent_cpu = (2*load_avg)/(2*load_avg + 1) * recent_cpu + nice. 
void renew_recent_cpu(struct thread* t)
{
	// (2*load_avg)/(2*load_avg + 1)
	t->recent_cpu=FMULF(FDIVF(FMULI(load_avg,2),FADDI(FMULI(load_avg,2),1)),t->recent_cpu);
	// 加 nice
	t->recent_cpu=FADDI(t->recent_cpu,t->nice);
}

/*获得当前ready_list大小，但是不包括 idle_thread*/
int64_t get_ready_threads(){
	int64_t rt=0;
	rt=rt+list_size(&ready_list);
	if(thread_current()!=idle_thread)
		rt++;
	return rt;
}

/* 修正，更新优先级*/
void renew_priority(struct thread* t)
{
	t->priority=PRI_MAX-F2INT45(FDIVI(t->recent_cpu,4))-(t->nice*2);
	if(t->priority>PRI_MAX)
		t->priority=PRI_MAX;
	if(t->priority<PRI_MIN)
		t->priority=PRI_MIN;
		
}

void renew_all_priority()
{
	struct list_elem *e;
	for (e = list_begin (&all_list); e != list_end (&all_list); e = list_next (e))
	{
		if(list_entry(e,struct thread,allelem)==idle_thread)
			continue;
		renew_priority(list_entry(e,struct thread,allelem));
	}
	
	list_sort(&ready_list,cmp_priority,NULL);
	intr_yield_on_return ();	
}
//重新计算所有线程的recent_cpu
void thread_all_renew(void)
{
	ASSERT (intr_get_level () == INTR_OFF);
	struct list_elem *e;
	for (e = list_begin (&all_list); e != list_end (&all_list); e = list_next (e))
	{
		renew_recent_cpu(list_entry(e,struct thread,allelem));
	}
}

//load_avg = (59/60)*load_avg + (1/60)*ready_threads.
void renew_load_avg(void)
{
	
	load_avg=FMULF(load_avg,FDIVI(INT2FLOAT(59),60))+FMULI(get_ready_threads(),FDIVI(INT2FLOAT(1),60));
}


/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) 
{	
  struct thread *t = thread_current ();

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
  {
    user_ticks++;
	if(thread_mlfqs)
	{
		t->recent_cpu=FADDI(t->recent_cpu,1);
	}
  }
#endif
  else
  {
    kernel_ticks++;
    //当前的线程 recent_cpu +1
	if(thread_mlfqs)
	{
		t->recent_cpu=FADDI(t->recent_cpu,1);
	}
}
  /*每100个timer_ticks(1s) 更新一次系统的load_avg */
  if(thread_mlfqs&&timer_ticks()%100==0)
  {
	 /*更新recent_cpu,load_avg*/
	 renew_load_avg();
	 thread_all_renew();
  }
  //每4个timer_ticks更新一次优先级
 if(thread_mlfqs&&timer_ticks()%4==0)
		renew_all_priority();
  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
  {
    intr_yield_on_return ();
  }
}




/* Prints thread statistics. */
void
thread_print_stats (void) 
{
  printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
          idle_ticks, kernel_ticks, user_ticks);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
               thread_func *function, void *aux) 
{
  struct thread *t;
  struct kernel_thread_frame *kf;
  struct switch_entry_frame *ef;
  struct switch_threads_frame *sf;
  tid_t tid;
  enum intr_level old_level;

  ASSERT (function != NULL);

  /* Allocate thread. */
  t = palloc_get_page (PAL_ZERO);
  if (t == NULL)
    return TID_ERROR;

  /* Initialize thread. */
  init_thread (t, name, priority);
  tid = t->tid = allocate_tid ();

  /* Prepare thread for first run by initializing its stack.
     Do this atomically so intermediate values for the 'stack' 
     member cannot be observed. */
  old_level = intr_disable ();

  /* Stack frame for kernel_thread(). */
  kf = alloc_frame (t, sizeof *kf);
  kf->eip = NULL;
  kf->function = function;
  kf->aux = aux;

  /* Stack frame for switch_entry(). */
  ef = alloc_frame (t, sizeof *ef);
  ef->eip = (void (*) (void)) kernel_thread;

  /* Stack frame for switch_threads(). */
  sf = alloc_frame (t, sizeof *sf);
  sf->eip = switch_entry;
  sf->ebp = 0;
   //printf("thread create %s\n",name);
  intr_set_level (old_level);

  /* Add to run queue. */
  thread_unblock (t);
  //printf("thread_current: %s,priority: %d,threat t: %s,priority: %d\n",thread_current()->name,thread_current()->priority,t->name,t->priority);
  //若产生了一个新的thread，且优先级比较高，需要尽快cpu时间，不要将这个放入到thread_unblock中
  //在本函数没问题，但是在其他函数调用thread_unblock()会出现问题
  if(t->priority > thread_current()->priority){
	thread_yield();
	}
  return tid;
}

/* Puts the current thread to sleep.  It will not be scheduled
   again until awoken by thread_unblock().

   This function must be called with interrupts turned off.  It
   is usually a better idea to use one of the synchronization
   primitives in synch.h. */
void
thread_block (void) 
{
  ASSERT (!intr_context ());
  ASSERT (intr_get_level () == INTR_OFF);
  thread_current ()->status = THREAD_BLOCKED; 
  schedule ();
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
   
//未改变，方案二尝试改变，但是会出错，不能确定什么时候应该交出cpu时间
void
thread_unblock (struct thread *t) 
{
  enum intr_level old_level;

  ASSERT (is_thread (t));

  old_level = intr_disable ();
  ASSERT (t->status == THREAD_BLOCKED);
  list_insert_ordered (&ready_list, &t->elem,cmp_priority,NULL);
  t->status = THREAD_READY;
  intr_set_level (old_level);
}

/* Returns the name of the running thread. */
const char *
thread_name (void) 
{
  return thread_current ()->name;
}

/* Returns the running thread.
   This is running_thread() plus a couple of sanity checks.
   See the big comment at the top of thread.h for details. */
struct thread *
thread_current (void) 
{
  struct thread *t = running_thread ();
  
  /* Make sure T is really a thread.
     If either of these assertions fire, then your thread may
     have overflowed its stack.  Each thread has less than 4 kB
     of stack, so a few big automatic arrays or moderate
     recursion can cause stack overflow. */
  ASSERT (is_thread (t));
  ASSERT (t->status == THREAD_RUNNING);

  return t;
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) 
{
  return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) 
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  intr_disable ();
  list_remove (&thread_current()->allelem);
  thread_current ()->status = THREAD_DYING;
  schedule ();
  NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) 
{
  struct thread *cur = thread_current ();
  enum intr_level old_level;
  
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  if (cur != idle_thread) 
	list_insert_ordered (&ready_list, &cur->elem,cmp_priority,NULL);
  cur->status = THREAD_READY;
  schedule ();
  intr_set_level (old_level);
}

/* Invoke function 'func' on all threads, passing along 'aux'.
   This function must be called with interrupts off. */
void
thread_foreach (thread_action_func *func, void *aux)
{
  struct list_elem *e;

  ASSERT (intr_get_level () == INTR_OFF);
  
  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      func (t, aux);
    }
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) 
{
  //使用高级调度，不能自己改变优先级
  if(thread_mlfqs)
	return;
  struct thread *curr = thread_current();
  //注意第三个参数要为true，因为要考虑到测test中会直接调用的时候都是用于考虑多次donate的情况的
  thread_set_priority_mlfq(thread_current(),new_priority,true);
}

int
thread_get_priority (void) 
{
  return thread_current ()->priority;
}

void
thread_set_nice (int nice) 
{
  thread_current()->nice=nice;
}

/* Returns the current thread's nice value. */
int
thread_get_nice () 
{
  return thread_current()->nice;
}
//TImer_FREQ = 100 , 返回时需要值 X 100,计算1s内的 load_avg 和 recent_cpu 
int
thread_get_load_avg (void) 
{
  return F2INT45(FMULI(load_avg,100));
}

int
thread_get_recent_cpu (void) 
{
  return F2INTTRIM(FMULI(thread_current()->recent_cpu,100));
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) 
{
  struct semaphore *idle_started = idle_started_;
  idle_thread = thread_current ();
  sema_up (idle_started);

  for (;;) 
    {
      /* Let someone else run. */
      intr_disable ();
      thread_block ();

      /* Re-enable interrupts and wait for the next one.

         The `sti' instruction disables interrupts until the
         completion of the next instruction, so these two
         instructions are executed atomically.  This atomicity is
         important; otherwise, an interrupt could be handled
         between re-enabling interrupts and waiting for the next
         one to occur, wasting as much as one clock tick worth of
         time.

         See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
         7.11.1 "HLT Instruction". */
      asm volatile ("sti; hlt" : : : "memory");
    }
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) 
{
  ASSERT (function != NULL);

  intr_enable ();       /* The scheduler runs with interrupts off. */
  function (aux);       /* Execute the thread function. */
  thread_exit ();       /* If function() returns, kill the thread. */
}

/* Returns the running thread. */
struct thread *
running_thread (void) 
{
  uint32_t *esp;

  /* Copy the CPU's stack pointer into `esp', and then round that
     down to the start of a page.  Because `struct thread' is
     always at the beginning of a page and the stack pointer is
     somewhere in the middle, this locates the curent thread. */
  asm ("mov %%esp, %0" : "=g" (esp));
  return pg_round_down (esp);
}

/* Returns true if T appears to point to a valid thread. */
static bool
is_thread (struct thread *t)
{
  return t != NULL && t->magic == THREAD_MAGIC;
}

/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority)
{
  ASSERT (t != NULL);
  ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
  ASSERT (name != NULL);

  memset (t, 0, sizeof *t);
  t->status = THREAD_BLOCKED;
  strlcpy (t->name, name, sizeof t->name);
  t->stack = (uint8_t *) t + PGSIZE;
  
  //若要使用高级调度
  if(thread_mlfqs)
  {
	t->recent_cpu=0;
	t->nice=0;
	renew_priority(t);
  }
  else
  {

     t->priority = t->old_priority = priority;
  }
  t->magic = THREAD_MAGIC;
  //设置当前进程是没被donate。
  t->donated  = false;
  //当前线程未被任何锁block
  t->blocked = NULL;
  list_init(&t->locks);
  
  list_push_back (&all_list, &t->allelem);
  
}

/* Allocates a SIZE-byte frame at the top of thread T's stack and
   returns a pointer to the frame's base. */
static void *
alloc_frame (struct thread *t, size_t size) 
{
  /* Stack data is always allocated in word-size units. */
  ASSERT (is_thread (t));
  ASSERT (size % sizeof (uint32_t) == 0);

  t->stack -= size;
  return t->stack;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
static struct thread *
next_thread_to_run (void) 
{
	//我们小组约定，ready_list里面的进程总是有序的（插入时就必须有序）
  if (list_empty (&ready_list))
    return idle_thread;
  else
    return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/* Completes a thread switch by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.  This function is normally invoked by
   thread_schedule() as its final action before returning, but
   the first time a thread is scheduled it is called by
   switch_entry() (see switch.S).

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function.

   After this function and its caller returns, the thread switch
   is complete. */
void
thread_schedule_tail (struct thread *prev)
{
  struct thread *cur = running_thread ();
  
  ASSERT (intr_get_level () == INTR_OFF);

  /* Mark us as running. */
  cur->status = THREAD_RUNNING;

  /* Start new time slice. */
  thread_ticks = 0;

#ifdef USERPROG
  /* Activate the new address space. */
  process_activate ();
#endif

  /* If the thread we switched from is dying, destroy its struct
     thread.  This must happen late so that thread_exit() doesn't
     pull out the rug under itself.  (We don't free
     initial_thread because its memory was not obtained via
     palloc().) */
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread) 
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
}

/* Schedules a new process.  At entry, interrupts must be off and
   the running process's state must have been changed from
   running to some other state.  This function finds another
   thread to run and switches to it.

   It's not safe to call printf() until thread_schedule_tail()
   has completed. */
static void
schedule (void) 
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) 
{
  static tid_t next_tid = 1;
  tid_t tid;

  lock_acquire (&tid_lock);
  tid = next_tid++;
  lock_release (&tid_lock);

  return tid;
}

/* Offset of `stack' member within `struct thread'.
   Used by switch.S, which can't figure it out on its own. */
uint32_t thread_stack_ofs = offsetof (struct thread, stack);
bool cmp_priority (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux){
     return list_entry(a,struct thread,elem)->priority > 
                list_entry(b,struct thread,elem)->priority;								 
}
void checkInvoke(struct thread* t,void* aux UNUSED){
	//因为checkInvoke直接在alllist中取的，所以要确保当前处理的进程是不在readylist中的
	//其实可以改进为另外增加一个 block_list,30号交时间紧迫，就没有增加
  if(t->status == THREAD_BLOCKED && t->ticks_block > 0){
     t->ticks_block --;
     //如果可以唤醒的话，就unblock
     if(t->ticks_block == 0){
       thread_unblock(t);
     }
  }
}
void thread_set_priority_mlfq(struct thread* curr, int newPriority,bool nest){
	  
	  //若使用的是BSD，那么不能让程序自己线程自己设置自己的优先级，因此直接block掉
     if(thread_mlfqs)
	   return;
	  
	  enum intr_level old = intr_disable();
	  //情况1： 当前的线程的优先级没有donate过得，那么像创建一样直接设置就好
	  if(!curr->donated){
		curr->priority = curr->old_priority = newPriority;
	  }//情况2：当前线程是接受了donate的，那么设置新的优先级就要看了
	  else if(nest){
		  if(curr->priority > newPriority){
			//情况2.1：	donate过的，但是现在不是因为donate而设置优先级
			//那么，若新的优先级比较小，人家本意应该是
			//设置原来的优先级的，所以当前的priority不变，免得之前donate失效
			curr->old_priority = newPriority;
		  }else{
			//情况2.2：	donate过，但是新的优先级比较大，那么直接设置为新的priority
			//，这是临时提高其优先级，为多次donate使用，和情况3一样
			curr->priority = newPriority;
		  }
	  }else//情况3： 已经donate过了，现在可能是再一次donate的情况，就不要更改old_priority了，只是临时改当前的优先级
		curr->priority = newPriority;
		
	if(list_entry(list_begin(&ready_list),struct thread,elem)->priority > newPriority)
	{
		thread_yield();
	}
	intr_set_level(old);
}