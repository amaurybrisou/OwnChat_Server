#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <limits.h>
#include <inttypes.h>



#define MAX_TIMEOUT  120 /* timeout in s */

#define MAX 1024
#define NB_USER 100
#define NAMESIZE 12
#define TIMESIZE 24


#define OPT "p:"

char message[MAX];
 
time_t t_cur;
    
char cur_time[TIMESIZE];

struct user{
    int fd;
    char name[NAMESIZE];
    char address[20];

    long t_con;
}users;

int sock; 

int curnb_user;




struct user connectlist[NB_USER];  /* Array of connected sockets so we know who
	 		we are talking to */
fd_set socks;        /* Socket file descriptors we want to wake
					up for, using select() */
int highsock;
	

char temp_name[NAMESIZE];

int get_citation();
int disconnect( struct user * connectlist);
void server(int port);
char* getIP(int connection);
int get_time(time_t* t);
void warning_disconnection(int fd, char* name, char* address);
void warning_connection(char* name, char* address);
void build_select_list();
void deal_with_data(int listnum	) ;
void set_non_blocking(int sock);
void handle_new_connection();
void fatal(int i, char *message);
void read_socks();
int read_from( int fd, char* buffer, int len);
void send_to( int cli_fd, char* buffer);
int create_socket_uni_tcp_listen (int port);
int create_socket_uni_tcp(int port);


