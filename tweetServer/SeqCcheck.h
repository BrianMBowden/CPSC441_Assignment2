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
 * ============================================================================
 */
#ifndef SEQCCHECK_H
#define SEQCCHECK_H

#define MAP 0xFFFF
#define WORD 0xFF

//general helping functions
void SinitializeMap(char ** map);
void SdestroyMap(char ** map);
void SprintMap(char ** map);
int SaddToMap(char** map, char* string, int key);
char* Sretreive(char ** map, char* encoded);

//sequential indexing
char* encodeStringSeq(char** map, char* string);

//wordsum indexing
int encodeStringSum(const char* string, int start, int end);
char* getEncodingSum(char ** map, char* string);
#endif