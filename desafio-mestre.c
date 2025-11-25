#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- 1. Definições e Estruturas de Dados ---

#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3

/**
 * @struct Peca
 */
typedef struct
{
    int id;
    char nome[2];
} Peca;

/**
 * @struct FilaCircular (Next Queue)
 */
typedef struct
{
    Peca itens[CAPACIDADE_FILA];
    int frente;
    int traseira;
    int tamanho;
} FilaCircular;

/**
 * @struct Pilha (Hold Stack)
 */
typedef struct
{
    Peca itens[CAPACIDADE_PILHA];
    int topo;
} Pilha;

// --- Estrutura e Variáveis Globais de HISTÓRICO (para a função Desfazer) ---

// Tipo de operação que ocorreu
typedef enum
{
    OP_NENHUMA,
    OP_JOGAR,
    OP_RESERVAR
    // Para simplificar, Desfazer focará apenas nas operações que alteram ambas
} TipoOperacao;

// Variáveis para salvar o estado da última operação JOGAR ou RESERVAR
TipoOperacao ultima_operacao = OP_NENHUMA;
Peca peca_historico_jogada = {-1, "XX"}; // Peça que foi jogada/reservada
Peca peca_historico_nova = {-1, "XX"};   // Peça nova que foi gerada e inserida

// --- 2. Variável e Função de Geração de Peças ---

static int proximo_id = 1;

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

Peca criarPecaErro()
{
    Peca peca_erro = {-1, "XX"};
    return peca_erro;
}

// --- 3. Funções da FILA CIRCULAR ---

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
        return 0;
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
        return criarPecaErro();
    }
    Peca peca_removida = fila->itens[fila->frente];
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->tamanho--;
    return peca_removida;
}

// Obtém a peça da frente sem remover
Peca espiarFila(const FilaCircular *fila)
{
    if (filaVazia(fila))
    {
        return criarPecaErro();
    }
    return fila->itens[fila->frente];
}

// Obtém o índice anterior à frente (para desfazer)
int obterIndiceAnteriorFrente(const FilaCircular *fila)
{
    return (fila->frente - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
}

// Obtém o índice anterior à traseira (última peça inserida)
int obterIndiceAnteriorTraseira(const FilaCircular *fila)
{
    return (fila->traseira - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
}

// --- 4. Funções da PILHA LINEAR ---

void inicializarPilha(Pilha *pilha)
{
    pilha->topo = -1;
}

int pilhaCheia(const Pilha *pilha)
{
    return pilha->topo == CAPACIDADE_PILHA - 1;
}

int pilhaVazia(const Pilha *pilha)
{
    return pilha->topo == -1;
}

int empilhar(Pilha *pilha, Peca peca)
{
    if (pilhaCheia(pilha))
    {
        return 0;
    }
    pilha->topo++;
    pilha->itens[pilha->topo] = peca;
    return 1;
}

Peca desempilhar(Pilha *pilha)
{
    if (pilhaVazia(pilha))
    {
        return criarPecaErro();
    }
    Peca peca_removida = pilha->itens[pilha->topo];
    pilha->topo--;
    return peca_removida;
}

// Obtém a peça do topo sem remover
Peca espiarPilha(const Pilha *pilha)
{
    if (pilhaVazia(pilha))
    {
        return criarPecaErro();
    }
    return pilha->itens[pilha->topo];
}

// --- 5. Funções de Visualização e Inicialização ---

void visualizarFila(const FilaCircular *fila)
{
    printf("\n Fila de Peças Futuras (Tamanho: %d/%d) \n", fila->tamanho, CAPACIDADE_FILA);

    if (filaVazia(fila))
    {
        printf(" A fila está vazia.\n");
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

void visualizarPilha(const Pilha *pilha)
{
    printf("\n🔋 Pilha de Reserva (Tamanho: %d/%d) \n", pilha->topo + 1, CAPACIDADE_PILHA);

    if (pilhaVazia(pilha))
    {
        printf(" A pilha de reserva está vazia.\n");
        printf("---------------------------------------------------\n");
        return;
    }

    printf(" Topo (Peça Reservada) -> ");
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

void inicializarFilaAutomatica(FilaCircular *fila)
{
    for (int i = 0; i < CAPACIDADE_FILA; i++)
    {
        Peca nova = gerarPeca();
        enfileirar(fila, nova);
    }
}

// --- 6. Funções Estratégicas do Nível Mestre ---

/**
 * Troca a peça do topo da pilha com a peça da frente da fila.
 */
void trocarPilhaFila(FilaCircular *fila, Pilha *pilha)
{
    Peca peca_fila = espiarFila(fila);
    Peca peca_pilha = espiarPilha(pilha);

    if (peca_fila.id == -1 || peca_pilha.id == -1)
    {
        printf("❌ Erro: Uma das estruturas está vazia. Não é possível trocar.\n");
        return;
    }

    // 1. Remove Logicamnte (Não usa desenfileirar/desempilhar para manter IDs)
    Peca temp_fila = desenfileirar(fila);
    Peca temp_pilha = desempilhar(pilha);

    // 2. Troca os itens
    // Fila recebe o item da Pilha
    enfileirar(fila, temp_pilha);
    // Pilha recebe o item da Fila
    empilhar(pilha, temp_fila);

    // 3. Ajusta os índices para que o item empilhado vá para o topo
    // E o item enfileirado vá para a frente (posição inicial)
    // Isso é complexo na Circular. Vamos simplificar apenas trocando o conteúdo do índice

    // Desfaz o desenfileirar/enfileirar/desempilhar/empilhar, e faz a troca direta
    // Reverter o desenfileirar/enfileirar para realizar a troca direta no array é mais seguro

    // A maneira mais simples e segura:
    // A. Removo os dois
    Peca front = desenfileirar(fila);
    Peca top = desempilhar(pilha);

    // B. Insiro o trocado
    empilhar(pilha, front); // A peça da frente da fila vai para o topo da pilha

    // C. Enfileiro a peça da pilha na fila, mas preciso que ela vá para a posição FRENTE
    // Para simplificar, ela vai para a traseira, mas isso não simula a troca "no lugar"

    // Solução mais limpa: Troca de conteúdo
    // 1. Copia o conteúdo da frente da fila
    Peca temp_peca_fila = fila->itens[fila->frente];

    // 2. Copia o conteúdo do topo da pilha
    Peca temp_peca_pilha = pilha->itens[pilha->topo];

    // 3. Faz a troca
    fila->itens[fila->frente] = temp_peca_pilha;
    pilha->itens[pilha->topo] = temp_peca_fila;

    printf("\n🔄 Troca Realizada:\n");
    printf("   Fila (Frente): [ID:%d|%s] <- Novo\n", peca_pilha.id, peca_pilha.nome);
    printf("   Pilha (Topo): [ID:%d|%s] <- Novo\n", peca_fila.id, peca_fila.nome);

    // Resetar histórico
    ultima_operacao = OP_NENHUMA;
}

/**
 * Tenta reverter a última operação de JOGAR ou RESERVAR.
 * Nota: Implementação simplificada que desfaz o último Enqueue e reverte o Dequeue/PUSH.
 */
void desfazerUltimaJogada(FilaCircular *fila, Pilha *pilha)
{
    if (ultima_operacao == OP_NENHUMA)
    {
        printf("\n❌ Não há nenhuma operação recente (Jogar/Reservar) para desfazer.\n");
        return;
    }

    printf("\n⏪ Desfazendo a última operação (%s)...\n",
           ultima_operacao == OP_JOGAR ? "JOGAR" : "RESERVAR");

    // 1. Desfaz o último ENQUEUE (remoção da peça nova gerada)
    // A última peça inserida é sempre no índice anterior à traseira.
    if (fila->tamanho > 0)
    {
        fila->traseira = obterIndiceAnteriorTraseira(fila); // Volta a traseira
        fila->tamanho--;                                    // Decrementa o tamanho
        printf("   - [ID:%d|%s] (Nova Peça) removida da Traseira da Fila.\n", peca_historico_nova.id, peca_historico_nova.nome);
    }

    // 2. Reverte a peça jogada/reservada
    if (ultima_operacao == OP_JOGAR)
    {
        // Reverte o Dequeue: A peça jogada volta para a frente
        fila->frente = obterIndiceAnteriorFrente(fila); // Volta a frente
        fila->tamanho++;                                // Incrementa o tamanho
        fila->itens[fila->frente] = peca_historico_jogada;
        printf("   - [ID:%d|%s] (Peça Jogada) restaurada na Frente da Fila.\n", peca_historico_jogada.id, peca_historico_jogada.nome);
    }
    else if (ultima_operacao == OP_RESERVAR)
    {
        // Reverte o Dequeue da Fila: A peça reservada volta para a frente da Fila
        fila->frente = obterIndiceAnteriorFrente(fila); // Volta a frente
        fila->tamanho++;
        fila->itens[fila->frente] = peca_historico_jogada;
        printf("   - [ID:%d|%s] (Peça Reservada) restaurada na Frente da Fila.\n", peca_historico_jogada.id, peca_historico_jogada.nome);

        // Reverte o PUSH na Pilha: A peça que entrou na pilha deve ser removida
        if (pilha->topo > -1)
        {
            pilha->topo--;
            printf("   - A peça (PUSH) foi removida da Pilha de Reserva.\n");
        }
    }

    // Resetar o histórico após o desfazer
    ultima_operacao = OP_NENHUMA;
}

/**
 * Inverte o conteúdo da Fila com o conteúdo da Pilha.
 * (A Fila vira Pilha e a Pilha vira Fila)
 */
void inverterFilaComPilha(FilaCircular *fila, Pilha *pilha)
{
    // 1. Cria Pilha temporária para Fila e Fila temporária para Pilha
    Pilha temp_pilha;
    inicializarPilha(&temp_pilha);
    FilaCircular temp_fila;
    inicializarFila(&temp_fila);

    // 2. Fila atual -> Pilha temporária
    // Desenfileira e empilha, invertendo a ordem original da fila
    int original_fila_size = fila->tamanho;
    for (int i = 0; i < original_fila_size; i++)
    {
        Peca p = desenfileirar(fila);
        empilhar(&temp_pilha, p);
    }

    // 3. Pilha atual -> Fila temporária
    // Desempilha e enfileira, preservando a ordem original da pilha (LIFO)
    int original_pilha_size = pilha->topo + 1;
    for (int i = 0; i < original_pilha_size; i++)
    {
        Peca p = desempilhar(pilha);
        enfileirar(&temp_fila, p);
    }

    // 4. Pilha temporária -> Fila original
    // Desempilha e enfileira, invertendo a ordem da pilha temporária para manter a ordem da fila
    for (int i = 0; i < original_fila_size; i++)
    {
        Peca p = desempilhar(&temp_pilha);
        enfileirar(fila, p);
    }

    // 5. Fila temporária -> Pilha original
    // Desenfileira e empilha, invertendo a ordem da fila temporária para voltar à pilha
    for (int i = 0; i < original_pilha_size; i++)
    {
        Peca p = desenfileirar(&temp_fila);
        empilhar(pilha, p);
    }

    printf("\n🔁 Inversão Concluída: O conteúdo da Fila e da Pilha foram trocados.\n");

    // Resetar histórico
    ultima_operacao = OP_NENHUMA;
}

// --- 7. Função Principal (main) e Menu de Execução ---

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

    printf("👑 Bem-vindo ao Tetris Stack: Nível MESTRE! 👑\n");
    printf("Sistema de Integração Total com Estratégia Inicializado.\n");
    visualizarFila(&fila_futuras);
    visualizarPilha(&pilha_reserva);

    do
    {
        printf("\n--- 🧠 Menu Estratégico ---\n");
        printf("\n1. Jogar a Próxima Peça (Dequeue + Novo Enqueue)\n");
        printf("2. Reservar Peça (Dequeue da Fila -> PUSH na Pilha)\n");
        printf("3. Usar Peça Reservada (POP da Pilha)\n");
        printf("4. Trocar Peça (Topo da Pilha <-> Frente da Fila)\n");
        printf("5. Desfazer Última Jogada/Reserva\n");
        printf("6. Inverter Fila com Pilha\n");
        printf("7. Visualizar Ambos\n");
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
                peca_historico_jogada = desenfileirar(&fila_futuras);
                Peca nova_peca = gerarPeca();
                enfileirar(&fila_futuras, nova_peca);
                peca_historico_nova = nova_peca;

                ultima_operacao = OP_JOGAR;
                printf("\n🚀 Peça Jogada: [ID:%d|%s].\n", peca_historico_jogada.id, peca_historico_jogada.nome);
                printf("➕ Nova Peça Inserida na Fila: [ID:%d|%s].\n", nova_peca.id, nova_peca.nome);
            }
            else
            {
                printf("\n❌ Fila vazia! Não é possível jogar.\n");
                ultima_operacao = OP_NENHUMA;
            }
            break;
        }
        case 2:
        { // Reservar (Dequeue -> PUSH)
            if (pilhaCheia(&pilha_reserva))
            {
                printf("\n❌ Pilha de Reserva cheia! Não é possível reservar mais peças.\n");
                ultima_operacao = OP_NENHUMA;
            }
            else if (!filaVazia(&fila_futuras))
            {
                peca_historico_jogada = desenfileirar(&fila_futuras);
                empilhar(&pilha_reserva, peca_historico_jogada);

                Peca nova_peca = gerarPeca();
                enfileirar(&fila_futuras, nova_peca);
                peca_historico_nova = nova_peca;

                ultima_operacao = OP_RESERVAR;
                printf("\n📦 Peça Reservada: [ID:%d|%s] movida da Fila para a Pilha.\n", peca_historico_jogada.id, peca_historico_jogada.nome);
                printf("➕ Nova Peça Inserida na Fila: [ID:%d|%s].\n", nova_peca.id, nova_peca.nome);
            }
            else
            {
                printf("\n❌ Fila vazia! Não há peças para reservar.\n");
                ultima_operacao = OP_NENHUMA;
            }
            break;
        }
        case 3:
        { // Usar Peça Reservada (POP)
            if (!pilhaVazia(&pilha_reserva))
            {
                Peca peca_usada = desempilhar(&pilha_reserva);
                printf("\n✅ Peça Reservada Usada: [ID:%d|%s] removida da Pilha (POP).\n", peca_usada.id, peca_usada.nome);
                // Não gera nova peça nem salva histórico, pois é um uso estratégico.
                ultima_operacao = OP_NENHUMA;
            }
            else
            {
                printf("\n❌ Pilha de Reserva vazia! Nenhuma peça para usar.\n");
            }
            break;
        }
        case 4:
        { // Trocar Peça (Topo da Pilha <-> Frente da Fila)
            trocarPilhaFila(&fila_futuras, &pilha_reserva);
            break;
        }
        case 5:
        { // Desfazer Última Jogada
            desfazerUltimaJogada(&fila_futuras, &pilha_reserva);
            break;
        }
        case 6:
        { // Inverter Fila com Pilha
            inverterFilaComPilha(&fila_futuras, &pilha_reserva);
            break;
        }
        case 7:
        { // Visualizar
            // Já é feito no final do loop, mas permite visualização imediata
            break;
        }
        case 0:
        {
            printf("\n👋 Desafio Mestre Concluído! Encerrando o programa.\n");
            break;
        }
        default:
        {
            printf("\n❌ Opção inválida. Tente novamente.\n");
            break;
        }
        }

        // Exibe ambos após qualquer ação (exceto sair)
        if (opcao != 0)
        {
            visualizarFila(&fila_futuras);
            visualizarPilha(&pilha_reserva);
        }

    } while (opcao != 0);

    return 0;
}