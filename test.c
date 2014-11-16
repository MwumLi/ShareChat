#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	printf("%s\n", cuserid(NULL));
	return 0;
}
