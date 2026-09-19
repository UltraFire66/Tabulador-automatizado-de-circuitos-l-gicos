#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include "BT.h"

using namespace std;

cel* cria_cel(char v){

    cel* c = new cel;
    c->valor = v;
    c->FE = NULL;
    c->FD = NULL;

    return c;
};

arvore* cria_arvore(char valor){

    cel* r = cria_cel(valor);
    arvore* a1 = new arvore;
    a1->raiz = r;

    return a1;

};

arvore* cria_arvore(cel* r){

    arvore* a1 = new arvore;
    a1->raiz = r;

    return a1;

}

int conectar_filho_esquerda(cel* c,char v){

    if(c->FE != NULL)
        return 0;

    cel* c2 = new cel;
    c2->valor = v;
    c2->FE = NULL;
    c2->FD = NULL;
    c->FE = c2;

    return 1;
}

int conectar_filho_direita(cel* c,char v){

    if(c->FD != NULL)
        return 0;

    cel* c2 = new cel;
    c2->valor = v;
    c2->FE = NULL;
    c2->FD = NULL;
    c->FD = c2;

    return 1;
}

void buscar_em_ordem(cel* c){

    if(c == NULL){
        return;
    }

    buscar_em_ordem(c->FE);
    cout << c->valor << " ";
    buscar_em_ordem(c->FD);

};

void imprimir_arvore(cel* c, string prefixo, bool ultimo) {

    if (c == NULL)
        return;

    cout << prefixo;

    if (ultimo)
        cout << "└── ";
    else
        cout << "├── ";

    cout << "(" << c->valor << ")" << endl;

    // Se tiver os dois filhos
    if (c->FE != NULL && c->FD != NULL) {

        imprimir_arvore(c->FE, prefixo + "│   ", false);
        imprimir_arvore(c->FD, prefixo + "    ", true);

    }
    // Se tiver apenas filho esquerdo
    else if (c->FE != NULL) {

        imprimir_arvore(c->FE, prefixo + "    ", true);

    }
    // Se tiver apenas filho direito
    else if (c->FD != NULL) {

        imprimir_arvore(c->FD, prefixo + "    ", true);
    }
}