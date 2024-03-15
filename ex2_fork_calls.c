#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	fork();
	fork();
	fork();
	printf("hello\n");            

	// A total of 3 processes are running on this program's execution alongside their father
	
	return 0;
}
