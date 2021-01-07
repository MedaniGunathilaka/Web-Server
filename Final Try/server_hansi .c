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


#define PORT 5000  /* Server port where we are connecting(Standard HTTP Port)*/
char *html_type;
char buffer_time[30000] = {0};

int init_server;
int init_socket;
long get_v;
int listen_no;
int bind_no;
struct sockaddr_in sck_addrs;
int address_length = sizeof(sck_addrs);


void socket_binder()/*associates the socket with its local address */
    {
        bind_no = bind(init_server, (struct sockaddr *)&sck_addrs, sizeof(sck_addrs));
        if ( bind_no== -1)
        {
            perror("An error has been occured in socket_binder() function");
            exit(EXIT_FAILURE);
        }
    }



void socket_maker()//Making the socket
    {
        init_server = socket(AF_INET, SOCK_STREAM, 0);
        if ((init_server) == -1)
        {
            perror("An error has been occured in socket_maker() function"); 
            exit(EXIT_FAILURE);
        }
    }



void listenr()
{ listen_no = listen(init_server, 10);
    if (listen_no== -1)
    {
        perror("In listen");
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
        FILE *file = fopen(file_name, "r"); //fopen = open file
        size_t bufsize;
        if (file != NULL)
        {
            if (fseek(file, 0L, SEEK_END) == 0) //fseek - sets the file position of the sream to given offset
            {                         
                bufsize = ftell(file); //return current position of file
                entry = malloc(sizeof(char) * (bufsize + 1));
                fseek(file, 0L, SEEK_SET);
                fread(entry, sizeof(char), bufsize, file);
                response_send(sr, "HTTP/1.1 200 OK", entry, bufsize);
            }
        }
        else
        {
        char *error = "<!DOCTYPE html>"//char *error = "404 NOT FOUND";Here a html file shows 404 not found using an image
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
                "</body>"
                "</html>";
            
                html_type = "text/html";
                response_send(sr, "HTTP/1.1 404 NOT FOUND", error, strlen(error));
        }
    }


int main(int argc, char const *argv[])//Main function
{
    char req_type[4];
    char req_path[1024];

    socket_maker();//Making a new socket or mounting a new socket
   
    sck_addrs.sin_addr.s_addr = INADDR_ANY;
    sck_addrs.sin_port = htons(PORT);
    sck_addrs.sin_family = AF_INET;

    /*memset(sck_addrs.sin_zero, '\0', sizeof sck_addrs.sin_zero); //memset()=use to fill a block of memory with a particular value.*/

    //init socket going to listen some addresses.The address is here.
    socket_binder();
    listenr();

    while (1)
    {
        printf("\n****Just waiting for the port****\n\n");
        if ((init_socket = accept(init_server, (struct sockaddr *)&sck_addrs, (socklen_t *)&address_length)) < 0)//While listening from the init_socket(in the port 3060)
        //Create a new socket to communicate with the client
        {
            perror("error has been occured in accept");
            exit(EXIT_FAILURE);
        }

        read(init_socket, buffer_time, 1024); //Reads data which previously written to a file
        sscanf(buffer_time, "%s %s", req_type, req_path);

        char *sk = strrchr(req_path, '.') + 1; //Return last occurence in a string
        char *name = strtok(req_path, "/");  // Split string to a token

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
			
			"<a href='http://localhost:5000/video.html'>Video Page</a><br/>"
			"<a href='http://localhost:5000/audio.html'>Music Page</a>"
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