#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

#define NUM_PROCS 20    
#define ALLOC_SIZE 4096
#define NUM_ALLOCS 50    

void consume_memory() {
    char *mem;
    for (int i = 0; i < NUM_ALLOCS; i++) {
        mem = sbrk(ALLOC_SIZE);
        if (mem == (void *)-1) {
            printf(1, "sbrk failed at iteration %d\n", i);
            break;
        }
        // 메모리 접근 (페이지 실제 할당)
        for (int j = 0; j < ALLOC_SIZE; j += 4096) {
            mem[j] = j % 256;
        }
    }
}

int main() {
    int swap_in, swap_out;

    swapstat(&swap_in, &swap_out);
    printf(1, "Initial Swap state: In: %d, Out: %d\n", swap_in, swap_out);

    printf(1, "Creating processes to consume memory...\n");
    for (int i = 0; i < NUM_PROCS; i++) {
        int pid = fork();
        if (pid == 0) {
            consume_memory();

            swapstat(&swap_in, &swap_out);
            printf(1, "Child %d Swap state: In: %d, Out: %d\n", i, swap_in, swap_out);
            exit();
        } else if (pid < 0) {
            printf(1, "fork failed at iteration %d\n", i);
            break;
        }
    }

    // 부모 프로세스: 자식 프로세스 대기
    for (int i = 0; i < NUM_PROCS; i++) {
        wait();
    }

    // 최종 스왑 상태 확인
    swapstat(&swap_in, &swap_out);
    printf(1, "Final Swap state: In: %d, Out: %d\n", swap_in, swap_out);

    exit();
}