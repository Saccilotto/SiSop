#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int fd[2];			/* tubo de leitura do processo principal */
	pid_t pid, pidA, pidB;
	char buf[100];
	int i, n, cstat;

	if (pipe(fd) < 0) {
		fprintf(stderr,"Erro no pipe\n");
		_exit(1);
	}

	if ((pid = fork()) < 0){
		fprintf(stderr,"Erro no fork\n");
		_exit(1);
	}
	
   	if (pid == 0) {			/* primeiro processo descendente */
		char channel[30];
		close(fd[0]);
		sprintf(channel, "%d", fd[1]);
		execl("./ex2_son", "Son", channel, "1", NULL);
	}
	pidA = pid;

	if ((pid = fork()) < 0) {
		fprintf(stderr,"Erro no fork\n");
		_exit(1);
	}
	
	if (pid == 0) {			/* segundo processo descendente */
		char channel[30];
		close(fd[0]);
		sprintf(channel, "%d", fd[1]);
		execl("./ex2_son", "Son", channel, "2", NULL);
	}
	pidB = pid;

	close(fd[1]);

	n = read(fd[0], buf, 100);
	printf("%s\n", buf);
	n = read(fd[0], buf, 100);
	printf("%s\n", buf);
	waitpid(pidA, &cstat, 0);
	waitpid(pidB, &cstat, 0);

	_exit(0);
}

