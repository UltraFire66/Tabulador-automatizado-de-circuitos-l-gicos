#include "BT.h"
#include <iostream>

using namespace std;

int main(){

    // Cria a raiz
    arvore* a = cria_arvore('+');

    // Cria os filhos da raiz
    conectar_filho_esquerda(a->raiz, '.');
    conectar_filho_direita(a->raiz, '!');

    // Cria A e B como filhos de '.'
    conectar_filho_esquerda(a->raiz->FE, 'A');
    conectar_filho_direita(a->raiz->FE, 'B');

    // Cria C como filho de '!'
    conectar_filho_direita(a->raiz->FD, 'C');

    // Percorre a árvore em ordem
    cout << "Percurso em ordem: ";
    buscar_em_ordem(a->raiz);
    cout << endl;

    return 0;
}