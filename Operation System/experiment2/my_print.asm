global printW
global printR

SECTION		.data
color_red:	db	1BH,'[31;1m',0
.len 		equ 	$ - color_red

color_white:	db	1BH,'[37;1m',0
.len		equ	$ - color_white

SECTION .text

printR:
	push rax
	push rdi
	push rsi
	push rdx
	
	mov rax,1
	mov rdi,1
	mov rsi,color_red
	mov rdx,color_red.len
	syscall
	
	pop rdx
	pop rsi
	pop rdi
	pop rax
	call print
	
	ret

printW:
	push rax
	push rdi
	push rsi
	push rdx
	
	mov rax,1
	mov rdi,1
	mov rsi,color_white
	mov rdx,color_white.len
	syscall
	
	pop rdx
	pop rsi
	pop rdi
	pop rax
	call print
	
	ret

print:
	push rax
	push rdi
	push rsi
	push rdx

	call slen

	mov rsi,rdi
	mov rdx,rax
	mov rax,1
	mov rdi,1
	syscall
	
	pop rdx
	pop rsi
	pop rdi
	pop rax
	ret

slen:
	mov rax,rdi
nextchar:
	cmp byte[rax],0
	jz finish
	inc rax
	jmp nextchar

finish:
	sub rax,rdi
	ret

