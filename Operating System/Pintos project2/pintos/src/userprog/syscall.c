#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/input.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include <list.h>
#include "userprog/process.h"
#include <inttypes>
typedef int pid_t;
//typedef unsigned char   uint8_t;
static void syscall_handler (struct intr_frame *);
static int sys_get_arg3(struct intr_frame *f);
static int sys_get_arg2(struct intr_frame *f);
static int sys_get_arg1(struct intr_frame *f);

static int sys_write (int fd, const void *buffer, unsigned size);//
static int sys_halt(void);//
static int sys_create(const char* file,unsigned initial_size);//
static int sys_open(const char*file);//
static int sys_close(int fd);//
static int sys_read(int fd,void *buffer,unsigned size);//
static int sys_exec(const char *cmd);//
static int sys_wait(pid_t pid);//
static int sys_filesize(int fd);//
static int sys_tell(int fd);//
static int sys_seek(int fd,unsigned pos);//
static int sys_remove(const char *file);//
void sys_exit (int status);
static int fid=1;

static struct file* find_file_by_fd(int fd);//
static struct fd_elem *find_fd_elem_by_fd(int fd);//
static int alloc_fid(void);//
static struct fd_elem *find_fd_elem_in_curr_thread(int fd);

static struct lock file_lock;
static struct list file_list;
static void syscall_handler (struct intr_frame *f /*UNUSED*/) ;

struct fd_elem
{
	//char *file_name;
	int fd;
	struct file *file;
	struct list_elem file_elem;
	struct list_elem thread_elem;
};

void syscall_init (void) 
{
	list_init(&file_list);
	lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


//????
static void
syscall_handler (struct intr_frame *f /*UNUSED*/) 
{
   if (!(is_user_vaddr(f->esp)))
		sys_exit(-1);
	int sysnum=-1;
	sysnum=*(int *)(f->esp);
	int ret;
	switch (sysnum)
	{
		case SYS_HALT:ret=sys_halt();			break;
		case SYS_EXIT:sys_exit(sys_get_arg1(f));break;
 		case SYS_EXEC:ret=sys_exec(sys_get_arg1(f));break;
		case SYS_WAIT:ret=sys_wait(sys_get_arg1(f));break;
 		case SYS_OPEN:ret=sys_open(sys_get_arg1(f));break;
  		case SYS_TELL:ret=sys_tell(sys_get_arg1(f));break;
  		case SYS_CLOSE:ret=sys_close(sys_get_arg1(f));break;
 		case SYS_REMOVE:ret=sys_remove(sys_get_arg1(f));break;
 		case SYS_FILESIZE:ret=sys_filesize(sys_get_arg1(f));break;
 		case SYS_SEEK:ret=sys_seek(sys_get_arg1(f),sys_get_arg2(f));break;
  		case SYS_CREATE:ret=sys_create(sys_get_arg1(f),sys_get_arg2(f));break;
 		case SYS_READ:ret=sys_read(sys_get_arg1(f),sys_get_arg2(f),sys_get_arg3(f));break;
 		case SYS_WRITE:ret=sys_write(sys_get_arg1(f),sys_get_arg2(f),sys_get_arg3(f));break;
 		default:break;
	}
	f->eax=ret;
}

int sys_get_arg3(struct intr_frame *f)
{
	int *p;
	p=f->esp;
	if (!(is_user_vaddr(p+3)))
		sys_exit(-1);
	int ret;
	ret=*(p+3);
	return ret;
}

int sys_get_arg2(struct intr_frame *f)
{
	int *p;
	p=f->esp;
	if (!(is_user_vaddr(p+2)))
		sys_exit(-1);
	int ret;
	ret=*(p+2);
	return ret;
}

int sys_get_arg1(struct intr_frame *f)
{
	int *p;
	p=f->esp;
	if (!(is_user_vaddr(p+1)))
		sys_exit(-1);
	int ret;
	ret=*(p+1);
	return ret;
}

int sys_write (int fd, const void *buffer, unsigned length)
{
	struct file *f;
	int ret;	
	ret=-1;
	lock_acquire(&file_lock);
	
	if (fd==STDOUT_FILENO)
	{
	putbuf(buffer,length);
	}
	else if (!is_user_vaddr(buffer)||!is_user_vaddr(buffer+length))
	{
			
		lock_release(&file_lock);
		sys_exit(-1);
	}
	else
	{	
		f=find_file_by_fd(fd);
		if (!f)
		{
			lock_release(&file_lock);
			return ret;
		}
		ret=file_write(f,buffer,length);
	}
lock_release(&file_lock);
return ret;
}

void sys_exit (int status)
{
	struct thread *t,*p;
	struct list_elem *l;
	int f;
	t=thread_current();
	while (!list_empty(&t->files))
	{
		
		l=list_begin(&t->files);
		f=list_entry(l,struct fd_elem ,thread_elem)->fd;
		sys_close(f);
	}
	p=t->parent;
	p->ret_status=status;
	printf("%s: exit(%d)\n",t->name,status);
	thread_exit();
}

static 
int sys_create(const char* file,unsigned size)
{
    if (file==NULL)
	{
		sys_exit(-1);
	}
	if (strlen(file)==0)
	{
	    return 0;
	}
	return filesys_create (file, size) ;
}

int sys_exec(const char *cmd)
{
	if (!cmd||!is_user_vaddr(cmd))
	return -1;
	else
	return process_execute(cmd);
}

int sys_wait(pid_t pid)
{
	int t=process_wait(pid);
	return t;
}

int sys_halt(void)
{
		shutdown_power_off();
		return 0;
}

static struct file* find_file_by_fd(int fd)
{
	struct fd_elem *ret;
	ret=find_fd_elem_by_fd(fd);
	if (!ret)
	return NULL;
	return ret->file;
}

static struct fd_elem *find_fd_elem_by_fd(int fd)
{
	struct fd_elem *ret;
	struct list_elem *l;
	for (l=list_begin(&file_list);l!=list_end(&file_list);l=list_next(l))
	{
		ret=list_entry(l,struct fd_elem,file_elem);
		if (ret->fd==fd)
		return ret;
	}
}

static int sys_close(int fd)
{
	struct fd_elem *f;
	int ret;
	int p;
	f=find_fd_elem_in_curr_thread(fd);
	p=f->fd;

	if (!f||p!=fd)
	sys_exit(-1);
	file_close(f->file);
	list_remove(&f->file_elem);
	list_remove(&f->thread_elem);
	free(f);
	return 0;
}

static int sys_remove(const char *file)
{
	if (file==NULL)
	return false;
	if (!is_user_vaddr(file))
	sys_exit(-1);
	return filesys_remove(file);
}

static int sys_filesize(int fd)
{
	struct file *f;
	f=find_file_by_fd(fd);
	if (!f)
	return -1;
	return file_length(f);
}

static int sys_open(const char*file)
{
	struct file*f;
	struct fd_elem *fe;
	struct thread*t=thread_current();
	if (file==NULL){
		return -1;
	}
	if (!is_user_vaddr(file))
	sys_exit(-1);
	f=filesys_open(file);
	if (f==NULL) return -1;
	fe =(struct fd_elem*)malloc(sizeof(struct fd_elem));
	if (!fe)
	{
		file_close(f);
		return -1;
	}
	
	fe->file=f;
	fe->fd=alloc_fid();
	list_push_back(&file_list,&(fe->file_elem));
	list_push_back (&t->files, &(fe->thread_elem));
    return fe->fd;
}

static int sys_read(int fd,void *buffer,unsigned size)
{
    struct file *f;
    unsigned i;
    int ret;

    ret=-1;
    lock_acquire(&file_lock);
    if (buffer==NULL)
    {
	    lock_release(&file_lock);
	   sys_exit(-1);
    }

    if (fd==STDIN_FILENO)
    {
	    for (i=0;i!=size;i++)
	    *(unsigned char*)(buffer+i)=input_getc();
	    lock_release(&file_lock);
	    return ret;
    }	
    else if(!is_user_vaddr(buffer)||!is_user_vaddr(buffer+size))
    {
	    lock_release(&file_lock);
	    sys_exit(-1);
    }
    else  
    {
	    f=find_file_by_fd(fd);
	    if (!f)
	    {
		    lock_release(&file_lock);
		    return ret;
	    }
	    ret=file_read(f,buffer,size);
    }
    lock_release(&file_lock);
    return ret;
}

static int sys_tell(int fd)
{
    struct file *f;
    f=find_file_by_fd(fd);
    if (!fd) return -1;
    return file_tell(f);
}

static int alloc_fid(void)
{
	return ++fid;
}

static int sys_seek(int fd,unsigned pos)
{
	struct file *f;
	f=find_file_by_fd(fd);
	if (!f)
	return -1;
	file_seek(f,pos);
	return 0;
}


static struct fd_elem *find_fd_elem_in_curr_thread(int fd)
{
	struct fd_elem *ret;
	struct thread *t=thread_current();
	struct list_elem *l;
	for (l=list_begin(&(t->files));l!=list_end(&(t->files));l=list_next(l))
	{
		ret=list_entry(l,struct fd_elem,thread_elem);
		if (ret->fd==fd)
		return ret;
	}
}
