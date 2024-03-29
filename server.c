#include "functions.h"


int main (int argc, char* argv[]) { /* int argc, char** argv */

	int i,c;
	char* fin;
	int port;

	signal(SIGPIPE, SIG_IGN);
	
	
	if (argc < 2) {
		printf("Usage: %s -p <port>\r\n",argv[0]);
		exit(0);
	}
	
	port = 9500;
	
	while((c = getopt(argc, argv, OPT)) != -1){
		switch(c){
			case 'p':          /* get local listening port */
				port = (int) strtol(optarg, &fin, 10);
    			if( fin == optarg ){
		    	    fatal(0,"strtol");

    			}
    			if( ((port == LONG_MAX) || (port == LONG_MIN)) && (errno == ERANGE) ) {
        	        fatal(0,"Débordement");
        		}
    			break;
		}
	}

	/*	chdir("/"); */
	if( fork() != 0 )
	    exit(0);
	
	for ( i = 0; i < 255; i++)
	    close(i);
	server(port);

	return(0);
	
}  /* main */
	
