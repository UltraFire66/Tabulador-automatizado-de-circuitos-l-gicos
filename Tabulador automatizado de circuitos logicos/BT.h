#ifndef BT_H
#define BT_H

#include <string>

using namespace std;


struct cel{

    char valor;
    cel* FE;
    cel* FD;

};

struct arvore{

   cel* raiz;

};


cel* cria_cel(char v);

arvore* cria_arvore(char valor);

arvore* cria_arvore(cel* r);

int conectar_filho_esquerda(cel* c,char v);

int conectar_filho_direita(cel* c,char v);

void buscar_em_ordem(cel* c);

void imprimir_arvore(cel* c, string prefixo = "", bool ultimo = true);

#endif