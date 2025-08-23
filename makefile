MASTER:
	gcc -Wall -o master Master.c

MMU:
	gcc -Wall -o mmu MMU.c

PROCESS:
	gcc -Wall -o process process.c

SCHED:
	gcc -Wall -o sched sched.c

all: MASTER MMU PROCESS SCHED
	ipcrm -a		# remove all user shared memory just in case

clean:
	rm -f master mmu process sched
