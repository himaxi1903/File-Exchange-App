#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void serviceClient(int client){
	int bufferSize=256, readSize, fd;
	char ch, command[bufferSize];
	while(1){
		printf("Waiting for client to enter command...!\n");
		if(readSize=read(client, command, bufferSize) < 0){
			fprintf(stderr, "Error in reading!\n");
			exit(1);
		}
		
		printf("Command received from client: %s\n", command);
		
		if(strncmp(command,"get",3)==0){
			char *fileName=command;
			fileName+=4;
			fd = open(fileName, O_RDONLY);
			if (fd == -1){
				write(1,"File could not be opened! Please check if it exists and it does not permission issues\n",strlen("File could not be opened! Please check if it exists and it does not permission issues\n"));
			}
	    	while(read(fd, &ch, 1)>0){
	    		write(client, &ch, 1);
	    	}
	    	ch=4;
			write(client, &ch, 1);
	    	close(fd);
		}
		else if(strncmp(command,"put",3)==0){
			char *fileName=command;
			fileName+=4;

			fd=open(fileName, O_WRONLY|O_CREAT, 0700);
			if (fd == -1){
				write(1,"File could not be created! Check directory!\n",strlen("File could not be created! Check directory!\n"));
			}
			while(read(client, &ch, 1)>0){
	    		if(ch==4)
	    			break;
	    		write(fd, &ch, 1);
	    	}
	    	close(fd);
		}
		else if(strcmp(command,"quit")==0){
			close(client);
			exit(0);
		}
		else{
			printf("Wrong command! Please enter get <fileName>, put <fileName> or quit.\n");
		}
	}
    exit(0); 
}

int main(int argc, char *argv[]){  
	int sd, portNumber, client;
	struct sockaddr_in servAdd;     	//server socket address
	
	if(argc != 2){
		printf("Call using: %s <Port Number>\n", argv[0]);
		exit(0);
	}

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		fprintf(stderr, "Error in creating socket\n");
		exit(1);
	}
	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);
	
	bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
	listen(sd, 6);
	
	while(1){
		printf("Waiting for the client to connect...!\n");	
		client = accept(sd,(struct sockaddr*)NULL,NULL);
		printf("A Client connected!\n");
		
		if(!fork()){
		  serviceClient(client);
		}
	}
	return 0;
}
