#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PROCESSES 100

typedef struct {
    int id;
    char command[256];
    int dependencies[MAX_PROCESSES];
    int num_dependencies;
} Process;

typedef struct {
    int core_id;
    int process_id;
    pid_t pid;
} Core;

void read_input_file(const char *filename, Process processes[], int *num_processes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int id, dep;
    while (fgets(line, sizeof(line), file)) {
        Process *p = &processes[*num_processes];
        char dep_str[256];
        sscanf(line, "%d %s %s", &id, p->command, dep_str);
        p->id = id;
        p->num_dependencies = 0;
        
        char *token = strtok(dep_str, ",#");
        while (token) {
            dep = atoi(token);
            if (dep != 0) {
                p->dependencies[p->num_dependencies++] = dep;
            }
            token = strtok(NULL, ",#");
        }
        (*num_processes)++;
    }
    fclose(file);
}

int dependencies_completed(Process processes[], int num_processes, int completed[], int process_index) {
    for (int i = 0; i < processes[process_index].num_dependencies; i++) {
        int dep = processes[process_index].dependencies[i];
        int found = 0;
        for (int j = 0; j < num_processes; j++) {
            if (completed[j] == dep) {
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

void schedule_processes(Process processes[], int num_processes, int num_cores) {
    Core cores[num_cores];
    for (int i = 0; i < num_cores; i++) {
        cores[i].core_id = i;
        cores[i].process_id = -1;
        cores[i].pid = -1;
    }

    int completed[MAX_PROCESSES] = {0};
    int completed_count = 0;
    int start_time = time(NULL);

    while (completed_count < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].id != -1 && dependencies_completed(processes, num_processes, completed, i)) {
                for (int j = 0; j < num_cores; j++) {
                    if (cores[j].process_id == -1) {
                        cores[j].process_id = processes[i].id;
                        if ((cores[j].pid = fork()) == 0) {
                            printf("Processo %d executando\n", processes[i].id);
                            execlp(processes[i].command, processes[i].command, NULL);
                            perror("execlp");
                            exit(EXIT_FAILURE);
                        }
                        processes[i].id = -1;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < num_cores; i++) {
            if (cores[i].process_id != -1 && waitpid(cores[i].pid, NULL, WNOHANG) > 0) {
                completed[completed_count++] = cores[i].process_id;
                cores[i].process_id = -1;
                cores[i].pid = -1;
            }
        }
    }

    int end_time = time(NULL);
    printf("Makespan: %d segundos\n", end_time - start_time);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero_de_cores> <arquivo_de_entrada>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_cores = atoi(argv[1]);
    char *input_file = argv[2];

    Process processes[MAX_PROCESSES];
    int num_processes = 0;

    read_input_file(input_file, processes, &num_processes);
    schedule_processes(processes, num_processes, num_cores);

    return 0;
}
