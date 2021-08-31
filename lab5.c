#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//variáveis globais
int nthreads;
int * vetor;
int controle=0; //critério de parada da thread
pthread_mutex_t mutex;
pthread_cond_t cond;

//sincronização das funções
int barreira(void){
  pthread_mutex_lock(&mutex);
  controle++;
  if(controle==nthreads){
    pthread_cond_broadcast(&cond);
    controle=0;
  }
  else
    pthread_cond_wait(&cond,&mutex);
  pthread_mutex_unlock(&mutex);
  return 0;
}

//função executada pelas threads
void* tarefa(void* arg){
  int id = *((int*) arg);
  int * soma = (int*) malloc(sizeof(int));
  *soma=0;
  for(int i=0;i<nthreads;i++){ //realiza as N iterações
    for(int j=0;i<nthreads;j++){  //leitura do vetor
      *soma+=vetor[j];
    }
    barreira();
    pthread_mutex_lock(&mutex);
    vetor[id]=rand()%nthreads;
    pthread_mutex_unlock(&mutex);
    barreira();
  }
  pthread_exit((void*) soma);
}

//fluxo principal
int main(int argc, char* argv[]){

  //variáveis locais
    pthread_t tid[nthreads] ;  //id das threads
    int id[nthreads];           //vetor que armazena id das threads
    int retorno[nthreads];     //vetor que armazena o retorno das threads

  //leitura do valor de entrada
  if(argc<2){
    printf("Formato esperado:  %s <número de threads>\n",argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  //alocação de memória para vetor
  vetor = (int*) malloc(sizeof(int)*nthreads);
  if(vetor==NULL){
    fprintf(stderr,"ERRO--malloc()\n");
    return 2;
  }

  //inicialização do vetor
  for(int i=0;i<nthreads;i++){
    vetor[i]=rand()%nthreads;
  }

  //criação das threads
  for(int i=0;i<nthreads;i++){
    id[i]=i;
    if(pthread_create(&tid[i],NULL,tarefa,(void*) &id[i])){
      printf("--ERRO: pthread_create()\n");
      return 3;
    }
  }

  //aguarda término das threads
  for (int i=0;i<nthreads;i++){
    int * saida;
    if(pthread_join(tid[i],(void**) &saida)){
      fprintf(stderr,"ERRO--pthread_join() \n");
        return 4;
    }
    retorno[i]=*saida;
  }

  //verifica se os retornos foram iguais
  int igual=1;
  for (int i=0; i < (nthreads-1) ;i++){
    if(retorno[i]!=retorno[i+1]) igual=0;
  }
  if(igual) puts("É igual");

  //libera espaço na memória
    free(vetor);
    free(retorno);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

  return 0;
}
