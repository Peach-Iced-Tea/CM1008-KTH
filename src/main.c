#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_MSG_LEN 256
#define MAX_MSGS 6
#define PORT 12345

typedef struct {
    char messages[MAX_MSGS][MAX_MSG_LEN];
    int msg_count;
} ChatRoom;

// Function to initialize SDL and TTF
int init(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    *window = SDL_CreateWindow("Chat Room", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 0;
    }

    *font = TTF_OpenFont("arial.ttf", 16);
    if (*font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

// Render chat room messages
void render_messages(SDL_Renderer *renderer, TTF_Font *font, ChatRoom *chatRoom) {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Rect renderQuad;
    
    int y_offset = 50;
    for (int i = 0; i < chatRoom->msg_count; i++) {
        textSurface = TTF_RenderText_Solid(font, chatRoom->messages[i], textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        
        renderQuad = (SDL_Rect){20, y_offset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        y_offset += 30;
    }
}

// Function to connect to a server or create a new one
int connect_or_create_server(const char *ip, int *server_sock, int *client_sock, int *is_server) {
    struct sockaddr_in server_addr;
    *server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_sock < 0) {
        printf("Socket creation error\n");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return 0;
    }

    // Try connecting to the server
    if (connect(*server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // Failed to connect, attempt to create a server
        printf("No server found, creating a new session\n");
        *is_server = 1;
        *server_sock = socket(AF_INET, SOCK_STREAM, 0);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(*server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            printf("Bind failed\n");
            return 0;
        }

        listen(*server_sock, 1);
        printf("Server started, waiting for connection...\n");
        *client_sock = accept(*server_sock, NULL, NULL);
        printf("Client connected\n");
    } else {
        printf("Connected to server\n");
        *is_server = 0;
        *client_sock = *server_sock;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

    if (!init(&window, &renderer, &font)) {
        printf("Failed to initialize\n");
        return 1;
    }

    int running = 1;
    SDL_Event e;
    int server_sock = -1, client_sock = -1, is_server = 0;
    ChatRoom chatRoom = {.msg_count = 0};
    char ip[20] = "127.0.0.1";
    char input_message[MAX_MSG_LEN] = "";

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            // Handle input and chat logic here
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render messages
        render_messages(renderer, font, &chatRoom);

        // Render any other UI elements here (buttons, text boxes, etc.)
        
        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    close(server_sock);
    close(client_sock);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
