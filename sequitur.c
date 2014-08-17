#include "sequitur.h"

//juntar 2 simbolos, borrando los digramas viejos de la tabla de hash.

void join(symbols *left, symbols *right) {
  if (left->n) {
    delete_digram(left);
    if (right->p && right->n &&
        right->s/2 == right->p->s/2 &&
        right->s/2 == right->n->s/2){
            *find_digram(right) = right;
    }
    
    if (left->p && left->n &&
        left->s/2 == left->n->s/2 &&
        left->s/2 == left->p->s/2){
            *find_digram(left->p) = left->p;
    }
  }
  left->n = right; 
  right->p = left;
}


//ultima referencia a una regla: hay que borrarla!
void expand(symbols *s){
  symbols *left = prev(s);
  symbols *right = next(s);
  symbols *f = first(rule(s));
  symbols *l = last(rule(s));

  delrule(rule(s));
  symbols **m = find_digram(s);
  if (*m == s) *m = (symbols *) 1;
  s->s = 0; 
  //adios :c
  delsymb(s);
  
  join(left, f);
  join(l, right);

  *find_digram(l) = l;
}

//sustituir un digrama por un no terminal.
void substitute(symbols *s, rules *r){
    symbols *q = s->p;
    
    delsymb(next(q));
    delsymb(next(q));
    append(q, newsymbol((ulong) r, 1));
    if(!check(q))
        check(q->n);
} 

//digramas hacen match, reusar o crear regla.
void match(symbols *ss, symbols *m){
  rules *r;

  // reusar

  if (is_guard(m->p) && is_guard(m->n->n)){
    printf("reuso\n");
    r = rule(m->p);
    substitute(ss, r); 
  }
  else {
      printf("nueva regla\n");
    // nueva regla
    r = newrule();    
    if (nt(ss)){
      append(last(r), newsymbol(ss->s, true));
      }
    else{
      append(last(r), newsymbol(ss->s, false));
    }
    if(nt(next(ss))){
      append(last(r), newsymbol(ss->n->s, true));
    }
    else{
      append(last(r), newsymbol(ss->n->s, false));
    }
    substitute(m, r);
    substitute(ss, r);

    *find_digram(first(r)) = first(r);
  }
  // ver si alguna regla se usa menos de 2 veces, expandir.
  if (nt(first(r)) && freq(rule(first(r))) == 1) 
    expand(first(r));

}
  
int check(symbols *S) {
    if (is_guard(S) || is_guard(S->n)) return 0;
    symbols **x = find_digram(S);
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
symbols **find_digram(symbols *S){
  ulong one = S->s;
  ulong two = next(S)->s;

  int jump = HASH2(one);
  int insert = -1;
  int i = HASH(one, two);
  while (true) {
    symbols *m = table[i];
    if (m == -1) {
      if (insert == -1) insert = i;
      return &table[insert];
    } 
    else if ((int)m == 1){
        insert = i;
    }
    else if (m->s == one && next(m)->s == two){
      return &table[i];
    }
    i = (i + jump) % PRIME;
  }
}

//borrar de la tabla de hash

void delete_digram(symbols *s) {
  if (is_guard(s) || is_guard(s->n)) 
    return;
  symbols **m = find_digram(s);
  if (*m == s) *m = (symbols *)-1;
}

// imprimir las reglas

rules **R;
int Ri;

void p(rules *r) {
  symbols *p;
  for (p = first(r); !is_guard(p); p = next(p))
    if (nt(p)) {
      int i;
      if (R[rule(p)->number] == rule(p))
        i = rule(p)->number;
      else {
        i = Ri;
        rule(p)->number =  Ri;
        R[Ri++] = rule(p);
      }

      fprintf(stdout, "%d ", i);
    }
    else {
      if ((char)p->s == ' ') fprintf(stdout, "_");
      else if ((char)p->s == '\n') fprintf(stdout, "\\n");
      else if ((char)p->s == '\t') fprintf(stdout, "\\t");
      else if ((char)p->s == '\\' ||
               (char)p->s == '(' ||
               (char)p->s == ')' ||
               (char)p->s == '_' ||
               isdigit((char)p->s))
        fprintf(stdout, "\\%c", (char)p->s);
      else fprintf(stdout, "%c", (char)p->s);
      fprintf(stdout, " ");
    }
  fprintf(stdout,"\n");
}

void print()
{
  int i;
  R = (rules **) malloc(sizeof(rules *) * num_rules);
  memset(R, 0, sizeof(rules *) * num_rules);
  R[0] = (rules *)&S;
  Ri = 1;

  for (i = 0; i < Ri; i ++) {
    fprintf(stdout, "%d -> ", i);
    p(R[i]);
  }
  free(R);
}


//el compresor mismo.
void clearhash(){
    ulong i;
    for(i = 0; i < PRIME; i++){
        table[i] = (symbols *)malloc(sizeof(symbols));
        table[i] = (symbols *)-1;
    }
}

void printhash(){
    ulong i;
    for(i = 0; i < PRIME; i++){
        if (table[i] != (symbols *)-1)
            printf("%lu\n", i);
    }
}


int main(){
    clearhash();
    S = newrule();
    char get = getchar();
    fprintf(stderr,"char: %c\n", get);
    append(last(S), newsymbol(get, false));
  while (get != EOF) {
    get = getchar();
    append(last(S), newsymbol(get, false));
    check(prev(last(S)));
  }
  printhash();
  printf("reglas: %lu\n", num_rules);
  print();
    
    
    
}
