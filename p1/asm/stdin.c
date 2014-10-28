#include <unistd.h>

int main(int argc, char *argv[])
{
	char a;
	while(1)
	{
		if(read(0, &a, 1) != 1)
		{
			perror("read");
			return 1;
		}
		if(write(1, &a, 1) != 1)
		{
			perror("write");
			return 1;
		}
	}
	return 0;
}
