#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/syscall.h>

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

long instance = 0;

int sched_setattr(pid_t pid, struct sched_attr *attr, unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

void busy_loop(const char *name)
{
	volatile unsigned long i = 0;
	while (1) {
		i++;
		if (i % 100000000 == 0) {
			int cpu = sched_getcpu();

			printf("%s running on CPU %d, instance %ld\n", name, cpu, instance++);
		}
	}
}

int main()
{
	struct sched_attr attr;
	memset(&attr, 0, sizeof(attr));

	attr.size = sizeof(attr);
	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = 10 * 1000 * 1000;   // 10ms
	attr.sched_period  = 20 * 1000 * 1000;   // 20ms
	attr.sched_deadline = 20 * 1000 * 1000;

	if (sched_setattr(0, &attr, 0) < 0) {
		perror("sched_setattr failed");
		return 1;
	}

	printf("DL task started\n");

	busy_loop("TASK");
	return 0;
}
