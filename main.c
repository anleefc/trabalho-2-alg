#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int id;
  char *titulo;
  char *autor;
} Livro;
void lerTitulo(FILE *arq, char **titulo);
void lerAutor(FILE *arq, char **autor);

int main(void) {
  int n;
  FILE *arq;
  Livro *livro;
  int tamTitulo, tamAutor;
  char delimitadorTitulo = '|';
  char delimitador = '*';//indica o fim de um livro
  char buffer[100];

  scanf("%d", &n);

  arq = fopen("livros.bin", "wb+");
  if (arq == NULL) {
    printf("erro");
    exit(0);
  } else {
    livro = (Livro *)malloc(sizeof(Livro) * n);

    // leitura da entrada e escrita no arquivo
    for (int i = 0; i < n; i++) {

      // id
      scanf("%d", &livro[i].id);
      fwrite(&livro[i].id, sizeof(int), 1, arq); 

      // título
      scanf(" %[^(\r|\n)]", buffer);
      tamTitulo = strlen(buffer);
      livro[i].titulo = malloc(sizeof(char) * (tamTitulo + 1));
      strcpy(livro[i].titulo, buffer);
      fwrite(livro[i].titulo, sizeof(char) * tamTitulo, 1, arq); //escreve titulo no arquivo
      fwrite(&delimitadorTitulo, sizeof(char), 1, arq); //escreve delimitador de titulo no arquvo

      // autor
      scanf(" %[^(\r|\n)]", buffer);
      tamAutor = strlen(buffer);
      livro[i].autor = malloc(sizeof(char) * (tamAutor + 1));
      strcpy(livro[i].autor, buffer);
      fwrite(&tamAutor, sizeof(int), 1, arq); // escreve tamanho do autor no arquivo
      fwrite(livro[i].autor, sizeof(char) * tamAutor, 1, arq); // escreve autor no arquivo
      fwrite(&delimitador, sizeof(char), 1, arq); // escreve delimitador no arquivo
      free(livro[i].titulo);
      free(livro[i].autor);
    }
    free(livro);
    fclose(arq);
    
    arq = fopen("livros.bin", "rb"); // abre para leitura
    if (arq == NULL) {
      printf("erro");
      exit(0);
    } else {
      livro = (Livro *)malloc(sizeof(Livro) * n);

      int m, x;
      scanf("%d", &m);
      x = n - m;

      // percorre o arquivo buscando "x" delimitadores
      int count = 0;
      int charAtual;
      if(x != 0){
        while((charAtual = fgetc(arq)) != EOF){  
          if(charAtual == delimitador){
            count++;
            fseek(arq, 4, SEEK_CUR);// pula 4 bytes(referentes ao tamAutor) caso o caracter "*" já seja utilizado no domínio dos dados
          }
          if(count == x)
            break;
        }
      }

      fseek(arq, -4, SEEK_CUR);//volta os 4 bytes pulados anteriormente
      for (int i = 0; i < m; i++) {
  
        int byte_offset = ftell(arq);
        
        fread(&livro[i].id, sizeof(int), 1, arq); // leitura do id
        printf ("Id: %d\n", livro[i].id);
        
        lerTitulo(arq, &livro[i].titulo); // leitura do titulo
        printf ("Titulo: %s\n", livro[i].titulo);
        
        lerAutor(arq, &livro[i].autor); //leitura do autor       
        printf ("Autor: %s\n", livro[i].autor);
        
        fseek(arq, 1, SEEK_CUR);//posiciona na próxima posição pra leitura livro seguinte

        printf("Byte offset: %d\n", byte_offset);
        if(i < (m-1)) printf("\n");
        
        free(livro[i].titulo);
        free(livro[i].autor);
      }
    }

    free(livro);
    fclose(arq);
  }
}
void lerTitulo(FILE *arq, char **titulo) {
  char buffer[100];
  int charAtual;
  int i = 0;

  //lê o título armazendando caracter por caracter no variável "buffer"
  while((charAtual = fgetc(arq)) != EOF && charAtual != '|'){
    buffer[i++] = charAtual;
  }
    buffer[i] = '\0';
    *titulo = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(*titulo, buffer);//copia o conteúdo de "buffer" para "*titulo"
}

void lerAutor(FILE *arq, char **autor) {
  int tamAutor;
  
  fread(&tamAutor, sizeof(int), 1, arq); //lê o tamanho do autor em caracteres
  *autor = (char *)malloc(sizeof(char) * (tamAutor + 1));
  fread(*autor, sizeof(char) * tamAutor, 1, arq);//lê  o autor com base no tamanho "tamAutor"
  (*autor)[tamAutor] = '\0'; // adiciona caractere "null" ao fim da string
}
