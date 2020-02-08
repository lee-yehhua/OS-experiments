read_a_line:
	push eax
	push edx
	push ecx
	
	mov eax,msg
	call sprintLF

	mov edx,512
	mov ecx,input
	mov ebx,0
	mov eax,3
	int 80h

	pop ecx
	pop edx
	pop eax
	ret
	
getXY:
	push esi
	push ecx
	push ebx
	push eax
	push edx
	push edi
	mov esi,input
	mov edx,0
	mov edi,0
	mov ecx,0
	mov eax,0
	mov [flagX],eax
	mov [flagY],eax

.pushLoop:
	mov bl,[esi+ecx]	;mov a single byte into ebx register's lower half
	cmp bl,0Ah		;to see if the input ends
	jz .popLoopY		;jump to if zero
	sub bl,0		;
	push ebx
	inc ecx
	jmp .pushLoop

.popLoopY:
	pop eax
	dec ecx
	cmp eax,32
	jz .popLoopX
	cmp eax,45
	jz .setFlagY
	
	;sub eax,48
	;mov [y+edi],eax
	push eax
	mov bl,[esp]
	mov [y+edi],bl
	pop eax

	inc edi

	jmp .popLoopY

.setFlagY:
	pop eax
	dec ecx
	mov eax,1
	mov [flagY],eax
	jz .popLoopX

.popLoopX:
	cmp ecx,0
	jz .finishedXY
	pop eax
	dec ecx
	cmp eax,45
	jz .setFlagX
	
	;sub eax,48
	
	push eax
	mov bl,[esp]
	mov [x+edx],bl
	pop eax

	inc edx

	jmp .popLoopX

.setFlagX:
	mov eax,1
	mov [flagX],eax
	jz .finishedXY

.finishedXY:
	mov [lenY],edi
	mov [lenX],edx
	pop edi
	pop edx
	pop eax
	pop ebx
	pop ecx
	pop esi
	ret

extendx:
	push ecx
	push eax
	push ebx

	mov bl,[lenX]
	sub bl,0
	mov ecx,ebx

.zeroLoopx:
	cmp	ecx,50
	jge	.finishedZEROx
	
	mov	eax,48
	mov	[x+ecx],eax
	inc ecx

	jmp .zeroLoopx

.finishedZEROx:
	mov [lenX],ecx
	pop ebx
	pop eax
	pop ecx
	ret


extendy:
	push ecx
	push eax
	push ebx

	mov bl,[lenY]
	sub	bl,0
	mov ecx,ebx

.zeroLoopy:
	cmp	ecx,50
	jge	.finishedZEROy
	
	mov	eax,48
	mov	[y+ecx],eax
	inc ecx

	jmp .zeroLoopy

.finishedZEROy:
	mov [lenY],ecx
	pop ebx
	pop eax
	pop ecx
	ret


