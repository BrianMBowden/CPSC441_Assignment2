/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * ============================================================================
 * tweetClient.h
 * ============================================================================
 * Client side of cryptoTweet
 * ============================================================================
 */
#ifndef TWEETCLIENT_H
#define TWEETCLIENT_H

#define LOCAL_IP "127.0.0.1" //change this string if another IP is necessary
#define PORT 6666
#define MAX_WORD_LENGTH 0xFF
#define MAX_TWEET 0x0FFF

void menuPrompt();
void menuPrompt2();
void intitializeClient(struct sockaddr_in *address);
char* getSentence();
char* receiveFromServer(int* sockfd);
void handleError(int error);

#endif