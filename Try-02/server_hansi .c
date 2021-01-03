
/*
server_hansi.c
 *
 *  Created on: January 01, 2021
 *      Author: Medani
 *
 * A web server in C language using only the standard libraries.
 * 
 *
 */

#include <stdio.h>//standard input output header file
#include <sys/socket.h>//internet protocol family
#include <unistd.h> //standard symbolic constants and types
#include <stdlib.h>//standard library definitions
#include <netinet/in.h>//sockaddr_in structure
#include <string.h>//string operations like memset
#include <ctype.h>//declares several functions that are useful for testing and mapping characters
#include <errno.h> //give unique system error numbers
#include <arpa/inet.h> //for "in_port" type, "in_addr_t" type, also the in_addr type structure.


#define PORT 8001  /* Server port where we are connecting(Standard HTTP Port)*/
char *html_type;
//char *init_msg = "Hello init_server";
char buffer_time[30000] = {0};


int init_server, 
int init_socket;
long get_v;
struct sockaddr_in sck_addrs;
int address_length = sizeof(sck_addrs);

void socket_binder()
{
    if (bind(init_server, (struct sockaddr *)&sck_addrs, sizeof(sck_addrs)) == -1)
    {
        perror("error has been occured in socket_binder() function");
        exit(EXIT_FAILURE);
    }
}

void socket_maker()
{
    if ((init_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("error has been occured in socket_maker() function"); 
        exit(EXIT_FAILURE);
    }
}


void listen()
{
    if (listen(init_server, 10) < 0)
    {
        perror("error has been occured in listen() function");
        exit(EXIT_FAILURE);
    }
}

void response_send(int sr, char *header, void *body, int content)
{
    char response[content + 100];
    int response_length = sprintf(response, 
                                  "%s\n"
                                  "Connection: close\n"
                                  "Content-Length: %d\n"
                                  "Content-Type: %s\n"
                                  "\n",
                                  header,
                                  content,
                                  html_type);
     /*Copy block of memory from a location to an another location*/
    memcpy(response + response_length, body, content); 
     /*Transmit a message to an another socket*/
    send(sr, response, response_length + content, 0); 
}

void send_file(int sr, char *file_name)
{
    char *entry;
    FILE *file = fopen(file_name, "r"); //fopen - open file
    size_t bufsize;
    if (file != NULL)
    {
        if (fseek(file, 0L, SEEK_END) == 0)
        {                          //fseek - sets the file position of the sream to given offset
            bufsize = ftell(file); //return current position of file
            entry = malloc(sizeof(char) * (bufsize + 1));
            fseek(file, 0L, SEEK_SET);
            fread(entry, sizeof(char), bufsize, file);
            response_send(sr, "HTTP/1.1 200 OK", entry, bufsize);
        }
    }
    else
    {
	   char *error = "<!DOCTYPE html>"
			 "<html>"
			 "<head>"
			     "<title>Page Title</title>"
			 "</head>"
			 "<style>"
                 "img {width: 40%;}"
			 	 ".center {display: block;margin-left: auto; margin-right: auto;width: 50%;}"
	             "body {background-image: url('error404.jpg');}"
			 "</style>"
 			 "<body>"
				"<h1 style='text-align:center;'>WELCOME!!!!</h1>"
   				"<p style='text-align:center'>Web Server Assignment</p>"
    			"<p style='text-align:center;'>Test Web Page</p>"
			 "</html>";
        //char *error = "404 NOT FOUND";
        html_type = "text/html";
        response_send(sr, "HTTP/1.1 404 NOT FOUND", error, strlen(error));
    }
}

int main(int argc, char const *argv[])
{
    char req_type[4];
    char req_path[1024];

    socket_maker();
    sck_addrs.sin_family = AF_INET;
    sck_addrs.sin_addr.s_addr = INADDR_ANY;
    sck_addrs.sin_port = htons(PORT);

    memset(sck_addrs.sin_zero, '\0', sizeof sck_addrs.sin_zero); //memset() is used to fill a block of memory with a particular value.

    socket_binder();
    listen();

    while (1)
    {
        printf("\n****Waiting for the port****\n\n");
        if ((init_socket = accept(init_server, (struct sockaddr *)&sck_addrs, (socklen_t *)&address_length)) < 0)
        {
            perror("error has been occured in accept");
            exit(EXIT_FAILURE);
        }

        read(init_socket, buffer_time, 1024); //reads data previously written to a file
        sscanf(buffer_time, "%s %s", req_type, req_path);

        char *sk = strrchr(req_path, '.') + 1; //return last occurence in string
        char *name = strtok(req_path, "/");      // split string to token

        if (sk)
            html_type = sk;
        else
            html_type = NULL;

        if (!strcmp(req_type, "GET") && !strcmp(req_path, "/"))
        { // compare two string character by character
            char *data = "<!DOCTYPE html>"
			 "<html>"
			 "<head>"
			 "<title>Page Title</title>"
			 "</head>"
			 "<style>"
                         	"img {width: 40%;}"
                         	"img {border-radius: 50%;}"
			 	".center {display: block;margin-left: auto; margin-right: auto;width: 50%;}"
	                 	"body {background-image: url('img3.jpg');}"
			 "</style>"
 			 "<body>"
				"<h1 style='text-align:center;'>WELCOME!!!!</h1>"
   				" <p style='text-align:center'>Web Server Assignment</p>"
    				"<p style='text-align:center;'>Test Web Page</p>"
   			 	"<img src='img2.jpg' class='center'>"
				"<a href='http://localhost:8001/lec.pdf'>Lecture PDF</a><br/>"
				"<a href='http://localhost:8001/video.html'>Video Page</a><br/>"
				"<a href='http://localhost:8001/audio.html'>Music Page</a>"
			 "</body>"
			 "</html>";
            html_type = "text/html";
            response_send(init_socket, "HTTP/1.1 200 OK", data, strlen(data));
        }
        else
        {
            send_file(init_socket, name);
        }
        printf("****Response has been sent from init_server****");
    }
    return 0;
}
