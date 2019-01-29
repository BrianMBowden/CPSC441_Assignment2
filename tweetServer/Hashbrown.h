/* ============================================================================
 * Author: Brian Bowden
 * Date: October 17, 2018
 * ============================================================================
 * Hashbrown.c
 * ============================================================================
 * Hashing and pseudo-encryption library for strings
 * ============================================================================
 */
#ifndef HASHBROWN_H
#define HASHBROWN_H

#define MAX_MAP_SIZE 0xFFFF
#define MAX_WORD_SIZE 0xFF
#define __DEBUG 1

int generateKey(char* string);
char* encodeMyString(char* string, int start, int end);
void initializeMap(char ** map);
void destroyMap(char ** map);
void printMap(char ** map);
int addToMap(char ** map, int key, char* string);
char* retreiveFromMap(char** map, char* encoded);
char* retreiveEncoding(char ** map, char* string);
char* hashbrown(char** map, char* string);

#endif