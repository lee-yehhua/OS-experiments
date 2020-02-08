mul:
	push eax
	push ecx
	push ebx
	push esi

	mov ecx,0
	mov esi,resTmpMul
	call Zero


.mulLoop:
	cmp ecx,25
	jge .finish

	mov bl,[x+ecx]
	sub bl,48	
	mov eax,ebx

	inc ecx

	cmp eax,0
	jz .mulLoop
	
	mov esi,tmp
	call Zero
	
	call getTmp
	call addPart
	jmp .mulLoop

.finish:
	call getMulAnswer

	pop esi
	pop ebx
	pop ecx
	pop eax
	ret

Zero:
	push esi
	push ecx
	push eax
	mov ecx,0

.setZero:
	cmp ecx,50
	jge .finish1
	mov eax,48
	mov [esi+ecx],eax
	inc ecx
	jmp .setZero

.finish1:
	pop eax
	pop ecx
	pop esi
	ret

getTmp:
	push eax
	push ecx
	push edi
	push esi
	push edx
	push ebx

	mov edi,0
	mov edx,0
	sub ecx,1
	mov esi,0
	push esi

.mulPartLoop:
	
	cmp ecx,50
	jge .finish2
	
	mov bl,[y+edi]
	sub bl,48
	
	pop esi
	push eax
	
	mul ebx
	add eax,esi
	
	mov edx,0
	mov esi,10
	idiv esi
	add edx,48
	mov [tmp+ecx],edx

	mov esi,eax
	pop eax
	push esi

	inc edi
	inc ecx
	jmp .mulPartLoop

.finish2:
	pop edx

	pop ebx
	pop edx
	pop esi
	pop edi
	pop ecx
	pop eax
	ret
	
addPart:
	push ecx
	push ebx
	push eax
	push edx
	push esi
	
	mov ecx,0
	mov eax,0

.addPartLoop:
	cmp ecx,50
	jge	.finish3

	mov bl,[tmp+ecx]
	sub bl,48
	add eax,ebx
	
	mov bl,[resTmpMul+ecx]
	sub bl,48
	add eax,ebx

	mov edx,0
	mov esi,10
	idiv esi
	add edx,48
	
	push edx	
	mov bl,[esp]
	pop edx

	mov [resTmpMul+ecx],bl

	inc ecx
	jmp .addPartLoop

.finish3:
	pop esi
	pop edx
	pop eax
	pop ebx
	pop ecx
	ret

getMulAnswer:

	push eax
	push ebx
	push ecx
	push edi

	mov ecx,0
	mov edi,0

.pushStack:
	cmp ecx,50
	jge .removeZeroRes
	
	mov bl,[resTmpMul+ecx]
	sub bl,0
	push ebx
	
	inc ecx
	jmp .pushStack

.removeZeroRes:
	cmp ecx,1
	jle	.getMulAnswer
	pop eax
	mov edx,eax
	push eax
	cmp edx,48
	jne .getMulAnswer
	pop eax
	dec ecx
	jmp .removeZeroRes

.getMulAnswer:
	cmp ecx,0
	jz .retAnswer
	mov bl,[esp]
	mov [resMul+edi],bl
	inc edi
	pop eax
	dec ecx
	jmp .getMulAnswer

.retAnswer:
	pop edi
	pop ecx
	pop ebx
	pop eax
	ret
