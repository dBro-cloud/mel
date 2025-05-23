#include <stdio.h>
#include <stdlib.h>

#define TAPE_SIZE 30000

//При запуске программа принимает файл с кодом
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Открываем файл с кодом
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Записываем код в память
    fseek(file, 0, SEEK_END);
    long program_size = ftell(file);
    rewind(file);

    char *program = malloc(program_size + 1);
    if (!program) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }
    fread(program, 1, program_size, file);
    program[program_size] = '\0';
    fclose(file);

    // Инициализируем ленту и указатель
    unsigned char tape[TAPE_SIZE] = {0};
    unsigned char *ptr = tape;

    // Выполнение кода
    for (long i = 0; i < program_size; i++) {
        switch (program[i]) {
            case '>':
                if (ptr == tape + TAPE_SIZE - 1) {
                    fprintf(stderr, "Error: pointer moved beyond tape end\n");
                    free(program);
                    return 1;
                }
                ptr++;
                break;
            case '<':
                if (ptr == tape) {
                    fprintf(stderr, "Error: pointer moved before tape start\n");
                    free(program);
                    return 1;
                }
                ptr--;
                break;
            case '+':
                (*ptr)++;
                break;
            case '-':
                (*ptr)--;
                break;
            case 'p':
                putchar(*ptr);
                break;
            case 'i':
                *ptr = (unsigned char)getchar();
                break;
            case '(':
                if (*ptr == 0) {
                    int loop = 1;
                    while (loop > 0) {
                        i++;
                        if (i >= program_size) {
                            fprintf(stderr, "Error: unmatched '('\n");
                            free(program);
                            return 1;
                        }
                        if (program[i] == '(') loop++;
                        else if (program[i] == ')') loop--;
                    }
                }
                break;
            case ')':
                if (*ptr != 0) {
                    // Возвращаемся к соответствующей '('
                    int loop = 1;
                    while (loop > 0) {
                        i--;
                        if (i < 0) {
                            fprintf(stderr, "Error: unmatched ')'\n");
                            free(program);
                            return 1;
                        }
                        if (program[i] == ')') loop++;
                        else if (program[i] == '(') loop--;
                    }
                }
                break;
            default:
                // Игнорируем все остальные символы
                break;
        }
    }

    free(program);
    return 0;
}
