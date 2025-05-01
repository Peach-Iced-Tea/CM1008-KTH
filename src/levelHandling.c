#include "levelHandling.h"

struct clientMap {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int width, height;                  //Width and Height of the entire "Map"

    //Tileset *TileSheet;                //The corresponding Tileset For now a struct thatcontains the .png
    SDL_Rect tileSheetPosition;         //The position in the tileset (currently not sure if this is actially needed)
};

struct serverMap {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int width, height;                  //Width and Height of the entire "Map"   
};



ClientMap *createClientMap(SDL_Renderer *pRenderer) {
    ClientMap *pMap = malloc(sizeof(ClientMap));

    //pMap->TileSheet = createTileset(pRenderer);

    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerData[i] = NULL;
    }
    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerSize[i] = 0;
    }

    pMap->height = 0;
    pMap->width = 0;

    pMap->tileSheetPosition.w = 32;
    pMap->tileSheetPosition.h = 32;
    pMap->tileSheetPosition.x = 0;
    pMap->tileSheetPosition.y = 0;
    return pMap;
}


ServerMap *createServerMap() {
    ServerMap *pMap = malloc(sizeof(ServerMap));

    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerData[i] = NULL;
    }
    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerSize[i] = 0;
    }
    pMap->height = 0;
    pMap->width = 0;
}