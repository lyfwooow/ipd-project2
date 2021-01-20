#include "ballot_box.h"
#include "helpers.h"

#include <ipd.h>

#include <stdlib.h>
#include <string.h>

// A `ballot_box_t` (defined in `ballot_box.h`) is a linked list made
// of `struct bb_node`s. Each node owns the next node, so the whole list
// is owned by the pointer to the first node.
struct bb_node
{
    ballot_t ballot;
    struct bb_node* next;
};

// The empty ballot box is the null pointer.
const ballot_box_t empty_ballot_box = NULL;

void bb_destroy(ballot_box_t bb)
{
   ballot_box_t curr = bb;
   while(curr != NULL){
       ballot_destroy(curr->ballot);
       ballot_box_t next = curr->next;
       free(curr);
       curr = next;
   }
}

void bb_insert(ballot_box_t* bbp, ballot_t ballot)
{
    ballot_box_t head = mallocb(sizeof *head, "bb_insert");
    head->ballot = ballot;
    head->next   = *bbp;
    *bbp = head;
}

ballot_box_t read_ballot_box(FILE* inf)
{
    ballot_t line;
    ballot_box_t bb = empty_ballot_box;
    while((line = read_ballot(inf)) != NULL){
        bb_insert(&bb, line);
    }
    return bb;
}

vote_count_t bb_count(ballot_box_t bb)
{
    vote_count_t result = vc_create();
    if(bb->next == NULL)
        count_ballot(result, bb->ballot);
    while(bb){
        count_ballot(result, bb->ballot);
        bb = bb->next;
    }
    return result;
}

void bb_eliminate(ballot_box_t bb, const char* candidate)
{
    if(bb->next == NULL)
        ballot_eliminate(bb->ballot, candidate);
    while(bb){
        ballot_eliminate(bb->ballot, candidate);
        bb = bb->next;
    }
}

char* get_irv_winner(ballot_box_t bb)
{
    vote_count_t result= bb_count(bb);
    if (result == NULL){
        return NULL;
        vc_destroy(result);
    }

    while(vc_lookup(result,vc_max(result)) <= (vc_total(result)/2.0))
    {
        bb_eliminate(bb, vc_min(result));
        vc_destroy(result);
        result = bb_count(bb);
    }
    const char* str= vc_max(result);
    char* winner = strdupb(str,"get_irv_winner");
    vc_destroy(result);
    return winner;
}
