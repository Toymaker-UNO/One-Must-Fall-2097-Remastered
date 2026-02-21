#include "utils/time_fmt.h"
#include "utils/allocator.h"
#include <time.h>

char *format_time(void) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *buffer = (char *)malloc(32);
    strftime(buffer, 32, "%Y%m%d_%H%M%S", tm);
    return buffer;
}
