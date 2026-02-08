#include <stdio.h>
#include <unistd.h>


__attribute__((constructor))
static void init(void)
{
    static auto t = std::thread(thread_main);
}