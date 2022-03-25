#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

char referecne[100][5];
char page_fault[100][5];
char table[8][101][5];
char time_ref[100][40];

bool exists;
int buffer_size;
int ref_size = 100;
char buffer[8][5];
int fault_counter;
char time_buff[40];
char time_nano[11];

void format(){

    FILE* fp;
    if (buffer_size == 4){
        fp = fopen("UdodenkoN_output_4.txt", "a");
    }
    else {
        fp = fopen("UdodenkoN_output_8.txt", "a");
    }
    char str_time[45];
    fputs("Timestamp:          |", fp);
    for (int i = 0; i < 100; ++i){
        sprintf(str_time, " %s", time_ref[i]);
        fputs(str_time, fp);
        int k = 19 - strlen(time_ref[i]);
        for (int g = 0; g < k; ++g){
            fputs(" ", fp);
        }
        fputs("|", fp);
        memset(str_time, 0, sizeof(str_time));
    }
    fputs("\n", fp);
    for (int l = 0; l < 101; ++l){
        fputs("---------------------", fp);
    }
    fputs("\n", fp);

    char str[30];

    for (int i = 0; i < buffer_size; ++i){
        strcpy(table[i][0], "#");
    }

    for (int i = 0; i < buffer_size; ++i){
        for (int j = 0; j < 101; ++j){
            if (strlen(table[i][j]) == 1){
                sprintf(str, "          %s         |", table[i][j]);
            }
            else {
                sprintf(str, "         %s         |", table[i][j]);
            }
            fputs(str, fp);
            memset(str, 0, sizeof(str));
        }
        fputs("\n", fp);
        for (int l = 0; l < 101; ++l){
            fputs("---------------------", fp);
        }
        fputs("\n", fp);
    }
    fputs("Page Fault:         |", fp);
    for (int k = 0; k < 100; ++k){
        sprintf(str, "         %s         |", page_fault[k]);
        fputs(str, fp);
        memset(str, 0, sizeof(str));
    }
    char str_fault[50];
    sprintf(str_fault, "\nThe total number of page faults: %d\n\n\n", fault_counter);
    fputs(str_fault, fp);
    memset(str_fault, 0, sizeof(str_fault));

    fclose(fp);
}

void print_ref(char alg_name[]){
    FILE* fp;
    if (buffer_size == 4){
        fp = fopen("UdodenkoN_output_4.txt", "a");
    }
    else {
        fp = fopen("UdodenkoN_output_8.txt", "a");
    }

    char str[100];
    sprintf(str, "Page frame size: %d, Algorithm: %s\nReference string:   |", buffer_size, alg_name);
    fputs(str, fp);
    char str_ref[30];
    for (int i = 0; i < 100; ++i){
        if (strlen(referecne[i]) == 1){
            sprintf(str_ref, "          %s         |", referecne[i]);
        }
        else {
            sprintf(str_ref, "         %s         |", referecne[i]);
        }
        fputs(str_ref, fp);
        memset(str_ref, 0, sizeof(str_ref));
    }
    fputs("\n", fp);
    for (int l = 0; l < 101; ++l){
        fputs("---------------------", fp);
    }
    fputs("\n", fp);
    fclose(fp);
}

void get_time(int i){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    memset(time_buff, 0, sizeof(time_buff));
    memset(time_nano, 0, sizeof(time_nano));
    strftime(time_buff, sizeof (time_buff), "%T", localtime(&ts.tv_sec));
    sprintf(time_nano, ".%ld", ts.tv_nsec);
    strncat(time_buff, time_nano, sizeof(time_nano));
    strcpy(time_ref[i], time_buff);
}

void fifo(){

    print_ref("FIFO");
    for (int i = 0; i < ref_size; ++i){
        exists = false;

        get_time(i);

        for (int j = 0; j < buffer_size; ++j){
            if (strcmp(buffer[j], referecne[i]) == 0){ // means that they are equal
                exists = true;
                strcpy(page_fault[i],"NO");
                break;
            }
        }
        if (!exists){
            strcpy(page_fault[i], "PF");
            fault_counter += 1;

            for (int k = 0; k < buffer_size-1; ++k){
                strcpy(buffer[k], buffer[k+1]);
            }
            strcpy(buffer[buffer_size-1], referecne[i]);
            
        }

        for (int row = 0; row < buffer_size; ++row){
            strcpy(table[row][i+1], buffer[buffer_size - 1 - row]);
        } 
    }

    format();

}

void lru(){

    print_ref("LRU");
    int index;

    for (int i = 0; i < ref_size; ++i){
        index = 0;
        exists = false;

        get_time(i);

        for (int j = 0; j < buffer_size; ++j){
            if (strcmp(buffer[j], referecne[i]) == 0){ // means that they are the same
                strcpy(page_fault[i],"NO");
                exists = true;
                index = j;
                break;
            }
        }
        if (!exists){
            strcpy(page_fault[i],"PF");
            fault_counter += 1;
        }

        for (int k = index; k < buffer_size-1; ++k){
            strcpy(buffer[k], buffer[k+1]);
        }
        strcpy(buffer[buffer_size-1], referecne[i]);

        for (int row = 0; row < buffer_size; ++row){
            strcpy(table[row][i+1], buffer[buffer_size - 1 - row]);
        } 

    }

    format();

}

void second_chance(){

    print_ref("SECOND CHANCE");
    int buffer_second_chance[buffer_size];
    for (int i = 0; i < buffer_size; ++i){
        buffer_second_chance[i] = 0;
    }
    int index_to_replace = buffer_size - 1;
    bool index_replaced;

    for (int i = 0; i < ref_size; ++i){
        exists = false;
        get_time(i);

        for (int j = 0; j < buffer_size; ++j){
            if (strcmp(buffer[j], referecne[i]) == 0){ // means that they are the same
                exists = true;
                strcpy(page_fault[i],"NO");
                buffer_second_chance[j] = 1;
            }
        }
        if (!exists){
            strcpy(page_fault[i],"PF");
            fault_counter += 1;
            index_replaced = false;
            for (int k = 0; k < buffer_size; ++k){
                if (buffer_second_chance[index_to_replace] == 1){
                    buffer_second_chance[index_to_replace] = 0;
                }
                else {
                    strcpy(buffer[index_to_replace], referecne[i]);
                    index_replaced = true;
                }
                --index_to_replace;
                if (index_to_replace < 0){
                    index_to_replace = buffer_size - 1;
                }
                if (index_replaced == true){
                    break;
                }
            }
        }

        for (int row = 0; row < buffer_size; ++row){
            strcpy(table[row][i+1], buffer[buffer_size - 1 - row]);
        } 

    }

    format();

}

int main(){

    FILE* fp;
    char str[5];
    // fp = fopen("UdodenkoN_input.txt", "w+");
    // for (int i = 0; i < 100; ++i){
    //     memset(str, 0, sizeof(str));
    //     sprintf(str, "%d ", (rand() % 15));
    //     fputs(str, fp);
    // }
    // fclose(fp);

    fp = fopen("UdodenkoN_input.txt", "r");
    for (int i = 0; i < 100; ++i){
        memset(str, 0, sizeof(str));
        fscanf(fp, "%s", str);
        strcpy(referecne[i], str);
    }
    fclose(fp);
    
    while(1){
        fault_counter = 0;
        char choice;
        printf("Please select a paging algorithm (Choices are 1-4)\n");
        printf("1.FIFO\n2.LRU\n3.Second Chance\n4.Quit\n");
        scanf(" %c", &choice);
        if (choice == '4'){return 0;}
        if (choice != '1' && choice != '2' && choice != '3'){
            printf("Wrong Input. Input should be 1-4\n\n");
            continue;
        }

        printf("Please select the frames number (Choices are 4 and 8)\n");
        scanf("%d", &buffer_size);
        if (buffer_size != 4 && buffer_size != 8){
            printf("Wrong input, the page frame can be either 4 or 8\n\n");
            continue;
        }
        printf("\n");

        for (int i = 0; i < buffer_size; ++i){
            strcpy(buffer[i], "#");
        }

        switch (choice){
            case '1':
                fifo();
                break;
            case '2':
                lru();
                break;
            case '3':
                second_chance();
                break;
        }
    }
}