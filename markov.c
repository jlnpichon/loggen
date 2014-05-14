#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "learning-text.c"

enum {
    NPREF   = 2,    /* number of prefix words */
    NHASH   = 4093, /* size of state hash table array */
    MAXGEN  = 10000 /* maximum words generated */
};

enum {
    MULTIPLIER = 31 /* hash function multiplier */
};

struct state { /* prefix + suffix list */
    char    *pref[NPREF];	  /* prefix words */
    struct suffix  *suf;		/* list of suffixes */
    struct state   *next;		  /* next in hash table */
};

struct suffix { /* list of suffixes */
    char    *word;		  /* suffix */
    struct suffix  *next;		  /* next in list of suffixes */
};

struct state	*statetab[NHASH];   /* hash table of states */

char NONWORD[] = "\n"; /* cannot appear as real word */

/* hash: compute hash value for array of NPREF strings */
unsigned int hash(char *s[NPREF])
{
    unsigned int h;
    unsigned char *p;
    int i;

    h = 0;
    for (i = 0; i < NPREF; i++) {
        for (p = (unsigned char *) s[i]; *p != '\0'; p++) {
            h = MULTIPLIER * h + *p;
        }
    }
    return h % NHASH;
}

/* lookup: search for prefix; create if required.
 *  returns pointer if present or created; NULL if not.
 *  creation doesn't strdup so strings mustn't change later
 */
struct state* lookup(char *prefix[NPREF], int create)
{
    int i, h;
    struct state *sp;

    h = hash(prefix);
    for (sp = statetab[h]; sp != NULL; sp = sp->next) {
        for (i = 0; i < NPREF; i++) {
            if (strcmp(prefix[i], sp->pref[i]) != 0)
                break;
        }
        if ( i == NPREF )
            return sp;
    }
    if (create) {
        sp = (struct state *) malloc(sizeof(struct state));
        if (sp == NULL)
            exit(2);
        for (i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}

/* addsuffix: add to state, suffix must not change later */
void addsuffix(struct state *sp, char *suffix)
{
    struct suffix *suf;

    suf = (struct suffix *) malloc(sizeof(struct suffix));
    if (suf == NULL)
        exit(2);
    suf->word = suffix;
    suf->next = sp->suf;
    sp->suf = suf;
}

/* add: add word to suffix list, update prefix */
void add(char *prefix[NPREF], char *suffix)
{
    struct state *sp;

    sp = lookup(prefix, 1); /* create if not found */
    addsuffix(sp, suffix);
    /* move the words down the prefix */
    memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
    prefix[NPREF - 1] = suffix;
}

/* build: read input, build prefix table */
void build(char *prefix[NPREF], char *text)
{
    char buf[100], fmt[10];
    int nread;

    /* create a format string; %s could overflow buf */
    sprintf(fmt, "%%%lds", sizeof(buf)-1);
    while ((nread = sscanf(text, fmt, buf)) != -1) {
        add(prefix, strdup(buf));
        text += strlen(buf);
    }
}

void generate(char *buf, int buflen, int nwords)
{
    struct state *sp;
    struct suffix *suf;
    char *prefix[NPREF], *w;
    int i, nmatch;

    for (i = 0; i < NPREF; i++) /* reset initial prefix */
        prefix[i] = NONWORD;

    for (i = 0; i< nwords; i++) {
        sp = lookup(prefix, 0);
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next) {
            if (rand() % ++nmatch == 0) /* prob = 1/nmatch */
                w = suf->word;
        }
        if (strcmp(w, NONWORD) == 0)
            break;
#if 0
        printf("%s\n", w);
#endif
        strncat(buf, w, buflen);
        buflen -= strlen(buf);
        strncat(buf, " ", buflen);
        buflen -= strlen(buf);
        memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
        prefix[NPREF - 1] = w;
    }
}

void markov_init(void)
{
    int i, nwords = MAXGEN;
    char *prefix[NPREF];

    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;

    build(prefix, learning_text);
    add(prefix, NONWORD);
}

void generate_sentence(char *buf, int buflen, int nwords)
{
    generate(buf, buflen, nwords);
}
