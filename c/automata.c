#include "automata.h"

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

int afd_readfd(struct afd *afd, FILE *f)
{
	if(fgets(afd->buffer, MAX_LINE, f) == NULL)
	{
		perror("fgets");
		return 1;
	}
	afd->buffer[strlen(afd->buffer) - 1] = '\0';
	return 0;
}

int main(int argc, char *argv[])
{
	struct afd *afd;
	
	// TODO comprobaciones
	afd_init(&afd);
	afd_readfd(afd, stdin);
	printf("%s\n", afd->buffer);
	afd_free(afd);

	return 0;
}
