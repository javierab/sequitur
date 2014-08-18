#include "sequitur.h"

//juntar 2 simbolos, borrando los digramas viejos de la tabla de hash.

void join(Symbol *left, Symbol *right) {
  fprintf(stderr,"join\n");
  if (left->n) {
    del(left);
    //se borra para que no haya problema con pares que se superponen.
    if (right->p && right->n &&
        value(right) == value(right->p) &&
        value(right) == value(right->n)){
            *find_digram(right) = right;
    }
    
    if (left->p && left->n &&
        value(left) == value(left->n) &&
        value(left) == value(left->p)){
            *find_digram(left->p) = left->p;
    }
  }
  left->n = right; 
  right->p = left;
}


//ultima referencia a una regla: hay que borrarla!
void expand(Symbol *s){
  fprintf(stderr,"expando\n");
  Symbol *left = prev(s);
  Symbol *right = next(s);
  Symbol *f = first(rule(s));
  Symbol *l = last(rule(s));

  delrule(rule(s));
  Symbol **m = find_digram(s);
  if (*m == s) *m = (Symbol *) 1;
  s->s = 0; 
  //delsymb(s);
  
  join(left, f);
  join(l, right);

  //adios :c
  *find_digram(l) = l;
}

//sustituir un digrama por un no terminal.
void substitute(Symbol *s, Rule *r){
    fprintf(stderr,"substituyo\n");
    Symbol *q = s->p;
    delsymb(next(q));
    delsymb(next(q));
    append(q, newsymbol((ulong) r, true));
    if(!check(q))
        check(q->n);
}

//digramas hacen match, reusar o crear regla.
void match(Symbol *ss, Symbol *m){
  Rule *r;

  // reusar

  if (isGuard(prev(m)) && isGuard(next(next(m)))){
    fprintf(stderr, "reuso\n");
    r = rule(prev(m));
    substitute(ss, r); 
  }
  else {
    fprintf(stderr, "nueva regla\n");
    // nueva regla
    r = newrule();  
    if (nt(ss)){
      append(last(r), newsymbol((ulong)rule(ss), true));
      }
    else{
      append(last(r), newsymbol(value(ss), false));
    }
    if(nt(next(ss))){
      append(last(r), newsymbol((ulong)rule(next(ss)), true));
    }
    else{
      append(last(r), newsymbol(value(ss->n), false));
    }
    substitute(m, r);
    substitute(ss, r);

    *find_digram(first(r)) = first(r);
  }
  // ver si alguna regla se usa menos de 2 veces, expandir
  fprintf(stderr, "regla se usa %d veces\n", r->count);
  if (nt(first(r)) && freq(rule(first(r))) == 1) 
    expand(first(r));

}
  
int check(Symbol *S) {
    if (isGuard(S) || isGuard(S->n)) return 0;
    Symbol **x = find_digram(S);
    if ((int)*x <= 1) {
      *x = S;
      return 0;
    }
    if ((int)*x > 1 && (*x)->n != S){
        match(S, *x);
    }
    return 1;  
}


/*Operaciones de la tabla de Hash*/

//op. tabla de hash para buscar e insertar digramas.
Symbol **find_digram(Symbol *S){
  if(S->s == NULL) return -1;
  ulong one = S->s;
  ulong two = next(S)->s;

  int jump = HASH2(one);
  int insert = -1;
  int i = HASH(one, two);
  while (true) {
    Symbol *m = table[i];
    if (!m) {
      if (insert == -1) insert = i;
      return &table[insert];
    } 
    else if ((int)m == 1){
        insert = i;
    }
    else if (raw(m) == one && raw(next(m)) == two){
      return &table[i];
    }
    i = (i + jump) % PRIME;
  }
}

//borrar de la tabla de hash

void del(Symbol *s) {
      fprintf(stderr,"donde\n");
  if (s->n == NULL) return;
  fprintf(stderr,"delete: %d %d\n", isGuard(s), isGuard(next(s)));
  if (isGuard(s) || isGuard(s->n)){ 
      fprintf(stderr,"donde\n");
    return;
  }
  fprintf(stderr,"se\n");
  Symbol **m = find_digram(s);
    fprintf(stderr,"cae\n");
  if (*m == s) *m = (Symbol *) 1;
    fprintf(stderr,"esto\n");
}

// imprimir las reglas

void printfirst(Rule *S){

Symbol *n = first(S);
    int i;
    for (i = 0; i <20; i++){
        n = n->n;
    }
}

Rule **R;
int Ri;

void p(Rule *r) {
  Symbol *p;
  for (p = first(r); !isGuard(p); p = next(p))
    if (nt(p)) {
      int i;

      if (R[rule(p)->number] == rule(p))
        i = rule(p)->number;
      else {
        i = Ri;
        rule(p)->number = Ri;
        R[Ri ++] = rule(p);
      }
      printf("%d ", i);
    }
    else {
      if (value(p) == ' ') printf("%c",'_');
      else if (value(p) == '\n') printf("\\n");
      else if (value(p) == '\t') printf("\\t");
      else if (value(p) == '\\' ||
               value(p) == '(' ||
               value(p) == ')' ||
               value(p) == '_' ||
               isdigit(value(p)))
        printf("\\%c", (char)value(p));
      else printf("%c", (char)value(p));
      printf(" ");
    }
  printf("\n");
}

void print(Rule *S){
  R = (Rule **) malloc(sizeof(Rule *) * num_Rule);
  memset(R, 0, sizeof(Rule *) * num_Rule);
  R[0] = S;
  Ri = 1;
  int i;
  for (i = 0; i < Ri; i ++) {
    printf("%d -> ", i);
    p(R[i]);
  }
  free(R);
}

int main(){
    //clearhash();
    S = newrule();
    ulong get = getchar();
    append(last(S), newsymbol(get, false));
  while (true) {
    get = getchar();
    if(get == EOF) break;
    append(last(S), newsymbol(get, false));
    check(prev(last(S)));
  }
  //printhash();
  printf("reglas: %lu\n", num_Rule);
  print(S); 
}
