/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>

#include <string.h>

#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno; // port number
    socklen_t clilen;
     
    char buffer[1024];
     
     /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;
     
    int n;
    if (argc < 2) {
    	fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
	}
     
     /*Create a new socket
       AF_INET: Address Domain is Internet 
       SOCK_STREAM: Socket Type is STREAM Socket */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); //atoi converts from String to Integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");
     
    listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
   

    while(1){ 
    	 clilen = sizeof(cli_addr);
     /*accept function: 
       1) Block until a new connection is established
       2) the new socket descriptor will be used for subsequent communication with the newly connected client.
     */
     	 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	 if (newsockfd < 0) 
        	  error("ERROR on accept");
     
     	 bzero(buffer,1024);
     	 n = read(newsockfd,buffer,1023); //Read is a block function. It will read at most 255 bytes
     	 if (n < 0) error("ERROR reading from socket");
     	 printf("Here is the message: %s\n",buffer);
     
    // n = write(newsockfd,"I got your message",18); //NOTE: write function returns the number of bytes actually sent out ?> this might be less than the number you told it to send
     	 if (n < 0) error("ERROR writing to socket");
   
	 char *tmp1, *tmp2, *fname, *ext;
	 int fsize;
	 FILE *fp;
         char *f_fname = (char *)malloc(sizeof(char)*1000);
	 char *buff = (char *)malloc(sizeof(char)*1000);
	 ssize_t result;

	 tmp1 = strtok(buffer,"\n");
//	 printf("first string:%s\n",tmp1);

	 
	 tmp2 = strtok(tmp1," /");
	 tmp2 = strtok(NULL," /");
//	 printf("file name:%s\n",tmp2);
	 strcpy(f_fname,tmp2);
	 

	 fname = strtok(tmp2,".");
	 ext = strtok(NULL,".");
//	 printf("file name:%s\n",f_fname);
//	 printf("fname:%s\n",fname);
//	 printf("ext:%s\n",ext);

	 if((fp = fopen(f_fname,"r")) == NULL)
	 {
		 error("read_error");
	 }
	 else{
		 
	 	fseek(fp,0,SEEK_END);
	 	fsize = ftell(fp);
	 	fseek(fp,0,SEEK_SET);

	 //find file size

	 	char *data = (char *)malloc(fsize);
	 	fread(data, sizeof(char), fsize+1,fp);
	 	if(strcmp(ext,"html")==0)
	 	{
		     sprintf(buff,"HTTP/1.1 200 OK\r\nServer: myserver\r\nContent-Type: text/%s\r\nContent-Length: %d\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\n\r\n",ext,fsize);
//		     printf("buff:\n%s\n",buff);
//		     printf("data:\n%s\n",data);
		     write(newsockfd,buff,strlen(buff));
		     write(newsockfd,data,fsize);	
	 	}
		else if(strcmp(ext,"jpg")==0||strcmp(ext,"gif")==0)
	 	{
		     sprintf(buff,"HTTP/1.1 200 OK\r\nServer: myserver\r\nContent-Type: image/%s\r\nContent-Length: %d\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\n\r\n",ext,fsize);
//		     printf("buff:\n%s\n",buff);
//		     printf("data:\n%s\n",data);	
		     write(newsockfd,buff,strlen(buff));
		     write(newsockfd,data,fsize);	
		}
		else if(strcmp(ext,"ico")==0)
		{
		    sprintf(buff,"HTTP/1.1 200 OK\r\nServer: myserver\r\nContent-Type: image/x-icon\r\nContent_length: %d\r\nAccept_Range: bytes\r\nConnection: keep-alive\n\r\n",fsize);
		    write(newsockfd,buff,strlen(buff));
		    write(newsockfd,data,fsize);
		}
		else if(strcmp(ext,"pdf")==0)
		{
		    sprintf(buff,"HTTP/1.1 200 OK\r\nServer: myserver\r\nContent-Type: application/%s\r\nContent-Length:%d\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\n\r\n",ext,fsize);
		    write(newsockfd,buff,strlen(buff));
		    write(newsockfd,data,fsize);
		}
		else if(strcmp(ext,"mp3")==0)
		{
		    sprintf(buff,"HTTP/1.1200 OK\r\nServer: myserver\r\nContent-Type: audio/%s\r\nContent-Length: %d\r\nAccept-Rages: bytes\r\nConnection: keep-alive\n\r\n",ext,fsize);
		    write(newsockfd,buff,strlen(buff));
		    write(newsockfd,data,fsize);
		}
		else
		{
			sprintf(buff,"HTTP/1.1 404 Bad Request\r\nConnetion: close\r\n\r\n");


		}
		fclose(fp);
		free(data);
		free(buff);
		free(f_fname);
	 }
    close(newsockfd);
    }
    close(sockfd);
     return 0; 
}
