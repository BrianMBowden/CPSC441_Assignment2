/* ============================================================================
 * Author: Brian Bowden
 * Date: October 20, 2018
 * Due: October 21, 2018
 * Class: CPSC 441 || Tut 04
 * ============================================================================
 * tweetClient.c
 * executable: ./tweetClient
 * ============================================================================
 * Client side of cryptoTweet
 * TCP
 * 
 * initialize client
 * open socket, attempt connection
 * menu prompt for user for encryption, decryption or quit
 * user enters string based on their choice from menu
 * if they are encrypting, they will be given additional choice about which method
 *  of encryption
 * wraps string with method to pass to server
 * waits for a response
 * quits when the method chosen is quit
 * 
 * 
 * ============================================================================
 * Bugs: it is not recommended to use punctuation
 * 
 * Testing: Testing done on my laptop, which means the IP is always local
 *          (can change the IP address in the tweetClient.h definitions under LOCAL_IP)
 *          Done with two terminal windows for client and server
 * 
 * ============================================================================
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/time.h>
#include <sys/select.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#include "tweetClient.h"

//const char dont_change[] = "\n\r\\\"\',.!@$#&{}()*+-=_:;?|1234567890<> ";

int main(int argc, char** argv){

    //network variables
    int sock_fd;
    struct sockaddr_in server;

    //message variables
    char tweet[MAX_TWEET];
    char tweet_back[MAX_TWEET];

    //menu variables
    char choice;

    //variables to handle whether to handle errors
    int con;

    /* Initialize address */
    intitializeClient(&server);

    /* Use localhost address */
    server.sin_addr.s_addr = inet_addr(LOCAL_IP); //may need to change this for other testing

    /* Create client socket, check for errors */
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1){
        handleError(1);
    }

    /* attempt connection */
    con = connect(sock_fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if (con == -1){
        handleError(2);
    } 

    printf("Connection accepted!!! Hooray!!!\n\n");

    menuPrompt();
    /* wait for user choice */
    scanf(" %c", &choice);

    while (choice != 'q' && choice != 'Q'){
        if (choice == 't' || choice == 'T'){
            //enter encryption side
            printf("--------------- ENCRYPTION MODE ---------------\n\n");
            getchar(); //flush the buffer 
            printf("please enter your message:\n");
            //add room for wrapper
            strcpy(tweet+2, getSentence());
            menuPrompt2();
            /* wait for user to choose method of encryption */
            scanf(" %c", &choice);
            // make sure user enters a valid choice
            if (choice != 'w' && choice != 's' && choice != 'h'){ 
                printf("not a valid choice, returning to starting menu...\n");
            }
            else{
                getchar(); //flush the buffer
                //wrapping the tweet
                tweet[0] = choice;
                tweet[1] = '\n';
                printf("sending \" %s \" to server to be encoded...\n", tweet);
                /* send tweet to server to be encoded */
                send(sock_fd, tweet, strlen(tweet), 0);
                strcpy(tweet_back, receiveFromServer(&sock_fd));
                printf("message received from server: \n %s\n", tweet_back);
                printf("-----------------------------------------------\n\n");
            }
        }
        else if (choice == 'd' || choice == 'D'){
            printf("--------------- DECRYPTION MODE ---------------\n\n");
            getchar(); //flush the buffer
            printf("please enter your message:\n");
            // add room for wrapper
            strcpy(tweet + 2, getSentence());
            printf("sending \" %s \" to server to be decoded...\n", tweet);
            //wrap the encoded tweet
            tweet[0] = 'd';
            tweet[1] = '\n';
            /* send encoded tweet to server */
            send(sock_fd, tweet, strlen(tweet), 0);
            strcpy(tweet_back, receiveFromServer(&sock_fd));
            printf("message received from server: \n %s\n", tweet_back);
            printf("-----------------------------------------------\n\n");
        }
        else{
            printf("not a valid choice, please try again\n"); 
            getchar(); //flush the buffer
        }
        //some sleep time to see any extra messages
        sleep(1);
        //prompt for more input
        menuPrompt();
        scanf(" %c", &choice);
        if (choice == 'q' || choice == 'Q'){
            getchar();
            strcpy(tweet, "q\n");
            /* make sure the server knows we're leaving */
            send(sock_fd, tweet, strlen(tweet), 0);
        }

    }

    printf("quitting program...\n");
    close(sock_fd);

    return 0;
}

void menuPrompt(){
    printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    printf("Please choose from the selections below\n");
    printf("      t - write a tweet\n");
    printf("      d - decrypt a tweet\n");
    printf("      q - quit\n");
    printf("selection: ");
}

void menuPrompt2(){
    printf("\n\nhow would you like to encode your tweet?\n");
    printf("      s - sequentially\n");
    printf("      w - word sum\n");
    printf("      h - hashbrowns\n");
    printf("      q - quit\n");
    printf("selection: ");
}

//client side initialization
void intitializeClient(struct sockaddr_in *address){

    memset(address, 0, sizeof(&address)); //initialize address_server to zero
    address->sin_family = AF_INET;  //AF_INET == IPv4 protocol
    address->sin_port = htons(PORT); //convert little endian to big endian
    address->sin_addr.s_addr = htonl(INADDR_ANY); //IADDR_ANY == any local IP

    //recycled from TCP-proxy assignment
}

//parses user input for full sentence, add null terminator
char* getSentence(){
    char* sentence;
    char c;
    int length;
    length = 0;
    sentence = (char*)malloc(sizeof(char) * MAX_TWEET);
    while ( ((c = getchar()) != '\n') && length < MAX_TWEET){
        sentence[length++] = c;
    }
    sentence[length] = '\0';
    return sentence;
}

//receive messages from server
char* receiveFromServer(int* sockfd){
    char* message_rcvd;
    int bytes_rcvd;
    message_rcvd = (char*)malloc(MAX_TWEET * sizeof(char));
    if ((bytes_rcvd = recv(*sockfd, message_rcvd, MAX_TWEET, 0)) > 0){
        message_rcvd[bytes_rcvd] = '\0';
    }
    else{
        close(*sockfd);
        handleError(3);
    }
    return message_rcvd;
}

//simple error handler
void handleError(int error){
    switch(error){
        case 1:
            printf("Socket creation failed... \n");
            exit(error);
        case 2:
            printf("Connection failed... \n");
            exit(error);
        case 3:
            printf("Receiving failed... \n");
            exit(error);
        default:
            printf("Oops, something went horribly wrong!\n");
            exit(error);
    }
}

