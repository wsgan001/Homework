/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init (struct semaphore *sema, unsigned value) 
{
  ASSERT (sema != NULL);

  sema->value = value;
  
  //初始化新添加的几个元素
  list_init (&sema->waiters);
  sema->lock_priority = -1;
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down (struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  //线程一直等待另外的线程signal，否则不断入列，注意
  while (sema->value == 0) 
    {
	//能够运行这段代码的线程肯定不是在ready_list里面的。所以不用担心重复插入
      list_insert_ordered(&sema->waiters,&thread_current()->elem,cmp_priority,NULL);
      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema) 
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0) 
    {
      sema->value--;
      success = true; 
    }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema) 
{
  struct thread *wake_up,*curr;
  
  enum intr_level old_level;

  ASSERT (sema != NULL);
  //wake是信号量signal后将会唤醒的线程
  wake_up = NULL;
  curr = thread_current();
  
  old_level = intr_disable ();
  
  //当前信号量
  //由于一个线程可能会同时获得几个locks，因此，可能在处理另外的锁的时候会得到别的线程
  //donate到的优先级，这时候sema->waiters里面的线程可能不是有序的了，当然，在将线程插入到
  //waiters之前还是要用insert_order来尽量减少sort时间（毕竟有些不会获得几个锁）
  //注意，约定thread_unblock不会马上抢占CPU，只是会改变线程状态而已，下次schedule生效
  if (!list_empty (&sema->waiters)) 
  {
	  list_sort(&sema->waiters,cmp_priority,NULL);
	  wake_up = list_entry(list_pop_front(&sema->waiters),struct thread,elem);
	  thread_unblock(wake_up);
  }
  sema->value++;
  //当前线程应该是将其unblock后，有可能当前执行sema_up的线程优先级是比较低的，就要放弃CPU了
  //如果只有一个锁，这个情况是不存在的，因为当前的肯定是拿到了最高优先级的（donate来的），但是，
  //由于有多个锁，可能别的进程会被donate更高的priority(然后被另外的锁block掉了)
  if(wake_up != NULL && wake_up->priority > curr->priority){
	thread_yield();
  }
  intr_set_level (old_level);
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void) 
{
  struct semaphore sema[2];
  int i;

  printf ("Testing semaphores...");
  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++) 
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
  printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_) 
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++) 
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);

  lock->holder = NULL;
  sema_init (&lock->semaphore, 1);
  lock->lock_priority = -1;
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));
  //My code
  //curr是当前正在运行的，也是想要获取锁的进程
  //thrd则是获得锁的进程，注意下面有一个过程，使得thrd成为最终让curr停止的根本原因，
  //因为存在需要嵌套donate的情况
  
  struct thread *curr,*thrd;
  //当前进程正在争的锁
  struct lock *another;
  
  enum intr_level old_level;
  old_level = intr_disable();
  curr = thread_current();
  thrd = lock->holder;
  //先设置curr->blocked,若能够成功取得锁就会去掉
  curr->blocked = another = lock;
  
  //下面将curr的priority给donate给thrd，会出现嵌套的情况，具体看文档
  //若是mlfqs，则不会出现donate的情况，所有都是根据recent_cpu和nice算的，就可以跳过了
  while(!thread_mlfqs && thrd != NULL && thrd->priority < curr->priority)
  {
  //My code
   thrd->donated = true;
   thread_set_priority_mlfq(thrd,curr->priority,true);
   if(another -> lock_priority < curr->priority)
		{
		 another->lock_priority = curr->priority;
		 list_remove(&another->holder_elem);
		 list_insert_ordered(&thrd->locks,&another->holder_elem,cmp_priority2,NULL);
		}
   if(thrd->status == THREAD_BLOCKED && thrd->blocked != NULL)
   {
	   another = thrd->blocked;
	   thrd = thrd->blocked->holder; 
   }
   else
	break;
  }
  //这时候明显sema_down是不可能有结果的，所以线程会一直等待直到能够拿到锁
  sema_down (&lock->semaphore);
  lock->holder = curr;
  lock->lock_priority = curr->priority;
  //printf("\nlock3456: %x holder:%s, priority: %d  %s\n",lock,curr->name,curr->priority,thread_current()->name);
  curr->blocked = NULL;
  //更新当前线程获得的锁的链表
  list_insert_ordered(&lock->holder->locks,&lock->holder_elem,cmp_priority2,NULL);
  intr_set_level(old_level);
}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  success = sema_try_down (&lock->semaphore);
  if (success)
    lock->holder = thread_current ();
  return success;
}

/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release (struct lock *lock) 
{
  //My code
  struct thread * curr;
  struct list_elem *l;
  struct lock* another;
  enum intr_level old_level;
 
 
  //My code
  curr = thread_current();
  //if(! thread_mlfqs){
	ASSERT(curr->blocked == NULL)
  //}
  
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));
  old_level = intr_disable();
  //printf("\nlocks   %x\n",lock);
  //当前锁暂时被设置为NULL，在下一个sema_down的时候，就是在等待程序会重新设置holder
  lock->holder = NULL;
  //将当前lock从线程的locklist 中去掉
  list_remove(&lock->holder_elem);
  lock->lock_priority = PRI_MIN - 1;
  //signal，并且准备交出cpu
  sema_up (&lock->semaphore);
  //处理完了这个锁，开始处理本线程其他的锁
  if(list_empty(&curr->locks))
  {
	  curr->donated = false;
	  //不设置old_priority,因为donated已经为false了，只要设置priority就行
	  thread_set_priority(curr->old_priority);
  }else{
	  l = list_front(&curr->locks);
	  another = list_entry(l,struct lock,holder_elem);
	  //if(another == 0xc000ef10){
		// while(another->lock_priority == 34); 
		 //}
		//if(another == 0xc000eee0){
			// printf("\n%d %d\n",another->lock_priority,curr->priority);
			 //}
	  if(another->lock_priority != PRI_MIN - 1){
		  //another是当前的curr拿到的锁的最后一个，那个锁记录着线程curr拿到锁的时候的优先级
	     thread_set_priority_mlfq(curr,another->lock_priority,false);
	     
	  }else{
		  //没有锁了，直接设置为最最最初的优先级就好，就是没有donate那一个
		thread_set_priority(curr->old_priority);
	  }
  }
  intr_set_level(old_level);
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock) 
{
  ASSERT (lock != NULL);

  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem 
  {
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);
  list_init (&cond->waiters);
}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock) 
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));
  
  sema_init (&waiter.semaphore, 0);
    waiter.semaphore.lock_priority = thread_current()->priority;
    
  list_insert_ordered(&cond->waiters,&waiter.elem,cmp_priority_sema_elem,NULL);
  //list_push_back(&cond->waiters,&waiter.elem);
  lock_release (lock);
  //等待这个cond被某一次signal的时候，有signal线程从cond->waiter中sema_up
  sema_down (&waiter.semaphore);
  
  //马上获取这个锁。
  lock_acquire (lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters)) 
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}
bool cmp_priority2 (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux){
     return list_entry(a,struct lock,holder_elem)->lock_priority > 
                list_entry(b,struct lock,holder_elem)->lock_priority;								 
}
bool cmp_priority_sema_elem (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux){
     return list_entry(a,struct semaphore_elem,elem)->semaphore.lock_priority > 
                list_entry(b,struct semaphore_elem,elem)->semaphore.lock_priority;								 
}
