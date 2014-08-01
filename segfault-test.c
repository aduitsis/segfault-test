#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

static long int counter;

static jmp_buf buf;
static int savesigs; 

volatile sig_atomic_t eflag = 0;

void segfault_sigaction(int signal, siginfo_t *si, void *arg) {
	//printf("segfault at address %p, code was %d\n", si->si_addr, si->si_code);
	counter+=1024;
	//printf("counter=%x\n",counter);
	siglongjmp(buf,1);
}

void main(int argc,char *argv[]) {
	printf("hello world\n");

	struct sigaction sa,oldact;

	long int val[2];

	for(int i=0;i<2;i++) {
		val[i] = strtol(argv[i+1], NULL, 16);
		if ((errno == ERANGE && (val[i] == LONG_MAX || val[i] == LONG_MIN)) || (errno != 0 && val[i] == 0)) {
			perror("strtol");
			exit(0);
		}
		//printf("%x\n",val[i]);
	}


	int previous_cannot_read = -1;

	memset(&sa , 0 , sizeof( sa ) );
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = segfault_sigaction;
	if( sigaction(SIGSEGV, &sa, &oldact) < 0 ) {
		perror("Cannot set signal handler");
	}

	for( counter=val[0] ; counter<=val[1] ;  ) {
		int cannot_read = sigsetjmp(buf,1);

		if( counter != val[0] ) {
			if( cannot_read != previous_cannot_read ) {
				if(cannot_read) {
					printf("address %x unreadable\n",counter-1);
				}
				else {
					printf("address %x readable\n",counter-1);
				}
			}
		}
		previous_cannot_read = cannot_read;

		uint8_t *pointer = (uint8_t*) counter;

		if( counter % 1024 == 0 ) printf("%x\n",counter); 

		uint8_t dummy = *pointer;
		//printf("%02x",*pointer);


		counter+=1024;
	}

}
