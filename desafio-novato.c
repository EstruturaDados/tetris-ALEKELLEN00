#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- 1. Definição da Estrutura de Dados (Peça e Fila) ---

// Capacidade fixa da fila, conforme a instrução do desafio
#define CAPACIDADE 5

/**
 * @struct Peca
 * Representa uma peça do jogo.
 */
typedef struct
{
    int id;       // Identificador único da peça
    char nome[2]; // Tipo da peça (ex: "I", "O", "T", etc.)
} Peca;

/**
 * @struct FilaCircular
 * Implementa a Fila Circular de peças futuras.
 */
typedef struct
{
    Peca itens[CAPACIDADE]; // Array que armazena as peças
    int frente;             // Índice do primeiro elemento (Dequeue)
    int traseira;           // Índice da próxima posição livre (Enqueue)
    int tamanho;            // Número atual de elementos na fila
} FilaCircular;

// --- 2. Variável e Função de Geração de Peças ---

// Variável estática para garantir IDs únicos
static int proximo_id = 1;

/**
 * Gera uma nova peça com um ID único e um tipo aleatório (I, O, T, L, J, S, Z).
 * @return A nova estrutura Peca gerada.
 */
Peca gerarPeca()
{
    Peca nova_peca;
    nova_peca.id = proximo_id++;

    const char *tipos[] = {"I", "O", "T", "L", "J", "S", "Z"};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);

    // Escolhe um tipo de peça aleatoriamente
    int indice_aleatorio = rand() % num_tipos;

    // Copia o nome da peça para a struct
    strcpy(nova_peca.nome, tipos[indice_aleatorio]);

    return nova_peca;
}

// --- 3. Funções de Manipulação da Fila Circular ---

/**
 * Inicializa a fila.
 */
void inicializarFila(FilaCircular *fila)
{
    fila->frente = 0;
    fila->traseira = 0;
    fila->tamanho = 0;
}

/**
 * Verifica se a fila está cheia.
 */
int filaCheia(const FilaCircular *fila)
{
    return fila->tamanho == CAPACIDADE;
}

/**
 * Verifica se a fila está vazia.
 */
int filaVazia(const FilaCircular *fila)
{
    return fila->tamanho == 0;
}

/**
 * Insere uma peça no final da fila (Enqueue).
 * Usa o operador módulo (%) para circular o array.
 */
int enfileirar(FilaCircular *fila, Peca peca)
{
    if (filaCheia(fila))
    {
        printf("⚠️ Erro: Fila cheia! Não é possível inserir.\n");
        return 0;
    }

    fila->itens[fila->traseira] = peca;
    // Move a traseira para o próximo índice de forma circular
    fila->traseira = (fila->traseira + 1) % CAPACIDADE;
    fila->tamanho++;

    return 1;
}

/**
 * Remove e retorna a peça da frente da fila (Dequeue).
 * Usa o operador módulo (%) para circular o array.
 */
Peca desenfileirar(FilaCircular *fila)
{
    if (filaVazia(fila))
    {
        printf("⚠️ Erro: Fila vazia! Nenhuma peça para jogar.\n");
        Peca peca_erro = {-1, "XX"};
        return peca_erro;
    }

    Peca peca_removida = fila->itens[fila->frente];

    // Move a frente para o próximo índice de forma circular
    fila->frente = (fila->frente + 1) % CAPACIDADE;
    fila->tamanho--;

    return peca_removida;
}

/**
 * Exibe o estado atual da fila, mostrando a ordem da frente para a traseira.
 */
void visualizarFila(const FilaCircular *fila)
{
    printf("\n  Fila de Peças Futuras (Tamanho: %d/%d)  \n", fila->tamanho, CAPACIDADE);

    if (filaVazia(fila))
    {
        printf("A fila está vazia.\n");
        return;
    }

    int i = fila->frente;
    int count = 0;
    printf("  Frente (Próxima) -> ");

    // Percorre a fila desde a "frente" até o último elemento inserido
    while (count < fila->tamanho)
    {
        printf("[ID:%d|%s]", fila->itens[i].id, fila->itens[i].nome);

        if (count < fila->tamanho - 1)
        {
            printf(" -> ");
        }

        i = (i + 1) % CAPACIDADE; // Passa para o próximo índice
        count++;
    }

    printf(" <- Traseira\n");
    printf("---------------------------------------------------\n");
}

/**
 * Preenche a fila circular com 5 peças iniciais para iniciar o jogo.
 */
void inicializarFilaAutomatica(FilaCircular *fila)
{
    for (int i = 0; i < CAPACIDADE; i++)
    {
        Peca nova = gerarPeca();
        enfileirar(fila, nova);
    }
}

// --- 4. Função Principal (main) e Menu de Execução ---

int main()
{
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    FilaCircular fila_futuras;
    int opcao;

    inicializarFila(&fila_futuras);
    inicializarFilaAutomatica(&fila_futuras); // Preenche a fila com 5 peças

    printf("✨ Bem-vindo ao Tetris Stack: Nível Novato! ✨\n");
    printf("Sistema de Fila Circular de Peças Futuras Inicializado.\n");
    visualizarFila(&fila_futuras); // Exibe o estado inicial

    do
    {
        printf("\n--- 🎮 Menu de Ações ---\n");
        printf("\n1. Jogar a Próxima Peça (Dequeue + Novo Enqueue)\n");
        printf("2. Visualizar Fila Atual\n");
        printf("0. Sair do Programa\n");
        printf("\nEscolha uma opção: ");

        if (scanf("%d", &opcao) != 1)
        {
            printf("🚫 Entrada inválida. Por favor, digite um número.\n");
            while (getchar() != '\n')
                ; // Limpa o buffer
            opcao = -1;
            continue;
        }

        switch (opcao)
        {
        case 1:
        {
            if (!filaVazia(&fila_futuras))
            {
                // 1. Jogar (Remove a Peça da Frente)
                Peca peca_jogada = desenfileirar(&fila_futuras);
                printf("\n🚀 Peça Jogada: [ID:%d|%s].\n", peca_jogada.id, peca_jogada.nome);

                // 2. Inserir (Adiciona uma Nova Peça na Traseira)
                Peca nova_peca = gerarPeca();
                enfileirar(&fila_futuras, nova_peca);
                printf("➕ Nova Peça Inserida: [ID:%d|%s].\n", nova_peca.id, nova_peca.nome);

                // 3. Visualiza o estado atualizado
                visualizarFila(&fila_futuras);
            }
            else
            {
                printf("A fila está vazia! Não há peças para jogar.\n");
            }
            break;
        }
        case 2:
        {
            visualizarFila(&fila_futuras);
            break;
        }
        case 0:
        {
            printf("\n👋 Desafio Novato Concluído! Encerrando o programa.\n");
            break;
        }
        default:
        {
            printf("\n❌ Opção inválida. Tente novamente.\n");
            break;
        }
        }
    } while (opcao != 0);

    return 0;
}