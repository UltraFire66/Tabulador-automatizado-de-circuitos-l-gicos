#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include "BT.h"

#include <windows.h>

using namespace std;

//função que recebe um numero em string e retorna ele convertido em int
int string_to_int(string s){

    int num = 0;
    
    for(int i = 0 ; i < s.size() ; i++){
        num = num + ((int) (s[i] - '0') * pow(10,(s.size()-1) - i));
    }
 
    return num;

}

queue<char> shuttingYard(string expressao){
    
    stack<char> pilha;
    queue<char> fila;

    for(auto c : expressao){

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

    return fila;
}

arvore* fazerArvore(queue<char> fila){

    stack<cel*> pilha;

    

    while(!fila.empty()){

        char c = fila.front();

        if((c > 64 && c < 91) || (c > 96 && c < 123)){
    
            cel* no = cria_cel(c); 
            
            pilha.push(no);
    
        }

        if(c == '+' || c == '.' || c == '!'){

            cel* no = cria_cel(c);

            if(c != '!'){
                no->FD = pilha.top();
                pilha.pop();
                no->FE = pilha.top();
                pilha.pop();
            }
            else{ //no caso do not (!) só coloca um filho do lado direito
                no->FD = pilha.top();
                pilha.pop();
            }
            

            pilha.push(no);
        }
        fila.pop();
    }

    arvore* a = cria_arvore(pilha.top());

    return a;
}

int percorreArvore(cel* no,vector<int> variaveis, unordered_map<char,int> Mvariaveis){

    if((no->valor > 64 && no->valor < 91) || (no->valor > 96 && no->valor < 123)){
    
        return variaveis[Mvariaveis[no->valor]];

    }
    
    if((no->valor == '!')){
        return !percorreArvore(no->FD,variaveis,Mvariaveis);
    }

    if((no->valor == '.')){
        return percorreArvore(no->FE,variaveis,Mvariaveis) & percorreArvore(no->FD,variaveis,Mvariaveis);
    }

    if((no->valor == '+')){
        return percorreArvore(no->FE,variaveis,Mvariaveis) | percorreArvore(no->FD,variaveis,Mvariaveis);
    }



}

int main(){
    SetConsoleOutputCP(CP_UTF8); //definindo UTF8 para conseguir alguns caracteres especiais
    
    
    // ===================================== etapa 1: ler o arquivo de texto e identificar as variaveis =======================
    
    int numVariaveis = 0;
    int numCasos = 0;
    unordered_map<char,int> Mvariaveis;
    vector<char> Vvariaveis;

    ifstream f("teste.txt"); //tenta abrir arquivo
    
    if(!f.is_open()){ //dá erro caso nao consiga abrir
        printf("erro abrindo o arquivo txt");
        return 0;
    }

    string cl; //string pra armazenar a linha atual (current line)

    getline(f,cl); //pegando a primeira linha (usos subsequentes semprem pegam a linha debaixo)

    string expressao;
    expressao = cl; //armazenando expressao

    getline(f,cl);
    numCasos = string_to_int(cl); //armazenando numero de casos

    cout << "expressao: " << expressao << endl;

    cout << "numero de casos: " << numCasos << endl;

    cout << endl;

    //variaveis são identificadas como letras, diferenciando maisculas de minúsculas
    for(int i = 0 ; i < expressao.size() ; i++){

        if((expressao[i] > 64 && expressao[i] < 91) || (expressao[i] > 96 && expressao[i] < 123)){
            
            if(Mvariaveis.find(expressao[i]) == Mvariaveis.end()){ //caso variavel ainda nao tenha sido vista, adiciona ela no map e vector
                Mvariaveis.insert({expressao[i],numVariaveis});
                Vvariaveis.push_back(expressao[i]);
                numVariaveis++;
            }
            
        }

    }

    //imprimindo o map (nao mantem a ordem)
    for (auto variavel : Mvariaveis) {
        cout << variavel.first << " -> " << variavel.second << endl;
    }

    cout << endl;


    //imprimindo o vector (mantem a ordem)
    for (int i = 0 ; i < Vvariaveis.size();i++){
        cout << Vvariaveis[i] << " -> " << i << endl; 
    }
    
    cout << endl;
    cout << "quantidade de variaveis encontradas: " << numVariaveis << endl;
    cout << endl;


    // ===================================== etapa 3: construir a arvore da expressão =======================
    
  
    cout << endl << "shuttingYard: " << endl << endl;
    queue<char> posFixa;
    posFixa = shuttingYard(expressao);

    /*while(!posFixa.empty()){   //imprime a Shutting Yard

        cout << posFixa.front() << " ";
        posFixa.pop();
    }*/

    arvore* a1 = fazerArvore(posFixa);

    imprimir_arvore(a1->raiz);

    cout << endl;

    // ===================================== etapa 2: identificar casos de teste e guardá-los em uma estrutura =======================
    
    int acertos = 0;
    int erros = 0;


    cout << "======Casos======" <<  endl;

    //definindo estrutura do caso de teste
    typedef struct{

        vector<int> variaveis;
        int resultado; 

    }casot;

    casot caso;

    getline(f,cl); //linha contendo apenas um /n

    for(int i = 0 ; i < numCasos ; i++){

        getline(f,cl); //linha contendo o caso de teste
        int cont = 0;

        while(cl[cont] != '|'){
            if(cl[cont] == '1' || cl[cont] == '0'){
                
                caso.variaveis.push_back((int) (cl[cont]-'0')); //colocando os valores do txt em ordem no vector e resultado do caso
            
            }

             cont++;
        }

        caso.resultado = (int) (cl[cont+2]-'0');


        //imprimindo o caso atual na tela
        cout << "caso: ";

        for(auto v : caso.variaveis){
            cout << v << " , ";
        }
        cout << " | " << caso.resultado << endl;


        
        //aqui eu preciso percorrer a arvore e verificar se o resultado esta certo
        int resultadoReal = percorreArvore(a1->raiz,caso.variaveis,Mvariaveis);

        cout << endl << "resultado real: " << resultadoReal << endl << endl;
        
        if(caso.resultado == resultadoReal){
            acertos++;
        }
        else{
            erros++;
        }

        caso.variaveis.clear();//limpando o vetor de variaveis para colocar as variaveis do proximo caso
    }
    
    float porcentagem = (float) acertos/numCasos;

    cout << endl << endl;

    cout << "acertos: " << acertos << endl;
    cout << "erros: " << erros << endl;
    cout << "numero de casos: " << numCasos << endl;
    cout << "porcentagem de acertos: " << porcentagem * 100 << "%" <<  endl << endl;

    f.close(); //fechando o arquivo de texto
    return 0;
}