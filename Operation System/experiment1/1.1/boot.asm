%include "io.inc"
SECTION .data
msg db "hello OS World",0ah,0dh
msglen equ ($ - msg)

SECTION .text
global CMAIN
CMAIN:
    mov edx,msglen
    mov ecx,msg
    mov ebx,1d
    mov eax,4d
    int 80h
    
    mov ebx,0d
    mov edx,1d
    int 80h