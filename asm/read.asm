	global main
	extern fgets
	extern puts
	extern stdin

section .bss
	buffer: resb 1024

section .text

main:
	push	stdin
	push	10
	push	buffer
	call	fgets

	push	buffer
	call	puts
	mov	eax, 0
	ret
