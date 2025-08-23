/*
Name: Sanskar Mittal and Karthik Reddy
Roll Number: 21CS10057 and 21CS30058
Semester: 6
Lab Assignment: 8 : Simulating Virtual Memory through (Pure) Demand Paging
File: process.c
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

int signal_msg = 0;

void signalHandler(int sig) {
    if (sig == SIGUSR1) {
        signal_msg = 1; // Signal received
    }

    // reset the signal handler
    signal(SIGUSR1, signalHandler);
}

struct msgbuf {
    long m_type;
    struct message_ {
        int pid;
    } message;
};

struct pageFrame {
    long m_type;
    struct message {
        int pid;
        int data;
    } message;
};

int main(int argc, char *argv[]) {
    int MQ1_id = atoi(argv[1]);
    int MQ3_id = atoi(argv[2]);

    char *ref_str = argv[3];
    int process_id = getpid();

    struct msgbuf msg;
    msg.m_type = 1;
    msg.message.pid = process_id;

    signal(SIGUSR1, signalHandler);

    // Send the process id to the scheduler
    msgsnd(MQ1_id, &msg, sizeof(msg.message), 0);

    if (signal_msg == 0) {
        pause();
    }
    signal_msg = 0;

    struct pageFrame pgf_msg;

    char *token = strtok(ref_str, ",");
    while (token != NULL) {
        int page_no = atoi(token);

        // Send the page request to MMU
        pgf_msg.m_type = 1;
        pgf_msg.message.pid = process_id;
        pgf_msg.message.data = page_no;
        msgsnd(MQ3_id, &pgf_msg, sizeof(pgf_msg.message), 0);

        // Wait for the response from MMU
        msgrcv(MQ3_id, &pgf_msg, sizeof(pgf_msg.message), process_id, 0);

        if (pgf_msg.message.data >= 0) {
            // MMU sent a valid frame number
            token = strtok(NULL, ",");
        } else if (pgf_msg.message.data == -1) {
            // MMU sent a page fault
            if (signal_msg == 0) {
                pause();
            }
            signal_msg = 0;
            token = strtok(NULL, ",");
        } else if (pgf_msg.message.data == -2) {
            // MMU sent a termination signal
            exit(0);
        } else {
            perror("Invalid data received from MMU");
            exit(-1);
        }
    }

    // Send the termination signal to MMU
    pgf_msg.m_type = 1;
    pgf_msg.message.pid = process_id;
    pgf_msg.message.data = -9;
    msgsnd(MQ3_id, &pgf_msg, sizeof(pgf_msg.message), 0);

    return 0;
}