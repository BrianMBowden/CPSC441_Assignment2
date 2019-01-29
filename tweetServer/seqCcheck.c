/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * ============================================================================
 * seqCcheck.c
 * ============================================================================
 * Hashing and pseudo-encryption library for strings
 * 
 * Strings are encoded by either:
 *      Sequential Indexing or,
 *      WordSum 
 *  
 *  Order:
 *      char* tweetWord
 *      char* map[SIZE]
 *      char* encodedWord
 *      
 *      SinitializeMap
 * 
 *      Seq -- 
 *          encodedWord = encodeStringSeq(map, tweetWord)
 *          tweetWord = Sretreive(map, encodedWord)
 *      WordSum --
 *           encodedWord = getEncodingSum(map, tweetWord)
 *           tweetWord = Sretreive(map, encodedWord)
 * 
 *      SDestroyMap
 *          
 * ============================================================================
 * Bugs:
 * 
 * Testing:
 * 
 * ============================================================================
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "SeqCcheck.h"

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: SinitializeMap(char** map)
 *  Input: map - uninitialized hashmap
 *  
 *  Usage: point everything in the map at NULL so there is no garbage
 * -----------------------------------------------
 * */
void SinitializeMap(char ** map){
    for(int i = 0; i < MAP; i++){
        map[i] = NULL;
    }
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: SdestroyMap(char** map)
 *  Input: map - pre-initialized hashmap of strings (char*)
 *  
 *  Usage: deallocate space in the map when we're done with it
 * -----------------------------------------------
 * */
void SdestroyMap(char ** map){
    for (int i = 0; i < MAP; i++){
        free(map[i]);
    }
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: SprintMap(char** map)
 *  Input: map - hashmap containing strings and NULL's
 *  
 *  Usage: mainly for debugging, prints out anything (actually) in the map
 * -----------------------------------------------
 * */
void SprintMap(char ** map){
    //loop through the whole map
    for (int i = 0; i < MAP; i++){
        //print the string if is not NULL
        if (map[i] != NULL){
            printf("%s ------ %d\n", map[i], i);
        }
    }
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: Sretreive(char** map, char* encoded)
 *  Input: map - pre-initialized hashmap of strings (char*)
 *         encoded - string under some encoding (from this library)
 *  Returns: the decoded string (from the hashmap) OR NULL
 *  Usage: pulling a string from the hashmap that has been encoded and 
 *         hashed using the functions in this library
 * NOTE: If nothing in the hashmap exists at the given location, NULL will be returned
 *       user should probably error check this
 * -----------------------------------------------
 * */
char* Sretreive(char ** map, char* encoded){
    int key;
    //check for "0x" prefix
    if (strstr(encoded, "0x") != NULL){
        key = strtol(encoded, NULL, 0);
    }
    //string-to-long, (string pointer, NULL, 0 if "0x" 16 if not)
    //strtol handles both types of encodings, just need to adjust the base 
    else{
        return(NULL);
    }
    return(map[key]);
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: SaddToMap(char** map, char* string, int key)
 *  Input: map - pre-initialized hashmap of strings (char*)
 *         string - (char*) string to add into hashmap
 *         key - index where we want to put the string in the map
 *  Returns: 1 if no collisions happened
 *           0 if there is a collision
 *  Usage: try to put the string into the map at the given index
 *  NOTE: THIS FUNCTION DOES NOT HANDLE COLLISIONS, ONLY NOTIFYS USER OF THEM
 * -----------------------------------------------
 * */
int SaddToMap(char** map, char* string, int key){
    //check if the spot is empty
    if (map[key] == NULL){
        // allocate space for the string in the map
        map[key] = (char*)malloc(strlen(string) * sizeof(char));
        //copy the string into the available spot
        strcpy(map[key], string);
        // tell the user we did good
        return 1;
    }
    else if (strcmp(map[key], string) == 0){
        // the specific string already exists
        return 1;
    }
    //spot in map is taken
    else{
        //notify user
        return 0;
    }
}


//sequential indexing
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: encodeStringSeq(char** map, char* string)
 *  Input: map - pre-initialized hashmap of strings (char*)
 *         string - the string to be encoded, indexed and hashed
 *  Returns: "encoded" string
 *  Usage: each string is placed in the next available spot in the hashmap,
 *         this also becomes its key, and its encoding (in hexadecimal)
 * -----------------------------------------------
 * */
char* encodeStringSeq(char** map, char* string){
    int key = 0;
    char* encoding;
    //loop through map to find next available opening
    for(; key < MAP; key++){
        //check if we successfully added to the map
        if (SaddToMap(map, string, key)){ 
            encoding = (char*)malloc(7 * sizeof(char));
            //"encode" if successful
            sprintf(encoding, "0x%X", key);
            break;
        }
    }
    return(encoding);
}



//use this to get the hash key of unencoded string
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: encodeStringSum(char* string, int start, int end)
 *  Input: string - string of characters to be evaluted and summed
 *         start - index for the where to start counting
 *         end - index for where to stop counting
 *  Returns: value of summed ascii characters in string
 *  Usage: add ascii charcters
 * -----------------------------------------------
 * */
int encodeStringSum(char* string, int start, int end){
    int value;
    if (start == end){
        //base case
        return(string[start]);
    }
    else{
        //recurse
        value = string[start] + encodeStringSum(string, start + 1, end);
    }
    return value;
}

//use this to get the encoded string of a string
/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: GetEncodingSum (char** map, char* string) 
 *  Input: map - pre-initialized hashmap of strings (char*)
 *         string - the string to be encoded, indexed and hashed
 *  Returns: "encoded" string
 *  Usage: Hashing function for word sum indexing 
 * -----------------------------------------------
 * */
char* getEncodingSum(char** map, char* string){
    char* encoding;
    int key;
    encoding = (char*)malloc(10 * sizeof(char));
    //create key through word sum
    key = encodeStringSum(string, 0, strlen(string) - 1);
    //format encoded string into hex characters
    sprintf(encoding, "0x%04X", key);
    //put the string in the map 
    SaddToMap(map, string, key); 
    return(encoding);
}


