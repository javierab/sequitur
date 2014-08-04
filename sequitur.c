#include "sequitur.h"

rule *S;                 // Regla inicial
int num_rules = 0;        // numero de reglas
int num_symbols = 0;      // numero de simbolos
static char* input = "asasasasdadaasasdadaasdasdassasadas";


void compress(){

    int this = 0;
    char pos = input[this];
	rule *s = newRule(pos);

    //printRule(s);
    while (this < strlen(input)){
        this++;
        addSymbol(s, input[this]);
    }
    printRule(s);
	
}



int main(int argc, char **argv){
    compress();
}
