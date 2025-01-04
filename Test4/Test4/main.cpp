#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROCESS_COUNT 5

// �������״̬
#define READY 'R'
#define END 'E'

// ������̿��ƿ�ṹ
typedef struct PCB {
    char name[3];       // ������
    int priority;       // ������
    int runtime;        // Ҫ������ʱ��
    char state;         // ״̬��READY or END
    struct PCB* next;   // ָ�룬ָ����һ������
} PCB;

// �������̶���
PCB* createProcessQueue() {
    PCB* head = NULL, * prev = NULL;
    for (int i = 1; i <= PROCESS_COUNT; i++) {
        PCB* newProcess = (PCB*)malloc(sizeof(PCB));
        snprintf(newProcess->name, 3, "P%d", i);
        newProcess->priority = rand() % 10 + 1; // �������������
        newProcess->runtime = rand() % 7 + 1;  // �������Ҫ������ʱ��
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

// �����ȼ�������У��Ӵ�С��,ð������
PCB* sortQueueByPriority(PCB* head) {
    if (head == NULL || head->next == NULL) {
        return head; // �������Ϊ�ջ�ֻ��һ��Ԫ�أ�ֱ�ӷ���
    }

    PCB* i, * j;
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->priority < j->priority) {
                // ���������ڵ������
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

// ��ӡ���̶���
void printQueue(PCB* head) {
    PCB* current = head;
    printf("��ǰ����״̬��\n");
    printf("������\t������\t����ʱ��\t״̬\n");
    while (current) {
        printf("%s\t%d\t%d\t\t%c\n", current->name, current->priority, current->runtime, current->state);
        current = current->next;
    }
    printf("\n");
}

// �����ȼ��������
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

// ģ�������
void schedule(PCB** head) {
    while (*head) {
        PCB* current = *head; // ���׽���
        printf("���Ƚ��̣�%s\n", current->name);

        // ģ������һ��
        current->priority--; // �������� 1
        current->runtime--;  // ����ʱ��� 1

        // ����״̬
        if (current->runtime == 0) {
            current->state = END;
            *head = current->next; // �Ӷ������Ƴ�
            free(current);
        }
        else {
            current->state = READY;
            *head = current->next; // �Ӷ���ͷ�Ƴ�
            current->next = NULL;
            *head = insertByPriority(*head, current); // �����ȼ����²������
        }

        printQueue(*head);
    }
    printf("���н��̶��ѽ�����\n");
}

// ������
int main() {
    srand((unsigned int)time(NULL)); // �������������
    PCB* processQueue = createProcessQueue();
    printf("��ʼ���̶��У�\n");
    printQueue(processQueue);

    // ��������������̶���
    processQueue = sortQueueByPriority(processQueue);
    printf("�����Ľ��̶��У�\n");
    printQueue(processQueue);

    schedule(&processQueue);  // ִ�е���
    return 0;
}
