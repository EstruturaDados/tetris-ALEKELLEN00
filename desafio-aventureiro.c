#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- 1. Definições e Estruturas de Dados ---

// Capacidades fixas, conforme as instruções do desafio
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3

/**
 * @struct Peca
 * Representa uma peça do jogo.
 */
typedef struct
{
    int id;       // Identificador único
    char nome[2]; // Tipo da peça
} Peca;

/**
 * @struct FilaCircular
 * Implementa a Fila Circular de peças futuras (Next Queue).
 */
typedef struct
{
    Peca itens[CAPACIDADE_FILA];
    int frente;
    int traseira;
    int tamanho;
} FilaCircular;

/**
 * @struct Pilha
 * Implementa a Pilha Linear de reserva de peças (Hold Stack).
 */
typedef struct
{
    Peca itens[CAPACIDADE_PILHA];
    int topo; // Índice do último elemento inserido (o topo da pilha)
} Pilha;

// --- 2. Variável e Função de Geração de Peças ---

static int proximo_id = 1;

/**
 * Gera uma nova peça com um ID único e um tipo aleatório.
 */
Peca gerarPeca()
{
    Peca nova_peca;
    nova_peca.id = proximo_id++;

    const char *tipos[] = {"I", "O", "T", "L", "J", "S", "Z"};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);

    int indice_aleatorio = rand() % num_tipos;
    strcpy(nova_peca.nome, tipos[indice_aleatorio]);

    return nova_peca;
}

// --- 3. Funções de Manipulação da FILA CIRCULAR ---

void inicializarFila(FilaCircular *fila)
{
    fila->frente = 0;
    fila->traseira = 0;
    fila->tamanho = 0;
}

int filaCheia(const FilaCircular *fila)
{
    return fila->tamanho == CAPACIDADE_FILA;
}

int filaVazia(const FilaCircular *fila)
{
    return fila->tamanho == 0;
}

int enfileirar(FilaCircular *fila, Peca peca)
{
    if (filaCheia(fila))
    {
        return 0; // Fila cheia
    }

    fila->itens[fila->traseira] = peca;
    fila->traseira = (fila->traseira + 1) % CAPACIDADE_FILA;
    fila->tamanho++;

    return 1;
}

Peca desenfileirar(FilaCircular *fila)
{
    if (filaVazia(fila))
    {
        Peca peca_erro = {-1, "XX"};
        return peca_erro;
    }

    Peca peca_removida = fila->itens[fila->frente];
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->tamanho--;

    return peca_removida;
}

// --- 4. Funções de Manipulação da PILHA LINEAR ---

/**
 * Inicializa a pilha. O topo aponta para -1 (vazio).
 */
void inicializarPilha(Pilha *pilha)
{
    pilha->topo = -1;
}

/**
 * Verifica se a pilha está cheia.
 */
int pilhaCheia(const Pilha *pilha)
{
    return pilha->topo == CAPACIDADE_PILHA - 1;
}

/**
 * Verifica se a pilha está vazia.
 */
int pilhaVazia(const Pilha *pilha)
{
    return pilha->topo == -1;
}

/**
 * Insere um item no topo da pilha (PUSH).
 */
int empilhar(Pilha *pilha, Peca peca)
{
    if (pilhaCheia(pilha))
    {
        printf("⚠️ Erro: Pilha de reserva cheia! Capacidade máxima de %d.\n", CAPACIDADE_PILHA);
        return 0;
    }

    pilha->topo++;                    // Incrementa o topo
    pilha->itens[pilha->topo] = peca; // Insere o item
    return 1;
}

/**
 * Remove e retorna o item do topo da pilha (POP).
 */
Peca desempilhar(Pilha *pilha)
{
    if (pilhaVazia(pilha))
    {
        printf("⚠️ Erro: Pilha de reserva vazia! Nenhuma peça para usar.\n");
        Peca peca_erro = {-1, "XX"};
        return peca_erro;
    }

    Peca peca_removida = pilha->itens[pilha->topo]; // Pega o item do topo
    pilha->topo--;                                  // Decrementa o topo
    return peca_removida;
}

// --- 5. Funções de Visualização ---

/**
 * Exibe o estado atual da fila de peças futuras.
 */
void visualizarFila(const FilaCircular *fila)
{
    printf("\n Fila de Peças Futuras (Tamanho: %d/%d)  \n", fila->tamanho, CAPACIDADE_FILA);

    if (filaVazia(fila))
    {
        printf("A fila está vazia.\n");
        return;
    }

    int i = fila->frente;
    int count = 0;
    printf(" Frente (Próxima) -> ");

    while (count < fila->tamanho)
    {
        printf("[ID:%d|%s]", fila->itens[i].id, fila->itens[i].nome);

        if (count < fila->tamanho - 1)
        {
            printf(" -> ");
        }

        i = (i + 1) % CAPACIDADE_FILA;
        count++;
    }

    printf(" <- Traseira\n");
    printf("---------------------------------------------------\n");
}

/**
 * Exibe o estado atual da pilha de reserva.
 */
void visualizarPilha(const Pilha *pilha)
{
    printf("\n🔋 Pilha de Reserva (Tamanho: %d/%d)  \n", pilha->topo + 1, CAPACIDADE_PILHA);

    if (pilhaVazia(pilha))
    {
        printf(" A pilha de reserva está vazia.\n");
        printf("---------------------------------------------------\n");
        return;
    }

    printf("   Topo (Peça Reservada) -> ");
    // Percorre a pilha de cima para baixo
    for (int i = pilha->topo; i >= 0; i--)
    {
        printf("[ID:%d|%s]", pilha->itens[i].id, pilha->itens[i].nome);

        if (i > 0)
        {
            printf(" | ");
        }
    }
    printf(" <- Base\n");
    printf("---------------------------------------------------\n");
}

/**
 * Preenche a fila circular com 5 peças iniciais.
 */
void inicializarFilaAutomatica(FilaCircular *fila)
{
    for (int i = 0; i < CAPACIDADE_FILA; i++)
    {
        Peca nova = gerarPeca();
        enfileirar(fila, nova);
    }
}

// --- 6. Função Principal (main) e Menu de Execução ---

int main()
{
    srand(time(NULL));

    FilaCircular fila_futuras;
    Pilha pilha_reserva;
    int opcao;

    // Inicialização
    inicializarFila(&fila_futuras);
    inicializarFilaAutomatica(&fila_futuras); // Fila cheia com 5 peças
    inicializarPilha(&pilha_reserva);

    printf("⭐ Bem-vindo ao Tetris Stack: Nível Aventureiro! ⭐\n");
    printf("Sistema de Fila e Pilha Inicializado.\n");
    visualizarFila(&fila_futuras);
    visualizarPilha(&pilha_reserva);

    do
    {
        printf("\n--- 🎮 Menu de Ações ---\n");
        printf("\n1. Jogar a Próxima Peça (Dequeue da Fila + Novo Enqueue)\n");
        printf("2. Reservar Peça (Dequeue da Fila -> PUSH na Pilha)\n");
        printf("3. Usar Peça Reservada (POP da Pilha)\n");
        printf("4. Visualizar Ambos (Fila e Pilha)\n");
        printf("0. Sair do Programa\n");
        printf("\nEscolha uma opção: ");

        if (scanf("%d", &opcao) != 1)
        {
            printf("🚫 Entrada inválida. Por favor, digite um número.\n");
            while (getchar() != '\n')
                ;
            opcao = -1;
            continue;
        }

        switch (opcao)
        {
        case 1:
        { // Jogar (Dequeue e Novo Enqueue)
            if (!filaVazia(&fila_futuras))
            {
                // 1. Dequeue: Remove a peça da frente para jogar
                Peca peca_jogada = desenfileirar(&fila_futuras);
                printf("\n🚀 Peça Jogada: [ID:%d|%s].\n", peca_jogada.id, peca_jogada.nome);

                // 2. Enqueue: Insere automaticamente uma nova peça
                Peca nova_peca = gerarPeca();
                enfileirar(&fila_futuras, nova_peca);
                printf("➕ Nova Peça Inserida na Fila: [ID:%d|%s].\n", nova_peca.id, nova_peca.nome);
            }
            else
            {
                printf("\n❌ Fila vazia! Não é possível jogar. (Isso não deve ocorrer, pois a fila é mantida cheia).\n");
            }
            break;
        }
        case 2:
        { // Reservar (Dequeue -> PUSH)
            if (pilhaCheia(&pilha_reserva))
            {
                printf("\n❌ Pilha de Reserva cheia! Não é possível reservar mais peças.\n");
            }
            else if (!filaVazia(&fila_futuras))
            {
                // 1. Dequeue: Remove a peça da frente da fila
                Peca peca_reservar = desenfileirar(&fila_futuras);

                // 2. PUSH: Empilha na pilha de reserva
                empilhar(&pilha_reserva, peca_reservar);
                printf("\n📦 Peça Reservada: [ID:%d|%s] movida da Fila para a Pilha.\n", peca_reservar.id, peca_reservar.nome);

                // 3. Enqueue: Insere automaticamente uma nova peça na fila
                Peca nova_peca = gerarPeca();
                enfileirar(&fila_futuras, nova_peca);
                printf("➕ Nova Peça Inserida na Fila para manter o fluxo: [ID:%d|%s].\n", nova_peca.id, nova_peca.nome);
            }
            else
            {
                printf("\n❌ Fila vazia! Não há peças para reservar.\n");
            }
            break;
        }
        case 3:
        { // Usar Peça Reservada (POP)
            if (!pilhaVazia(&pilha_reserva))
            {
                // POP: Remove a peça do topo da pilha (Última reservada)
                Peca peca_usada = desempilhar(&pilha_reserva);
                printf("\n✅ Peça Reservada Usada: [ID:%d|%s] removida da Pilha.\n", peca_usada.id, peca_usada.nome);
                printf("A peça removida da pilha é a peça jogada.\n");
            }
            else
            {
                printf("\n❌ Pilha de Reserva vazia! Nenhuma peça para usar.\n");
            }
            break;
        }
        case 4:
        { // Visualizar
            visualizarFila(&fila_futuras);
            visualizarPilha(&pilha_reserva);
            break;
        }
        case 0:
        {
            printf("\n👋 Desafio Aventureiro Concluído! Encerrando o programa.\n");
            break;
        }
        default:
        {
            printf("\n❌ Opção inválida. Tente novamente.\n");
            break;
        }
        }

        // Após qualquer ação que altere o estado, exibe ambos (exceto a opção 4, que já exibe)
        if (opcao != 4 && opcao != 0)
        {
            visualizarFila(&fila_futuras);
            visualizarPilha(&pilha_reserva);
        }

    } while (opcao != 0);

    return 0;
}