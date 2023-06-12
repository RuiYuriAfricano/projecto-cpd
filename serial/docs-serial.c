#include "string.h"
#include "list.h"
#define MAX_D 20000
#define MAX_S 1000
#define MAX_C 60

//Estrutura para o armário
typedef struct arm {
    float medias[MAX_S];
    List * docs;
} TipoArmario;

//Declaração das funções
void lerEntrada(char *fileName);
void atribuirDocs();
void calcularMedia();
void moverDocumento();//Dentro desta função vamos calcular as distâncias
void inicializarVetor(float * vet);
void gravarSaida();


//Variaveis globais
int C = 4, D = 10, S = 3;
float docs[MAX_D][MAX_S];
TipoArmario Cabinets[MAX_C];
//Função principal
int main(int argc, char **argv){
		
	int num_armarios = -1;

	if (argc < 2) {
	    printf("Erro: nome do arquivo de entrada não especificado.\n");
	    return 1;
	}
	
	// abre o arquivo de entrada
	//Chamar funcao lerEntrada()
	lerEntrada(argv[1]);
	
	// verifica se foi passado o número de armários como argumento
	if (argc >= 3) {
	    num_armarios = atoi(argv[2]);
	    C = num_armarios;
	}	
	
	int k;
	//inicializar as listas
	for(k = 0; k < C; k++){
		Cabinets[k].docs = create_list();
	}
	
	//Chamar funcao atribuirDocs()
	atribuirDocs();
	//
	//chamar funcao calcularMedia()
	calcularMedia();
	
	//chamar funcao moverDocumento
	moverDocumento();

	
	//chamar mostrar saída
	gravarSaida();
	
	printf("\n");
	//eliminar as listas
	for(k = 0; k < C; k++){
		free_list(Cabinets[k].docs);
	}
	system("pause");
	return 0;
}

//Implementação das funções

//Funcao a ser chamada inicialmente para atribuir os docs aos armarios respectivos
void atribuirDocs(){
		int i, contarArmario = 0;
		
		for(i = 0; i < D; i++){
			int *ptr = (int*)malloc(sizeof(int));
			*ptr = i;
			add_to_list(Cabinets[i%C].docs, ptr);
		}
}

//Funcao para calcular média, de cada assunto do armarios, com base nos documentos existentes neles
void calcularMedia(){
	
	int k, i, j, l;//i-itera as linhas, j-itera as colunas, k-conta armarios, l-conta as médias
	float soma = 0.0;
	
	for(k = 0; k < C; k++){
		l = 0;
		inicializarVetor(Cabinets[k].medias);
		for(j = 0; j < S; j++){
				soma = 0.0;
				for(i = 0; i < D; i++){
					if(search_in_list(Cabinets[k].docs, i) > -1){
					soma += docs[i][j];
					}
				}
				Cabinets[k].medias[l] = soma/Cabinets[k].docs->size;
				l++;
			}
			putchar('\n');
		}
}


//Essa funcao é para varrer sempre o vetor dos armarios que guarda as médias. como as médias variam com base nos documentos, daí a necessidade de varrer sempre que variar.
void inicializarVetor(float * vet){
	int i;
	for(i = 0; i < S; i++){
		*(vet+i) = -1.0;
	}
}

void moverDocumento(){
	int i, j, k, guardaK;
	float d, q, menorDistancia;
	int mudanca = 1;
	
	do{
	
		//Em qual posicao k devo colocar o i?
		int contNaoMudanca = 0;
		for(i = 0; i < D; i++){//Itera os documentos
			guardaK = -1;
			//Cálculo e comparação das distância 
			for(k = 0; k < C; k++){//itera os armarios
				d = 0.0;
					for(j = 0; j < S; j++){//itera os assuntos
						q = pow((docs[i][j] - Cabinets[k].medias[j]), 2);
						d += q;
					}
					
					//guarda a maior distancia e ao armario respectivo
					if(k == 0){
						menorDistancia = d;
						guardaK = 0;
					}else if (d < menorDistancia){
						menorDistancia = d;
						guardaK = k;
					}
				}
					//procurar o armario em que o documento a ser movido estava e removê-lo de lá
					int m, x = 0;
					for(m = 0; m < C; m++){
						if(search_in_list(Cabinets[m].docs, i) > -1 && m != guardaK){
							Cabinets[m].docs = remove_in_list(Cabinets[m].docs, i);
							x = 1; //tem armário antigo diferente do armário a ser movido
						}
					}
				
				if(guardaK > -1 && x == 1){
					
					int *ptr = (int*)malloc(sizeof(int));
					*ptr = i;
					//Move para o outro armário
					add_to_list(Cabinets[guardaK].docs, ptr);
				}else{
					contNaoMudanca++;	
				}
				
				
			}
			
			if(contNaoMudanca == D) 
				mudanca = 0;
			else
				calcularMedia(); //Recalcula as médias em todos armarios
			
			
	}while(mudanca);
}

//Ler ficheiro de entrada

void lerEntrada(char *fileName) {
    FILE *arquivo;
    char linha[1000];
    char *token;
	int contLinha = 0;
    arquivo = fopen(fileName, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir ficheiro.\n");
        return;
    }
	
    while (fgets(linha, 1000, arquivo) != NULL) {
        
        token = strtok(linha, " ");//split em C
        int contColuna = 0;
   		while (token != NULL) {
   			
   			if(contLinha == 0 && contColuna == 0){
	   			C = atoi(token);	//atoi converte	string para inteiro		
			}
			else if(contLinha == 0 && contColuna == 1){
	   			D = atoi(token);
			}
			else if(contLinha == 0 && contColuna == 2){
	   			S = atoi(token);
			}
			
			if(contLinha > 0 && contColuna <= S){
				if(contColuna > 0){
					docs[contLinha-1][contColuna-1] = atof(token);
				}
				
			}
			
			contColuna++;
			token = strtok(NULL, " "); 
    	}
    	contLinha++;
	}
	

    fclose(arquivo);
    return;
}


//Funcao de saída
void gravarSaida(){
	
	FILE *arquivo;
	char str1[10], str2[10];
	int i, k;
	
	printf("**************************Processada a saida no ficheiro*******************************\n");
	arquivo = fopen("docs.out", "w");
	fprintf(arquivo, "%s", "");//limpa o ficheiro
	fclose(arquivo);
	for(i = 0; i < D; i++){
		for(k = 0; k < C; k++){
			if(search_in_list(Cabinets[k].docs, i) > -1){
				arquivo = fopen("docs.out", "a"); // abre o arquivo para adição
				sprintf(str1, "%d", i);
				sprintf(str2, "%d", k);
				strcat(str1, " ");
				strcat(str1, str2);
				fprintf(arquivo, "%s\n", str1); // escreve a nova linha no arquivo
				fclose(arquivo); // fecha o arquivo
			}
				
				
		}
	}
	
}
