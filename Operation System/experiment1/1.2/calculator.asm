
;------------------------------------------
; int atoi(Integer number)
; Ascii to integer function (atoi)
atoi:
    push    ebx             ; preserve ebx on the stack to be restored after function runs
    push    ecx             ; preserve ecx on the stack to be restored after function runs
    push    edx             ; preserve edx on the stack to be restored after function runs
    push    esi             ; preserve esi on the stack to be restored after function runs
    mov     esi, eax        ; move pointer in eax into esi (our number to convert)
    mov     eax, 0          ; initialise eax with decimal value 0
    mov     ecx, 0          ; initialise ecx with decimal value 0
 
.multiplyLoop:
    xor     ebx, ebx        ; resets both lower and uppper bytes of ebx to be 0
    mov     bl, [esi+ecx]   ; move a single byte into ebx register's lower half
    cmp     bl, 48          ; compare ebx register's lower half value against ascii value 48 (char value 0)
    jl      .finished       ; jump if less than to label finished
    cmp     bl, 57          ; compare ebx register's lower half value against ascii value 57 (char value 9)
    jg      .finished       ; jump if greater than to label finished
 
    sub     bl, 48          ; convert ebx register's lower half to decimal representation of ascii value
    add     eax, ebx        ; add ebx to our interger value in eax
    mov     ebx, 10         ; move decimal value 10 into ebx
    mul     ebx             ; multiply eax by ebx to get place value
    inc     ecx             ; increment ecx (our counter register)
    jmp     .multiplyLoop   ; continue multiply loop
 
.finished:
    mov     ebx, 10         ; move decimal value 10 into ebx
    div     ebx             ; divide eax by value in ebx (in this case 10)
    pop     esi             ; restore esi from the value we pushed onto the stack at the start
    pop     edx             ; restore edx from the value we pushed onto the stack at the start
    pop     ecx             ; restore ecx from the value we pushed onto the stack at the start
    pop     ebx             ; restore ebx from the value we pushed onto the stack at the start
    ret
;------------------------------------------
; void iprint(Integer number)
; Integer printing function (itoa)
iprint:
    push    eax             ; preserve eax on the stack to be restored after function runs
    push    ecx             ; preserve ecx on the stack to be restored after function runs
    push    edx             ; preserve edx on the stack to be restored after function runs
    push    esi             ; preserve esi on the stack to be restored after function runs
    mov     ecx, 0          ; counter of how many bytes we need to print in the end
 
divideLoop:
    inc     ecx             ; count each byte to print - number of characters
    mov     edx, 0          ; empty edx
    mov     esi, 10         ; mov 10 into esi
    idiv    esi             ; divide eax by esi
    add     edx, 48         ; convert edx to it's ascii representation - edx holds the remainder after a divide instruction
    push    edx             ; push edx (string representation of an intger) onto the stack
    cmp     eax, 0          ; can the integer be divided anymore?
    jnz     divideLoop      ; jump if not zero to the label divideLoop
 
printLoop:
    dec     ecx             ; count down each byte that we put on the stack
    mov     eax, esp        ; mov the stack pointer into eax for printing
    call    sprint          ; call our string print function
    pop     eax             ; remove last character from the stack to move esp forward
    cmp     ecx, 0          ; have we printed all bytes we pushed onto the stack?
    jnz     printLoop       ; jump is not zero to the label printLoop
 
    pop     esi             ; restore esi from the value we pushed onto the stack at the start
    pop     edx             ; restore edx from the value we pushed onto the stack at the start
    pop     ecx             ; restore ecx from the value we pushed onto the stack at the start
    pop     eax             ; restore eax from the value we pushed onto the stack at the start
    ret
 
 
;------------------------------------------
; void iprintLF(Integer number)
; Integer printing function with linefeed (itoa)
iprintLF:
    call    iprint          ; call our integer printing function
 
    push    eax             ; push eax onto the stack to preserve it while we use the eax register in this function
    mov     eax, 0Ah        ; move 0Ah into eax - 0Ah is the ascii character for a linefeed
    push    eax             ; push the linefeed onto the stack so we can get the address
    mov     eax, esp        ; move the address of the current stack pointer into eax for sprint
    call    sprint          ; call our sprint function
    pop     eax             ; remove our linefeed character from the stack
    pop     eax             ; restore the original value of eax before our function was called
    ret
 
 
;------------------------------------------
; int slen(String message)
; String length calculation function
slen:
    push    ebx
    mov     ebx, eax
 
nextchar:
    cmp     byte [eax], 0
    jz      finished
    inc     eax
    jmp     nextchar
 
finished:
    sub     eax, ebx
    pop     ebx
    ret
 
 
;------------------------------------------
; void sprint(String message)
; String printing function
sprint:
    push    edx
    push    ecx
    push    ebx
    push    eax
    call    slen
 
    mov     edx, eax
    pop     eax
 
    mov     ecx, eax
    mov     ebx, 1
    mov     eax, 4
    int     80h
 
    pop     ebx
    pop     ecx
    pop     edx
    ret
 
 
;------------------------------------------
; void sprintLF(String message)
; String printing with line feed function
sprintLF:
    call    sprint
 
    push    eax
    mov     eax, 0AH
    push    eax
    mov     eax, esp
    call    sprint
    pop     eax
    pop     eax
    ret
 
 
;------------------------------------------
; void exit()
; Exit program and restore resources
quit:
    mov     ebx, 0
    mov     eax, 1
    int     80h
    ret
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

