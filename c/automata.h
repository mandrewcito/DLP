#include "list.h"
#include "queue.h"

#define MAX_LINE 1024

struct afd_tran
{
	char *ini;
	char *fin;
	char *sym;
};

struct afd
{
	struct list_t trans;
	struct list_t st;
	char *sti;
	struct list_t stf;
	struct list_t alfa;

	char buffer[MAX_LINE];
};
