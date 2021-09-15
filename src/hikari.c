#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mc48.h"
#include "xoshiro256starstar.h"

#define TRUE 1
#define FALSE 0

/**
 *  Vincent AUBRIOT
 *  A (not so intelligent) collision checker and security breaker for ht48.  
 *  Provided "As it is" with no guarantee.
 */

// Custom hash table implementation

// Structures
struct element;

typedef struct element {
    uint8_t hash[6];
    uint8_t message[16];
    struct element* next;
} element_t;

typedef struct hashmap {
    size_t size;
    struct element **table;
} hashmap_t;

// New list instanciation
hashmap_t* listHMInstanciate(){
    hashmap_t *hm = NULL;

    // Instanciate hashmap
    hm = calloc(1, sizeof(hashmap_t));
    // I have no idea why this size but I need to fit as many unique hashes as possible in a fair time
    hm->size = 281474976;

    size_t s = hm->size * sizeof(element_t*);
    void * a = malloc(s);

    if (a == NULL) {
        printf("HASH MAP CRASH - IF REACHED THEN SOMETHING WENT BADLY WRONG\n");
        exit(2);
    }

    hm->table = a;

    for (size_t i = 0; i < hm->size; i++) {
        hm->table[i] = NULL;
    }

    return hm;
}

// Generate the hash table identifier
int listHMHash(hashmap_t *lhm, uint8_t h[6]) {
    size_t i;

    i = h[0] ^ (((uint64_t)h[1]) << 8) ^ (((uint64_t)h[2]) << 16) ^ (((uint64_t)h[3]) << 24) ^ (((uint64_t)h[4]) << 32) ^ (((uint64_t)h[5]) << 40);
    i = i % lhm->size;
    return i;
}

// Fill the last node with the infos and append a new empty element
void listHMInsert(hashmap_t* lhm, uint8_t h[6], uint8_t m[16]) {

    element_t *e = calloc(1, sizeof(element_t));
    element_t *curr = NULL;
    element_t *last = NULL;
   
    for (int i = 0; i < 6; i++) {
        e->hash[i] = h[i];
    }

    for (int i = 0; i < 16; i++) {
        e->message[i] = m[i];
    }

    e->next = NULL;

    size_t code = listHMHash(lhm, h);
    
    curr = lhm->table[code];

    if (curr == NULL) {
        // No insertion yet
        lhm->table[code] = e;
    } else {
        while(curr != NULL) {
            last = curr;
            curr = curr->next;
        }

        last->next = e;
    }

}

// Check if hashes are equal, used in retrieve
int areHashesEqual(uint8_t h1[6], uint8_t h2[6]) {
    int eq = TRUE;

    for (int i = 0; i < 6; i++){
        if (h1[i] != h2[i]) {
            eq = FALSE;
        }
    }


    return eq;
}

// Check if hash is already in there
int listHMPresent(hashmap_t* lhm, uint8_t h[6]) {
    size_t code = listHMHash(lhm, h);
    element_t* curr = lhm->table[code];
    while(curr != NULL) {
        // While next element != NULL, means that we can use the element
        if (areHashesEqual(curr->hash, h)) {
            return TRUE;
        }
        curr = curr->next;
    }
    return FALSE;
} 

// Get a message having a particular hash
void listHMRetrieve(hashmap_t* lhm, uint8_t h[6], uint8_t mout[16]) {
    int found = FALSE;
    size_t code = listHMHash(lhm, h);
    element_t* curr = lhm->table[code];
    while(curr != NULL && found == FALSE) {
        // While next element != NULL, means that we can use the element
        if (areHashesEqual(curr->hash, h)) {
            for (int j = 0; j < 16; j++){
                mout[j] = curr->message[j];
            }
            found = TRUE;
        }
        curr = curr->next;
    }
} 

// Free the list
void listHMFree(hashmap_t* lhm) {
    for (size_t i = 0; i < lhm->size; i++) {
        element_t* curr = lhm->table[i];
        while(curr != NULL) {
            element_t* tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(lhm->table);
    free(lhm);
}

// End of Hash map

/**
 *  A function checking for collisions between two messages
 * 
 *  Input : h - The hash resulting from the check
 *  Input : m1 - The first message to check
 *  Input : m2 - The second message to check
 * 
 *  Output : use h, m1 and m2 values (replaced with the outputs)
 */
void find_col(uint8_t h[6], uint8_t m1[16], uint8_t m2[16]) {
    // New list
    hashmap_t* lhm;
    int cmpt;
    lhm = listHMInstanciate();
    cmpt = 0;

    while (TRUE) {
        // Draw our new random message
        uint8_t* randmsg = calloc(16, sizeof(uint8_t));
        for (int j = 0; j < 16; j++){
            randmsg[j] = (uint8_t) xoshiro256starstar_random();
        }

        // Get hash of message
        uint8_t* out = (uint8_t *) malloc(sizeof(uint8_t) * 6);
        for (int j = 0; j < 6; j++){
            out[j] = h[j];
        }
        tcz48_dm(randmsg, (uint8_t *) out);

        if (listHMPresent(lhm, out)) {
            // Hash is in list !
            uint8_t ret[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            listHMRetrieve(lhm, out, ret);

            // For now print messages
            for (int j = 0; j < 16; j++){
                printf("%02X", randmsg[j] );
                m1[j] = randmsg[j];
            }
            printf(" ");
            for (int j = 0; j < 16; j++){
                printf("%02X", ret[j] );
                m2[j] = ret[j];
            }
            printf(" ");
            for (int j = 0; j < 6; j++){
                h[j] = out[j];
            }
            // Print the log2 of hashs computed
            printf("%f\n", log2(cmpt));
            listHMFree(lhm);
            return;
        } else {
            // Insert and move to next try
            listHMInsert(lhm, out, randmsg);
            cmpt++;
            free(out);
            free(randmsg);
        }

    }

}

/**
 *  The main entry point for the attack
 * 
 *  Input : d - How many d's on your attack today ? 
 * 
 *  No output
 */
void attack(int d) {
    
    if (d <= 0){
        // Positive integer only
        printf("d should be a positive integer only.\n");
        return;
    }

    // Setup data holders
    uint8_t* h = (uint8_t *) malloc(sizeof(uint8_t) * 6);

    uint8_t* m1 = (uint8_t *) malloc(sizeof(uint8_t) * 16);
    uint8_t* m2 = (uint8_t *) malloc(sizeof(uint8_t) * 16);

    // Iteration #i
    for (int i = 1; i <= d; i++) {
        // Refeed the hash at each iteration
        h[0] = IVB0;
        h[1] = IVB1;
        h[2] = IVB2;
        h[3] = IVB3;
        h[4] = IVB4;
        h[5] = IVB5;

        // Take an array of accurate size to store the messages caught
        int msgSize = 2 * i;

        uint8_t* msg[msgSize];
        for (int i = 0; i < msgSize; i++)
            msg[i] = (uint8_t *) malloc(sizeof(uint8_t) * 16);
        
        // Current slot to store first message that collides
        int slot = 0;

        // For each iteration i, find i collisions
        printf("----------ITERATION %d----------\n",i);
        for (int j = 1; j <= i; j++) {
            // j represents the current collision that is being computed in the iteration

            // Generate messages - Use a new fct to check if a specific key is already there ?
            find_col(h, m1, m2);

            // Store first message
            for (int k = 0; k < 16; k++){
                msg[slot][k] = m1[k];
            }
            slot += 1;

            // Store second message
            for (int k = 0; k < 16; k++){
                msg[slot][k] = m2[k];
            }
            slot += 1;
        }

        // Build the array to host all message combinations (2^i messages of size 16 * i bits)
        int finalMsgSize = pow(2, i);
        int finalEntSize = i * 16;

        uint8_t* finalMsg[finalMsgSize];
        for (int i = 0; i < finalMsgSize; i++)
            finalMsg[i] = (uint8_t *) malloc(sizeof(uint8_t) * finalEntSize);

        /**
         * Process to enumerate the messages :
         * Go from finalMsgSize-1 to 0
         * iterate through each bit to decide which message to print (there are i bits.)
         * slider starts at 0. If current bit is 1, read at slider+1 otherwise slider
         * add 2 to slider to go to next number
         * won't be read if outside of scope (finalMsgSize) as we should be outside of loop
         **/

        int slider = 0; // The offset used to determine which message to select
        int data = 0; // The size from where to start writing data

        /**
         * Inspired from the provided output file on original code
         * where the combination of messages looked like a binary code representing 
         * a number between 0 and 2^i.
         * 
         * For each bit, tells us if this is the first message or the second one
         * that will be put on one part of the final message.
         * 
         * Quick example:
         * i = 3 so 2^3 = 8 combination messages can be built
         * Take binary codes for 0, 1, 2, 3, 4, 5, 6, 7 and build the associated combination messages.
         **/

        for (int j = finalMsgSize-1; j >= 0; j--) {
            // Bug fix: without this, builds bad messages
            // data is the offset where the selected message will be inserted
            // slider is the message block considered. Either choose the first or second one.
            if (j%1 == 1) {
                data = finalEntSize-16;
                slider = finalMsgSize-2;
            } else {
                data = 0;
                slider = 0;
            }
            int savej = j;
            // For each possible message, check-do-shift
            for(int b = i-1; b >= 0; b--){
                int res = j>>b & 1;
                // If 0 then take on slider, otherwise slider + 1
                for (int k = 0; k < 16; k++){
                    finalMsg[j][data+k] = msg[slider+res][k];
                }
                // Bug fix part 2
                if (savej%1 == 1) {
                    data -= 16;
                    slider -= 2;
                } else {
                    data += 16;
                    slider += 2;
                }
            }

            // Print final combination for this number
            for (int k = 0; k < finalEntSize; k++){
                printf("%02X",finalMsg[j][k]);
            }
            printf("\t");

            // Print hash
            uint8_t* hout = (uint8_t *) malloc(sizeof(uint8_t) * 6);
            ht48(finalMsg[j], finalEntSize, hout);

            for (int k = 0; k < 6; k++){
                printf("%02X",hout[k]);
            }

            printf("\n");
        }
    }
}

int main(int argc, char const *argv[])
{
    printf("Hikari collision checker for ht48\n\n");
    int d = 1;

    if (argc == 2) {
        d = atoi(argv[1]);
    }

    printf("Starting attack with d = %d\n", d);

    printf("只今計算します。しばらくお待ちください。\n");
    attack(d);

    return 0;
}
