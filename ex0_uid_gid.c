#include <stdio.h>
#include <unistd.h>

int main(void)
{
	printf("I am process %d\n", getpid());	// the current process ID
	printf("My parent is %d\n", getppid());	// the current process's parent proccess ID (The one that spawned the current one)

	printf("My real user ID is       %d\n", getuid());  // the UNIX ID of the user that started the process 	 
	printf("My effective user ID is  %d\n", geteuid());	// the effective user ID determines the user and iniated process permissions(if the process is running as root, the effective user ID will be 0)
	printf("My real group ID is      %d\n", getgid());	// The unix group ID the process is running under (may be a list of processes from the same program)
	printf("My effective group ID is %d\n", getegid()); // The effective group ID determines the group and its permissions.

	return 0;
}
