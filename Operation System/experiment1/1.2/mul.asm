SECTION	.data
msg	db	'Please input x and y: ',0h
dump	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
res:	db	"0"
SECTION	.bss
tmp:	resb	256
X:	resb	256
Y:	resb	256

SECTION	.text
global _start

_start:
	call	getInput
multiplication:
	mov	ecx,X
	mov	edi,Y
	push	eax
	push	edx
	push	ebx
	mov	edx,res
	mov	ebx,0
	mov	esi,0
	
	cmp	byte[ecx],48
	jne	beginMul
	mov	eax,X
	call	sprintLF
	jmp	quitMul
beginMul:
	mov	bl,byte[edi+esi]
mulAddLoop:
	cmp	bl,48		;count==0?
	je	moveRight
	call	add_mul
	dec	bl
	jmp	mulAddLoop
moveRight:
	inc	esi
	cmp	byte[edi+esi],10
	je	endMul
	mov	byte[edx+esi],48	;res+'0'
	jmp	beginMul
endMul:
	pop	ebx
	pop	edx
	pop	eax
	mov	eax,res
removeZeroLoopM:
	dec	eax
	cmp	byte[eax],0
	jz	printM
	jmp	removeZeroLoopM
printM:
	inc	eax
	call	sprintLF
	
quitMul:
	mov	eax,1
	mov	ebx,0
	int	80h
	

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

add_mul:
	push 	eax
	push 	ebx
	push 	edx
	push 	edi
	push 	esi
	mov 	ebx,0
	mov 	esi,0
	mov 	edx,res
	mov 	edi,X

calLenX:
	inc 	ebx
	cmp 	byte[edi+ebx],10
	je 	calLenR
	jmp 	calLenX

calLenR:
	inc 	esi
	cmp 	byte[edx+esi],0
	je 	beginAdd
	jmp 	calLenR

beginAdd:
	dec 	esi
	dec 	ebx
addLoop:
	mov 	al,byte[edi+ebx]
	add 	byte[edx+esi],al
	cmp 	byte[edx+esi],96
	jb 	checkSum
	sub 	byte[edx+esi],48
checkSum:
	cmp 	byte[edx+esi],57
	jna 	else
	sub 	byte[edx+esi],10
	dec 	esi
	inc 	byte[edx+esi]
	cmp 	byte[edx+esi],1
	jne 	moreCarry
	add 	byte[edx+esi],48
moreCarry:
	inc 	esi
else:
	dec 	esi
	dec 	ebx
	cmp 	byte[edx+esi],0
	jne 	addLoop
	cmp 	byte[edi+ebx],0
	jne 	addLoop


	pop 	esi
	pop 	edi
	pop 	edx
	pop 	ebx
	pop 	eax
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

	

