//#define NDEBUG

#include "automata.h"
#include "reduce.h"
#include "dbg.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int afd_init(struct afd **afd)
{
	(*afd) = (struct afd *) calloc(1, sizeof(struct afd));

	if(!(*afd))
	{
		perror("calloc");
		return -1;
	}

	return 0;
}

static void afd_free_tran(void *p)
{
	free(p);
}

void afd_free(struct afd *afd)
{
	list_clear(&afd->st);
	list_clear(&afd->stf);
	list_clear(&afd->alfa);
	list_clear_func(&afd->trans, afd_free_tran);
	
	free(afd);
}

int afd_fread(struct afd *afd, FILE *f)
{
	if(fgets(afd->buffer, MAX_LINE, f) == NULL)
	{
		debug("fgets no leyó nada");
		return -1;
	}
	debug("Máximo %d, leído %lu", MAX_LINE, strnlen(afd->buffer, MAX_LINE-2));
	if(strnlen(afd->buffer, MAX_LINE-1) == (MAX_LINE-1))
	{
		debug("Línea demasiado larga. Máximo %d caracteres", MAX_LINE-2);
		return -1;
	}
	afd->buffer[strlen(afd->buffer) - 1] = '\0';
	return 0;
}

#ifndef NDEBUG

static void afd_print_str(void *s)
{
	debug("%s", (char *) s);
}

static void afd_print_tran(void *ptr)
{
	struct afd_tran *t = (struct afd_tran *) ptr;
	debug("%s -- %s --> %s", t->ini, t->sym, t->fin);
}

#endif

static int afd_parse_list(struct list_t *l, char *str)
{
	char *p, *save;
	debug("afd_parse_list(, %s)", str);
	p = strtok_r(str, " ", &save);

	while(p)
	{
		debug("Nuevo elemento %s", p);
		if(list_add(l, p))
		{
			debug("list_add");
			return -1;
		}

		p = strtok_r(NULL, " ", &save);
	}

#ifndef NDEBUG
	list_map(l, afd_print_str);
#endif
	return 0;
}

static int afd_parse_tran_str(struct afd_tran *tran, char *str)
{
	char *save;
	tran->ini = strtok_r(str, " ", &save);
	tran->fin = strtok_r(NULL, " ", &save);
	tran->sym = strtok_r(NULL, " ", &save);

	if(!tran->ini || !tran->fin || !tran->sym)
	{
		debug("afd_parse_tran_str");
		return -1;
	}

	return 0;
}

static int afd_parse_trans(struct list_t *l, char *str, char **save)
{
	char *p;
	struct afd_tran *tran;
	p = strtok_r(NULL, ";", save);

	while(p)
	{
		debug("Nueva transición %s", p);
		if(!(tran = malloc(sizeof(struct afd_tran))))
		{
			debug("malloc");
			return -1;
		}

		if(afd_parse_tran_str(tran, p))
		{
			debug("afd_parse_tran_str");
			return -1;
		}

		if(list_add(l, tran))
		{
			debug("list_add");
			return -1;
		}

		p = strtok_r(NULL, ";", save);
	}
#ifndef NDEBUG
	list_map(l, afd_print_tran);
#endif
	return 0;
}

static int afd_parse_sti(struct afd *afd, char *str)
{
	char *p, *save;
	p = strtok_r(str, " ", &save);

	if(!p)
	{
		debug("Estado inicial no encontrado");
		return -1;
	}

	afd->sti = p;

	return 0;
}

static void afd_print_dot_tran(struct afd *afd, void *ref)
{
	struct afd_tran *tran;
	struct list_node_t * node;

	for(node = afd->trans.start; node != NULL; node = node->next)
	{
		tran = (struct afd_tran *) node->ptr;
		printf("\t\t\"%p-%s\" -> \"%p-%s\" [label = \"%s\"];\n",
			ref, tran->ini, ref,  tran->fin, tran->sym);
		printf("\t\t\"%p-%s\" [label = \"%s\"];\n",
			ref, tran->ini, tran->ini);
		printf("\t\t\"%p-%s\" [label = \"%s\"];\n",
			ref, tran->fin, tran->fin);
	}
}

static void afd_print_dot_stf(struct afd *afd, void *ref)
{
	char *st;
	struct list_node_t * node;

	for(node = afd->stf.start; node != NULL; node = node->next)
	{
		st = (char *) node->ptr;
		printf("\t\t\"%p-%s\" [peripheries = 2];\n", ref, st);
	}
}

void afd_print_dot_subgraph(struct afd *afd, char *title)
{
	printf("\tsubgraph \"cluster%p\" {\n", title);

	afd_print_dot_tran(afd, title);
	afd_print_dot_stf(afd, title);
	printf("\t\t\"%p-START_NODE\" [style = invis];\n", title);
	printf("\t\t\"%p-START_NODE\" -> \"%p-%s\";\n", title, title, afd->sti);
	
	printf("\t\tlabel = \"%s\";\n", title);

	printf("\t}\n");
}

void afd_print_dot_start()
{
	printf("digraph {\n");
	printf("\tcompound=true;\n");
}

void afd_print_dot_end()
{
	printf("}");
}

int afd_parse(struct afd *afd)
{
	char *p, *save;
	p = strtok_r(afd->buffer, ";", &save);

	debug("Estados totales %s", p);

	if(afd_parse_list(&afd->st, p) != 0)
	{
		debug("afd_parse_list");
		return -1;
	}
	
	p = strtok_r(NULL, ";", &save);

	if(afd_parse_list(&afd->alfa, p) != 0)
	{
		debug("afd_parse_list");
		return -1;
	}
	
	p = strtok_r(NULL, ";", &save);

	if(afd_parse_sti(afd, p))
	{
		debug("afd_parse_sti");
		return -1;
	}
	
	p = strtok_r(NULL, ";", &save);

	if(afd_parse_list(&afd->stf, p) != 0)
	{
		debug("afd_parse_list");
		return -1;
	}

	if(afd_parse_trans(&afd->trans, p, &save))
	{
		debug("afd_parse_trans");
		return -1;
	}

	return 0;
}

int afd_cmp(const void *a, const void *b)
{
	return strcmp((const char *) a, (const char *) b);
}

int afd_reduce(struct afd *afd)
{
	struct queue_t estados;
	struct list_t visitados;
	struct list_t nuevas_trans;
	struct list_node_t *node;
	struct afd_tran *tran, *tran2;
	void *st;

	debug("AFD ptr = %p", afd);

	queue_empty(&estados);
	list_empty(&visitados);
	list_empty(&nuevas_trans);

	if(queue_push(&estados, afd->sti))
	{
		debug("queue_push");
		return -1;
	}
	
	if(list_add(&visitados, afd->sti))
	{
		debug("list_add");
		return -1;
	}

	while(!queue_pop(&estados, &st))
	{
		debug("Estado %s", (char *) st);
		for(node = afd->trans.start; node != NULL; node = node->next)
		{
			debug("Transición %p", node->ptr);
#ifndef NDEBUG
			afd_print_tran(node->ptr);
#endif
			tran = (struct afd_tran *) node->ptr;
			if(strcmp(tran->ini, st) == 0)
			{
				if((tran2 = malloc(sizeof(struct afd_tran))) == NULL)
				{
					perror("malloc");
					return -1;
				}
				memcpy(tran2, tran, sizeof(struct afd_tran));
				if(list_add(&nuevas_trans, tran2))
				{
					debug("list_add");
					return -1;
				}
				if(list_find(&visitados, tran2->fin, afd_cmp) == NULL)
				{
					debug("Añadiendo %s a visitados", tran2->fin);
					if(list_add(&visitados, tran2->fin))
					{
						debug("list_add");
						return -1;
					}
					if(queue_push(&estados, tran2->fin))
					{
						debug("queue_push");
						return -1;
					}
				}
			}
		}
	}

#ifndef NDEBUG
	debug("Antiguas transiciones");
	list_map(&afd->trans, afd_print_tran);
	debug("Nuevas transiciones");
	list_map(&nuevas_trans, afd_print_tran);
#endif
	list_clear_func(&afd->trans, afd_free_tran);
	list_clear(&visitados);
	memcpy(&afd->trans, &nuevas_trans, sizeof(struct list_t));
	
	return 0;
}

int main(int argc, char *argv[])
{
	struct afd *afd;
	
	// TODO comprobaciones
	if(afd_init(&afd))
	{
		debug("afd_init");
		return 1;
	}
	if(afd_fread(afd, stdin))
	{
		debug("afd_fread");
		return 1;
	}
	
	debug("Buffer %s", afd->buffer);

	if(afd_parse(afd))
	{
		debug("afd_parse");
		return 1;
	}

	afd_print_dot_start();

	afd_print_dot_subgraph(afd, "Original");

	debug("Antes de afd_reduce_asm &(%p) afd->sti[0] = %c", &afd->sti, afd->sti[0]);
/*
	if(afd_reduce(afd))
	{
		return 1;
	}
*/
	if(afd_reduce_asm(afd))
	{
		debug("------ ERROR EN ASM ------");
		return 1;
	}
	
	afd_print_dot_subgraph(afd, "Mínimo conexo");

	afd_print_dot_end();

	afd_free(afd);

	return 0;
}
