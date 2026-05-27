#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sched.h>

#define NUM_RUNS	20

struct sched_attr {
	__u32 size;
	__u32 sched_policy;
	__u64 sched_flags;

	__s32 sched_nice;
	__u32 sched_priority;

	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
};

int stop = 0;

int sched_setattr(pid_t pid, struct sched_attr *attr, unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

void run_task(int id)
{
	struct sched_attr attr;
	static long instance = 0;

	memset(&attr, 0, sizeof(attr));

	attr.size = sizeof(attr);
	attr.sched_policy = SCHED_DEADLINE;

	attr.sched_runtime  = 10 * 1000 * 1000;
	attr.sched_deadline = 20 * 1000 * 1000;
	attr.sched_period   = 20 * 1000 * 1000;

	if (sched_setattr(0, &attr, 0) < 0) {
		perror("sched_setattr");
		exit(1);
	}

	printf("TASK %d started\n", id);

	volatile unsigned long i = 0;

	while (i < NUM_RUNS * 100000000) {
		i++;

		if (i % 100000000 == 0) {
			int cpu = sched_getcpu();

			printf("TASK %d running on CPU %d, instance %ld\n", id, cpu, instance++);
		}
	}
}

int main()
{
	int num_tasks = 6;

	for (int i = 0; i < num_tasks; i++) {

		pid_t pid = fork();

		if (pid == 0) {
			run_task(i);
			return 0;
		}
	}

	for (int i = 0; i < 5; i++) {}
		sleep(1);

	stop = 1;

	return 0;
}