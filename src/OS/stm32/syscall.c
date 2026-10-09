
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
extern char _end;         // Конец .bss (начало кучи)
extern char _heap_limit;  // Максимальный адрес кучи

void* _sbrk(int incr) {
    static char* heap_end = 0;
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end; // Начало кучи
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    // Проверка: куча не должна пересекаться с областью стека
    if (heap_end > &_heap_limit) {
        return (void*)-1; // ENOMEM
    }

    return (void*)prev_heap_end;
}

// Обязательные заглушки для newlib
int _write(int file, char* ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    return -1; // Не реализовано
}

void _exit(int status) {
    (void)status;
    while(1); // Заглушка
}

int _close(int file) {
    errno = ENOSYS;
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0; // Нет ввода с терминала
}
