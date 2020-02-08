SECTION .data
msg	db	'Please input x and y: ',0h

SECTION .bss
X:	resb	256
Y:	resb	256
lenX:	resb	4
lenY:	resb	4


SECTION .text
global _start

_start:
	call	getInput
	call	calLenX
	call	calLenY
	call	addition
	
	call	quit
	
getInput:
	push	eax
	push	edx
	push	ecx
	mov	eax,msg
	call	sprintLF
	
	mov	edx,256
	mov	ecx,X
	mov	ebx,0
	mov	eax,3
	int	80h
	
	mov	edx,256
	mov	ecx,Y
	mov	ebx,0
	mov	eax,3
	int	80h
	
	pop	ecx
	pop	edx
	pop	eax
	ret

calLenX:
	push	ecx
	mov	ecx,0
.loopLenX:
	cmp	byte[X+ecx],10
	je	.finishLenX
	inc	ecx
	jmp	.loopLenX
.finishLenX:
	dec	ecx
	mov	[lenX],ecx
	pop	ecx
	ret

calLenY:
	push	ecx
	mov	ecx,0
.loopLenY:
	cmp	byte[Y+ecx],10
	je	.finishLenY
	inc	ecx
	jmp	.loopLenY
.finishLenY:
	dec	ecx
	mov	[lenY],ecx
	pop	ecx
	ret
	
addition:
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi	
	mov	esi,[lenX]
	mov	edi,[lenY]
	mov	ebx,X
	mov	ecx,Y
addLoop:
	mov	al,byte[ecx+edi]
	add	byte[ebx+esi],al
	cmp	byte[ebx+esi],96
	jb	checkSum
	sub	byte[ebx+esi],48
checkSum:
	cmp	byte[ebx+esi],57	
	jna	else
	sub	byte[ebx+esi],10	;sum>=10
	dec	esi			;forCarry
	inc	byte[ebx+esi]
	cmp	byte[ebx+esi],1		;1XXX?
	jne	moreCarry
	add	byte[ebx+esi],48	;noMoreCarry
moreCarry:
	inc	esi
else:
	dec	esi
	dec	edi
	cmp	byte[ecx+edi],0
	jne	addLoop
	cmp	byte[ebx+esi],0
	jne	addLoop

	mov	eax,X
removeZeroLoop:
	dec	eax
	cmp	byte[eax],0
	jz	finishAddition
	jmp	removeZeroLoop
finishAddition:
	inc	eax
	call	sprint
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret


;------------------------------------------
slen:
	push ebx
	mov ebx,eax
	
nextchar:
	cmp byte[eax],0
	jz finished
	inc eax
	jmp nextchar
	
finished:
	sub eax,ebx
	pop ebx
	ret

sprint:
	push edx
	push ecx
	push ebx
	push eax
	call slen

	mov edx,eax
	pop eax
	
	mov ecx,eax
	mov ebx,1
	mov eax,4
	int 80h
	
	pop ebx
	pop ecx
	pop edx
	ret

sprintLF:
	call sprint
	
	push eax
	mov eax,0AH
	push eax
	mov eax,esp
	call sprint
	pop eax
	pop eax
	ret

quit:
	mov ebx,0
	mov eax,1
	int 80h
	ret

