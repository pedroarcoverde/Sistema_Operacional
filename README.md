# Escalonador de workflow de processos

## Explicação do código:
### Estruturas:

- Process: Representa um processo no workflow, contendo seu ID, comando e dependências.
- Core: Representa um núcleo de processamento, contendo o ID do núcleo, ID do processo em execução e o PID do processo.

### Função read_input_file:

- Lê o arquivo de entrada e popula a lista de processos com seus comandos e dependências.

### Função dependencies_completed:

- Verifica se todas as dependências de um processo foram concluídas.

### Função schedule_processes:

- Gerencia a execução dos processos, respeitando o número de núcleos disponíveis e as dependências dos processos.
- Utiliza fork para criar processos filhos que executam os comandos especificados.
- Usa waitpid para monitorar a conclusão dos processos e atualizar a lista de processos concluídos.

### Função main:

- Lê o número de núcleos e o arquivo de entrada a partir dos argumentos da linha de comando.
- Lê os processos do arquivo de entrada e chama a função de escalonamento.


Este código fornece um escalonador básico que executa processos em núcleos disponíveis, respeitando as dependências especificadas. Dependendo dos requisitos adicionais ou melhorias desejadas, o código pode ser expandido e otimizado.
