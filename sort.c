//===========================================
//== Universidade Federal de Minas Gerais  ==
//== Valéria Pereira de Souza              ==
//== TP 1 - Malditos Macacos               == 
//===========================================
// I ackwnoledge that there is a lot of refactoring that could be done in this code. But I was young back then and... who has the time

#include <stdio.h>
#include <string.h>
#include "mem.h"

int a_menor_que_b(char* a, char* b, int len) {
   
    int i=0;

    //Procura a primeira letra diferente. Quando acha, testa e retorna.
    //Se acabar o vetor e não achar nenhuma letra diferente, é pq são iguais e retorna 0
    for (i=0; i<len; i++){
        if(a[i] != b[i]){
            if(a[i]<b[i])
                return 1;
            else
                return 0;
        }
    }

    return 0;
 }

char** cria_vetor_strings(int lenString, int m){ 
      
    char **vetor_string;
    int i;

    vetor_string = (char**) mathias_malloc(m * sizeof(char*));

    for(i=0; i<m; i++){
        vetor_string[i] = (char*) mathias_malloc(lenString+2 * sizeof(char));
        vetor_string[i][0] = -1;
    }

    return vetor_string;
 }

void ordena_m_strings(int m, int lenString, char **vetor_string){
    
    int i, j;
    char *aux = (char*) mathias_malloc ((lenString+2)*sizeof(char));

    for(i=1; i<m; i++){
        for(j=0; j<m-1; j++){
            if (strcmp(vetor_string[j], vetor_string[j+1])>0){
                strcpy(aux, vetor_string[j]);
                strcpy(vetor_string[j], vetor_string[j+1]);
                strcpy(vetor_string[j+1], aux);
            }
        }
    }

    mathias_free(aux);
 }

int monta_blocos(FILE *file, int tam){
    
    int menor, totalStrings=0;

    FILE *f1, *f2, *f3, *f4, *fita;
    f1 = fopen("f1.txt", "w");
    f2 = fopen("f2.txt", "w");
    f3 = fopen("f3.txt", "w");
    f4 = fopen("f4.txt", "w");
    if(f1==NULL || f2==NULL || f3==NULL || f4==NULL)
        printf("Nao foi possivel abrir as fitas!\n");

    char *stringA, *stringB;
    stringA = (char*) mathias_malloc(tam+1 * sizeof(char));
    stringB = (char*) mathias_malloc(tam+1 * sizeof(char));
    
    stringA[0]=-1;
    stringB[0]=-1;

    fita = f1;

    while ( fscanf(file, "%[^\n]%*c", stringA) != EOF &&  fscanf(file, "%[^\n]%*c", stringB) != EOF){
        if (stringB[0]==-1){
            fprintf(fita, "%s\n", stringA);
            totalStrings++;
        }else{
            totalStrings+=2;
            menor = a_menor_que_b(stringA, stringB, tam);

            if (menor==1)
                fprintf(fita, "%s\n%s\n", stringA, stringB);
            else if (menor ==0)
                fprintf(fita, "%s\n%s\n", stringB, stringA);
            if(fita==f1)
                fita = f2;
            else if(fita==f2)
                fita = f1;

            stringA[0]=-1;
            stringB[0]=-1;
        }

    }
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    mathias_free(stringA);
    mathias_free(stringB);
    return totalStrings;
 }

int cabe_em_mm (int totalStrings, int lenString, unsigned int memory){

    int m, sizeStructure;

    sizeStructure = totalStrings*( sizeof(char*) + sizeof(char)*(lenString+2)  );
    m = memory*1000/sizeStructure;

    if(m>=1)
        return 1;
    return 0;
 }

void external_sort(const char* input_file, const char* output_file, unsigned int memory) {

    int tam, i, menor, totalStrings, nPassadas=0, m, cabe;
    int tamBloco=2, faltaEscrever1=2, faltaEscrever2=2, a=0, b=0;
    char *stringA, *stringB, **vetor_string, c;
    
    FILE *entrada, *saida, *fitaL1, *fitaL2, *fitaE, *f1, *f2, *f3, *f4;
    entrada = fopen(input_file, "r");
    saida = fopen(output_file, "w");

    if(entrada == NULL || saida == NULL)
        printf("Nao foi possivel abrir o arquivo de entrada!\n");
    
    fscanf(entrada, "%d%c", &tam, &c);
    
    totalStrings = monta_blocos(entrada, tam);
    cabe = cabe_em_mm(totalStrings, tam, memory);

    //CASO 1: todas as strings cabem em MP. Ordenar direto
    if(cabe){
        rewind(entrada);
        fscanf(entrada, "%d%c", &tam, &c);

        m = totalStrings;               //Atualiza valor de m para evitar desperdício de memória

        vetor_string = cria_vetor_strings(tam, m);

        //Lê todas as strings
        for(i=0; i<m; i++)
            fgets(vetor_string[i], tam+2, entrada);

        ordena_m_strings(m, tam, vetor_string);

        //Printa saida
        fprintf(saida, "%d\n",tam);
        for(i = 0; i < m; i++)
            fprintf(saida, "%s",vetor_string[i]);

        //Libera estrutura
        for(i=0; i<m; i++)
            mathias_free(vetor_string[i]);
        mathias_free(vetor_string);

    }//Fim do caso 1
    

    //CASO 2: Não cabem em MP. Rodar intercalação balanceada
    else{
        printf("caso 2\n");   
        f1 = fopen("f1.txt", "r+");
        f2 = fopen("f2.txt", "r+");
        f3 = fopen("f3.txt", "r+");
        f4 = fopen("f4.txt", "r+");
        fitaL1 = f1;
        fitaL2 = f2;
        fitaE = f3;

        if(f1==NULL || f2==NULL || f3==NULL || f4==NULL)
            printf("Nao foi possivel reabrir as fitas!\n");

        stringA = mathias_malloc((tam+1) * sizeof(char));
        stringB = mathias_malloc((tam+1) * sizeof(char));

        //Inicializa controladores de EOF
        a=fscanf(fitaL1,"%[^\n]%*c", stringA);
        b=fscanf(fitaL2,"%[^\n]%*c", stringB);


        //INTERCALAÇÃO COMPLETA 
        for (i=0; tamBloco<=totalStrings; i++){

            tamBloco*=2;

            //UMA PASSADA DE INTERCALAÇÃO (até EOF)
            while (a!=EOF || b!= EOF){

                //Calcula controlador de qts strings falta tratar de cada bloco
                faltaEscrever1=faltaEscrever2=tamBloco/2;           

                //Se acabar strings de uma das fitas, desce todas as strings da outra fita
                if(a==EOF){                                         
                    while(b!=EOF){                                  
                        fprintf(fitaE, "%s\n", stringB);
                        b = fscanf(fitaL2, "%[^\n]%*c", stringB);
                    }
                    break;
                }
                if(b==EOF){
                    while(a!=EOF){
                        fprintf(fitaE, "%s\n", stringA);
                        a = fscanf(fitaL1, "%[^\n]%*c", stringA);
                    }
                    break;
                }

                //INTERCALAÇÃO DE DOIS BLOCOS (até zerar cada bloco)
                while (faltaEscrever1 != 0 || faltaEscrever2 != 0){

                    menor=strcmp(stringA, stringB);

                    //Desce a menor string,
                    //Sobe a proxima string da mesma fita de origem da string que desceu
                    // e atualiza controlador de EOF
                    if(menor<0){                                     
                        fprintf(fitaE, "%s\n", stringA);
                        faltaEscrever1--;
                        a = fscanf(fitaL1, "%[^\n]%*c", stringA);
                    }else{
                        fprintf(fitaE, "%s\n", stringB);
                        faltaEscrever2--;
                        b = fscanf(fitaL2, "%[^\n]%*c", stringB);
                    }

                    //Se acabarem as chaves do bloco ou acabar o arquivo inteiro
                    //Desce todas a chaves restantes do bloco oposto
                    if (faltaEscrever1 == 0 || a == EOF){            
                        while (faltaEscrever2 != 0 && b !=EOF){                
                            fprintf(fitaE, "%s\n", stringB);
                            faltaEscrever2--;
                            b = fscanf(fitaL2, "%[^\n]%*c", stringB);
                        }
                        faltaEscrever1 = faltaEscrever2 = 0;
                    } else if (faltaEscrever2 == 0 || b == EOF){
                        while (faltaEscrever1 != 0 && a != EOF){
                            fprintf(fitaE, "%s\n", stringA);
                            faltaEscrever1--;
                            a = fscanf(fitaL1, "%[^\n]%*c", stringA);
                        }
                        faltaEscrever1 = faltaEscrever2 = 0;
                    }
                } //FIM DA INTERCALAÇÃO DE DOIS BLOCOS (flag: zerou faltaEscrever)

                //Troca a fita de escrita para escrever o próximo bloco intercalado
                if (fitaE==f3){
                    fitaE = f4;
                }else if(fitaE==f4){
                    fitaE = f3;
                }else if (fitaE==f1){
                    fitaE = f2;
                }else if(fitaE==f2){
                    fitaE = f1;
                }
            }
            //FIM DE UMA PASSADA COMPLETA (até EOF)
            nPassadas++;

            //RE-INICIALIZANDO MARCADORES

            //1. limpa os arquivos que eram de leitura, pois serão de escrita; 
            //  rebobina as fitas de escrita, pois serão de leitura;
            //  troca quem é fita de LEITURA/ESCRITA.
            if(fitaL1==f1){
                fitaL1 = freopen("f1.txt", "w+", fitaL1);
                fitaL2 = freopen("f2.txt", "w+", fitaL2);
                rewind(f3);
                rewind(f4);
                fitaL1 = f3;
                fitaL2 = f4;
                fitaE = f1;
            } else if(fitaL1==f3) {
                fitaL1 = freopen("f3.txt", "w+", fitaL1);
                fitaL2 = freopen("f4.txt", "w+", fitaL2);
                rewind(f1);
                rewind(f2);
                fitaL1 = f1;
                fitaL2 = f2;
                fitaE = f3;
            }
            
            //2. Atualiza tamanho inicial do bloco
            faltaEscrever1*=2;
            faltaEscrever2*=2;

            //3. Coleta as primeiras strings das novas fitas trocadas (que eram de escrita)
            //  Atualiza controladores de EOF
            a=fscanf(fitaL1,"%[^\n]%*c", stringA);
            b=fscanf(fitaL2,"%[^\n]%*c", stringB);    
            
        } //FIM DA ORDENAÇÃO


        //Ao final de ordenação, identifica em qual fita de saida estão as strings ordenadas
        // e direciona para a fita de saída
        a=1;

        if(nPassadas%2==1){
            fprintf(saida, "%d\n", tam);
            rewind(f3);
            a = fscanf(f3, "%[^\n]%*c", stringA);
            while(a!=EOF){
                fprintf(saida, "%s\n", stringA);
                a = fscanf(f3, "%[^\n]%*c", stringA);
            }
        } else { 
            fprintf(saida, "%d\n", tam);
            rewind(f1);
            a = fscanf(f1, "%[^\n]%*c", stringA);
            while(a!=EOF){
                fprintf(saida, "%s\n", stringA);
                a = fscanf(f1, "%[^\n]%*c", stringA);
            }
        }

        fclose(entrada);
        
        fclose(f1);
        fclose(f2);
        fclose(f3);
        fclose(f4);
            mathias_free(stringA);
        mathias_free(stringB);  
        printf("nPassadas%d\n", nPassadas);
    }//Fim do caso 2


    remove("f1.txt");
    remove("f2.txt");
    remove("f3.txt");
    remove("f4.txt");

    fclose(saida);

}
