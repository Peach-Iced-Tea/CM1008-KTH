#ifdef _WIN32
#include <windows.h> 
DWORD WINAPI server_thread(LPVOID lpParam);
HANDLE server_thread;
#else
#include <pthread.h>
void* server_thread(void* arg);
pthread_t server_thread;
#endif

int SDL_main(int argc, char* argv[]);
