section .data
hello: db "Please input x,y:",10,0
section .bss
pt resb 255
name resb 255
name1 resb 255

section .text
global main
main:
mov eax,hello
call sprint
mov edx,255
mov ecx,name
mov eax,3
mov ebx,0
int 80h
push ecx
mov edx,255
mov ecx,name1
mov eax,3
mov ebx,0
int 80h
pop ecx
mov edi,name1

push ebx
mov ebx,0
mov esi,0

loop0:
inc ebx
cmp byte[edi+ebx],10
je loop1
jmp loop0

loop1:
inc esi
cmp byte[ecx+esi],10
je loop2
jmp loop1

;loop: 
;mov ah,[edi+esi]
;add [ecx+esi],ah
;sub byte[ecx+esi],48
;cmp byte[ecx+esi],57
;jna else
;sub byte[ecx+esi],10
;dec esi
;inc byte[ecx+esi]
;inc esi
;else:
;inc esi
;cmp byte[ecx+esi],10
;jne loop

loop2:
dec esi
dec ebx
loop3:
mov al,byte[edi+ebx]
add byte[ecx+esi],al
cmp byte[ecx+esi],96
jb loop31
sub byte[ecx+esi],48
loop31:
cmp byte[ecx+esi],57
jna else
sub byte[ecx+esi],10
dec esi
inc byte[ecx+esi]
cmp byte[ecx+esi],1
jne loop32
add byte[ecx+esi],48
loop32:
inc esi
else:
dec esi
dec ebx
cmp byte[edi+ebx],0
;cmp ebx,0
jne loop3
cmp byte[ecx+esi],0
;cmp esi,0
jne loop3

loop33:
pop ebx
;pop eax

;push eax
mov eax,name
loop5:
dec eax
;add byte[eax],48
cmp byte[eax],0
je print
jmp loop5
;inc eax
print:
inc eax
call sprint
pop eax
quit:
mov eax,1
mov ebx,0
int 80h

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
