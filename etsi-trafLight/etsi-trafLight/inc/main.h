#ifndef MAIN_H
#define MAIN_H

#define DEBUG 1

#if DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
    #else
    #define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif // DEBUG

#endif // MAIN_H
