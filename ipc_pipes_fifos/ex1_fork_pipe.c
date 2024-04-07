#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	/* cada um (pai e filho) tera uma copia dessas variaveis apos fork() */
	int fd[2];
	int val, pid;
	char buf[100];

	val = pipe(fd);
	/* deu ruim no pipe */
	if (val) return -1;

	pid = fork();
	if (pid > 0) {
		/* pai */
		printf("I am parent\n");
		/* fecha para leitura, pois vai escrever */
		close(fd[0]);
		strcpy(buf, "hello world!");
		write(fd[1], buf, strlen(buf));
		/* espera o filho terminar */
		wait(0);

		return 0;
	} else {
		/* filho */
		printf("I am child\n");
		/* fecha para escrita, pois vai ler */
		close(fd[1]);
		read(fd[0], buf, 100);
		printf("read: %s\n", buf);

		return 0;
	}

	return 0;
}
