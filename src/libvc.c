#include "libvc.h"
#include <ipd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// This definition is private to this file; code in all other files
// can only handle pointers to it: values of type `struct vote_count*`
// (a/k/a `vote_count_t`), not values of type `struct vote_count`. Only
// code in this file sees this definition, so only code in this file can
// work with `struct vote_count`s directly.
struct vote_count
{
    char*  candidate;
    size_t count;
};

// A vote_count_t will be a pointer to a `malloc`ed array of
// MAX_CANDIDATES `struct vote_count`s, with the following invariant:
//
//  - Some prefix of `candidate` fields are distinct, OWNED, `malloc`ed
//  strings, and the remaining `candidate` fields are NULL.
//
//  - When a `candidate` field is NULL the value of the corresponding
//  `count` does not mean anything, but when the `candidate` is non-NULL
//  then the `count` must be a properly initialized count.

/*
 * Q: Where are header comments for the following functions?
 * A: libvc.h
 */

// Creates and returns a new, empty `vote_count_t` on the heap. Every
// successful call to `vc_create` allocates a new object that must
// subsequently be deallocated exactly once using `vc_destroy`.
//
// OWNERSHIP: The caller takes ownership of the result.
//
// ERRORS: Returns NULL if memory cannot be allocated.
vote_count_t vc_create(void)
{
    vote_count_t new = malloc(sizeof(struct vote_count)*MAX_CANDIDATES);

    if (!new){
        return NULL;
    }

    for(size_t i = 0; i < MAX_CANDIDATES; i++) {
        new[i].candidate = NULL;
    }
    return new;
}

void vc_destroy(vote_count_t vc)
{
    for(size_t i = 0; i < MAX_CANDIDATES; i++){
        free(vc[i].candidate);
    }

    free(vc);
}

size_t* vc_update(vote_count_t vc, const char *name)
{
    for(size_t i = 0; i < MAX_CANDIDATES; i++) {
        if(vc[i].candidate == NULL){
            size_t name_size = strlen(name) + 1;
            char *c = malloc(sizeof(char) * name_size);
            if(!c){
                perror("vc_update");
                exit(1);
            }
            vc[i].candidate = strcpy(c, name);
            vc[i].count = 0;
            return &vc[i].count;
        }
        else if (strcmp((vc[i].candidate), name) == 0) {
            return &vc[i].count;
        }
    }

    return NULL;
}

size_t vc_lookup(vote_count_t vc, const char* name)
{
    for(size_t i = 0; i < MAX_CANDIDATES; i++) {
        if(vc[i].candidate == NULL){
            return 0;
        }
        if (strcmp((vc[i].candidate), name) == 0) {
            return vc[i].count;
        }
    }
    return 0;
}

size_t vc_total(vote_count_t vc)
{
    size_t sum = 0;
    for(size_t i = 0; i < MAX_CANDIDATES; i++){
        if(vc[i].candidate == NULL) {
            return sum;
        }
        sum += vc[i].count;
    }

    return sum;
}

const char* vc_max(vote_count_t vc)
{
    size_t index_max = 0;
    for(size_t i = 0; i < MAX_CANDIDATES; i++){
        if(vc[i].candidate == NULL){
            return vc[index_max].candidate;
        }
        else if(vc[i].count > vc[index_max].count){
            index_max = i;
        }
    }

    return vc[index_max].candidate;
}

const char* vc_min(vote_count_t vc)
{
    if(vc[0].candidate == NULL){
        return NULL;
    }
    size_t index_min = 0;
    for(size_t i = 0; i < MAX_CANDIDATES; i++){
        if(vc[i].candidate == NULL){
            if(vc[index_min].count != 0) {
                return vc[index_min].candidate;
            }
            return NULL;
        }
        else if(vc[i].count <= vc[index_min].count && vc[i].count != 0){
            index_min = i;
        }
    }
    if(vc[index_min].count != 0) {
        return vc[index_min].candidate;
    }
    return NULL;
}


// candidate names are left-aligned in a 20-character column, followed
// by a single space, and then the counts right-aligned in a 9-character
// column.
void vc_print(vote_count_t vc)
{
    for(size_t i = 0; i < MAX_CANDIDATES; i++) {
        if(vc[i].candidate == NULL){
            break;
        }
        printf("%-20s %9lu\n", vc[i].candidate, vc[i].count);
    }
}
