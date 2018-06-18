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

#define MAX 5000
#define MAXBUF 100
#define MAX_DIGIT 9

enum ruletypes {
  INVALID = 0,
  LITERAL,
  DIGIT,
  RANGE,
  DICT
};

typedef struct Rule Rule;
struct Rule {
  int type;
  char *value;
  Rule *next;
};

/*
 * external variables
 */

extern char *dict[];
extern const int dict_length;

/*
 * function declarations
 */
/* input and output */
Rule *parseRuleString(Rule *, char**);
void printRules(Rule *);

/* rule creation */
Rule *add(Rule *, Rule *);
Rule *createLiteral(char*);
Rule *createDigitRule();
Rule *createRangeRule(char[]);
Rule *createDictRule();
void repeatLastRule(Rule *, char[]);
int randomIntInclusive(int, int);

/* memory allocation */
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

  ruleList = parseRuleString(ruleList, argv);

  printRules(ruleList);
}

void printRules(Rule *listp) {
  char *result = emalloc(sizeof(char));
  for ( ; listp != NULL; listp = listp->next) {
    result = realloc(result, sizeof(result) + sizeof(listp->value));
    strcat(result, listp->value);
  }
  printf("%s\n", result);
}

/*
 * parseRuleString: iterate through string and create rules base
 * on the character sequences encountered
 */
Rule *parseRuleString(Rule *listp, char **s) {

  do {
    // escaped chars
    if (**s == '\\') {
      if (*++(*s) == 'd') {
        listp = add(listp, createDigitRule());
      } else if (**s == 'w') {
        listp = add(listp, createRangeRule("a-zA-Z0-9_"));
      } else if (**s == 'y') {
        listp = add(listp, createDictRule());
      } else {
        char buf[] = {**s, '\0'};
        listp = add(listp, createLiteral(buf));
      }

    // bracketed ranges
    } else if (**s == '[') {
      char buf[MAXBUF];
      int i;

      i = 0;

      while(*++(*s) != ']') {
        buf[i++] = **s;
      }

      if (strlen(buf) > 0) {
        listp = add(listp, createRangeRule(buf));
      }

    // quantifiers
    } else if (**s == '{') {
      char buf[MAXBUF];
      int i;

      i = 0;

      while(*++(*s) != '}') {
        buf[i++] = **s;
      }

      if (strlen(buf) > 0) {
        repeatLastRule(listp, buf);
      }

    // all other characters
    } else {
      char buf[] = {**s, '\0'};
      listp = add(listp, createLiteral(buf));
    }
  } while (*++(*s) != '\0');

  return listp;
}

/*
 * RULE CREATION
 */

/* add: add a rule to the end of rule list */
Rule *add(Rule *listp, Rule *newp) {
  Rule *p;

  if (listp == NULL) {
    return newp;
  }
  for (p = listp; p->next != NULL; p = p->next)
    ;

  p->next = newp;
  return listp;
}

/* createLiteral: creates a rule of the character passed to it */
Rule *createLiteral(char *token) {
  return newRule(LITERAL, strdupl(token));
}

/* createDigitRule: creates a rule of a random number 0-9 */
Rule *createDigitRule() {
  char token[] = {randomIntInclusive(0, MAX_DIGIT) + '0', '\0'};
  return newRule(DIGIT, strdupl(token));
}

/* createRangeRule: creates a rule based on a bracketed range of characters */
Rule *createRangeRule(char buf[]) {
  int possible[MAX]; // possible values
  unsigned int i, j, start, end;

  j = 0;

  for (i = 0; i < strlen(buf) - 1; i++) {
    if (buf[i] == ']') {
      break; // done creating range
    } else if (buf[i] == '|') {
      continue; // since creation is "positive", we just ignore OR
    } else if (buf[i] == '-' && i != 0) {
      start = buf[i - 1];
      end = buf[++i];
    } else if (buf[i + 1] == '-') {
      continue; // ignore prior to `-` handle it in the above cond
    } else {
      possible[j++] = buf[i];
      continue; // literal charcter, add to possible and continue
    }

    possible[j++] = randomIntInclusive(start, end);
  }

  char token[] = {possible[randomIntInclusive(0, j - 1)], '\0'};

  return newRule(RANGE, strdupl(token));
}

/* createDictRule: create a rule from a random dictionary word */
Rule *createDictRule() {
  return newRule(DICT, dict[randomIntInclusive(0, dict_length - 1)]);
}


void repeatLastRule(Rule *listp, char buf[]) {
}

/* randomIntInclusive: return int between l to u, inclusive*/
int randomIntInclusive(int l, int u) {

  if (u < l) {
    fprintf(stderr, "randomIntInclusive: lower bound greater than upper bound");
    exit(1);
  }

  int offset = u - l;

  int div = RAND_MAX/(offset + 1);
  int val;

  do {
    rand();
    val = rand() / div;
  } while (val > u);

  return val + l;
}

/*
 * MEMORY ALLOCATION
 */

/* newRule: allocate a new rule and populate its fields */
Rule *newRule(int type, char *value) {
  Rule *newp = (Rule *) emalloc(sizeof(Rule));
  newp->type = type;
  newp->value = value;
  newp->next = NULL;
  return newp;
}


/* strdupl: allocate memory for and memcpy a string */
char *strdupl(char *src) {
  size_t len = strlen(src) + 1;
  char *s = malloc(len);
  if (s == NULL)
    return NULL;
  return (char *) memcpy(s, src, len);
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

