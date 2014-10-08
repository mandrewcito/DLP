//#define NDEBUG

#include "automata.h"
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
		return 1;
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
		perror("fgets");
		return 1;
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

	while(p != NULL)
	{
		debug("Nuevo elemento %s", p);
		if(list_add(l, p) != 0)
		{
			debug("list_add");
			return 1;
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

	return 0;
}

static int afd_parse_trans(struct list_t *l, char *str, char **save)
{
	char *p;
	struct afd_tran *tran;
	p = strtok_r(NULL, ";", save);

	while(p != NULL)
	{
		debug("Nuevo elemento %s", p);
		if((tran = malloc(sizeof(struct afd_tran))) == NULL)
		{
			debug("malloc");
			return 1;
		}

		afd_parse_tran_str(tran, p);

		if(list_add(l, tran) != 0)
		{
			debug("list_add");
			return 1;
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

	if(p == NULL)
	{
		debug("Estado inicial no encontrado");
		return 1;
	}

	afd->sti = p;

	return 0;
}

static void afd_print_dot_tran(void *ptr)
{
	struct afd_tran *tran = (struct afd_tran *) ptr;
	printf("\t\"%s\" -> \"%s\" [label = \"%s\"];\n",
		tran->ini, tran->fin, tran->sym);
}

static void afd_print_dot_stf(void *ptr)
{
	char *stf = (char *) ptr;
	printf("\t\"%s\" [peripheries = 2];\n", stf);
}

int afd_print_dot(struct afd *afd)
{
	printf("digraph G {\n");

	list_map(&afd->trans, afd_print_dot_tran);
	list_map(&afd->stf, afd_print_dot_stf);
	printf("\tSTART_NODE [style = invis];\n");
	printf("\tSTART_NODE -> \"%s\";\n", afd->sti);

	printf("}\n");

	return 0;
}

int afd_parse(struct afd *afd)
{
	char *p, *save;
	p = strtok_r(afd->buffer, ";", &save);

	debug("Estados totales %s", p);

	if(afd_parse_list(&afd->st, p) != 0)
	{
		debug("afd_parse_list");
		return 1;
	}
	
	p = strtok_r(NULL, ";", &save);

	if(afd_parse_list(&afd->alfa, p) != 0)
	{
		debug("afd_parse_list");
		return 1;
	}
	
	p = strtok_r(NULL, ";", &save);

	afd_parse_sti(afd, p);
	
	p = strtok_r(NULL, ";", &save);

	if(afd_parse_list(&afd->stf, p) != 0)
	{
		debug("afd_parse_list");
		return 1;
	}

	afd_parse_trans(&afd->trans, p, &save);

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

	queue_empty(&estados);
	list_empty(&visitados);
	list_empty(&nuevas_trans);

	queue_push(&estados, afd->sti);
	list_add(&visitados, afd->sti);
	while(!queue_pop(&estados, &st))
	{
		for(node = afd->trans.start; node != NULL; node = node->next)
		{
			debug("Coger transición %p", node->ptr);
			afd_print_tran(node->ptr);
			tran = (struct afd_tran *) node->ptr;
			debug("tran->ini = %s", tran->ini);
			debug("st = %s", (char *) st);
			if(strcmp(tran->ini, st) == 0)
			{
				debug("Estado inicial = %s", tran->ini);
				if((tran2 = malloc(sizeof(struct afd_tran))) == NULL)
				{
					return 1;
				}
				memcpy(tran2, tran, sizeof(struct afd_tran));
				list_add(&nuevas_trans, tran2);
				if(list_find(&visitados, tran2->fin, afd_cmp) == NULL)
				{
					debug("Añadiendo %s a visitados", tran2->fin);
					list_add(&visitados, tran2->fin);
					queue_push(&estados, tran2->fin);
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
	afd_init(&afd);
	afd_fread(afd, stdin);
	debug("Buffer %s", afd->buffer);

	if(afd_parse(afd) != 0)
	{
		debug("afd_parse");
		return 1;
	}

	afd_print_dot(afd);

	afd_reduce(afd);
	
	afd_print_dot(afd);

	afd_free(afd);

	return 0;
}
