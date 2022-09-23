#ifndef PANIC_H
#define PANIC_H

#ifdef NDEBUG
#define panic_if(condition) ((void)0)
#else
#define panic_if(condition, message) if (condition) panic(message)
#endif

void panic(const char* message);

#endif

