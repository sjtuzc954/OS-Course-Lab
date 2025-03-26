#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define MAX_LLM_PAGE_NUM 32
#define TOTAL_PAGES 128

int main() {
    const char *filename = "weight.dat";
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, TOTAL_PAGES * PAGE_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    char *map = mmap(NULL, TOTAL_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LLM, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    // for (int j = 0; j < 2; j++) {
    for (size_t i = 0; i * 2 < TOTAL_PAGES; i++) {
        int page_number = 2 * i;
        map[page_number * PAGE_SIZE] = 'A';
        printf("Accessed page %d\n", page_number);
    }
    // }

    // 解除映射
    if (munmap(map, TOTAL_PAGES * PAGE_SIZE) == -1) {
        perror("munmap");
    }

    close(fd);
    return EXIT_SUCCESS;
}