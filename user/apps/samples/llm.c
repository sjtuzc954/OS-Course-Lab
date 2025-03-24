#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define MAX_LLM_PAGE_NUM 2

int main() {
    const char *filename = "weight.dat";
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, 8 * PAGE_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    char *map = mmap(NULL, 8 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LLM, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    int pages_to_access[] = {0, 2, 4, 6};
    // for (int j = 0; j < 2; j++) {
    for (size_t i = 0; i < sizeof(pages_to_access) / sizeof(pages_to_access[0]); i++) {
        int page_number = pages_to_access[i];
        map[page_number * PAGE_SIZE] = 'A' + page_number;  // 将每个页面的第一个字节设置为不同的字符
        printf("Accessed page %d, first byte: %c\n", page_number, map[page_number * PAGE_SIZE]);
    }
    // }

    // 解除映射
    if (munmap(map, 8 * PAGE_SIZE) == -1) {
        perror("munmap");
    }

    close(fd);
    return EXIT_SUCCESS;
}