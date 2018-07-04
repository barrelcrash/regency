/*
 * regency
 *
 * generate strings using regex-like
 * patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_POSSIBLE_RANGE 10000
#define MAX_RESULT 100000
#define MAXBUF 100
#define MAX_DIGIT 9

enum ruletypes {
  INVALID = 0,
  RANGE,
  DICT
};

typedef struct Rule Rule;
struct Rule {
  int type;
  char *range;
  Rule *next;
  Rule *tail;
};

/*
 * external variables
 */
extern char *dict[];
extern const int dict_length;

/*
 * function declarations
 */
Rule *parseRuleString(Rule *, char*);
void printRules(Rule *);
Rule *add(Rule *, Rule *);
Rule *createRangeRule(char[]);
Rule *createDictRule();
Rule *newRule(int, char *);
char *strdupl(char *);
void *emalloc(unsigned);

/*
 * function definitions
 */
int main(int argc, char **argv) {

  int opt;

  // option flags
  while (--argc > 0 && *(*++argv) == '-') {
    while ((opt = *++(*argv))) {
      switch (opt) {
        case 'x':
          printf("sample opt1 found\n");
          break;
      }
    }
  }

  if (argc != 1)
    printf("usage: regency [-x] [pattern]\n");

  srand(time(NULL));

  Rule *ruleList = NULL;

  ruleList = parseRuleString(ruleList, *argv);

  printRules(ruleList);
}

void printRules(Rule *listp) {
  for ( ; listp != NULL; listp = listp->next) {
    printf("type: %d, range: %s\n", listp->type, listp->range);
  }
}

/*
 * parseRuleString: iterate through string and create rules base
 * on the character sequences encountered
 */
Rule *parseRuleString(Rule *listp, char *s) {

  do {
    // escaped chars
    if (*s == '\\') {
      if (*++s == 'd') {
        listp = add(listp, createRangeRule("0-9"));
      } else if (*s == 'w') {
        listp = add(listp, createRangeRule("a-zA-Z0-9_"));
      } else if (*s == 'y') {
        listp = add(listp, createDictRule());
      } else {
        char buf[2] = {*s, '\0'};
        listp = add(listp, createRangeRule(buf));
      }

    // bracketed ranges
    } else if (*s == '[') {
      char rangebuf[MAXBUF];
      int i;

      i = 0;

      while(*++s != ']') {
        rangebuf[i++] = *s;
      }

      rangebuf[++i] = '\0';

      if (strlen(rangebuf) > 0) {
        listp = add(listp, createRangeRule(strdupl(rangebuf)));
      }

    // quantifiers
    } else if (*s == '{') {
      char lenbuf[MAXBUF];
      int i;

      i = 0;

      while(*++s != '}') {
        lenbuf[i++] = *s;
      }

      lenbuf[++i] = '\0';

      if (strlen(lenbuf) > 0) {
        // TODO: rebuild quantifiers
        // listp = parseQuantifier(listp, buf);
      }

    // all other characters
    } else {
      char buf[] = {*s, '\0'};
      listp = add(listp, createRangeRule(strdupl(buf)));
    }
  } while (*++s != '\0');

  return listp;
}

/* add: add a rule to the end of rule list */
Rule *add(Rule *listp, Rule *newp) {
  Rule *p;

  if (listp == NULL) {
    return newp;
  }
  for (p = listp; p->next != NULL; p = p->next)
    ;

  p->next = newp;
  listp->tail = newp;
  return listp;
}

/*
 * Rule Creation
 */

Rule *newRule(int type, char *range) {
  Rule *newp = (Rule *) emalloc(sizeof(Rule));
  newp->type = type;
  newp->range = range;
  newp->next = NULL;
  newp->tail = NULL;
  return newp;
}

/* createRangeRule: creates a rule based on a bracketed range of characters */
Rule *createRangeRule(char *buf) {
  Rule *new = newRule(RANGE, buf);
  return new;
}

Rule *createDictRule() {
  return newRule(DICT, NULL);
}

/* emalloc: malloc w/ error handling */
void *emalloc(unsigned size) {
  void *p = malloc(size);
  if (p == NULL) {
    fprintf(stderr, "emalloc: out of memory!\n");
    exit(1);
  }
  return p;
}

/* strdupl: allocate memory for and memcpy a string */
char *strdupl(char *src) {
  size_t len = strlen(src) + 1;
  char *s = malloc(len);
  if (s == NULL)
    return NULL;
  return (char *) memcpy(s, src, len);
}
