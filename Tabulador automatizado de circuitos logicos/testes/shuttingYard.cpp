#include <bits/stdc++.h>
#include <queue>
#include <stack>
#include <fstream>
#include <iostream>

using namespace std;

int main(){

    stack<char> pilha;
    queue<char> fila;
    ifstream f("../expressao.txt"); //tenta abrir arquivo

    if(!f.is_open()){ //dá erro caso nao consiga abrir
        printf("erro abrindo o arquivo txt");
        return 0;
    }

    string expressao; //string pra armazenar a linha atual (current line)

    getline(f,expressao); //pegando a primeira linha (usos subsequentes semprem pegam a linha debaixo)

    cout << expressao << endl;

    for(auto c : expressao){

        cout << c << " ";

        if((c > 64 && c < 91) || (c > 96 && c < 123)){

            fila.push(c);

        }

        if(c == '('){
            pilha.push(c);
        }

        if(c == '+' || c == '.' || c == '!'){           
            pilha.push(c);
        }

        if(c == ')'){
            while(pilha.top() != '('){
                
                fila.push(pilha.top());
                
                pilha.pop();
            }
            pilha.pop(); // tira o ( da pilha
        }

    }

    while(!pilha.empty()){

        fila.push(pilha.top());
        pilha.pop();
    }

    cout << endl << "fila : ";



}