
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void Reader_A();
void Reader_B();
void Reader_C();
void Writer_D();
void Writer_E();
void Monitor_F();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void init_clock();

/* keyboard.c */
PUBLIC void init_keyboard();

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);

/* console.c */
PUBLIC void out_char(CONSOLE* p_con, char ch, char color);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);

/* 以下是系统调用相关 */

/* proc.c */
PUBLIC int	sys_get_ticks();
PUBLIC int	sys_process_sleep(int milli_sec);  
PUBLIC int	sys_disp_str(char* str);
PUBLIC int	sys_P(SEMAPHORE* s);
PUBLIC int	sys_V(SEMAPHORE* s);


/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC  void	system_process_sleep(int milli_sec);
PUBLIC  void	system_disp_str(char* str);
PUBLIC	void	system_P(SEMAPHORE* s);
PUBLIC  void	system_V(SEMAPHORE* s);

