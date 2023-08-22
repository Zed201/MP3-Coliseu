#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
        int linha, coluna;
        // Nome
        char medic[50];
} LocalM;

typedef struct{
        int qtdM;
        char medic[50];
} TMed;

typedef struct 
{
        int qtd;
        TMed *Pedido;
} pedido;


LocalM *carregaConfig(int *tamC){
        // Ler o arquivo de configuracoes que vai dizer onde cada medicamente vai estar
        FILE *arq;
        LocalM *pont = NULL;
        int qtdMed = 0;
        arq = fopen("config.txt", "rt");
        while (!feof(arq))
        {
                LocalM *aux = NULL;
                aux = realloc(pont, sizeof(LocalM) * (qtdMed + 1));
                if (aux != NULL)
                {
                        pont = aux;
                } else {
                        printf("Problema de alocacao de memoria\n");
                        exit(1);
                }
                // No caso do txt as configuracoes estarao no formato de "linha", "coluna", "nome"\n
                fscanf("%d, %d, %49[^\n]\n", &pont[qtdMed].linha, &pont[qtdMed].coluna, pont[qtdMed].medic);
                qtdMed++;
        }
        fclose(arq);
        // "Retorna" a quantidade de configuracoes colocadas e o ponteiro com todas elas
        (*tamC) = qtdMed;
        return pont;
        
}
pedido *carregaPedidos(int *tamP){
        // Le todos os pedidos do bino.bin
        FILE *arq;
        pedido pedidos;
        int qtdPedidos = 0;
        arq = fopen("bino.bin", "rb");
        while (!feof(arq))
        {
                // Aux vai ser a quantidade de pedidos que ele vai ler(como se fosse lote), e aux2 e para colocar os pedidos
                // no lugar certo no vetor que vai ser retornado
                TMed *aux_p = NULL;
                int aux = -1, aux2 = -1;
                fread(&aux, sizeof(int), 1, arq);
                aux2 = qtdPedidos;
                qtdPedidos += aux;
                aux_p = realloc(pedidos.Pedido, sizeof(pedido) * qtdPedidos);
                if (aux_p != NULL)
                {
                        pedidos.Pedido = aux_p;
                } else {
                        printf("Problema de alocacao de memoria\n");
                        exit(1);
                }
                
                for (int i = 0; i < aux; i++, aux2++)
                {
                        fread(&pedidos.Pedido[aux2], sizeof(pedido), 1, arq);
                }
                
        }
        (*tamP) = qtdPedidos;
        return &pedidos;
        
}
TMed **inicialica(){
        // Os nomes tipo coli estavam se referindo a coliseu, que era uma struct apenas com int, mas ai eu so troquei algumas partes pois ja estava feito
        TMed **coli = NULL, **aux_p = NULL;
        int aux = 20;
        aux_p = realloc(coli, sizeof(TMed*) * aux);
        if (aux_p != NULL)
        {
                coli = aux_p;
        } else {
                printf("Problema de alocacao de memoria\n");
                exit(1);
        }
        
        for (int i = 0; i < 20; i++)
        {
                // criar uma pseudo piramide
                // alocando sempre 2 espacos a mais que o anterior
                TMed *aux_p2 = NULL;
                coli[i] = realloc(coli[i], sizeof(TMed) * aux);
                if (aux_p2 != NULL)
                {
                        coli[i] = aux_p2;
                } else {
                        printf("Problema de alocacao de memoria\n");
                        exit(1);
                }
                
                for (int z = 0; z < aux; z++)
                {
                        // Zera todos os espacos para colocar 20 apenas nos que tem a config
                        coli[i][z].qtdM = 0;
                }
                aux += 2;
        }
       return coli; 
}
void processaPedido(pedido meuPedido, TMed **estoque, LocalM *config, int tamC){
        FILE *arq;
        arq = fopen("pedidos.txt", "at");
        for (int i = 0; i < meuPedido.qtd; i++)
        {
                // Para cada pedido requisitado ele vai percorrer todos os pedidos seguindo as configs como lastro até achar um com nome correspondente
                // Quando ele achar ele tira a quantidade que e pedida em cada pedido
                for (int z = 0; z < tamC; z++)
                { // Roda todos as configs
                        if (strcmp(meuPedido.Pedido[i].medic, config[z].medic))
                        { // Achou o medicamente certo
                                fprintf(arq, "Separei %d unidades do medicamento %s\n", meuPedido.Pedido[i].qtdM, meuPedido.Pedido[i].medic);
                                // Pode dar uma invsao de memoria aqui, se ele tentar acessar algum local que nao pode
                                int qtdColunas[20] ={20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58};
                                if (qtdColunas[config[z].linha - 1] >= config[z].coluna)
                                { // ele so vai tirar medicamento se a posicao da coluna que ele ta pegando for valida com base na quantidade de colunas que determinada linha tem
                                        estoque[config[z].linha][config[z].coluna].qtdM -= meuPedido.Pedido[i].qtdM;
                                }
                        }
                }
        }   
        fclose(arq);
}
void processaPedidos(pedido *todosPedidos, int tamP, TMed **estoque, LocalM *config, int tamC){
        // So abre o arquivo para colocar todos os ++++++... e depois chama a funcao que vai "computar os pedidos", mas antes da fclose no arquivo
        FILE* arq;
        arq = fopen("pedidos.txt", "a");
        for (int i = 0; i < 70; i++)
        {
                fprintf(arq, "+");
        }
        fprintf(arq, "\n");
        fclose(arq);
        // Para cada pedio ele do vetor de todos os pedidos ele chama a funcao de processar/computar uma vez
        // passando o pedido em si, o estoque, as configs e o tamanho das configs
        for (int i = 0; i < tamP; i++)
        {
                processaPedido(todosPedidos[i], estoque, config, tamC);
        }
        // Desalocar todos os pedidos
        for (int i = 0; i < tamP; i++)
        {
                free(todosPedidos[i].Pedido);
        }
        free(todosPedidos);
        for (int i = 0; i < 20; i++)
        { // Desaloca apenas os que foram usados, no caso os que foram colocados medicamentos, usando config
                free(estoque[i]);
        }
        // Da free no estoque greal e depois nas configs
        free(estoque);
        free(config);
}

int main(){
        TMed **Coliseuzao = NULL;
        Coliseuzao = inicialica();
        int TamC = 0;
        LocalM *Configuracoes = carregaConfig(&TamC);
        // Quantidades de colunas de cada linha, no caso de cada linha + 1
        int qtdColunas[20] ={20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58};
        // percorrendo todas as configs e colocando 20 apenas nos respectivos, e colocando o nome tambem
        for (int i = 0; i < TamC; i++)
        {
                                // So repretindo o mesmo sistema de "segurança" que tem no ProcessaPedido
                                if (qtdColunas[Configuracoes[i].linha - 1] >= Configuracoes[i].coluna)
                                { // ele so vai tirar medicamento se a posicao da coluna que ele ta pegando for valida com base na quantidade de colunas que determinada linha tem
                                        Coliseuzao[Configuracoes[i].linha][Configuracoes[i].coluna].qtdM = 20;
                                        strcpy(Coliseuzao[Configuracoes[i].linha][Configuracoes[i].coluna].medic, Configuracoes[i].medic);
                                }
        }
        int tamP = 0;
        pedido *TodosPedidos = carregaPedidos(&tamP);
        processaPedidos(TodosPedidos, tamP, Coliseuzao, Configuracoes, TamC);
        return 0;
}