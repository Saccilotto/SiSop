#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	/* argv[1] - descritor de escrita
	argv[2] - posicao do filho */
	char texto[] = "  : Bom dia!";
	texto[0] = 'A' + atoi(argv[2])-1;
	write(atoi(argv[1]), texto, strlen(texto));
	_exit(0);
}
