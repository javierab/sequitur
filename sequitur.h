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
}symbols;

typedef struct{
	symbols *guard;			//nodo guard de la regla.
	int count;				//usos de la regla.
	int number;             //id de la regla.
}rules;

void join(symbols *left, symbols *right);
void substitute(symbols *s, rules *r);
void match(symbols *ss, symbols *m); 

extern symbols **find_digram(symbols *s);
void delete_digram(symbols *s);

//pequeñas funciones auxiliares y otros.
rules *S;
ulong num_rules = 0;
symbols *table[PRIME];

symbols *next(symbols *S){
    return S->n;
}
symbols *prev(symbols *S){
    return S->p;
}
//preguntas para las reglas
symbols *first(rules *R) {
    return R->guard->n;
}
symbols *last(rules *R){
    return R->guard->p;
}
//convertir el ulong en la regla. magia.
rules *rule(symbols *S){
    return (rules *)(S->s);
}
void append(symbols *r, symbols *s){
    join(s, r->n);
    join(r, s);
}
inline ulong value(symbols *S){
    return S->s/2;   
}
inline ulong raw(symbols *S){
    return S->s;   
}

int nt(symbols *S){
    return ((S->s % 2) == 0) && (S->s != 0);
}
int is_guard(symbols *S){
    if (nt(S) && prev(first(rule(S))) == S)
        return 1;
    else return 0;
}

void reuse(rules *R){
    R->count++;
}
void deuse(rules *R){
    R->count--;
}

int freq(rules *R){
    return R->count;
}
symbols *newsymbol(ulong sym, bool isrule){
    symbols *S = (symbols *)malloc(sizeof(symbols));
    if(!isrule){
        S->s = sym*2+1; //estrategia sugerida para que sea impar, y estén en otro espacio que los punteros a reglas.  
        fprintf(stderr,"nuevo simbolo con s = %c\n", (char)value(S)); 
    }
    else{
        S->s = sym;
        fprintf(stderr,"nueva regla con s = %lu\n", value(S)); 
    }
    S->p = S->n = 0;

    return S;
}

rules *newrule(){
    num_rules++;
    rules *r = (rules *)malloc(sizeof(rules));
    r->guard = newsymbol((ulong)r, true);
    join(r->guard, r->guard);
    r->count = 0;
    r->number = 0;
    return r;
}

void delrule(rules *r){
    free(r->guard);
    free(r);
    num_rules--;
}


bool isrule(symbols *s){
    long S = (long) s;
    if( ((S>>1)<<1) == S) //es par?
        return true;
    return false;
}

void delsymb(symbols *S){

    join(S->p, S->n);

    if(!(is_guard(S))){
        delete_digram(S);
        if (nt(S)) deuse(rule(S)); 
    }
}




