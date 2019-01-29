/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * Due: October 21, 2018
 * Class: CPSC 441 || Tut 04
 * 
 * ============================================================================
 * cryptoTweet.c
 * executable: ./tweetServer
 * ============================================================================
 * Server for crypto-tweet. 
 * TCP
 * 
 * reads a dictionary file (or creates it if it does not exist)
 * adds all words in dictionary (if any) to each of three hash maps
 * each hash map corresponds to a different style of encoding
 * 
 * opens socket, binds, listens
 * 
 * waits for input from client
 * parses client input for what to do
 * decrypts encoded data (looks in all hash tables)
 * encrypts plain text (client defines how they want it encoded)
 * sends back data based on client input
 * 
 * updates a dictionary of new words are sent.
 * 
 * ============================================================================
 * Bugs: - although tested, some errors are likely not handled properly because I 
 *       may not have thought about them at the time.
 *       - DOES NOT HANDLE PUNCTUATION! any punctuations will be included with 
 *       encryption or decryption, for best results, do not punctuate.
 *       - Dictionary gets wiped on any undefined behavior (SIGSEV, SIGKILL, etc.)
 *       - although the client side makes the input well-formed, a different client
 *         would not go over well...
 * 
 * Testing: Testing done on my laptop, which means the IP is always local
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

#include "cryptoTweet.h"
#include "SeqCcheck.h"
#include "Hashbrown.h"

const char dont_change[] = "\n\r\\\"\',.!@$#&{}()*+-=_:;?|1234567890<> ";

int main (int argc, char** argv){

    //file handling variables
    FILE *dictionary_fp;
    char BUFFER[MAX_BUFFER_SIZE];
    char DICTIONARY[MAX_BUFFER_SIZE];

    //hashtable variables
    char* map_seq[MAX_BUFFER_SIZE];
    char* map_sum[MAX_BUFFER_SIZE];
    char* map_brown[MAX_BUFFER_SIZE];

    //token parsing variables
    char* token;
    const char s[2] = ";"; //break buffer on new lines in dictionary
    
    //encoding variables
    char* encodedWord;

    //server variables
    int sock_fd;
    int accept_socket;
    struct sockaddr_in server;
    int bnd;

    //message variables
    char tweet[MAX_TWEET];
    char tweet_back[MAX_TWEET];
    char word[MAX_WORD];
    char method;
    char* start;
    char* white;
    char* end;

    //timeout variables
    fd_set set;
    struct timeval timeout;
    int rv;

    //open file
    dictionary_fp = fopen("./dictionary.txt", "r"); 

    //read from file into buffer
    if (fgets(BUFFER, MAX_BUFFER_SIZE, dictionary_fp) == NULL){
        printf("dictionary is empty\n");
    }

    //close file
    fclose(dictionary_fp);

    //re-open to truncate file
    dictionary_fp = fopen("./dictionary.txt", "w+");

    strcpy(DICTIONARY, BUFFER);
    //initialize hashtables
    SinitializeMap(map_seq);
    SinitializeMap(map_sum);
    initializeMap(map_brown);

    //put dictionary into hashtables
    token = strtok(BUFFER, s);
    while (token != NULL){
        encodedWord = encodeStringSeq(map_seq, token);
        encodedWord = getEncodingSum(map_sum, token);
        encodedWord = hashbrown(map_brown, token);
        token = strtok(NULL, s); //keep looping through 
    }

#if DEBUG
    SprintMap(map_seq);
    printf("\n");
    SprintMap(map_sum);
    printf("\n");
    printMap(map_brown);
    printf("\n");
#endif

    /* initialize server */
    intitializeServer(&server);

    /* Open socket */
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1){
        handleError(2);
    }
    /* bind to socket */
    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 ){
        close(sock_fd);
        handleError(3);
    }

    bzero(tweet_back, MAX_TWEET);


    /* start looping */
    while(sock_fd){
        /* listen on socket */
        if(listen(sock_fd, 1) == -1){
            close(sock_fd);
            handleError(4);
        }

        /* initialize timeout stuff */
        FD_ZERO(&set);
        FD_SET(sock_fd, &set);
        timeout.tv_sec = 120;
        timeout.tv_usec = 0;

        /* Don't listen forever */
        rv = select(sock_fd + 1, &set, NULL, NULL, &timeout);
        if (rv == -1){
            close(sock_fd);
            handleError(5);
        }
        if (rv == 0){
            printf("timeout\n");
            break;
        }
        /* credit to user: mpromonet from https://stackoverflow.com/questions/3444729/using-accept-and-select-at-the-same-time
        modified slightly 
        re-used this from my last assignment*/

        /* accept a connection */
        if((accept_socket = accept(sock_fd, NULL, NULL)) == -1){
            close(sock_fd);
            handleError(6);
        }

        /* get whatever client is sending */
        while(recv(accept_socket, tweet, MAX_TWEET, 0) > 0){
            printf("receiving something... \n");
            printf("-------------------------------------------\n");
            printf("%s\n", tweet);
            printf("-------------------------------------------\n");

            /* find out what we're doing */
            method = tweet[0];
            bzero(tweet_back, MAX_TWEET);

            if (method == 'd'){
                /* take sentence apart */
                start = strchr(tweet, '\n') + 1; //client well-formed input
                white = strchr(tweet, ' '); // first white space or null
                end = strchr(tweet, '\0'); //find null terminator
                while(start < end){
                    if (white != NULL){
                        // multiple words
                        strncpy(word, start, white - start);
                        printf("%s\n", word);
                        start = white + 1;
                        white = strchr(start, ' ');
                    }
                    else{
                        // last or only word
                        strncpy(word, start, end - start);
                        printf("%s\n", word);
                        start = end;
                    }
                    /* try encoding in all hashtables */
                    //sequential
                    if((encodedWord = Sretreive(map_seq, word)) != NULL){
                        printf("seqC hash\n");
                        strcat(encodedWord, " "); // add a space
                        strcat(tweet_back, encodedWord);
                    }
                    //word sum
                    else if((encodedWord = Sretreive(map_sum, word)) != NULL){
                        printf("word hash\n");
                        strcat(encodedWord, " "); // add a space
                        strcat(tweet_back, encodedWord);
                    }
                    //hashbrown
                    else if((encodedWord = retreiveFromMap(map_brown, word)) != NULL){
                        printf("hashbrowns\n");
                        strcat(encodedWord, " "); // add a space
                        strcat(tweet_back, encodedWord);
                    }
                    else{
                        strcat(tweet_back, " ??? "); // did not find word in any map
                    }
                }
                printf("sending | %s | through socket\n", tweet_back);
                /* send our decoded tweet back */
                send(accept_socket, tweet_back, strlen(tweet_back), 0);
            }
            /* quit was selected */
            else if (method == 'q'){
                /* close sockets */
                close(sock_fd);
                close(accept_socket);
                printf("exiting cryptoTweet\n");

                //update dictionary
                fputs(DICTIONARY, dictionary_fp);
                fclose(dictionary_fp);

                //destroy hashtables
                SdestroyMap(map_seq);
                SdestroyMap(map_sum);
                destroyMap(map_brown);

                // get out
                exit(-1);
            }
            else{
                /* encode sentence based on method */
                if (method == 's'){
                    //sequential
                    start = strchr(tweet, '\n') + 1;
                    white = strchr(tweet, ' ');
                    end = strchr(tweet, '\0'); //find null terminator
                    while(start < end){
                        if (white != NULL){
                            strncpy(word, start, white - start);
                            printf("%s\n", word);
                            start = white + 1;
                            white = strchr(start, ' ');
                        }
                        else{
                            strncpy(word, start, end - start);
                            printf("%s\n", word);
                            start = end;
                        }
                        //check if this word is in our running dictionary
                        if(!inDictionary(DICTIONARY, word)){
                            //if not, add it
                            addToDictionary(DICTIONARY, word);
                        }
                        encodedWord = encodeStringSeq(map_seq, word);
                        printf("%s\n", encodedWord);
                        strcat(encodedWord, " ");
                        strcat(tweet_back, encodedWord);
                    }
                    printf("sending | %s | through socket\n", tweet_back);
                    /* send our encoded tweet */
                    send(accept_socket, tweet_back, strlen(tweet_back), 0);
                }
                else if (method == 'w'){
                    //wordsum
                    start = strchr(tweet, '\n') + 1;
                    white = strchr(tweet, ' ');
                    end = strchr(tweet, '\0'); //find null terminator
                    while(start < end){
                        if (white != NULL){
                            strncpy(word, start, white - start);
                            printf("%s\n", word);
                            start = white + 1;
                            white = strchr(start, ' ');
                        }
                        else{
                            strncpy(word, start, end - start);
                            printf("%s\n", word);
                            start = end;
                        }
                        if(!inDictionary(DICTIONARY, word)){
                            addToDictionary(DICTIONARY, word);
                        }
                        encodedWord = getEncodingSum(map_sum, word);
                        printf("%s\n", encodedWord);
                        strcat(encodedWord, " ");
                        strcat(tweet_back, encodedWord);
                    }
                    printf("sending | %s | through socket\n", tweet_back);
                    /* send our encoded tweet */
                    send(accept_socket, tweet_back, strlen(tweet_back), 0);
                }
                else {
                    //hashbrown
                    start = strchr(tweet, '\n') + 1;
                    white = strchr(tweet, ' ');
                    end = strchr(tweet, '\0'); //find null terminator
                    while(start < end){
                        if (white != NULL){
                            strncpy(word, start, white - start);
                            printf("%s\n", word);
                            start = white + 1;
                            white = strchr(start, ' ');
                        }
                        else{
                            strncpy(word, start, end - start);
                            printf("%s\n", word);
                            start = end;
                        }
                        if(!inDictionary(DICTIONARY, word)){
                            addToDictionary(DICTIONARY, word);
                        }
                        encodedWord = hashbrown(map_brown, word);
                        printf("%s\n", encodedWord);
                        strcat(encodedWord, " ");
                        strcat(tweet_back, encodedWord);
                    }
                    printf("sending | %s | through socket\n", tweet_back);
                    /* send our encoded tweet */
                    send(accept_socket, tweet_back, strlen(tweet_back), 0);
                }
                printf("made it out, waiting for more input\n");
            }
            //reset everything to NULL to not get extra garbage in buffers
            memset(tweet, 0, MAX_TWEET);
            memset(tweet_back, 0, MAX_TWEET);
            start = NULL;
            white = NULL;
            end = NULL;
        }
        

    }


    //update dictionary
    fputs(DICTIONARY, dictionary_fp);
    fclose(dictionary_fp);

    //destroy hashtables
    SdestroyMap(map_seq);
    SdestroyMap(map_sum);
    destroyMap(map_brown);

    return 0;
}

//simple error handler
void handleError(int error){
    switch(error){
        case 1:
            puts("Failed to read Dictionary, exiting...");
            exit(error);
        case 2:
            printf("Socket creation failed... \n");
            exit(error);
        case 3:
            printf("Failed to bind socket...\n");
            exit(error);
        case 4:
            printf("Socket listening failed... \n");
            exit(error);
        case 5:
            printf("Timing failed... \n");
            exit(error);
        default:
            puts("something weird occurred, exiting...\n");
            exit(-1);
    }
}

//checks all entries in dictionary for an exact match to string
bool inDictionary(char* buffer, char* string){
    if(strstr(buffer, string) != NULL)
        return true;
    else
        return false;
}

//add string to dictionary
void addToDictionary(char* buffer, char* string){
    //remove null terminator, add it back in
    buffer[strlen(buffer)] = ';';
    strcat(buffer, string);
    buffer[strlen(buffer)] = '\0';
}

//initiatlization of sockaddr_in struct
void intitializeServer(struct sockaddr_in *address){

    memset(address, 0, sizeof(&address)); //initialize address_server to zero
    address->sin_family = AF_INET;  //AF_INET == IPv4 protocol
    address->sin_port = htons(PORT); //convert little endian to big endian
    address->sin_addr.s_addr = htonl(INADDR_ANY); //IADDR_ANY == any local IP

    //recycled from TCP-proxy assignment
}

