#include "list.h"
#include "queue.h"

#define MAX_LINE 1024

struct afd_st
{
	char *name;
};

struct afd_sym
{
	char *name;
};

struct afd_tran
{
	struct afd_st ini;
	struct afd_st fin;
	struct afd_sym sym;
};

struct afd
{
	struct list_t trans;
	struct list_t st;
	struct afd_st sti;
	struct list_t stf;
	struct list_t alfa;

	char buffer[MAX_LINE];
};
