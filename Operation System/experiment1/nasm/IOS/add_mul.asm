%include 'functions.asm'
%include 'addition.asm'
%include 'read.asm'
%include 'mul.asm'

SECTION .data
msg		db		'Please input x and y: ',0h

SECTION .bss
input:		resb	512
flagX:		resb	4
flagY:		resb	4
lenX:		resb	4
lenY:		resb	4
x:			resb	256
y:			resb	256
tmp:		resb	256
resAdd:		resb	256
resTmpMul:	resb	512
resMul:		resb	512

SECTION .text
global	_start

_start:
	call read_a_line

	call getXY
	call extendx
	call extendy
	
	call add
	mov eax,resAdd
	call sprintLF

	call mul
	mov eax,resMul
	call sprintLF

	call quit
