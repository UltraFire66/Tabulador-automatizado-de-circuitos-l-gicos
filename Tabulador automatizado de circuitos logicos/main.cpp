#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset> //biblioteca pra transformar um inteiro em binario

#include "BT.h"

#define txt "./exemplos/teste_normal_3000.txt"


#include <windows.h>

using namespace std;


/*

Atividade de computação paralela:

Quais são as principais etapas do algoritmo?

O algoritmo é dividido em 4 grandes etapas:

etapa 1: Leitura do arquivo txt, armazenando a expressão em uma string, o numero de casos em um int, as variaveis em um map e vector e o numero de variáveis em um int (99% de chance de ser seção crítica).

etapa 2: Construção da arvore. Depende somente de saber a expressão que está na string. Monta um parser com os dados da expressão e coloca eles na arvore (possível seção crítica)

etapa 3: Cada caso de teste é inserido na arvore e compara o resultado com o fornecido no arquivo txt para aquele caso. Caso esteja certo, acrescenta 1 na variavel de acertos, caso esteja errado acrescenta 1 na variavel de erros. (seção com melhor possibilidade de paralelismo, podemos dividir todos os casos entre os processadores, já que eles são indepedentes entre si, e a unica coisa que precisa ser compartilhada são as variaveis de acerto e erro)

etapa 4: Apresentar os resultados na tela, com a porcentagem de acerto. (sem possibilidade de paralelização)

O paralelismo será de dados, de tarefas ou híbrido?

Uma vez que os casos de testes são armazenados em uma única variável, que é sobrescrita para cada novo caso, o paralelismo será de tarefas. O número total de casos de teste será dividido entre os núcleos.

Qual parte pode ser executada simultaneamente?

????

Como o trabalho será dividido entre threads e processos?

????

Quais etapas ou dados possuem dependências?

A etapa 2 tem dependência da etapa 1. A etapa 3 tem dependência das etapas 1 e 2. A etapa 4 depende de todas as outras.

Existe risco de condição de corrida? Desenvolva

Sim, por mais que os testes dos casos de teste seja independentes, o resultado do teste deve incrementar a variavel de acertos ou erros. Assim, a condição de corrida existe para as variáveis de acertos e erros.

Será necessário sincronizar ou combinar resultados?

Sim, no mesmo caso citado acima.

*/




//função que recebe um numero em string e retorna ele convertido em int
int string_to_int(string s){

    int num = 0;
    
    for(int i = 0 ; i < s.size() ; i++){
        num = num * 10 + (s[i] - '0');
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

    ifstream f(txt); //tenta abrir arquivo
    ifstream teste(txt); //segunda instancia do arquivo txt só pra checar se entra no modo de tabela verdade ou nao. É fechado depois de conferir

    if(!f.is_open() || !teste.is_open()){ //dá erro caso nao consiga abrir
        printf("erro abrindo o arquivo txt");
        return 0;
    }

    string cl; //string pra armazenar a linha atual (current line)
    string cl2;

    getline(f,cl); //pegando a primeira linha (usos subsequentes semprem pegam a linha debaixo)
    getline(teste,cl2);

    string expressao;
    expressao = cl; //armazenando expressao

    getline(f,cl);
    getline(teste,cl2);
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
    
  
    cout << endl << "shuttingYard: ";
    queue<char> posFixa;
    posFixa = shuttingYard(expressao);

    int tamanhoFila = posFixa.size();

    for(int i = 0 ; i < tamanhoFila ; i++){
        cout << posFixa.front() << " ";
        posFixa.push(posFixa.front());
        posFixa.pop();
    }

    cout << endl << endl;
    cout << "=========== Árvore formada ==========" << endl << endl;
    arvore* a1 = fazerArvore(posFixa);

    imprimir_arvore(a1->raiz);

    cout << endl;

    // ===================================== etapa 2: identificar casos de teste e guardá-los em uma estrutura =======================
    
    int acertos = 0;
    int erros = 0;


    cout << "======Casos======" <<  endl << endl;

    //definindo estrutura do caso de teste
    typedef struct{

        vector<int> variaveis;
        int resultado; 

    }casot;

    casot caso;

    getline(f,cl); //linha contendo apenas um /n
    getline(teste,cl2);
    
    
    int ModoMegaBrain = 0; //se for positivo significa que tenho que fazer a tabela verdade inteira
    
    streampos PosicaoAntiga;

    getline(teste,cl2);
    
    int res = cl2.find('|');

    if(res == string::npos){  //isso nao funciona, qual a lógica por tras?
        ModoMegaBrain = 1;
    }
    
    teste.close();
    

    if(!ModoMegaBrain){


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

    }
    if(ModoMegaBrain){ //modo de montagem de tabela verdade

       cout << "====== Mega Brain ======" <<  endl << endl;
       
        numCasos = pow(2,numVariaveis);

       for(int i = 0 ; i < pow(2,numVariaveis) ; i++){
            
            getline(f,cl);
            caso.resultado = cl[0]-'0';

            for(int j = 0 ; j < numVariaveis ; j++){ // pegando o numero em binario e dividindo os bits para todas as variaveis usando mascaramento

                caso.variaveis.push_back((i >> (numVariaveis-1-j) & 1));
                cout << "  " << caso.variaveis[j];
            
            }
            cout << " | " << caso.resultado;

            // com as variaveis com seus respectivos valores, percorro a arvore e trago o resultado pra ver se o usuario acertou ou nao
            int resultadoReal = percorreArvore(a1->raiz,caso.variaveis,Mvariaveis);
    
            cout << endl << "resultado real: " << resultadoReal << endl << endl;
            
            if(caso.resultado == resultadoReal){
                acertos++;
            }
            else{
                erros++;
            }


            cout << endl;
            caso.variaveis.clear();//limpando o vetor de variaveis para colocar as variaveis do proximo caso
       }

    }

    // ===================================== etapa 4: Mostrar resultados na tela =======================

    float porcentagem = (float) acertos/numCasos;

    cout << endl << endl;

    cout << "acertos: " << acertos << endl;
    cout << "erros: " << erros << endl;
    cout << "numero de casos: " << numCasos << endl;
    cout << fixed << setprecision(2);
    cout << "porcentagem de acertos: " << porcentagem * 100 << "%" <<  endl << endl;

    f.close(); //fechando o arquivo de texto
    return 0;
}