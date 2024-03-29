#include "functions.h"



int create_socket_uni_tcp_listen (int port)
{

	struct sockaddr_in host_addr;
	int socket_listen_fd, reuse_addr = 1;

	if((socket_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    	fatal(0, "socket");
	    
	if(setsockopt(socket_listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0)
	    fatal(0, "setsockopt");

    


	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port); 
	host_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(host_addr.sin_zero), '\0', 8);

	
	if(bind( socket_listen_fd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
		fatal(0,"bind");
	    
	if(listen(socket_listen_fd,NB_USER))
		fatal(0,"listen");
	  

	return socket_listen_fd;

}

void build_select_list() {
	int listnum;	     
	
	FD_ZERO(&socks);

	
	FD_SET(sock,&socks);
	
	
	
	for (listnum = 0; listnum < NB_USER; listnum++) {
		if (connectlist[listnum].fd != 0) {
			FD_SET(connectlist[listnum].fd,&socks);
			if (connectlist[listnum].fd > highsock)
				highsock = connectlist[listnum].fd;
		}
	}
} 

int get_citation(){
    int i = 0;
    int s = rand() % 177;
    FILE *fd;


    printf("%d",s);
    fd = fopen("citations.txt","rb");
    while( (fread(message, MAX, 1, fd )) != 0  && i != s)
    {
        i++;
        memset(message,0,MAX);
    }
    fclose(fd);
    return(1);
}

int read_from( int fd, char* buffer, int len)
{

	struct sockaddr_in *client_addr;
	socklen_t sin_size;
	int length;


			sin_size = sizeof(struct sockaddr_in);
			
			client_addr = (struct sockaddr_in *)malloc(sin_size);
			
            if((length =  recvfrom(fd, buffer, len, 0, (struct sockaddr *)client_addr, &sin_size)) < 0){
				fatal(0,"recvfrom");
				return(-1);
			}

            buffer[MAX-1] = '\0'; /* ensure end string char is in our buffer   */

    return(length);
} /* Read_from */



int get_time(time_t* t)
{

	return strftime(cur_time, TIMESIZE, "%H:%M", localtime(t));	
}
	

void send_to(int fd, char* buffer)
{
	

	if(write(fd, buffer, strlen(buffer)) == 0)
		fatal(0, "write");

} /* send_to*/


char* getIP(int connection){

	struct sockaddr_in adresse;
	socklen_t longueur;

	longueur = sizeof( struct sockaddr_in);
	/* get Client Ip address */
	if((getpeername(connection,  (struct sockaddr *)& adresse, & longueur)) < 0){
		fatal(0, "getsockname");
	}

	return inet_ntoa(adresse.sin_addr);

}
