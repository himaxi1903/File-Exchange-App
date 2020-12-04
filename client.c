#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h> 
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{	
	srand(time(NULL));
	int server, portNo, fd, bufferSize=256, readSize;
	struct sockaddr_in servAdd;     											// server socket address
	char ch, message[bufferSize];
	
	if(argc != 3){
		printf("Call using: %s <IP Address> <Port Number>\n", argv[0]);
		exit(0);
	}
	
	if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error in creating socket\n");
		exit(1);
	}
	
	servAdd.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNo);
	servAdd.sin_port = htons((uint16_t)portNo);

	if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){						//converts an address from presentation to network format
		fprintf(stderr, " inet_pton() Failed!\n");
		exit(2);
	}
	if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
		fprintf(stderr, "connect() Failed!\n");
		exit(3);
	}
	
	while(1){
		write(1,"Enter command (get, put, quit): ",strlen("Enter command (get, put, quit): "));

		memset(message,0,strlen(message));
		readSize=read(0, message, bufferSize);

		int ln = strlen(message) - 1;
		if (*message && message[ln] == '\n') 
		    message[ln] = '\0';

		write(server, message, readSize);

		if(strncmp(message,"get",3)==0){
			char *fileName=message;
			fileName+=4;

			fd=open(fileName, O_WRONLY|O_CREAT, 0700);
			if (fd == -1){
				write(1,"File could not be created! Check directory!\n",strlen("File could not be created! Check directory!\n"));
			}

	    	while(read(server, &ch, 1)>0){
	    		if(ch==4)
	    			break;
	    		write(fd, &ch, 1);
	    	}
	    	close(fd);

	    	write(1,"File Downloaded Successfully!\n",strlen("File Downloaded Successfully!\n"));
		}
		else if(strncmp(message,"put",3)==0){
			char *fileName=message;
			fileName+=4;

			fd = open(fileName, O_RDONLY);
			if (fd == -1){
				write(1,"File could not be opened! Please check if it exists and it does not permission issues\n",strlen("File could not be opened! Please check if it exists and it does not permission issues\n"));
			}

	    	while(read(fd, &ch, 1)>0){
	    		write(server, &ch, 1);
	    	}
	    	ch=4;
			write(server, &ch, 1);
	    	close(fd);
	    	write(1,"File Uploaded Successfully!\n",strlen("File Uploaded Successfully!\n"));
		}
		else if(strcmp(message, "quit")==0){
			close(server);
			exit(0);
		}
		else{
			write(1,"Only get <fileName>, put get <fileName> or quit commands allowed!\n",strlen("Only get <fileName>, put get <fileName> or quit commands allowed!"));
		}
	}	
}
