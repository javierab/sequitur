#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <limits.h>

typedef unsigned long ulong;

int globalid = 1000;

typedef struct{
	char *char1;
	char *char2;
}digram;

typedef struct simb{
	struct simb *next, *prev;	//simbolos next y prev dentro de la regla.
	void * resp;			//si es no terminal, puntero a la regla donde va.
	char c;				//simbolo
}symbol;


typedef struct{
	symbol *guard;			//nodo guard de la regla.
	int count;				//usos de la regla.
	ulong id;					//nombre de la regla (no es necesario)
}rule;

ulong getId(){
    return ++globalid;
}

void printRule(rule *r){
    symbol *this = r->guard->next;
    fprintf(stdout, "%lu:", r->id);
    while(this != r->guard){
        fprintf(stdout, "%c", this->c);
        this = this->next;
    }
    fprintf(stdout, "\n");
}


rule *newRule(char n){
    fprintf(stderr, "char n: %c\n", n);
    rule *r = (rule *)malloc(sizeof(rule*));
    r->id = getId();
    r->count = 1;
    symbol *guard = (symbol *)malloc(sizeof(symbol*));
    symbol *s = (symbol *)malloc(sizeof(symbol*));
    char k = 'X';
    guard->c = k;
    s->c = n;
    //los enlazo hacia adelante
    guard->next = s;
    s->next = guard;
    //los enlazo hacia atrás
    guard->prev = s;
    s->prev = guard;
    //seteo como guard
    r->guard = guard;
    printf("char: %c\n", r->guard->prev->c);
    return r;
    
}

void addSymbol(rule *r, char n){
    symbol *aux = r->guard->prev;
    char auxc = aux->c;
    symbol *new = (symbol *)malloc(sizeof(symbol));
    fprintf(stderr,"newsymbol: %c\n", n);
    new->c = n;
    aux->c = auxc;
    printf("aux-c: %c, new-c: %c\n", aux->c, new->c);
    //uno el nuevo al guard
    r->guard->prev = new;
    new->next = r->guard;
    //uno el nuevo al aux
    aux->next = new;
    new->prev = aux;
}



/*Funciones de las reglas: agregar un simbolo, crear una regla, usar una regla existente, crear una regla, borrar una regla, buscar una regla */


rule *createRule(digram d);
void reuse(rule *r, char *c);
void deleteRule(rule *r);
rule *getRule(ulong id);
digram *join(symbol *left, symbol *right);
char *unjoin(digram *d);

