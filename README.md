# CGI Concurrent WebServer

This program is a webserver that serves a webpage for some basic math operations as well as some graphics.
The implementation follows RFC 1945 for the Hypertext Transfer Protocol -- HTTP/1.0
The code was developed and tested using Clion IDE.

Server configuration can be changed using config file ("conf/httpd.conf").


Here is an overview of how the program works:
 1) The web browser sends a request to the server on the specified port.
 2) The server determines whether the request will require running the CGI program.
  a) If it requires CGI, the server parses the request and creates a new child process to run the CGI program.
  b) if it does not require CGI, the server creates a new thread which becomes responsible of parsing the data in the request and responding to the sending client.
 3) The data needed for pthread and fork is encapsulated in the struct RequestData and passed to the handler functions.
 4) The child process output is redirected to the socket that holds the session between the server and the client.
    This allow the CGI output to be sent directly to the client.
 5) The CGI is written is Java; It takes the request as a program argument, parses the request data depending on the type of the    	request (POST or GET) and responds to the browser/client.
 6) All the requests are recorded in the access log file ("logs/access_log.txt").
 7) All the errors are recorded in the error log file ("logs/error_log.txt").
 8) Error prone functions return values are checked for correctness. In case of errors, error messages are logged, and the server   	terminates.

## USAGE
To start the server, follow the following commands to compile and run
1) gcc -pthread main.c
2) ./a.out

To send requests to the server, the easiest way is to use a web browser and hit the address below
localhost:3000

Note: the server port can be changed from the log file, but that would require changes in the 
CGI source code as well as the index.html file form actions.

To exit:
Hit Ctrl-C on the terminal (SIGINT)

