/*
Name: Sanskar Mittal and Karthik Reddy
Roll Number: 21CS10057 and 21CS30058
Semester: 6
Lab Assignment: 8 : Simulating Virtual Memory through (Pure) Demand Paging
File: sched.c
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

struct msgbuf {
    long m_type;
    struct message_ {
        int pid;
    } message;
};

struct schedMsg {
    long m_type;
    int message;
};


int main(int argc, char *argv[]) {
    int MQ1_id = atoi(argv[1]);
    int MQ2_id = atoi(argv[2]);
    int k = atoi(argv[3]);

    struct msgbuf msg;
    struct schedMsg schMsg;

    int cnt = 0;
    while (cnt < k) {

        // Select first process from ready queue
        msgrcv(MQ1_id, &msg, sizeof(msg.message), 1, 0);

        // Signal the process to start
        kill(msg.message.pid, SIGUSR1);

        // Get the response from MMU
        msgrcv(MQ2_id, &schMsg, sizeof(schMsg.message), 1, 0);

        if (schMsg.message == 1) {
            // Enqueue the current process and continue
            msg.m_type = 1;
            msg.message.pid = msg.message.pid;
            msgsnd(MQ1_id, &msg, sizeof(msg.message), 0);
        } else {
            // Schedule the next process
            ++cnt;
        }
    }
    // Signal the Master process
    kill(getppid(), SIGUSR1);

    pause();
}