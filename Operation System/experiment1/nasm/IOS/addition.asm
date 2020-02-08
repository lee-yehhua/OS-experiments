add:
	push ecx
	push ebx
	push eax
	push edx
	push edi
	push esi

	mov ecx,0	
	mov eax,0
	mov edi,0

.addLoop:
	cmp ecx,50
	jge .removeZero

	mov bl,[x+ecx]
	sub bl,48
	add eax,ebx

	mov bl,[y+ecx]
	sub bl,48
	add eax,ebx
	
	mov edx,0	;empty edx
	mov esi,10	
	idiv esi	;divide eax by esi
	add edx,48	;convert edx to its ascii
	push edx	;push edx (string representation of an intger) onto the stack
	
	inc ecx		;count each byte to print - number of characters
	jmp .addLoop


.removeZero:
	cmp ecx,1	
	jle .getAnswerLoop
	pop eax
	mov edx,eax
	push eax
	cmp edx,48
	jne .getAnswerLoop
	pop eax
	dec ecx
	jmp .removeZero

.getAnswerLoop:
	cmp ecx,0
	jz	.returnAnswer
	pop eax
	mov [resAdd+edi],eax
	inc edi
	dec ecx
	jmp .getAnswerLoop

.returnAnswer:
	pop esi
	pop edi
	pop edx
	pop eax
	pop ebx
	pop ecx
	ret

