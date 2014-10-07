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

void afd_free(struct afd *afd)
{
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

static void afd_print_str(void *s)
{
	printf("%s\n", (char *) s);
}

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

	list_map(l, afd_print_str);
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

	afd_free(afd);

	return 0;
}
