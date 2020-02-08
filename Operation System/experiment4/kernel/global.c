
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


PUBLIC	PROCESS	proc_table[NR_TASKS];

PUBLIC	TASK	task_table[NR_TASKS] = {
	{task_tty, STACK_SIZE_TTY, "tty"},
	{Reader_A, STACK_SIZE_Reader_A, "Reader_A"},
	{Reader_B, STACK_SIZE_Reader_B, "Reader_B"},
	{Reader_C, STACK_SIZE_Reader_C, "Reader_C"},
	{Writer_D, STACK_SIZE_Writer_D, "Writer_D"},
	{Writer_E, STACK_SIZE_Writer_E, "Writer_E"},
	{Monitor_F, STACK_SIZE_Monitor_F, "Monitor_F"}
};


PUBLIC	char		task_stack[STACK_SIZE_TOTAL];

PUBLIC	TTY		tty_table[NR_CONSOLES];
PUBLIC	CONSOLE		console_table[NR_CONSOLES];

PUBLIC	irq_handler	irq_table[NR_IRQ];

PUBLIC	system_call	sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_process_sleep, sys_disp_str,  sys_P, sys_V};

//color
PUBLIC  char		colors[NR_COLOR] = {WHITE_COLOR, RED_COLOR, GREEN_COLOR, BLUE_COLOR, X_COLOR, Y_COLOR};
