;kernel.asm
;Michael Black, 2007
;Modified by Asisten Sister, 2021

;kernel.asm contains assembly functions that you can use in your kernel

global _putInMemory
global _interrupt
global _makeInterrupt21
extern _handleInterrupt21

global _initDrawImage
global _drawImage
global _loadImage
global _imageFile


;void putInMemory (int segment, int address, char character)
_putInMemory:
	push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret

;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:	int 0x00	;call the interrupt (00 will be changed above)

	mov ah,0	;we only want AL returned
	pop bp
	ret

;void makeInterrupt21()
;this sets up the interrupt 0x21 vector
;when an interrupt 0x21 is called in the future, 
;_interrupt21ServiceRoutine will run

_makeInterrupt21:
	;get the address of the service routine
	mov dx,_interrupt21ServiceRoutine
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx

	iret

; _initDrawImage:
; 	pusha
; 	mov si, _imageFile
; 	call _drawImage ;draw image
; 	popa
; 	ret


; ;------------------------------------------------------
; ;si = image source
; _drawImage:
; 	pusha
; 	xor ax, ax
; 	; mov ax, [si] 
; 	; inc si
; 	lodsb			;x-length (first byte of binary)
; 	mov cx, ax                      
; 	; mov ax, [si] 
; 	; inc si
; 	lodsb			;y-length (2nd byte of binary)
; 	mov dx, ax                      
	
; 	.for_x:
; 		push dx
; 		.for_y:
; 			mov bh, 0               ;page number (0 is default)
; 			; mov ax, [si] 
; 			; inc si
; 			lodsb                   ;read byte to al (color) -> next byte
; 			; mov al, 0xE
; 			mov ah, 0xC             ;write pixel at coordinate (cx, dx)
; 			int 0x10                ;draw!
; 		sub dx, 1                   ;decrease dx by one and set flags
; 		jnz .for_y                  ;repeat for y-length
; 		pop dx                      ;restore dx (y-length)
; 	sub cx, 1                       ;decrease si by one and set flags
; 	jnz .for_x                      ;repeat for x-length
; 	popa                            ;restore everything
; 	ret


;------------------------------------------------------

; _loadImage:
; 	push bp
; 	mov bp,sp
; 	mov ax, _imageFile
; 	pop bp
; 	ret

_imageFile: incbin "image.bin"       ;include the raw image binary into the kernel


