#include "functions.h"


void server(int port){

    int listnum;
	struct timeval timeout;  /* Timeout for select */
	int readsocks;/* Number of sockets ready for reading */



     
    sprintf(message, "Launching server using port : %d", port); /*LOG_INFO to syslog */
    fatal(1, message);
    memset(message, 0, MAX);
    
	/* Obtain a file descriptor for our "listening" socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		fatal(0,"socket");
		exit(EXIT_FAILURE);
	}
	

	/* Set socket to non-blocking with our set_non_blocking routine */
	set_non_blocking(sock);


	sock = create_socket_uni_tcp_listen(port);
    if( sock == -1){  
        fatal(0, "Error listening socket");
    }
    fatal(1,"Server bound");
	
	/* Since we start with only one socket, the listening socket,
	   it is the highest socket so far. */
	   
	highsock = sock;
	memset((char *) &connectlist, 0, sizeof(connectlist));
    curnb_user = 0; /* set current connected user to 0 */

	while (1) { /* Main server loop */
		
		
        
        t_cur = time(NULL);


        for( listnum = 0; listnum < curnb_user; listnum++){
           if( ((t_cur - (connectlist[listnum].t_con)) > MAX_TIMEOUT) && (connectlist[listnum].fd != 0)){ /* every 2mn, admit all users disconnected ( they'll have to tell us they're still                                            alive "); */
                    
                    warning_disconnection(connectlist[listnum].fd, connectlist[listnum].name, connectlist[listnum].address);
                    disconnect(&connectlist[listnum]);
                       
            }
        }

        build_select_list();
             
            
            
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
	
		/* set server aware of incoming datas/connections */

		readsocks = select(highsock+1, &socks, (fd_set *) 0, 
		  (fd_set *) 0, &timeout);
		
		
		if (readsocks < 0) {
			fatal(0,"select");
			
		} else {
			read_socks();
		}
	}  /* while(1) */
}



void handle_new_connection() {
	/* This function aims to be sure that Client has a username and test if it doesn't already exists before going on creating connectlist entry ( including : socket file descriptor, name, IP address and filename to log all activity */

	int listnum;
	int connection;	     
	int isHere = 0;
	
	
	/* accept temporary connection */
	connection = accept(sock, NULL, NULL);

	if (connection < 0) {
		fatal(0,"accept");		
	}

	/* get client wanted name in temp_name ( we'll use many times this var later )*/
	memset(temp_name, 0, NAMESIZE);
	
	if ( read_from(connection,temp_name, NAMESIZE) <= 0 ) {
	    fatal(0, "recv_from"); 
	}	    

	temp_name[NAMESIZE-1] = '\0';
	/* check if client wanted name doesn't exists yet in connectlist array of structure*/
    for (listnum = 0; (listnum < curnb_user) && (connection != -1); listnum ++) {
        if ( strcmp( temp_name, connectlist[listnum].name) == 0) {
				
	                send_to(connection, "Username already exists or too long\n");
					isHere = 1;				
		}
        
    }

    
    
	
	

	if( isHere == 0){
		/* set non-blocking this new socket in order to let the server handles other connections */
		set_non_blocking(connection);
		/* Finally, Create a complete entry in the conectlist array of user structure */
		for (listnum = 0; (listnum < NB_USER) && (connection != -1); listnum ++)
			if (connectlist[listnum].fd == 0 ) {

				
				sprintf(message, "Connection of %s accepted: FD=%d; Slot=%d", temp_name, connection, listnum); /* server side */
			    fatal(1, message);
			    memset(message, 0, MAX);
			    
			    
				curnb_user++; /* one more user; */
				
			    connectlist[listnum].t_con = time(NULL); /* set time of connection */
			    
			    
			  
				connectlist[listnum].fd = connection; /* set connectlist file descriptor in ordre to send data to this 															user later */
		
				if( get_time(&t_cur) < 0){
					fatal(0,"strftime");
				}
			 
				strtok( temp_name, "\r\n"); /* supress odd CR CF if client is telnet for example. */
			    
                strncpy( connectlist[listnum].name, temp_name,strlen( temp_name)); /* copy the username */
                

					

            
				strncpy(connectlist[listnum].address, getIP(connection), 20); /* copy the IP address */


				warning_connection(connectlist[listnum].name, connectlist[listnum].address);	/* finally warn everybody that <temp_name> has just connect from the 													connection IP address */
			
			
			    sleep(1);
                get_citation();
                fatal(1,message);
  				send_to(connection, message); /* send proverbe */
               	memset(message,0,MAX);
            	
                	
                	
				connection = -1;
                

			}
		if (connection != -1) {
			/* No place left in the queue! */
			fatal(1,"No room left for new client");
			close(connection);
		
		}
	}
	
} /* handle_new_connection */

void warning_connection(char* name, char* address ) {

	/* This function aims to warn all connected users that someone connects, the message includes
		the username and his IP address, all of this being sent to all users defined in connectlist structure */

	int listnum;

     /* Create message; ex : Amaury Connects : IP : 127.0.0.1<LF> */   
	 sprintf(message,"%s Connects : IP : %s\n", name, address);
     
	 /* this bloc browses users defined in connectlist structure with listnum index. Warns each of them about this new mate arrival !*/
	 for (listnum = 0; listnum < NB_USER; listnum++) { 
			if(connectlist[listnum].fd != 0){
				
				send_to(connectlist[listnum].fd, message);
				
			} 
	}
	fatal(1,message);
	
	/*  reset Buffer content for late use */
	memset(message,0,MAX);
}

void warning_disconnection(int fd, char* name, char* address) {

	/* Exactly the same function as warning_connection() one. It just has a reverse role. */
	int listnum;
	
	sprintf(message,"%s Disconnects : IP : %s\n", name,  address);
	fatal(1, message);
	
		
	for (listnum = 0; listnum < NB_USER; listnum++) { /* warning all chatter that someone's just disconnect */
			if(connectlist[listnum].fd != 0 && connectlist[listnum].fd != fd){
				
				send_to(connectlist[listnum].fd, message);
				
			} 
			    
	}
	memset(message,0,MAX);
}
	

void set_non_blocking(int sock)
	/* This function is dedicated to set socket non-blocking */
{
	int opts; 

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		fatal(0,"fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		fatal(0,"fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
} /* Set_Non_BLocking */

int disconnect( struct user* user){
       
       
            
			
			user->t_con = 0;
			strncpy(user->name, "\00",NAMESIZE);
			strncpy(user->address, "\00", 20);
       
            curnb_user--;
            close(user->fd);
            user->fd = 0;
            return(0);
}
void deal_with_data(int listnum	) {		/* Current item in connectlist for for loops */
	
	int temp_fd;

	char buffer[MAX];     /* Buffer for socket reads */
    char buf[MAX+30];

	memset(buffer,0,MAX);
	memset(buf,0,MAX+30);
	memset( temp_name, 0, NAMESIZE);
	
	if (((read_from(connectlist[listnum].fd,buffer, MAX)) <= 0)) {
		/* Connection closed, close this end
		   and free up entries in connectlist */
			/*warning_disconnection(connectlist[listnum].name, connectlist[listnum].address);*/
			disconnect(&connectlist[listnum]);
			
	} else {
		/* We got some data, lets send them and save them to files*/
	       
			
		    temp_fd = connectlist[listnum].fd;


	
	
		
            
            if( ((strlen(buffer) == 0))){
                
                
                connectlist[listnum].t_con = t_cur;
			    
			    
			} else {
			    if( (strlen(buffer) == 1) && (strcmp(buffer, "w") == 0)){
                
                    send_to(temp_fd, "[#] User List\n");
                    for (listnum = 0; listnum < NB_USER; listnum++) {
				        if(connectlist[listnum].fd != 0){
                            memset(buf,0,MAX+30);                        
				            sprintf(buf, "[%d]: %s IP : %s\n", listnum, connectlist[listnum].name, connectlist[listnum].address);
				            sleep(1);
			                send_to(temp_fd, buf);
				        
				        }
				    }
			          
			    } else {
			       get_time(&t_cur);
		           strncpy(temp_name , connectlist[listnum].name, strlen(connectlist[listnum].name));		    
		           sprintf(buf,"%s : %s : %s\n", cur_time, temp_name, buffer);

		    	    for (listnum = 0; listnum < NB_USER; listnum++) {
			    	    if(connectlist[listnum].fd != 0){
				
			    		    send_to(connectlist[listnum].fd, buf);
				        }
			        }
			    
	    		memset(buffer,0,MAX);
    			memset(buf,0,MAX+30);
			    }
             }
			
			
	}
	
} /* deal_with_data */




void read_socks() {
	int listnum;	     
	
	if (FD_ISSET(sock,&socks)){
		handle_new_connection();                 
	}
	
	
	for (listnum = 0; listnum < NB_USER; listnum++) {
		if (FD_ISSET(connectlist[listnum].fd,&socks)){
	  		deal_with_data(listnum);
	 	} 
    } 
                  
	
}  /* Read_socks */





void fatal(int i, char *message) {
   
   openlog( "chat_server", LOG_PID, LOG_USER);
   if( i == 0){
       syslog(LOG_ERR, "%s : %s", message, strerror(errno));
   } if( i == 1){
       syslog(LOG_INFO, "%s",message);
   }
}


