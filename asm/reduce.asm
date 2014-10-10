	global afd_reduce_asm
	extern puts

	section .text

afd_reduce_asm:

	mov	rdi, message
	call	puts
	mov	eax, 0
	ret

message:
	db	"ASM has the power now!", 0
