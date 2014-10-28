	global afd_reduce_asm

;%define DEBUG

	extern afd_print_tran
	extern afd_cmp
	extern afd_free_tran
	extern printf
	extern queue_empty
	extern queue_push
	extern queue_pop
	extern list_empty
	extern list_add
	extern list_find
	extern list_map
	extern list_clear
	extern list_clear_func
	extern strcmp
	extern malloc
	extern free
	extern memcpy

	section .text

struc list_t
	.start	resq 1
	.end	resq 1
endstruc

struc list_node_t
	.prev	resq 1
	.next	resq 1
	.ptr	resq 1
endstruc

struc afd_tran
	.ini	resq 1
	.fin	resq 1
	.sym	resq 1
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

while_start:
;queue_pop(&estados, &st)
	lea	rdi, [rbp - estados]
	lea	rsi, [rbp - st]
	call    queue_pop

;while(!queue_pop(&estados, &st))
	test    eax, eax
	jne	while_end

;Dentro del bucle while

%ifdef DEBUG
;printf("Estado st = %s\n");
	mov	rdi, S_st
	mov	rsi, [rbp - st]
	mov	eax, 0
	call	printf
%endif

;for(node = afd->trans.start; node != NULL; node = node->next)
;Uso rax como contador node.

;node = afd->trans.start
	mov	rax, [rbp-afd]
	mov	rax, [rax+afd_t.trans]
	mov	[rbp-node], rax

for_start:
;Miro en el stack el puntero node
;node != NULL
	cmp	qword [rbp - node], 0
	je	for_end

%ifdef DEBUG
;Muestro la transición
	mov	rax, [rbp-node]
	mov	rdi, [rax+list_node_t.ptr]
	call	afd_print_tran
%endif

;tran = (struct afd_tran *) node->ptr;
	mov	rax, [rbp-node]
	mov	rax, [rax+list_node_t.ptr]
	mov	[rbp-tran], rax

;strcmp(tran->ini, st)
	mov	rdi, [rax+afd_tran.ini]
	mov	rsi, [rbp-st]
	call	strcmp

;if(strcmp(tran->ini, st) == 0)
	cmp	rax, 0
	jne	if_end

%ifdef DEBUG
;printf("st = %s", tran->ini)
	mov	rdi, S_st2
	mov	rax, [rbp-tran]
	mov	rsi, [rax+afd_tran.ini]
	mov	eax, 0
	call	printf
%endif

;malloc(sizeof(struct afd_tran))
	mov	rdi, afd_tran_size	;rdi / edi ?
	call	malloc

;if(malloc == NULL)
	cmp	rax, 0
	je	error_exit

;trans2 = malloc
	mov	[rbp-tran2], rax

;memcpy(tran2, tran, sizeof(struct afd_tran));
	mov	rdi, rax
	mov	rsi, [rbp-tran]
	mov	rdx, afd_tran_size
	call	memcpy

;list_add(&nuevas_trans, tran2)
	lea	rdi, [rbp-nuevas_trans]
	mov	rsi, [rbp-tran2]
	call	list_add

;if(list_add)
	cmp	rax, 0
	jne	error_exit

;list_find(&visitados, tran2->fin, afd_cmp)
	lea	rdi, [rbp-visitados]
	mov	rax, [rbp-tran2]
	mov	rsi, [rax+afd_tran.fin]
	mov	rdx, afd_cmp
	call	list_find

;if(list_find == NULL)
	cmp	rax, 0
	jne	if_end

%ifdef DEBUG
;printf("Añadiendo %s a visitados\n", tran2->fin)
	mov	rdi, S_st_add
	mov	rax, [rbp-tran2]
	mov     rsi, [rax+afd_tran.fin]
	mov	rax, 0
	call	printf
%endif

;list_add(&visitados, tran2->fin)
	lea	rdi, [rbp-visitados]
	mov	rax, [rbp-tran2]
	mov	rsi, [rax+afd_tran.fin]
	call	list_add

;if(list_add != 0)
	cmp	eax, 0
	jne	error_exit

;queue_push(&estados, tran2->fin)
	lea	rdi, [rbp-estados]
	mov	rax, [rbp-tran2]
	mov	rsi, [rax+afd_tran.fin]
	call	queue_push

;if(queue_push != 0)
	cmp	eax, 0
	jne	error_exit


if_end:
;node = node->next;
	mov	rax, [rbp - node]
	mov	rcx, [rax + list_node_t.next]
	mov	[rbp - node], rcx

	jmp	for_start
for_end:

	jmp	while_start
	
while_end:

;Mostrar los resultados
%ifdef DEBUG
;printf("Antiguas transiciones");
	mov	rdi, S_antiguas
	mov	rax, 0
	call	printf

;list_map(&afd->trans, afd_print_tran);
	mov	rax, [rbp-afd]
	lea	rdi, [rax+afd_t.trans]
	mov	rsi, afd_print_tran
	call	list_map

;printf("Nuevas transiciones");
	mov	rdi, S_nuevas
	mov	rax, 0
	call	printf

;list_map(&nuevas_trans, afd_print_tran);
	lea	rdi, [rbp-nuevas_trans]
	mov	rsi, afd_print_tran
	call	list_map
%endif

;Liberar memoria
;list_clear_func(&afd->trans, afd_free_tran);
	mov	rax, [rbp-afd]
	lea	rdi, [rax+afd_t.trans]
	mov	rsi, free
	call	list_clear_func

;list_clear(&visitados);
	lea	rdi, [rbp-visitados]
	call	list_clear

;memcpy(&afd->trans, &nuevas_trans, sizeof(struct list_t));
	mov	rax, [rbp-afd]
	lea	rdi, [rax+afd_t.trans]
	lea	rsi, [rbp-nuevas_trans]
	mov	rdx, list_t_size
	call	memcpy
	
;salir
	jmp	normal_exit


error_exit:

%ifdef DEBUG
;Mostrar error
	mov	rax, 0
	mov	rdi, S_error
	call	printf
%endif

;return -1
	mov	eax, -1
	jmp	return


normal_exit:
	xor	rax, rax

return:
	leave
	ret


;Mensajes
message:
	db	`ASM has the power now. ptr = %p\n`, 0
afd_sti:
	db	`afd->sti = %s\n`, 0
S_error:
	db	`Problemas\n`, 0
S_st:
	db	`Estado st = %s\n`, 0
S_st2:
	db	`st = %s\n`, 0
S_st_add:
	db	`Añadiendo %s a visitados\n`,0
S_nuevas:
	db	`Nuevas transiciones\n`,0
S_antiguas:
	db	`Antiguas transiciones\n`,0
