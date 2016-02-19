   mov ax, 0x07c0
   mov ds, ax
 
   mov si, msg
loop:
   lodsb
   or al, al ; end of string if 0
   jz hang   ; go to hang
   mov ah, 0x0E
   int 0x10
   jmp loop
 
hang:
   jmp hang
 
   times 510-($-$$) db 0
   db 0x55
   db 0xAA

msg   db 'SOME LONG MESSAGE ', 13, 10, 0

