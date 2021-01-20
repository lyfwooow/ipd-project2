#include "ballot.h"
#include "helpers.h"

#include <ipd.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A `ballot_t` (defined in `ballot.h`) is be a pointer to a
// heap-allocated `struct ballot`, with the following invariant:
//
//  - `length <= MAX_CANDIDATES`
//
//  - the first `length` elements of `entries` are initialized
//
//  - the `name` field of each of the first `length` entries is non-NULL
//    and points to an OWNED, heap-allocated string containing only
//    letters, all of which are uppercase (a la isupper(3)).
//
// The remaining elements of `entries` (`MAX_CANDIDATES - length`)
// should be considered uninitialized.

struct entry
{
    char* name;
    bool active;
};

struct ballot
{
    size_t length;
    struct entry entries[MAX_CANDIDATES];
};


ballot_t ballot_create(void)
{
    size_t i =0;

    ballot_t result = malloc(sizeof(struct ballot));
    if(!result){
        exit(2);
    }
    result->length = 0;

    return result;
}

void ballot_destroy(ballot_t ballot)
{
    if(ballot == NULL)
    {
        free(ballot);
    }else {
        size_t i = 0;
        while(i < ballot->length){
            free(ballot->entries[i].name);
            ++i;
        }
        free(ballot);
    }
}

void ballot_insert(ballot_t ballot, char* name)
{
    if ( ballot->length < MAX_CANDIDATES){
        clean_name(name);
        ballot->entries[ballot->length].active = true;
        ballot->entries[ballot->length].name = name;
        ballot->length += 1;
    }
    else exit(3);
}

const char* ballot_leader(ballot_t ballot)
{
    for(size_t i=0; i<ballot->length; ++i){
        if(ballot->entries[i].active){
            return ballot->entries[i].name;
        }
    }
    return NULL;
}

void ballot_eliminate(ballot_t ballot, const char* name)
{
    size_t i =0;
    size_t len = ballot->length;
    while(i<len && ballot->entries[i].name != NULL && name != NULL){
        if(strcmp(name, ballot->entries[i].name) == 0){
            ballot->entries[i].active = false;
        }
        ++i;
    }
}

void count_ballot(vote_count_t vc, ballot_t ballot)
{
    const char* first_cand = ballot_leader(ballot);
    if(first_cand != NULL){
        size_t *count_point = vc_update(vc, first_cand);
        if(count_point == NULL){
            exit(4);
        }
        ++*count_point;
    }
}

ballot_t read_ballot(FILE* inf)
{
    ballot_t ballot = ballot_create();
    char* line;
    while((line = fread_line(inf)) != NULL){
        if(*line == '%'){
            free(line);
        } else {
            clean_name(line);
            ballot_insert(ballot, line);
            free(line);
        }
    }
    return NULL;
}

void clean_name(char* name)
{
    size_t x = 0;
    size_t y = 0;
    while (name[x] != 0){
        if (isalpha(name[x]) != 0)
        {
            name[y] = toupper(name[x]);
            x++;
            y++;
        } else {
            x++;
        }
    }
    name[y] = 0;
}

void print_ballot(FILE* outf, ballot_t ballot)
{
    for (size_t i = 0; i < ballot->length; ++i) {
        bool active = ballot->entries[i].active;
        fprintf(outf, "%c%s%s\n",
                active? ' ' : '[',
                ballot->entries[i].name,
                active? "" : "]");
    }
}
