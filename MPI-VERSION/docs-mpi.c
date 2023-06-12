#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include "hash.h"

#define MAX_LINE_SIZE 1000

//Estrutura para o armário
typedef struct arm {
    float *medias;
    HashMap* docs;
} TipoArmario;

//Declaração das funções
void lerEntrada(char *fileName, float*** docs, int * C, int *D, int *S);
void inicializaHash(TipoArmario *Cabinets, int C);
void atribuirDocs(TipoArmario *Cabinets, int D, int C);
void calcularMedia(TipoArmario *Cabinets, float **docs, int D, int S, int C);
void moverDocumento(TipoArmario *Cabinets, int D, int C, int S, float **docs);//Dentro desta função vamos calcular as distâncias
void gravarSaida(char *fileName, TipoArmario *Cabinets, int D, int C);


//Função principal
int main(int argc, char **argv) {
    int num_armarios = -1;

    if (argc < 2) {
        printf("Erro: nome do arquivo de entrada não especificado.\n");
        return 1;
    }

    // Inicio região paralela
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    MPI_Status status;
    int id, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // Variaveis globais
    int C, D, S;

    // Criacao do matriz de assuntos dos documentos
    float **docs = NULL; // Alocando memória para as linhas

    // Criacao do vetor armario
    TipoArmario *Cabinets = NULL;

    // Criação do tipo MPI_TipoArmario
    int blocklengths[2] = {1, 1};
    MPI_Datatype types[2] = {MPI_FLOAT, MPI_BYTE};
    MPI_Aint offsets[2] = {offsetof(TipoArmario, medias), offsetof(TipoArmario, docs)};

    MPI_Datatype MPI_TipoArmario;
    MPI_Type_create_struct(2, blocklengths, offsets, types, &MPI_TipoArmario);
    MPI_Type_commit(&MPI_TipoArmario);

    if (!id) {
        // abre o arquivo de entrada
        // Chamar função lerEntrada()
        lerEntrada(argv[1], &docs, &C, &D, &S);

        // verifica se foi passado o número de armários como argumento
        if (argc >= 3) {
            num_armarios = atoi(argv[2]);
            C = num_armarios;
        }

        // enviar D, C, S
        MPI_Send(&D, 1, MPI_INT, 1, 333, MPI_COMM_WORLD);
        MPI_Send(&C, 1, MPI_INT, 1, 444, MPI_COMM_WORLD);
        MPI_Send(&S, 1, MPI_INT, 1, 555, MPI_COMM_WORLD);

        // enviar docs
        float m[D][S]; // Obtenha um ponteiro para o primeiro elemento da matriz
        
		for (int i = 0; i < D; i++) {
            for (int j = 0; j < S; j++) {
                m[i][j] = docs[i][j];
            }
        }

        MPI_Send(m, D * S, MPI_FLOAT, 1, 111, MPI_COMM_WORLD);

        // liberados os assuntos de cada documento
        for (int i = 0; i < D; i++) {
            free(docs[i]);
        }
        // liberado a matriz documentos
        free(docs);
        double end_time = MPI_Wtime();
        int terminou = 0;
        MPI_Recv(&terminou, 1, MPI_INT, 1, 999, MPI_COMM_WORLD, &status);
        if (terminou)
            printf("\n\nTempo de execucao: %f segundos\n", end_time - start_time);
    }

    if (id == 1) {
        // receber D, C, S
        MPI_Recv(&D, 1, MPI_INT, 0, 333, MPI_COMM_WORLD, &status);
        MPI_Recv(&C, 1, MPI_INT, 0, 444, MPI_COMM_WORLD, &status);
        MPI_Recv(&S, 1, MPI_INT, 0, 555, MPI_COMM_WORLD, &status);

        float m[D][S];
        // receber docs
        MPI_Recv(m, D * S, MPI_FLOAT, 0, 111, MPI_COMM_WORLD, &status);

        // Criacao do vetor armario
        Cabinets = (TipoArmario *)malloc(C * sizeof(TipoArmario)); // Alocando memória inicial;
        int ci;
        // alocar espaços para cada armarios armazenar nas medias de assuntos
        for (ci = 0; ci < C; ci++) {
            Cabinets[ci].medias = (float *)malloc(S * sizeof(float));
        }

        // inicializar as listas
        inicializaHash(Cabinets, C);

        // Alocar memória para a matriz docs no processo 1
        docs = (float **)malloc(D * sizeof(float *));

        for (int i = 0; i < D; i++) {
            docs[i] = (float *)malloc(S * sizeof(float));
            for (int j = 0; j < S; j++) {
                docs[i][j] = m[i][j];
            }
        }

        // Chamar funcao atribuirDocs()
        atribuirDocs(Cabinets, D, C);
        printf("\n**INICIO: ATRIBUIU DOCUMENTOS AOS ARMARIOS**\n");
        // chamar funcao calcularMedia()
        calcularMedia(Cabinets, docs, D, S, C);
        printf("\n**CALCULOU AS MEDIAS INICIALMENTE**\n");
        // chamar funcao moverDocumento
        moverDocumento(Cabinets, D, C, S, docs);
        printf("\n**MOVEU OS DOCUMENTOS PARA OS ARMARIOS RESPECTIVOS**\n");

        gravarSaida(argv[1], Cabinets, D, C);
        printf("\n");

        // eliminar as listas
        for (int k = 0; k < C; k++) {
            free(Cabinets[k].docs);
            free(Cabinets[k].medias);
        }
        // libera o vetor armario
        free(Cabinets);

        // liberados os assuntos de cada documento
        for (int i = 0; i < D; i++) {
            free(docs[i]);
        }
        // liberado a matriz documentos
        free(docs);

        int terminou = 1;
        MPI_Send(&terminou, 1, MPI_INT, 0, 999, MPI_COMM_WORLD);
    }

    // Fim da região paralela
    MPI_Finalize();

    return 0;
}



//Inicializa os hashing do armario
void inicializaHash(TipoArmario *Cabinets, int C){
	int i;
	for(i = 0; i < C; i++){
		Cabinets[i].docs = (HashMap*)malloc(sizeof(HashMap));
		initializeHashMap(Cabinets[i].docs);
	}
}

//Funcao a ser chamada inicialmente para atribuir os docs aos armarios respectivos
void atribuirDocs(TipoArmario *Cabinets, int D, int C){
		int i;
		for(i = 0; i < D; i++){
			insert(Cabinets[i%C].docs, i, i);
		}
}

//Funcao para calcular média, de cada assunto do armarios, com base nos documentos existentes neles
void calcularMedia(TipoArmario *Cabinets, float **docs, int D, int S, int C) {
    int k, i, j;
    
    omp_set_num_threads(4);
    #pragma omp parallel for shared(Cabinets, docs) private(k, i, j) schedule(static)
    for (k = 0; k < C; k++) {
        for (j = 0; j < S; j++) {
            float somaAssunto = 0.0;
            
            for (i = 0; i < D; i++) {
                if (search(Cabinets[k].docs, i) > -1) {
                    somaAssunto += docs[i][j];
                }
            }
            
            Cabinets[k].medias[j] = somaAssunto / Cabinets[k].docs->numOfElements;
        }
    }
}


void moverDocumento(TipoArmario *Cabinets, int D, int C, int S, float **docs) {
    int i, j, k;
    int mudanca = 1;
    
    do {
        int contNaoMudanca = 0;
        int posicoes[D];
        float distancias[D];
        
        for (i = 0; i < D; i++) {
            // Cálculo e armazenamento das distâncias
            
            for (k = 0; k < C; k++) {
                float d = 0.0;
                #pragma omp parallel for reduction(+:d)
                for (j = 0; j < S; j++) {
                    float q = pow((docs[i][j] - Cabinets[k].medias[j]), 2);
                    d += q;
                }
                
                if (k == 0 || d < distancias[i]) {
                    distancias[i] = d;
                    posicoes[i] = k;
                }
            }
            
            int guardaK = posicoes[i];
            int x = 0;
            
            if (search(Cabinets[guardaK].docs, i) == -1) {
                int m;
                #pragma omp parallel for
                for (m = 0; m < C; m++) {
                    int pos = search(Cabinets[m].docs, i);
                    if (pos > -1 && m != guardaK) {
                        delete_(Cabinets[m].docs, i); // remove da lista antiga
                        x = 1;
                    }
                }
            }
            
            if (x == 1) {
                insert(Cabinets[guardaK].docs, i, i);
            } else {
                contNaoMudanca++;
            }
        }
        
        if (contNaoMudanca == D) {
            mudanca = 0;
        } else {
            calcularMedia(Cabinets, docs, D, S, C);
        }
    } while (mudanca);
}



//Ler ficheiro de entrada
void lerEntrada(char *fileName, float*** docs, int * C, int *D, int *S) {
    FILE *arquivo;
    char linha[MAX_LINE_SIZE];
    char *token;
    int contLinha = 0;
    arquivo = fopen(fileName, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir ficheiro.\n");
        return;
    }
	
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        token = strtok(linha, " ");//split em C
        int contColuna = 0;
        while (token != NULL) {
            if (contLinha == 0 && contColuna == 0) {
                *C = atoi(token);	//atoi converte	string para inteiro	
					
            } else if (contLinha == 0 && contColuna == 1) {
                *D = atoi(token);
                //Criacao do matriz de assuntos dos documentos
				*docs = (float **)malloc((*D) * sizeof(float *)); // Alocando memória para as linhas

            } else if (contLinha == 0 && contColuna == 2) {
                *S = atoi(token);
                int ci = 0;
	    		// Alocando memória para as colunas de cada linha
	    		for (ci = 0; ci < (*D); ci++) {
	        		(*docs)[ci] = (float *)malloc((*S) * sizeof(float));
	    		}

            }
			
			
            if (contLinha > 0 && contColuna <= (*S)) {
                if (contColuna > 0) {
                    (*docs)[contLinha-1][contColuna-1] = atof(token);
                }
            }
			
            contColuna++;
            token = strtok(NULL, " "); 
        }
        contLinha++;
    }

    fclose(arquivo);
}


void gravarSaida(char *fileName, TipoArmario *Cabinets, int D, int C) {
    FILE *arquivo;
    char linha[MAX_LINE_SIZE];
    int i, k;

    
    char ext[7] = ".out";
    char outputFileName[60];
    strcpy(outputFileName, fileName); // Create a copy of the original fileName
    for (i = 0; outputFileName[i] !='\0' ; i++) {
		if(outputFileName[i] =='.')
			outputFileName[i] = '\0';
	}
    strcat(outputFileName, ext);

    arquivo = fopen(outputFileName, "w");
    
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo.\n");
        return;
    }

    for (i = 0; i < D; i++) {
        for (k = 0; k < C; k++) {
            if (search(Cabinets[k].docs, i) > -1) {
                snprintf(linha, sizeof(linha), "%d %d\n", i, k);
                fprintf(arquivo, "%s", linha);
            }
        }
    }

    fclose(arquivo);
    printf("\n*******************************************************************************************");
    printf("\n\n\t\t**TERMINADO COM SUCESSO** CONSULTE O DOCUMENTO GERADO: \"%s\"\n\n", outputFileName);
    printf("*******************************************************************************************");
}

