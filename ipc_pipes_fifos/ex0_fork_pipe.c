#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int pfds[2];
	char buf[30];

	/* abre um pipe */
	pipe(pfds);

	if (!fork()) {
		/* filho recebe pid zero no fork() */
		printf("CHILD: writing to the pipe\n");
		write(pfds[1], "test", 5);
		printf("CHILD: exiting\n");
		exit(0);
	} else {
		/* pai tem o pid do filho */
		printf("PARENT: reading from pipe\n");
		read(pfds[0], buf, 5);
		printf("PARENT: read \"%s\"\n", buf);
		/* espera o filho terminar */
		wait(NULL);
	}
	
	return 0;
}
