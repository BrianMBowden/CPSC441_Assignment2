/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * ============================================================================
 * cryptoTweet.c
 * ============================================================================
 * server side of cryptoTweet
 * ============================================================================
 */
#ifndef CRYPTOTWEET_H
#define CRYPTOTWEET_H

#define MAX_BUFFER_SIZE 0xFFFF
#define DEBUG 0
#define PORT 6666
#define MAX_TWEET 0x0FFF
#define MAX_WORD 0xFF

//server functions
void handleError(int error);
bool inDictionary(char* buffer, char* string);
void addToDictionary(char* buffer, char* string);

//network functions
void intitializeServer(struct sockaddr_in *address);
#endif
