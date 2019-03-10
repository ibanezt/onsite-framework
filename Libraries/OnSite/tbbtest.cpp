#include <tbb/task_scheduler_init.h>

int main(int argc, char **argv) {
	tbb::task_scheduler_init tsInit;
	return 0;
}
