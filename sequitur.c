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
    append(q, newsymbol((ulong) r, true));
    if(!check(q))
        check(q->n);
} 

//digramas hacen match, reusar o crear regla.
void match(symbols *ss, symbols *m){
  rules *r;

  // reusar

  if (is_guard(prev(m)) && is_guard(next(next(m)))){
    r = rule(prev(m));
    substitute(ss, r); 
  }
  else {
    // nueva regla
    r = newrule();    
    if (nt(ss)){
      append(last(r), newsymbol(rule(ss), true));
      }
    else{
      append(last(r), newsymbol(value(ss), false));
    }
    if(nt(next(ss))){
      append(last(r), newsymbol(rule(next(ss)), true));
    }
    else{
      append(last(r), newsymbol(value(ss->n), false));
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
  if(S->s == NULL) return -1;
  ulong one = S->s;
  ulong two = next(S)->s;

  int jump = HASH2(one);
  int insert = -1;
  int i = HASH(one, two);
  while (true) {
    symbols *m = table[i];
    if (m == NULL) {
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

void delete_digram(symbols *s) {
  if (is_guard(s) || is_guard(s->n)) 
    return;
  symbols **m = find_digram(s);
  if (*m == s) *m = (symbols *) 1;
}

// imprimir las reglas

rules **R;
int Ri;

void p(rules *r) {
  symbols *p = first(r);
  printf("CHARuu: %d\n", is_guard(p));
  while(p =! is_guard(p)){
    printf("CHAR: %c\n", value(p));
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
      //caracteres especialees
      if ((char)value(p) == ' ') fprintf(stdout, "_");
      else if ((char)value(p) == '\n') fprintf(stdout, "\\n");
      else if ((char)value(p) == '\t') fprintf(stdout, "\\t");
      else if ((char)value(p) == '\\' ||
               (char)value(p) == '(' ||
               (char)value(p) == ')' ||
               (char)value(p) == '_' ||
               isdigit((char)value(p)))
        fprintf(stdout, "\\%c", (char)value(p));
      else fprintf(stdout, "%c", (char)value(p));
      fprintf(stdout, " ");
    }
    p = p->n;
  }
  fprintf(stdout,"\n");
}

void print()
{
  int i;
  R = (rules **) malloc(sizeof(rules *) * num_rules);
  memset(R, 0, sizeof(rules *) * num_rules);
  R[0] = &S;
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
  printf("reglas: %lu\n", num_rules);
  print();
    
    
    
}
