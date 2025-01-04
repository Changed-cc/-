#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROCESS_COUNT 5

// 定义进程状态
#define READY 'R'
#define END 'E'

// 定义进程控制块结构
typedef struct PCB {
    char name[3];       // 进程名
    int priority;       // 优先数
    int runtime;        // 要求运行时间
    char state;         // 状态：READY or END
    struct PCB* next;   // 指针，指向下一个进程
} PCB;

// 创建进程队列
PCB* createProcessQueue() {
    PCB* head = NULL, * prev = NULL;
    for (int i = 1; i <= PROCESS_COUNT; i++) {
        PCB* newProcess = (PCB*)malloc(sizeof(PCB));
        snprintf(newProcess->name, 3, "P%d", i);
        newProcess->priority = rand() % 10 + 1; // 随机生成优先数
        newProcess->runtime = rand() % 7 + 1;  // 随机生成要求运行时间
        newProcess->state = READY;
        newProcess->next = NULL;
        if (head == NULL) {
            head = newProcess;
        }
        else {
            prev->next = newProcess;
        }
        prev = newProcess;
    }
    return head;
}

// 按优先级排序队列（从大到小）,冒泡排序
PCB* sortQueueByPriority(PCB* head) {
    if (head == NULL || head->next == NULL) {
        return head; // 如果队列为空或只有一个元素，直接返回
    }

    PCB* i, * j;
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->priority < j->priority) {
                // 交换两个节点的数据
                int tempPriority = i->priority;
                int tempRuntime = i->runtime;
                char tempState = i->state;
                char tempName[3];
                strcpy_s(tempName, i->name);

                i->priority = j->priority;
                i->runtime = j->runtime;
                i->state = j->state;
                strcpy_s(i->name, j->name);

                j->priority = tempPriority;
                j->runtime = tempRuntime;
                j->state = tempState;
                strcpy_s(j->name, tempName);
            }
        }
    }
    return head;
}

// 打印进程队列
void printQueue(PCB* head) {
    PCB* current = head;
    printf("当前队列状态：\n");
    printf("进程名\t优先数\t运行时间\t状态\n");
    while (current) {
        printf("%s\t%d\t%d\t\t%c\n", current->name, current->priority, current->runtime, current->state);
        current = current->next;
    }
    printf("\n");
}

// 按优先级插入队列
PCB* insertByPriority(PCB* head, PCB* process) {
    if (head == NULL || process->priority > head->priority) {
        process->next = head;
        return process;
    }
    PCB* current = head;
    while (current->next != NULL && current->next->priority >= process->priority) {
        current = current->next;
    }
    process->next = current->next;
    current->next = process;
    return head;
}

// 模拟调度器
void schedule(PCB** head) {
    while (*head) {
        PCB* current = *head; // 队首进程
        printf("调度进程：%s\n", current->name);

        // 模拟运行一次
        current->priority--; // 优先数减 1
        current->runtime--;  // 运行时间减 1

        // 更新状态
        if (current->runtime == 0) {
            current->state = END;
            *head = current->next; // 从队列中移除
            free(current);
        }
        else {
            current->state = READY;
            *head = current->next; // 从队列头移除
            current->next = NULL;
            *head = insertByPriority(*head, current); // 按优先级重新插入队列
        }

        printQueue(*head);
    }
    printf("所有进程都已结束。\n");
}

// 主函数
int main() {
    srand((unsigned int)time(NULL)); // 设置随机数种子
    PCB* processQueue = createProcessQueue();
    printf("初始进程队列：\n");
    printQueue(processQueue);

    // 按优先数排序进程队列
    processQueue = sortQueueByPriority(processQueue);
    printf("排序后的进程队列：\n");
    printQueue(processQueue);

    schedule(&processQueue);  // 执行调度
    return 0;
}
