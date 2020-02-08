
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键: 把光标移到第一列
	换行键: 把光标前进到下一行
*/


#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);

PRIVATE int esc_enter;
PRIVATE u8* esc_mode_ptr;

/*======================================================================*
			   init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD) */

	int con_v_mem_size                   = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr      = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit        = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0) {
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	else {
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}

	set_cursor(p_tty->p_console->cursor);
	esc_enter=0;
}


/*======================================================================*
			   is_current_console
*======================================================================*/
PUBLIC int is_current_console(CONSOLE* p_con)
{
	return (p_con == &console_table[nr_current_console]);
}


/*======================================================================*
			   out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE* p_con, char ch)
{
	u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);//当前位置指针
	switch(ch) {
	case '\n':
	    if(is_esc_mode){
	        esc_enter=1;
	        int length = (p_vmem-esc_mode_ptr)/2;//esc模式中输入的长度
	        u8* head = (u8*)(V_MEM_BASE+p_con->original_addr*2);//正常模式下的头指针
	        while (head<esc_mode_ptr){
	            int is_match=1;
	            int tmp_cnt=0;
	            u8* cur = head;
	            while (tmp_cnt<length){
	                if(*cur!=*(esc_mode_ptr+tmp_cnt*2)){
	                    is_match=0;
                        break;
	                }
	                tmp_cnt++;
	                cur+=2;
	            }
	            cur=head;
	            tmp_cnt=0;
	            if(is_match){
	                while (tmp_cnt<length){
	                    *(cur+1)=ESC_CHAR_COLOR;
	                    tmp_cnt++;
	                    cur+=2;
	                }
	            }
	            head+=2;
	        }
	    }
	    else{
            if (p_con->cursor < p_con->original_addr +
                                p_con->v_mem_limit - SCREEN_WIDTH) {
                p_con->cursor = p_con->original_addr + SCREEN_WIDTH *
                                ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
            }
	    }
		break;
	case '\b':
	    if(!esc_enter){
            if (p_con->cursor > p_con->original_addr) {
                if(*(p_vmem-2)=='\0'&&*(p_vmem-1)==DEFAULT_CHAR_COLOR){ //退格遇到空字符
                    while (*(p_vmem-2)=='\0'&&*(p_vmem-1)==DEFAULT_CHAR_COLOR){
                        p_con->cursor--;
                        if(p_con->cursor = p_con->original_addr + SCREEN_WIDTH *
                                ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH))
                            break;//只退一个换行
                        p_vmem-=2;
                    }
                }
                else if(*(p_vmem-2)==' '&&*(p_vmem-1)==ESC_CHAR_COLOR){//TAB
                    int tab_cnt=4;
                    while (*(p_vmem-2)==' '&&*(p_vmem-1)==ESC_CHAR_COLOR&&tab_cnt>0){
                        p_con->cursor--;
                        *(p_vmem-2)='\0';
                        *(p_vmem-1)=DEFAULT_CHAR_COLOR;
                        p_vmem-=2;
                        tab_cnt--;
                    }
                }
                else{
                    p_con->cursor--;
                    *(p_vmem-2) = '\0';
                    *(p_vmem-1) = DEFAULT_CHAR_COLOR;
                }
            }
	    }
		break;
	    case '\t':
	        if(!esc_enter){
                int cur_loc=8;
                while (cur_loc!=0){
                    p_con->cursor++;
                    *p_vmem=' ';
                    *(p_vmem+1)=ESC_CHAR_COLOR;
                    p_vmem+=2;
                    cur_loc-=2;
                }
	        }
            break;
        case '\e':
            if(!is_esc_mode){
                is_esc_mode=1;
                esc_mode_ptr=(u8*)(V_MEM_BASE+p_con->cursor*2);
            }
            else{
                if(esc_enter){
                    is_esc_mode=0;
                    esc_enter=0;
                    while (p_vmem>esc_mode_ptr){
                        p_vmem=(u8*)(V_MEM_BASE+p_con->cursor*2);
                        *p_vmem='\0';
                        *(p_vmem+1)=DEFAULT_CHAR_COLOR;
                        p_con->cursor--;
                    }
                    p_con->cursor++;
                    u8* cur_mem_ptr = (u8*)(V_MEM_BASE+p_con->original_addr*2);
                    while (cur_mem_ptr<p_vmem){
                        if(*(cur_mem_ptr)==' '&&*(cur_mem_ptr+1)==ESC_CHAR_COLOR){
                            cur_mem_ptr+=2;
                            continue;
                        }
                        *(cur_mem_ptr+1)=DEFAULT_CHAR_COLOR;
                        cur_mem_ptr+=2;
                    }
                }
            }
            break;
	default:
	    if(!esc_enter){
            if (p_con->cursor <
                p_con->original_addr + p_con->v_mem_limit - 1) {
                *p_vmem++ = ch;
                if(!is_esc_mode){
                    *p_vmem++ = DEFAULT_CHAR_COLOR;
                }
                else{
                    *p_vmem++ = ESC_CHAR_COLOR;
                }
                p_con->cursor++;
            }
	    }
		break;
	}

	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_con, SCR_DN);
	}

	flush(p_con);
}

/*======================================================================*
                           clean_screen
*======================================================================*/
PUBLIC void clean_screen(CONSOLE* p_con){
    u8* p_vmem;
    while (p_con->cursor>p_con->original_addr){
        p_vmem=(u8*)(V_MEM_BASE+p_con->cursor*2);
        p_con->cursor--;
        *(p_vmem-2)='\0';
        *(p_vmem-1)=DEFAULT_CHAR_COLOR;
    }
    p_con->cursor=p_con->original_addr;
    p_con->current_start_addr=p_con->original_addr;
    flush(p_con);
}

/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE* p_con)
{
        set_cursor(p_con->cursor);
        set_video_start_addr(p_con->current_start_addr);
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}



/*======================================================================*
			   select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console)	/* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES)) {
		return;
	}

	nr_current_console = nr_console;

	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}

/*======================================================================*
			   scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN) {
		if (p_con->current_start_addr + SCREEN_SIZE <
		    p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}

