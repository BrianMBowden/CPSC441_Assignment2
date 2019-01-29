/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * ============================================================================
 * Hashbrown.c
 * ============================================================================
 * Hashing and pseudo-encryption library for strings
 *  
 *  Order:
 *      char* tweetWord
 *      char* map[SIZE]
 *      char* encodedWord
 *      
 *      initializeMap
 * 
 *      encodedWord = hashbrown(map, tweetWord)
 *      tweetWord = retreiveFromMap(map, encodedWord)
 * 
 *      destroyMap
 *         
 * ============================================================================
 * Bugs: Although I have tested this quite a bit for collisions, there is still
 *       a chance there will be some, and this does not handle collisions
 * 
 * Testing: Testing done with various strings
 * 
 * ============================================================================
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Hashbrown.h"


//works
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: initializeMap(char ** map)
 *  Input: uninitialized hashmap
 *  
 *  Usage: set all members of a hashmap to NULL 
 * -----------------------------------------------
 * */
void initializeMap(char ** map){
    for(int i = 0; i < MAX_MAP_SIZE; i++){
        map[i] = NULL;
    }
}

// works
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: destroyMap(char** map)
 *  Input: map - initialized hashmap
 *  
 *  Usage: cleanup memory allocated to the map 
 * -----------------------------------------------
 * */
void destroyMap(char ** map){
    for (int i = 0; i < MAX_MAP_SIZE; i++){
        free(map[i]);
    }
}

//works
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: printMap(char** map)
 *  Input: map - initialized hashmap
 *  
 *  Usage: primarily used for testing, prints out all non-NULL members of 
 *         map 
 * -----------------------------------------------
 * */
void printMap(char ** map){
    for (int i = 0; i < MAX_MAP_SIZE; i++){
        if (map[i] != NULL){
            printf("%s ------ %d\n", map[i], i);
        }
    }
}

//works
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: addToMap(char** map, int key, char* string)
 *  Input: map - initialized hashmap
 *         key - index at which string will be placed in map
 *         string - the string to be stored in the map
 *  Returns: 1 - if storage was successful, or map contains the string
 *           0 - if storage was unsuccessful, something already stored not equal to string
 *  Usage: adding members to hashmap
 * -----------------------------------------------
 * */
int addToMap(char ** map, int key, char* string){
    if (map[key] == NULL){
        map[key] = (char*)malloc(sizeof(char) * strlen(string));
        strcpy(map[key],string);
        return 1;
    }
    else if(strcmp(map[key], string) == 0){
        return 1;
    }
    else{
#if __DEBUG
        printf("collision with %s and %s (inside map)\n", string, map[key]);
#endif
        return 0;
    }
}

//works
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: retreiveFromMap(char** map, char* encoded)
 *  Input: map - initialized hashmap
 *         encoded - string to be looked up inside map
 *  Returns: NULL if not found, 
 *           the string sought after if it exists in map
 *  Usage: pulling members out of a map
 * -----------------------------------------------
 * */
char* retreiveFromMap(char** map, char* encoded){
    return(map[generateKey(encoded)]);
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: generateKey(char* string)
 *  Input: string - the string associated (or to be associated) with a key
 *  Returns: the key associated with the string
 *  Usage: anytime a key is neccessary, retreival or addition
 * -----------------------------------------------
 * */
int generateKey(char* string){
    int value;
    value = strtol(string, NULL, 32); //string to long base 32
    return (value % MAX_MAP_SIZE); //make sure this number is within the bounds
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: encodeMystring(char* string, int start, int end)
 *  Input: string - the string to be encoded
 *         start - placement of the beginning of the string
 *         end - placement of the end of the string
 *  Returns: encoded string by Brian Bowden
 *  Usage: encoding of strings
 *  NOTES: this encoding is of my own design, recursively concatenates 
 *         the start and end indices in the string after being XOR'd, 
 *         making sure they will always be of an even length. If the 
 *         encoded string gets too large, it will be split in two equal
 *         parts, and each equal index of those strings will be XOR'd 
 *         together to get an integer value, weight. Weight in hex is 
 *         now the new start of the encoded string.  
 * -----------------------------------------------
 * */
char* encodeMyString (char* string, int start, int end){

    char* value;
    value = (char*)malloc(25 * sizeof(char));

    if (start == end){
        sprintf(value,"%02X",string[start]);
    }
    else if (start == end - 1){
        sprintf(value,"%02X",(string[start] ^ string[end]));
    }
    else{
        //recurse
        sprintf(value,"%02X",(string[start] ^ string[end]));
        strcat(value, encodeMyString(string, start + 1, end - 1));
        if (strlen(value) >= 10){
            //divide the string into two parts and operate on them
            char top[6];
            char block[6];
            strncpy(top, value, 5); 
            strncpy(block, value + 5, 5);
            top[5] = '\0';
            block[5] = '\0';
            int weight = 0;
            for(int i = 0; i < 5; i++){
                weight += (top[i] ^ block[i]); 
            }
            memset(value, 0, 25);
            sprintf(value, "%02X", weight);
        }
    }
    return value;
}
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: hashbrown(char** map, char* string)
 *  Input: map - initialized hash map
 *         string - string to be encoded
 *  Returns: encoded string
 *  Usage: function to be called by client to completely 
 *         encode a string. For ease of use.
 * -----------------------------------------------
 * */
char* hashbrown(char** map, char* string){
    char* encoding;
    int key;
    int success;
    encoding = encodeMyString(string, 0, strlen(string) - 1);
    key = generateKey(encoding);
    success = addToMap(map, key, string);
    //printf("original: %s\nencoded: %s\nkey: %d\nsuccess: %d\n\n", string, encoding, key, success);
    return encoding;
}
