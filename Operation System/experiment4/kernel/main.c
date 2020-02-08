
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#define	READ_FIRST 1
#define NUM_READERS 2
#define NUM_WRITERS 1

PRIVATE  SEMAPHORE	semaphores[4]={{1,0},{1,0},{NUM_WRITERS,0},{NUM_READERS,0}};
//S,rmutex,wmutex,read;
PRIVATE  SEMAPHORE*	first_semaphore;

PRIVATE  int readcount;

PRIVATE  void init_pro(){
	readcount=0;
	first_semaphore=semaphores;
}

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");
	init_pro();
	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 1;
	proc_table[1].ticks = proc_table[1].priority = 1;
	proc_table[2].ticks = proc_table[2].priority = 1;
	proc_table[3].ticks = proc_table[3].priority = 1;
	proc_table[4].ticks = proc_table[4].priority = 1;
	proc_table[5].ticks = proc_table[5].priority = 1;
	proc_table[6].ticks = proc_table[6].priority = 1;

	proc_table[0].wait = proc_table[0].sleep_ticks = 0;
	proc_table[1].wait = proc_table[1].sleep_ticks = 0;
	proc_table[2].wait = proc_table[2].sleep_ticks = 0;
	proc_table[3].wait = proc_table[3].sleep_ticks = 0;
	proc_table[4].wait = proc_table[4].sleep_ticks = 0;
	proc_table[5].wait = proc_table[5].sleep_ticks = 0;
	proc_table[6].wait = proc_table[6].sleep_ticks = 0;

	proc_table[0].next = 0;
	proc_table[1].next = 0;
	proc_table[2].next = 0;
	proc_table[3].next = 0;
	proc_table[4].next = 0;
	proc_table[5].next = 0;
	proc_table[6].next = 0;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
    	init_keyboard();

	restart();

	while(1){}
}

/*======================================================================*
                               Reader_A
 *======================================================================*/
void Reader_A()
{
	while (1) {
		system_P(first_semaphore+3);
		
		if(!READ_FIRST)system_P(first_semaphore);
		system_P(first_semaphore+1);
		if (readcount == 0)system_P(first_semaphore+2);
		readcount ++;
		system_V(first_semaphore+1);
		if(!READ_FIRST)system_V(first_semaphore);
			
		system_disp_str("Reader_A begins to read.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_A is reading.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_A finishes reading.\n");
		system_P(first_semaphore+1);
		readcount--;
		if(readcount == 0)system_V(first_semaphore+2);
		system_V(first_semaphore+1);
		
		system_V(first_semaphore+3);

		system_process_sleep(20000);
	}
}

/*======================================================================*
                               Reader_B
 *======================================================================*/
void Reader_B()
{
	while (1) {
		system_P(first_semaphore+3);
		
		if(!READ_FIRST)system_P(first_semaphore);
		system_P(first_semaphore+1);
		if (readcount == 0)system_P(first_semaphore+2);
		readcount ++;
		system_V(first_semaphore+1);
		if(!READ_FIRST)system_V(first_semaphore);
			
		system_disp_str("Reader_B begins to read.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_B is reading.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_B is reading.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_B finishes reading.\n");
		system_P(first_semaphore+1);
		readcount--;
		if(readcount == 0)system_V(first_semaphore+2);
		system_V(first_semaphore+1);
		
		system_V(first_semaphore+3);

		system_process_sleep(20000);
	}
}

/*======================================================================*
                               Reader_C
 *======================================================================*/
void Reader_C()
{
	while (1) {
		system_P(first_semaphore+3);
		
		if(!READ_FIRST)system_P(first_semaphore);
		system_P(first_semaphore+1);
		if (readcount == 0)system_P(first_semaphore+2);
		readcount ++;
		system_V(first_semaphore+1);
		if(!READ_FIRST)system_V(first_semaphore);
			
		system_disp_str("Reader_C begins to read.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_C is reading.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_C is reading.\n");
		system_process_sleep(10000);
		system_disp_str("Reader_C finishes reading.\n");
		system_P(first_semaphore+1);
		readcount--;
		if(readcount == 0)system_V(first_semaphore+2);
		system_V(first_semaphore+1);
		
		system_V(first_semaphore+3);

		system_process_sleep(20000);
	}
}

/*======================================================================*
                               Writer_D
 *======================================================================*/
void Writer_D()
{
	while (1) {

		if(!READ_FIRST)system_P(first_semaphore);
		system_P(first_semaphore+2);
		system_disp_str("Writer_D begins to write.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_D is writing.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_D is writing.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_D finishes writing.\n");
		system_V(first_semaphore+2);
		if(!READ_FIRST)system_V(first_semaphore);
		system_process_sleep(20000);
	}
}

/*======================================================================*
                               Writer_E
 *======================================================================*/
void Writer_E()
{
	while (1) {

		if(!READ_FIRST)system_P(first_semaphore);
		system_P(first_semaphore+2);
		system_disp_str("Writer_E begins to write.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_E is writing.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_E is writing.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_E is writing.\n");
		system_process_sleep(10000);
		system_disp_str("Writer_E finishes writing.\n");
		system_V(first_semaphore+2);
		if(!READ_FIRST)system_V(first_semaphore);
		system_process_sleep(20000);
	}
}

void get_num_read(char* res, int x){
	int len = 0;
	while(x > 0){
		res[len ++] = x % 10 + '0';
		x /= 10;
	}
	for (int i = 0, j = len - 1; i < j; i ++, j --){
		char temp = res[i];
		res[i] = res[j];
		res[j] = temp;
	}
	res[len] = '\0';
}

void Monitor_F()
{
	//1
	while(1){
		if(readcount > 0){
			system_disp_str("Current: reading; num of readers:  ");
			char num_read[10];
			get_num_read(num_read, readcount);
			system_disp_str(num_read);
			system_disp_str("\n");
			system_process_sleep(20000);
		}else{
			system_disp_str("Current:  writing.\n");
			system_process_sleep(20000);
		}
	}
}
