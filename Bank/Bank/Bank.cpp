#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int** Max, ** Allocation, ** Need;
int* Available;
int process_nums, types_of_resources;

// 打印资源状态表
void print_matrices() {
    printf("\n资源分配矩阵:\n");
    
    // 打印标题，调整间距
    printf("进程\t最大需求\t\t已占有资源数目\t\t最多还需分配\t\t各资源剩余数目\n");

    // 打印数据行
    for (int i = 0; i < process_nums; i++) {
        printf("P%-2d\t", i); // 进程编号左对齐，宽度为2
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Max[i][j]); // 每个数值占固定宽度4
        }
        printf("\t");
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Allocation[i][j]);
        }
        printf("\t");
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Need[i][j]);
        }
        if (i == 0) { // 打印可用资源，只打印在第一行
            printf("\t");
            for (int j = 0; j < types_of_resources; j++) {
                printf("%-4d", Available[j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}


// 获取用户输入或者随机生成资源数据
void get_data() {
    printf("请输入资源种类数和进程数目: ");
    scanf_s("%d %d", &types_of_resources, &process_nums);

    // 动态分配内存
    Available = (int*)malloc(types_of_resources * sizeof(int));
    Max = (int**)malloc(process_nums * sizeof(int*));
    Allocation = (int**)malloc(process_nums * sizeof(int*));
    Need = (int**)malloc(process_nums * sizeof(int*));
    for (int i = 0; i < process_nums; i++) {
        Max[i] = (int*)malloc(types_of_resources * sizeof(int));
        Allocation[i] = (int*)malloc(types_of_resources * sizeof(int));
        Need[i] = (int*)malloc(types_of_resources * sizeof(int));
    }

    // 输入或随机生成可用资源
    printf("请选择输入资源方式：1. 手动输入 2. 随机生成\n");
    int choice;
    scanf_s("%d", &choice);
    if (choice == 1) {
        printf("请输入当前可用的各资源数目: ");
        for (int i = 0; i < types_of_resources; i++) {
            scanf_s("%d", &Available[i]);
        }
        for (int i = 0; i < process_nums; i++) {
            printf("请输入P%d的最大需求资源数目: ", i);
            for (int j = 0; j < types_of_resources; j++) {
                scanf_s("%d", &Max[i][j]);
            }
        }
        for (int i = 0; i < process_nums; i++) {
            printf("请输入P%d已经分配的资源数目: ", i);
            for (int j = 0; j < types_of_resources; j++) {              
                scanf_s("%d", &Allocation[i][j]);                           
            }
        }
    }
    else if(choice == 2) {
        srand(time(NULL));
        printf("随机生成的当前可用资源数目: ");
        for (int i = 0; i < types_of_resources; i++) {
            Available[i] = rand() % 10 + 1;
            printf("%d ", Available[i]);
        }
        printf("\n");

        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                // 生成 Max 矩阵，保证其值适当大，以便测试资源申请不合理的情况
                Max[i][j] = rand() % 10 + 5;
            }
        }

        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                // 生成 Allocation 矩阵，并确保部分数据能够测试“找不到安全序列”的情况
                Allocation[i][j] = rand() % (Max[i][j] / 2 + 1); // 控制已分配的资源数不会过大
            }
        }

        // 特殊调整
        // 制造一个进程的 Need 大于 Max，测试“资源申请不合理”
        int test_process = rand() % process_nums;
        int test_resource = rand() % types_of_resources;
        Need[test_process][test_resource] = Max[test_process][test_resource] + 1; // 超过最大需求


        // 保持其他数据合理，确保有机会找到安全序列，进行正常分配
        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                Need[i][j] = Max[i][j] - Allocation[i][j];
            }
        }
    }
    else {
        printf("\n输入错误!");
    }  

    // 计算 Need 矩阵
    for (int i = 0; i < process_nums; i++) {
        for (int j = 0; j < types_of_resources; j++) {
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }

    print_matrices();
}

// 判断系统是否处于安全状态
bool is_safe() {
    int* work = (int*)malloc(types_of_resources * sizeof(int));//表示系统可用资源
    bool* finish = (bool*)malloc(process_nums * sizeof(bool));//记录该进程是否完成
    int* safe_sequence = (int*)malloc(process_nums * sizeof(int));//保存安全序列数组
    int count = 0;

    for (int i = 0; i < types_of_resources; i++) {
        work[i] = Available[i];
    }
    for (int i = 0; i < process_nums; i++) {
        finish[i] = false;
    }

    while (count < process_nums) {
        bool found = false;
        for (int i = 0; i < process_nums; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < types_of_resources; j++) {
                    //若当前所需的资源数大于当前可用资源
                    if (Need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }
                if (can_allocate) {
                    for (int j = 0; j < types_of_resources; j++) {
                        work[j] += Allocation[i][j];
                    }
                    safe_sequence[count++] = i;
                    finish[i] = true;
                    found = true;
                }
            }
        }
        if (!found) {
            printf("没有找到安全序列\n");
            free(work);
            free(finish);
            free(safe_sequence);
            return false;
        }
    }

    printf("安全序列为: ");
    for (int i = 0; i < process_nums; i++) {
        printf("P%d ", safe_sequence[i]);
    }
    printf("\n");

    free(work);
    free(finish);
    free(safe_sequence);
    return true;
}

// 请求资源分配
void request_resources() {
    int process_sequence;
    printf("请输入请求分配的进程的序号: ");
    scanf_s("%d", &process_sequence);

    if (process_sequence < 0 || process_sequence >= process_nums) {
        printf("进程号输入错误！\n");
        return;
    }

    int* Request = (int*)malloc(types_of_resources * sizeof(int));
    printf("请输入请求的各个资源的数目: ");
    for (int i = 0; i < types_of_resources; i++) {
        scanf_s("%d", &Request[i]);
    }

    for (int i = 0; i < types_of_resources; i++) {
        if (Request[i] > Need[process_sequence][i]) {
            printf("资源申请不合理\n");
            free(Request);
            return;
        }
        if (Request[i] > Available[i]) {
            printf("此时系统剩余的可用资源不能满足这次请求\n");
            free(Request);
            return;
        }
    }

    // 试分配
    for (int i = 0; i < types_of_resources; i++) {
        Available[i] -= Request[i];
        Allocation[process_sequence][i] += Request[i];
        Need[process_sequence][i] -= Request[i];
    }

    if (is_safe()) {
        printf("资源已成功分配\n");
    }
    else {
        printf("无法找到安全序列，回滚资源分配\n");
        for (int i = 0; i < types_of_resources; i++) {
            Available[i] += Request[i];
            Allocation[process_sequence][i] -= Request[i];
            Need[process_sequence][i] += Request[i];
        }
    }

    free(Request);
    print_matrices();
}

int main() {
    get_data();
    while (1) {
        char choice;
        printf("是否要请求资源分配？(y/n): ");
        scanf_s(" %c", &choice);
        if (choice == 'y') {
            request_resources();
        }
        else {
            break;
        }
    }

    // 释放动态分配的内存
    free(Available);
    for (int i = 0; i < process_nums; i++) {
        free(Max[i]);
        free(Allocation[i]);
        free(Need[i]);
    }
    free(Max);
    free(Allocation);
    free(Need);

    return 0;
}
