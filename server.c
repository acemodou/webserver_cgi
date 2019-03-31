#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "src/passiveTCP.c"
#define BUFF_SIZE 1024

/* Alll header contents accepted by our server */


static const char txt_head[] =  "HTTP/1.0 200 OK\n"\
                                        "Content-type: text/html\n\n";

static const char err_head[] =   "HTTP/1.0 404 NOT FOUND\n"\
                                        "Content-type: text/html\n\n";

static const char gif_head[] =   "HTTP/1.0 200 OK \n"\
                                        "Content-Type: image/gif \n"\
                                        "Last-Modified: Mon, 25 Apr 2005 21:06:18 GMT \n"\
                                        "Expires: Sun, 17 Jan 2038 19:14:07 GMT \n"\
                                        "Date: Thu, 09 Mar 2006 00:15:37 GMT\n\n";

static const char jpg_head[] =   "HTTP/1.0 200 OK \n"\
                                        "Content-Type: image/jpg \n"\
                                        "Last-Modified: Mon, 25 Apr 2005 21:06:18 GMT \n"\
                                        "Expires: Sun, 17 Jan 2038 19:14:07 GMT \n"\
                                        "Date: Thu, 09 Mar 2006 00:15:37 GMT\n\n";

static const char css_head[] = "HTTP/1.0 200 OK\n"\
                                        "Content-type: text/css\n\n";

static const char js_head[] = "HTTP/1.0 200 OK\n"\
                                        "Content-type: text/javascript\n\n";

#define STRLEN(s) (sizeof(s)/sizeof(s[0])-1)


int countConnect;
char* rt;
char* indFile;
char* port;
char * dummy;

/*This struct is used to move data around between threads, and even in the main thread CGI
  to enforce generalization whether data is copied to a thread or just passed to a function */
struct getdata{
    int client_desc;
    char request [BUFF_SIZE];
};

void pLog(char* lPath, char* ip, char* buffer) {
    FILE * access_log = fopen(lPath, "ab+");
    if (access_log == NULL){
        perror("cannot open access log file!");
        exit(1);
    }
    char *logMsg = (char*) malloc(sizeof(char)*(BUFF_SIZE+15));
    sprintf(logMsg,"%s\n%s\n\n",ip,buffer);
    if (fputs(logMsg, access_log) < 0){
        perror("error witing to access_log");
        exit(1);
    }

    if (fclose(access_log) != 0) {
        perror("error closing access log file!\n");
        exit(1);
    }
    free(logMsg);
}

void* Client_CGI_Handler(void * getData){
    struct getdata* getdata = (struct getdata *) getData;
    char* reBuff = getdata->request;
    int cSockID = getdata->client_desc;

    //Getting the client ip from the socket descriptor and printing to the access log
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(cSockID, (struct sockaddr *)&addr, &addr_size);
    char clientip [20];
    strcpy(clientip, inet_ntoa(addr.sin_addr));
    pLog("logs/access_log.txt",clientip,reBuff);

    //Copying the request data into a local array to tokenize and later clear the passed reference
    char messageBod [strlen(reBuff)];
    strcpy(messageBod,reBuff);

    //Parsing the requested file path
    strtok_r(reBuff," ", &reBuff);
    char * reqFile = strtok_r(reBuff," ", &dummy);
    reqFile=strtok_r(reqFile,"?", &dummy);

    //Forking to a new process to handle CGI
    int procID = fork();
    if(procID==0) { //child

        //Move output to connected socket
        dup2(cSockID,1);

        //Parsing from the path to get the required file name and type, also to get class name for running in Java
        //Note that we pass in the whole request to the java file and it parses the request whether it's post or get
        char dirParsing[strlen(reqFile)+1];
        strcpy(dirParsing,reqFile);
        char* savingPointer;
        char* dir = strtok_r(dirParsing+1,"/", &savingPointer);
        chdir(dir);
        dir=strtok_r(savingPointer,"/",&dummy);
        char* execName = strtok(dir,".");

        //Starting the java program that handles the CGI Request.
        int val=execlp ("java","java", execName, messageBod, (char*)NULL);
        if(val==-1) {
            perror("ERROR on exec");
            exit(1);
        }
    } else {
        //Closing the client socket in the parent, which gets closed automatically in the child
        //when it terminates
        close(cSockID);
        free(getData);
    }
}

void* clientHandler(void * getData) {
    char *seBuff = (char *) malloc(sizeof(char) * BUFF_SIZE);
    struct getdata* getdata = (struct getdata *) getData;
    char* reBuff = getdata->request;
    int cSockID = getdata->client_desc;
    int fedd = -1;
    int rdSz = 0;

    //Getting the client ip from the socket descriptor and printing to the access log
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(cSockID, (struct sockaddr *)&addr, &addr_size);
    char clientip [20];
    strcpy(clientip, inet_ntoa(addr.sin_addr));
    pLog("logs/access_log.txt",clientip,reBuff);

    //Copying the request data into a local array to tokenize and later clear the passed reference
    char messageBod [strlen(reBuff)];
    strcpy(messageBod, reBuff);

    //Parsing the requested file path
    strtok_r(reBuff, " ", &reBuff);
    char *reqFile = strtok_r(reBuff, " ", &reBuff);
    reqFile = strtok_r(reqFile, "?", &dummy);

    //Sending the correct HTTP header based on the file extension
    if (strcmp(reqFile, "/") == 0) {
        fedd = open(indFile, O_RDONLY);
        send(cSockID, txt_head, STRLEN(txt_head), 0);
        while (rdSz = read(fedd, seBuff, BUFF_SIZE - 1)) {
            send(cSockID, seBuff, rdSz, 0);
        }
    } else {
        fedd = open(reqFile + 1, O_RDONLY);
        if (fedd == -1) {
            send(cSockID, err_head, STRLEN(err_head), 0);
            fedd = open("404.html", O_RDONLY);
        } else {
            char fileExtParser[strlen(reqFile)];
            strcpy(fileExtParser, reqFile);
            char *fileName;
            strtok_r(fileExtParser, ".", &fileName);
            char *extension = strtok_r(fileName, ".", &dummy);
            if (strcmp(extension, "gif") == 0)
                send(cSockID, gif_head, STRLEN(gif_head), 0);
            else if (strcmp(extension, "jpg") == 0)
                send(cSockID, jpg_head, STRLEN(jpg_head), 0);
            else if (strcmp(extension, "html") == 0)
                send(cSockID, txt_head, STRLEN(txt_head), 0);
            else if (strcmp(extension, "css") == 0)
                send(cSockID, css_head, STRLEN(css_head), 0);
            else
                send(cSockID, js_head, STRLEN(js_head), 0);       
        }
        //Responding to the client by sending the opened file requested.
        while(rdSz = read(fedd,seBuff,BUFF_SIZE))
            send(cSockID,seBuff,rdSz,0);
    }
    //Closing the client socket and freeing the heap data
    close(cSockID);
    free(seBuff);
    free(getData);
}

void signal_Handler(int signal) {
    if (signal == SIGINT) {
        exit(0);
    }
}


void confReader(int* countConnect, char* rt, char* indFile, char* port) {
    FILE * fstream = fopen("./conf/httpd.conf","r");
    if (fstream == NULL){
        perror("cannot open config file!");
        exit(1);
    }
    size_t len;
    char** lines = (char **) malloc(sizeof(char*) * 4);
    for(int i=0; i<4; i++) {
        lines[i] = (char *) malloc(sizeof(char) * 100);
        if(getline(&lines[i], &len, fstream) == -1){
            perror("error reading from config file!");
            exit(EXIT_FAILURE);
        }
    }

    char* temp;
    temp = strtok(lines[0],"=");
    temp = strtok(NULL,"=");
    *countConnect = atoi(temp);
    temp = strtok(lines[1],"=");
    strcpy(rt,strtok(NULL,"="));
    rt[strlen(rt)-1] = '\0';
    temp = strtok(lines[2],"=");
    strcpy(indFile, strtok(NULL,"="));
    indFile[strlen(indFile)-1] = '\0';
    temp = strtok(lines[3],"=");
    strcpy(port,strtok(NULL,"="));
    port[strlen(port)-1] = '\0';
}

int main( int argc, char **argv ) {
    //Initializing and reading in the config file data then changing to rt directory.
    countConnect = 0;
    rt = (char *) malloc(sizeof(char) * 100);
    indFile = (char *) malloc(sizeof(char) * 100);
    port = (char *) malloc(sizeof(char) * 6);
    confReader(&countConnect, rt, indFile, port);
    chdir(rt);

    //Redirecting the stderr to the error log file
    freopen("logs/error_log.txt","a+",stderr);

    //Registering the signal handler for when the server is Interrupted
    signal(SIGINT, signal_Handler);
    int sockfedd, newsockfedd, clilen;
    ssize_t n;
    char *reBuff = (char *) malloc(sizeof(char) * BUFF_SIZE);

    //Using the abstraction of creating a TCP socket to hide unnecessary details
    sockfedd = passiveTCP(port,BUFF_SIZE);
    listen(sockfedd, 5);

    /* Accept actual connection from the client */
    while (1) {
        struct sockaddr_in cli_addr;
        clilen = sizeof(cli_addr);
        newsockfedd = accept(sockfedd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfedd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        /* If connection is established then start communicating */
        n = recv(newsockfedd, reBuff, BUFF_SIZE - 1, 0);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        if(n==0)
            continue;
        reBuff[n] = '\0';

        struct getdata* getdata = (struct getdata*) malloc(sizeof(struct getdata));
        getdata->client_desc = newsockfedd;
        strcpy(getdata->request, reBuff);
        bzero(reBuff, BUFF_SIZE+1);
        if (strstr(getdata->request, ".class") != NULL) {
            Client_CGI_Handler(getdata);
        } else{
            pthread_t tid;
            if (pthread_create(&tid, NULL, clientHandler, (void *) getdata) < 0) {
                perror("could not create thread");
                exit(1);
            }
            int err = pthread_detach(tid);
            if (err) {
                perror("could not make thread detachable");
            }
        }

    }
    return 0;
}
