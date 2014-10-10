	global main
	extern puts

	section .text

main:

	mov	rdi, message
	call	puts
	mov	eax, 0
	ret

message:
	db	"Hola, mundo", 0
