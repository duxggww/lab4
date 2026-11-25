#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>

typedef struct Node {
    int data;
    int level;
    struct Node* left;
    struct Node* right;
} Node;

Node* search(Node* root, int value) {
    if (root == NULL || root->data == value)
        return root;

    if (value < root->data)
        return search(root->left, value);
    return search(root->right, value);
}

int get_height(Node* root) {
    if (root == NULL) return 0;
    int left_h = get_height(root->left);
    int right_h = get_height(root->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

Node* insert(Node* root, int value, int level, int allow_duplicates, int* max_level_error) {
    if (root == NULL) {
        if (level > 10) {
            *max_level_error = 1;
            return NULL;
        }
        Node* node = (Node*)malloc(sizeof(Node));
        if (node == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(EXIT_FAILURE);
        }
        node->data = value;
        node->level = level;
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    if (value < root->data) {
        Node* new_node = insert(root->left, value, level + 1, allow_duplicates, max_level_error);
        if (new_node != NULL || !(*max_level_error)) {
            root->left = new_node;
        }
    }
    else if (value > root->data) {
        Node* new_node = insert(root->right, value, level + 1, allow_duplicates, max_level_error);
        if (new_node != NULL || !(*max_level_error)) {
            root->right = new_node;
        }
    }
    else if (value == root->data && allow_duplicates) {
        Node* new_node = insert(root->right, value, level + 1, allow_duplicates, max_level_error);
        if (new_node != NULL || !(*max_level_error)) {
            root->right = new_node;
        }
    }

    return root;
}

int count_occurrences(Node* root, int value) {
    if (root == NULL)
        return 0;

    return (root->data == value)
        + count_occurrences(root->left, value)
        + count_occurrences(root->right, value);
}

void find_levels(Node* root, int value, int* levels, int* count) {
    if (root == NULL)
        return;

    if (root->data == value) {
        levels[*count] = root->level;
        (*count)++;
    }

    find_levels(root->left, value, levels, count);
    find_levels(root->right, value, levels, count);
}

void print_tree(Node* root, int level, int is_root) {
    int i;
    if (root == NULL)
        return;

    print_tree(root->right, level + 1, 0);

    for (i = 0; i < level; ++i)
        printf("    ");

    if (is_root) {
        printf("[%d] (уровень %d)\n", root->data, root->level);
    }
    else {
        printf("%d (уровень %d)\n", root->data, root->level);
    }

    print_tree(root->left, level + 1, 0);
}

void free_tree(Node* root) {
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void) {
    Node* root = NULL;
    int value, target, choice, allow_duplicates;
    Node* found;
    int max_level_error;

    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "");

    printf("Выберите режим работы дерева:\n");
    printf("1. С повторами\n");
    printf("2. Без повторов\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Ошибка ввода. Завершение программы.\n");
        return 1;
    }

    allow_duplicates = (choice == 1) ? 1 : 0;

    printf("\nВведите целые числа для построения дерева (-1 - завершить):\n");
    printf("(Ограничение: не более 10 уровней)\n");

    while (1) {
        printf("Число: ");
        if (scanf("%d", &value) != 1) {
            printf("Ошибка ввода. Завершение программы.\n");
            free_tree(root);
            return 1;
        }
        if (value == -1)
            break;

        if (root == NULL) {
            max_level_error = 0;
            root = insert(root, value, 1, allow_duplicates, &max_level_error);
            if (max_level_error) {
                printf("Ошибка: добавление превысит максимальный уровень (10).\n");
            }
        }
        else {
            Node* existing = search(root, value);
            if (existing != NULL && !allow_duplicates) {
                printf("Значение %d уже есть - повтор не добавлен.\n", value);
            }
            else {
                int current_height = get_height(root);
                int temp_error = 0;

                if (current_height >= 10) {
                    printf("Ошибка: достигнут максимальный уровень дерева (10).\n");
                }
                else {
                    root = insert(root, value, 1, allow_duplicates, &temp_error);
                    if (temp_error) {
                        printf("Ошибка: добавление превысит максимальный уровень (10).\n");
                    }
                }
            }
        }
    }

    printf("\nПостроенное дерево (корень слева, уровни):\n");
    print_tree(root, 0, 1);

    printf("\nВведите значение для поиска: ");
    if (scanf("%d", &target) != 1) {
        printf("Ошибка ввода.\n");
        free_tree(root);
        return 1;
    }

    found = search(root, target);
    if (found != NULL) {
        printf("Поиск: значение %d найдено.\n", target);

        int levels[1000];
        int level_count = 0;
        find_levels(root, target, levels, &level_count);

        printf("Найдено на %d уровнях: ", level_count);
        for (int i = 0; i < level_count; i++) {
            printf("%d", levels[i]);
            if (i < level_count - 1) printf(", ");
        }
        printf("\n");
    }
    else {
        printf("Поиск: значение %d не найдено.\n", target);
    }

    printf("Число вхождений значения %d: %d\n",
        target, count_occurrences(root, target));

    free_tree(root);
    return 0;
}