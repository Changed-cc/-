#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int** Max, ** Allocation, ** Need;
int* Available;
int process_nums, types_of_resources;

// ��ӡ��Դ״̬��
void print_matrices() {
    printf("\n��Դ�������:\n");
    
    // ��ӡ���⣬�������
    printf("����\t�������\t\t��ռ����Դ��Ŀ\t\t��໹�����\t\t����Դʣ����Ŀ\n");

    // ��ӡ������
    for (int i = 0; i < process_nums; i++) {
        printf("P%-2d\t", i); // ���̱������룬���Ϊ2
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Max[i][j]); // ÿ����ֵռ�̶����4
        }
        printf("\t");
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Allocation[i][j]);
        }
        printf("\t");
        for (int j = 0; j < types_of_resources; j++) {
            printf("%-4d", Need[i][j]);
        }
        if (i == 0) { // ��ӡ������Դ��ֻ��ӡ�ڵ�һ��
            printf("\t");
            for (int j = 0; j < types_of_resources; j++) {
                printf("%-4d", Available[j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}


// ��ȡ�û�����������������Դ����
void get_data() {
    printf("��������Դ�������ͽ�����Ŀ: ");
    scanf_s("%d %d", &types_of_resources, &process_nums);

    // ��̬�����ڴ�
    Available = (int*)malloc(types_of_resources * sizeof(int));
    Max = (int**)malloc(process_nums * sizeof(int*));
    Allocation = (int**)malloc(process_nums * sizeof(int*));
    Need = (int**)malloc(process_nums * sizeof(int*));
    for (int i = 0; i < process_nums; i++) {
        Max[i] = (int*)malloc(types_of_resources * sizeof(int));
        Allocation[i] = (int*)malloc(types_of_resources * sizeof(int));
        Need[i] = (int*)malloc(types_of_resources * sizeof(int));
    }

    // �����������ɿ�����Դ
    printf("��ѡ��������Դ��ʽ��1. �ֶ����� 2. �������\n");
    int choice;
    scanf_s("%d", &choice);
    if (choice == 1) {
        printf("�����뵱ǰ���õĸ���Դ��Ŀ: ");
        for (int i = 0; i < types_of_resources; i++) {
            scanf_s("%d", &Available[i]);
        }
        for (int i = 0; i < process_nums; i++) {
            printf("������P%d�����������Դ��Ŀ: ", i);
            for (int j = 0; j < types_of_resources; j++) {
                scanf_s("%d", &Max[i][j]);
            }
        }
        for (int i = 0; i < process_nums; i++) {
            printf("������P%d�Ѿ��������Դ��Ŀ: ", i);
            for (int j = 0; j < types_of_resources; j++) {              
                scanf_s("%d", &Allocation[i][j]);                           
            }
        }
    }
    else if(choice == 2) {
        srand(time(NULL));
        printf("������ɵĵ�ǰ������Դ��Ŀ: ");
        for (int i = 0; i < types_of_resources; i++) {
            Available[i] = rand() % 10 + 1;
            printf("%d ", Available[i]);
        }
        printf("\n");

        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                // ���� Max ���󣬱�֤��ֵ�ʵ����Ա������Դ���벻��������
                Max[i][j] = rand() % 10 + 5;
            }
        }

        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                // ���� Allocation ���󣬲�ȷ�����������ܹ����ԡ��Ҳ�����ȫ���С������
                Allocation[i][j] = rand() % (Max[i][j] / 2 + 1); // �����ѷ������Դ���������
            }
        }

        // �������
        // ����һ�����̵� Need ���� Max�����ԡ���Դ���벻����
        int test_process = rand() % process_nums;
        int test_resource = rand() % types_of_resources;
        Need[test_process][test_resource] = Max[test_process][test_resource] + 1; // �����������


        // �����������ݺ���ȷ���л����ҵ���ȫ���У�������������
        for (int i = 0; i < process_nums; i++) {
            for (int j = 0; j < types_of_resources; j++) {
                Need[i][j] = Max[i][j] - Allocation[i][j];
            }
        }
    }
    else {
        printf("\n�������!");
    }  

    // ���� Need ����
    for (int i = 0; i < process_nums; i++) {
        for (int j = 0; j < types_of_resources; j++) {
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }

    print_matrices();
}

// �ж�ϵͳ�Ƿ��ڰ�ȫ״̬
bool is_safe() {
    int* work = (int*)malloc(types_of_resources * sizeof(int));//��ʾϵͳ������Դ
    bool* finish = (bool*)malloc(process_nums * sizeof(bool));//��¼�ý����Ƿ����
    int* safe_sequence = (int*)malloc(process_nums * sizeof(int));//���氲ȫ��������
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
                    //����ǰ�������Դ�����ڵ�ǰ������Դ
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
            printf("û���ҵ���ȫ����\n");
            free(work);
            free(finish);
            free(safe_sequence);
            return false;
        }
    }

    printf("��ȫ����Ϊ: ");
    for (int i = 0; i < process_nums; i++) {
        printf("P%d ", safe_sequence[i]);
    }
    printf("\n");

    free(work);
    free(finish);
    free(safe_sequence);
    return true;
}

// ������Դ����
void request_resources() {
    int process_sequence;
    printf("�������������Ľ��̵����: ");
    scanf_s("%d", &process_sequence);

    if (process_sequence < 0 || process_sequence >= process_nums) {
        printf("���̺��������\n");
        return;
    }

    int* Request = (int*)malloc(types_of_resources * sizeof(int));
    printf("����������ĸ�����Դ����Ŀ: ");
    for (int i = 0; i < types_of_resources; i++) {
        scanf_s("%d", &Request[i]);
    }

    for (int i = 0; i < types_of_resources; i++) {
        if (Request[i] > Need[process_sequence][i]) {
            printf("��Դ���벻����\n");
            free(Request);
            return;
        }
        if (Request[i] > Available[i]) {
            printf("��ʱϵͳʣ��Ŀ�����Դ���������������\n");
            free(Request);
            return;
        }
    }

    // �Է���
    for (int i = 0; i < types_of_resources; i++) {
        Available[i] -= Request[i];
        Allocation[process_sequence][i] += Request[i];
        Need[process_sequence][i] -= Request[i];
    }

    if (is_safe()) {
        printf("��Դ�ѳɹ�����\n");
    }
    else {
        printf("�޷��ҵ���ȫ���У��ع���Դ����\n");
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
        printf("�Ƿ�Ҫ������Դ���䣿(y/n): ");
        scanf_s(" %c", &choice);
        if (choice == 'y') {
            request_resources();
        }
        else {
            break;
        }
    }

    // �ͷŶ�̬������ڴ�
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
