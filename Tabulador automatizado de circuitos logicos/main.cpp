#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset> //biblioteca pra transformar um inteiro em binario
#include <chrono>  // para usar o relogio
#include "BT.h"

#define txt "./exemplos/mega_brain_atomico.txt"


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

// ============================================================
// FUNÇÕES AUXILIARES PARA VALIDAÇÃO DO ARQUIVO
// ============================================================

// Remove espaços em branco no início e no final de uma string.
string trim(string s) {

    size_t inicio = s.find_first_not_of(" \t\r\n");

    if (inicio == string::npos)
        return "";

    size_t fim = s.find_last_not_of(" \t\r\n");

    return s.substr(inicio, fim - inicio + 1);
}


// ============================================================
// VALIDAÇÃO DA EXPRESSÃO LÓGICA
// ============================================================

class ValidadorExpressao {

private:

    string expressao;
    size_t posicao = 0;

public:
    set<char> variaveis;

    ValidadorExpressao(string e) {
        expressao = e;
    }

    void ignorarEspacos() {
        while (posicao < expressao.size() &&
               isspace((unsigned char)expressao[posicao])) {
            posicao++;
        }
    }

    bool expressaoLogica() {
        if (!operando())
            return false;

        ignorarEspacos();
        while (posicao < expressao.size()) {
            char c = expressao[posicao];

            if (c == '+' || c == '.') {
                posicao++;

                if (!operando())
                    return false;
            } else{
                break;
            }
            ignorarEspacos();
        }
        return true;
    }

    bool operando() {
        ignorarEspacos();
        if (posicao >= expressao.size())
            return false;

        char c = expressao[posicao];

        // Operador NOT
        if (c == '!') {
            posicao++;
            return operando();
        }

        // Expressão entre parênteses
        if (c == '(') {
            posicao++;

            if (!expressaoLogica())
                return false;
            ignorarEspacos();

            if (posicao >= expressao.size() ||
                expressao[posicao] != ')') {
                return false;
            }

            posicao++;
            return true;
        }

        // Variáveis: letras maiúsculas ou minúsculas
        if (isalpha((unsigned char)c)) {
            variaveis.insert(c);
            posicao++;
            return true;
        }
        return false;
    }

    bool validar() {
        ignorarEspacos();
        if (!expressaoLogica())
            return false;
        ignorarEspacos();
        // Não pode haver caracteres restantes.
        return posicao == expressao.size();
    }
};

// ============================================================
// VALIDAÇÃO DO NÚMERO DE CASOS
// ============================================================

bool validarNumeroCasos(string linha, int &numero) {
    linha = trim(linha);
    if (linha.empty())
        return false;

    // Aceita somente números inteiros positivos.
    for (char c : linha) {

        if (!isdigit((unsigned char)c))
            return false;
    }

    try {

        long long valor = stoll(linha);

        if (valor <= 0 || valor > INT_MAX)
            return false;

        numero = (int)valor;

    }
    catch (...) {

        return false;
    }

    return true;
}


// ============================================================
// VALIDAÇÃO DE UM CASO CONVENCIONAL
// ============================================================

bool validarCaso(string linha, int numVariaveis, int numeroLinha) {

    linha = trim(linha);

    if (linha.empty()) {

        cerr << "Erro na linha " << numeroLinha
             << ": linha vazia." << endl;

        return false;
    }

    // Deve existir exatamente um separador '|'.
    size_t separador = linha.find('|');

    if (separador == string::npos ||
        linha.find('|', separador + 1) != string::npos) {

        cerr << "Erro na linha " << numeroLinha
             << ": separador '|' ausente ou duplicado." << endl;

        return false;
    }

    string valores = trim(linha.substr(0, separador));
    string resultado = trim(linha.substr(separador + 1));

    // Impede vírgulas no início, no final ou valores ausentes.
    if (valores.empty() ||
        valores.front() == ',' ||
        valores.back() == ',') {

        cerr << "Erro na linha " << numeroLinha
            << ": separacao incorreta entre variaveis."
            << endl;

        return false;
    }

    // Verifica o resultado esperado.
    if (resultado != "0" && resultado != "1") {

        cerr << "Erro na linha " << numeroLinha
             << ": resultado deve ser 0 ou 1." << endl;

        return false;
    }

    // Separa os valores das variáveis pelas vírgulas.
    stringstream ss(valores);

    string valor;
    int quantidade = 0;

    while (getline(ss, valor, ',')) {

        valor = trim(valor);

        // Cada variável deve possuir exatamente um dígito.
        if (valor != "0" && valor != "1") {

            cerr << "Erro na linha " << numeroLinha
                 << ": valor de variavel invalido: '"
                 << valor << "'. Utilize somente 0 ou 1."
                 << endl;

            return false;
        }

        quantidade++;
    }

    // Verifica se a quantidade de valores está correta.
    if (quantidade != numVariaveis) {

        cerr << "Erro na linha " << numeroLinha
             << ": quantidade incorreta de variaveis. Esperado: "
             << numVariaveis << ", encontrado: "
             << quantidade << "." << endl;

        return false;
    }

    return true;
}


// ============================================================
// VALIDAÇÃO DO ARQUIVO COMPLETO
// ============================================================

bool validarArquivo(string caminho) {

    ifstream arquivo(caminho);

    if (!arquivo.is_open()) {

        cerr << "Erro: nao foi possivel abrir o arquivo."
             << endl;

        return false;
    }

    vector<string> linhas;
    string linha;

    // Lê todas as linhas sem alterar o arquivo original.
    while (getline(arquivo, linha)) {

        linhas.push_back(linha);
    }

    arquivo.close();

    // Verifica se existem pelo menos as três linhas iniciais.
    if (linhas.size() < 3) {

        cerr << "Erro: arquivo incompleto." << endl;

        return false;
    }

    // --------------------------------------------------------
    // 1. Validação da expressão
    // --------------------------------------------------------

    ValidadorExpressao validador(linhas[0]);

    if (!validador.validar()) {

        cerr << "Erro na linha 1: expressao logica invalida."
             << endl;

        cerr << "Verifique os operadores, operandos e parenteses."
             << endl;

        return false;
    }

    int numVariaveis = validador.variaveis.size();

    if (numVariaveis == 0) {

        cerr << "Erro: a expressao nao possui variaveis."
             << endl;

        return false;
    }

    // --------------------------------------------------------
    // 2. Validação do número de casos
    // --------------------------------------------------------

    int numCasosDeclarados;

    if (!validarNumeroCasos(linhas[1], numCasosDeclarados)) {

        cerr << "Erro na linha 2: numero de casos invalido."
             << endl;

        return false;
    }

    // --------------------------------------------------------
    // 3. Validação da linha separadora
    // --------------------------------------------------------

    if (!trim(linhas[2]).empty()) {

        cerr << "Erro na linha 3: era esperada uma linha vazia."
             << endl;

        return false;
    }

    // --------------------------------------------------------
    // 4. Identificação do modo de execução
    // --------------------------------------------------------

    if (linhas.size() <= 3) {

        cerr << "Erro: nenhum caso de teste encontrado."
             << endl;

        return false;
    }

    bool modoMegaBrain =
        (linhas[3].find('|') == string::npos);

    int quantidadeLinhas = (int)linhas.size() - 3;

    // --------------------------------------------------------
    // 5. Validação do modo Mega Brain
    // --------------------------------------------------------

    if (modoMegaBrain) {

        // O programa original utiliza int para os índices
        // e operações de deslocamento de bits.
        if (numVariaveis >= 31) {

            cerr << "Erro: quantidade de variaveis muito grande "
                 << "para o modo Mega Brain." << endl;

            return false;
        }

        unsigned long long quantidadeEsperada =
            1ULL << numVariaveis;

        if (quantidadeLinhas != quantidadeEsperada) {

            cerr << "Erro: quantidade incorreta de linhas "
                 << "na tabela verdade." << endl;

            cerr << "Esperado: " << quantidadeEsperada
                 << ", encontrado: " << quantidadeLinhas
                 << "." << endl;

            return false;
        }

        // Cada linha deve conter exclusivamente 0 ou 1.
        for (int i = 3; i < (int)linhas.size(); i++) {

            string resultado = trim(linhas[i]);

            if (resultado != "0" && resultado != "1") {

                cerr << "Erro na linha " << i + 1
                     << ": resultado da tabela verdade "
                     << "deve ser exclusivamente 0 ou 1."
                     << endl;

                return false;
            }
        }
    }

    // --------------------------------------------------------
    // 6. Validação do modo convencional
    // --------------------------------------------------------

    else {

        // Compara a quantidade declarada com as linhas existentes.
        if (quantidadeLinhas != numCasosDeclarados) {

            cerr << "Erro: quantidade incorreta de casos de teste."
                 << endl;

            cerr << "Declarado: " << numCasosDeclarados
                 << ", encontrado: " << quantidadeLinhas
                 << "." << endl;

            return false;
        }

        // Valida individualmente cada caso.
        for (int i = 3; i < (int)linhas.size(); i++) {

            if (!validarCaso(linhas[i], numVariaveis, i + 1)) {

                return false;
            }
        }
    }

    cout << "Validacao concluida: arquivo correto!"
         << endl << endl;

    return true;
}


//função que recebe um numero em string e retorna ele convertido em int
int string_to_int(string s){

    int num = stoi(s);

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
    //SetConsoleOutputCP(CP_UTF8); //definindo UTF8 para conseguir alguns caracteres especiais
    
    //auto start = chrono::high_resolution_clock::now(); //iniciando o relogio

    SetConsoleOutputCP(CP_UTF8);

    // =====================================
    // ETAPA 0: VALIDAR ENTRADAS
    // =====================================

    if (!validarArquivo(txt)) {

        cout << endl;
        cout << "Execucao interrompida devido a erros "
             << "no arquivo de entrada." << endl;

        return 1;
    }

    // Início da medição do tempo original.
    auto start = chrono::high_resolution_clock::now();

   
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

    auto etapa1 = chrono::high_resolution_clock::now(); //finalizando o relogio e mostrando o tempo na tela

    chrono::duration<double> elapsed1 = etapa1 - start;
  
    
    cout << endl;
    //imprimindo o vector (mantem a ordem)
    for (int i = 0 ; i < Vvariaveis.size();i++){
        cout << Vvariaveis[i] << " -> " << i << endl; 
    }
    
    cout << endl;
    cout << "quantidade de variaveis encontradas: " << numVariaveis << endl;
    cout << endl;


    // ===================================== etapa 2: construir a arvore da expressão =======================
    
    
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
    
    auto etapa2 = chrono::high_resolution_clock::now(); //finalizando o relogio e mostrando o tempo na tela

    chrono::duration<double> elapsed2 = etapa2 - etapa1;
  


    // ===================================== etapa 3: identificar casos de teste e guardá-los em uma estrutura =======================
    
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

    auto etapa3 = chrono::high_resolution_clock::now(); //finalizando o relogio e mostrando o tempo na tela

    chrono::duration<double> elapsed3 = etapa3 - etapa2;


    // ===================================== etapa 4: Mostrar resultados na tela =======================

    float porcentagem = (float) acertos/numCasos;

    cout << endl << endl;

    cout << "acertos: " << acertos << endl;
    cout << "erros: " << erros << endl;
    cout << "numero de casos: " << numCasos << endl;
    cout << fixed << setprecision(2);
    cout << "porcentagem de acertos: " << porcentagem * 100 << "%" <<  endl << endl;

    f.close(); //fechando o arquivo de texto

    auto etapa4 = chrono::high_resolution_clock::now(); //finalizando o relogio e mostrando o tempo na tela

    
    
    
    chrono::duration<double> elapsed4 = etapa4 - etapa3;
    
    chrono::duration<double> elapsedTotal = etapa4 - start;
    cout << fixed << setprecision(3);
    cout << "Tempo de execução da primeira etapa: " << elapsed1.count() << " s" << endl;
    cout << "Tempo de execução da segunda etapa: " << elapsed2.count() << " s" << endl;
    cout << "Tempo de execução da terceira etapa: " << elapsed3.count() << " s" << endl;
    cout << "Tempo de execução da quarta etapa: " << elapsed4.count() << " s" << endl;
    cout << "Tempo de execução total: " << elapsedTotal.count() << " s" << endl;

    return 0;
}