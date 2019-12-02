//Includes
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//variaveis globais 
//Limite é o numero de vezes que repetiremos os 'for's
//Deve ser uma entrada
int Limite;
//leitores e escritores são contadores
int leitores=0, escritores=0;
//variaveis para sincronizacao
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_leit, cond_escr;
//variável compartilhada de escrita e leitura
int papel=0;

//Files
FILE * file0;
FILE * file1;
FILE * file2;
FILE * file3;
FILE * file4;
FILE * file5;
FILE * file6;
FILE * file7;
FILE * file8;
// Métodos dos Leitores

// Função para contabilizar entrada na leitura
int EntraLeitura(int id){
    //variavel com o valor lido pelo leitor
    //será escrito no arquivo dele
    int vouEscreverNoArquivo;
    //dou lock no mutex da variavel de condiçao
    pthread_mutex_lock(&mutex);
    //enquanto nenhum escritor tiver escrito
    //ou enquanto um escritor estiver escrevendo
    while(escritores>0){
        //thread leitora espera
        pthread_cond_wait(&cond_leit, &mutex);
    }
    printf("Thread %d está lendo", id);
    leitores++;
    //Le a variavel compartilhada com os escritores
    vouEscreverNoArquivo = papel;
    //libero o lock
    pthread_mutex_unlock(&mutex);
    return vouEscreverNoArquivo;
}

// Função para contabilizar saída na leitura
void SaiLeitura(){
    //dou lock
    pthread_mutex_lock(&mutex);
    //menos um leitor (quer dizer que um leitor já leu)
    leitores--;
    //se todos os leitores já tiverem lido,
    //ou seja, 'leitores'==0
    //libero o cond signal pros escritores
    if(leitores==0){
        pthread_cond_signal(&cond_escr);
    }
    pthread_mutex_unlock(&mutex);
}

// Leitores
void *leitor(void * arg){
    int tid = *(int*) arg;
    int i;
    //variavel com o valor lido pelo leitor
    //será escrito no arquivo dele
    int vouEscreverNoArquivo;
    for(i=0; i<Limite+1; i++){
        //Entro na Leitura passando o ID
        vouEscreverNoArquivo = EntraLeitura(tid);
        //cria arquivo para colocar o valor lido 
        switch(tid){
            case (0):
                file0 = fopen("Arquivo0.txt", "w");
                fprintf(file0, "%d", vouEscreverNoArquivo);
                fclose(file0);
            case (1):
                file1 = fopen("Arquivo1.txt", "w");
                fprintf(file1, "%d", vouEscreverNoArquivo);
                fclose(file1);
            case (2):
                file2 = fopen("Arquivo2.txt", "w");
                fprintf(file2, "%d", vouEscreverNoArquivo);
                fclose(file2);
            case (3):
                file3 = fopen("Arquivo3.txt", "w");
                fprintf(file3, "%d", vouEscreverNoArquivo);
                fclose(file3);
            case (4):
                file4 = fopen("Arquivo4.txt", "w");
                fprintf(file4, "%d", vouEscreverNoArquivo);
                fclose(file4);
            case (5):
                file5 = fopen("Arquivo5.txt", "w");
                fprintf(file5, "%d", vouEscreverNoArquivo);
                fclose(file5);
            case (6):
                file6 = fopen("Arquivo6.txt", "w");
                fprintf(file6, "%d", vouEscreverNoArquivo);
                fclose(file6);
            case (7):
                file7 = fopen("Arquivo7.txt", "w");
                fprintf(file7, "%d", vouEscreverNoArquivo);
                fclose(file7);
            case (8):
                file8 = fopen("Arquivo8.txt", "w");
                fprintf(file8, "%d", vouEscreverNoArquivo);
                fclose(file8);
        }
        SaiLeitura();
    }
    pthread_exit(NULL);
}


// Métodos dos Escritores

// Função para contabilizar entrada na escrita
void EntraEscrita(int id){
    pthread_mutex_lock(&mutex);
    //Para escrever, nenhum leitor pode estar lendo
    //nenhum escritor pode estar escrevendo
    while ((escritores > 0) || (leitores > 0)){
        pthread_cond_wait(&cond_escr, &mutex);
    }
    printf("Thread %d está escrevendo", id);
    escritores++;
    pthread_mutex_unlock(&mutex);
}

// Função para contabilizar saída na escrita
void SaiEscrita(){
    pthread_mutex_lock(&mutex);
    escritores--;
    //libero para que todos os Leitores leiam
    pthread_cond_broadcast(&cond_leit);
    //libero para que o proximo escritor possa escrever
    pthread_cond_signal(&cond_escr);
    pthread_mutex_unlock(&mutex);
}

// Escritores
void *escritor(void * arg){
    int tid = *(int*) arg;
    int i;
    //while é um looping que devemos controlar
    //para testar as leituras e escritas
    for(i=0; i<Limite; i++){
        EntraEscrita(tid);
        pthread_mutex_lock(&mutex);
        //escrever na variavel compartilhada
        papel = tid;
        pthread_mutex_unlock(&mutex);
        SaiEscrita();
    }
    pthread_exit(NULL);
}


// Função Main
int main (int argc, char *argv[]){
    int t, *id;
    int qtdLeitores, qtdEscritores;
    //recebo parametros da linha de comando
    //recebo a quantidade de cada tipo de thread
    qtdLeitores = atoi (argv[1]);
    qtdEscritores = atoi (argv[2]);
    //Numero de escritas que devem ser realizadas
    Limite = atoi (argv[3]);
    //numero de threads
    pthread_t tid[qtdEscritores+qtdLeitores];
    
    //for para criar as threads
    for(t=0; t<qtdEscritores; t++) {
        id = malloc(sizeof(int));
        if (id == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        if(pthread_create(&tid[t], NULL, escritor, (void*) id))
            printf("--ERRO: pthread_create()\n"); exit(-1);
    }
    for(t=0; t<qtdLeitores; t++) {
        id = malloc(sizeof(int));
        if (id == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        if(pthread_create(&tid[t], NULL, leitor, (void*) id))
            printf("--ERRO: pthread_create()\n"); exit(-1);
    }
}