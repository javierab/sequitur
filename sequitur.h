#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define PRIME 2265539
//double hashing
#define HASH(one, two) (((one) << 16 | (two)) % PRIME)
#define HASH2(one) (17 - ((one) % 17))

typedef unsigned long ulong;

typedef struct simb{
    struct simb *n, *p;         //simbolos next y prev
    ulong s;                //valor del simbolo
}Symbol;

typedef struct{
	Symbol *guard;			//nodo guard de la regla.
	int count;				//usos de la regla.
	int number;             //id de la regla.
}Rule;

void join(Symbol *left, Symbol *right);
void substitute(Symbol *s, Rule *r);
void match(Symbol *ss, Symbol *m); 

extern Symbol **find_digram(Symbol *s);
void del(Symbol *s);

//pequeñas funciones auxiliares y otros.
Rule *S;
ulong num_Rule = 0;
Symbol *table[PRIME];

Symbol *next(Symbol *S){
    return S->n;
}
Symbol *prev(Symbol *S){
    return S->p;
}
//preguntas para las reglas
Symbol *first(Rule *R) {
    return R->guard->n;
}
Symbol *last(Rule *R){
    return R->guard->p;
}
//convertir el ulong en la regla. magia.
Rule *rule(Symbol *S){
    return (Rule *)(S->s);
}
void append(Symbol *r, Symbol *s){
    join(s, r->n);
    join(r, s);
}
inline ulong value(Symbol *S){
    return S->s/2;   
}
inline ulong raw(Symbol *S){
    return S->s;   
}

int nt(Symbol *S){
    return ((S->s % 2) == 0) && (S->s != 0);
}
int isGuard(Symbol *S){
    if (nt(S) && prev(first(rule(S))) == S)
        return 1;
    else return 0;
}

void reuse(Rule *R){
    R->count++;
}
void unuse(Rule *R){
    R->count--;
}

int freq(Rule *R){
    return R->count;
}
Symbol *newsymbol(ulong sym, bool isrule){
    Symbol *S = (Symbol *)malloc(sizeof(Symbol));
    if(!isrule){
        S->s = sym*2+1; //estrategia sugerida para que sea impar, y estén en otro espacio que los punteros a reglas.  
        fprintf(stderr,"nuevo simbolo con s = %c\n", (char)value(S)); 
    }
    else{
        S->s = sym;
        reuse(rule(S));
        fprintf(stderr,"nueva regla con s = %lu\n", value(S)); 
    }
    S->p = S->n = 0;

    return S;
}

Rule *newrule(){
    num_Rule++;
    Rule *r = (Rule *)malloc(sizeof(Rule));
    r->guard = newsymbol((ulong)r, true);
    join(r->guard, r->guard);
    r->count = 0;
    r->number = 0;
    return r;
}

void delrule(Rule *r){
    free(r->guard);
    free(r);
    num_Rule--;
}


void delsymb(Symbol *S){
    join(S->p, S->n);
    if(!(isGuard(S))){
        del(S);
        if (nt(S)) unuse(rule(S)); 
    }
}




