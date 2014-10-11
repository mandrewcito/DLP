	global afd_reduce_asm
	extern printf
	extern queue_empty
	extern queue_push
	extern queue_pop
	extern list_empty
	extern list_add

	section .text

struc list_t
	.start	resq 1
	.end	resq 1
endstruc

struc queue_t
	.start	resq 1
	.end	resq 1
endstruc

struc afd_t
	.trans	resb list_t_size
	.st	resb list_t_size
	.sti	resq 1
	.stf	resb list_t_size
	.alfa	resb list_t_size
	.buffer	resb 1024
endstruc

afd_reduce_asm:

;int afd_reduce_asm(struct afd *afd)
;rdi contiene afd

;Reservar espacio para:
;	struct afd *afd
;	struct queue_t estados
;	struct list_t visitados;
;	struct list_t nuevas_trans;
;	struct list_node_t *node;
;	struct afd_tran *tran, *tran2;
;	void *state;

;Guardar en la pila, la dirección base de la pila
	push	rbp
;Establecer la dirección base le la pila a la cima
	mov	rbp, rsp

%define afd		0x8
%define estados 	0x18
%define visitados	0x28
%define nuevas_trans	0x38
%define node		0x40
%define tran		0x48
%define tran2		0x50
%define st		0x58

;Reservar espacio
	sub	rsp, st

;Guardar el puntero afd en la pila
	mov	[rbp-afd], rdi

;queue_empty(&estados);
	lea	rdi, [rbp - estados]
;	mov	rax, 0
	call	queue_empty

;list_empty(&visitados);
	lea	rdi, [rbp - visitados]
;	mov	rax, 0
	call	list_empty

;list_empty(&nuevas_trans);
	lea	rdi, [rbp - nuevas_trans]
;	mov	rax, 0
	call	list_empty

;queue_push(&estados, afd->sti);
	lea     rdi, [rbp - estados]
	mov	rsi, [rbp - afd]
	mov	rsi, [rsi + afd_t.sti]
	call	queue_push

;if(queue_push(&estados, afd->sti))
	test	eax, eax
	jne	error_exit

;list_add(&visitados, afd->sti)
	lea     rdi, [rbp - visitados]
	mov     rsi, [rbp - afd]
	mov	rsi, [rsi + afd_t.sti]
	call	list_add

;if(list_add(&visitados, afd->sti))
	test    eax, eax
	jne     error_exit

;queue_pop(&estados, &st)
	lea	rdi, [rbp - estados]
	lea	rsi, [rbp - st]
	call    queue_pop

while_start:
;while(!queue_pop(&estados, &st))
	test    eax, eax
	jne	while_end

	mov	rdi, S_st
	lea	rsi, [rbp - st]
	mov	eax, 0
	call	printf

	jmp	while_start




while_end:

;salir
	jmp	normal_exit

;Almacenar rax y rcx
;	push	rax
;	push	rcx


error_exit:

;Mostrar el valor de la pila
	mov	rax, 0
	mov	rdi, S_error
	call	printf
	mov	eax, -1
	jmp	return

;Recuperar rcx y rax
;	pop     rcx
;	pop     rax


normal_exit:
	xor	rax, rax
;	mov	eax, 0
return:

	leave
	
	ret

message:
	db	`ASM has the power now. ptr = %p\n`, 0

afd_sti:
	db	`afd->sti = %s\n`, 0
S_error:
	db	`Problemas\n`, 0
S_st:
	db	`Estado st = %s\n`, 0
