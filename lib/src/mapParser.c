#include "mapParser.h"

struct tileset{
    xmlChar *imgRef;
    xmlChar *tilecount;
    xmlChar *columns;
    xmlChar *width;
    xmlChar *height;

    SDL_Texture *tileSheet;
};

struct clientMap {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int width, height;                  //Width and Height of the entire "Map"

    Tileset *layerTiles;                //The corresponding Tileset For now a struct thatcontains the .png
    SDL_Rect tileSheetPosition;         //The position in the tileset (currently not sure if this is actially needed)
};

Tileset *createTileset(SDL_Renderer *pRenderer) {
    Tileset *pTileset = malloc(sizeof(Tileset));

    pTileset->imgRef = NULL;
    pTileset->tilecount = NULL;
    pTileset->columns = NULL;
    pTileset->width = NULL;
    pTileset->height = NULL;

    pTileset->tileSheet = IMG_LoadTexture(pRenderer, "lib/resources/mapData/mountainTrumps.png");
    if (pTileset->tileSheet == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    return pTileset;
}

ClientMap *createClientMap(SDL_Renderer *pRenderer) {
    ClientMap *pMap = malloc(sizeof(ClientMap));

    pMap->layerTiles = createTileset(pRenderer);

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


void mapLoadTileset(const char *filename, Tileset *devTiles) {
    xmlDoc *doc = xmlReadFile(filename, NULL, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse %s\n", filename);
        return;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, (const xmlChar *)"tileset") != 0) {
        fprintf(stderr, "Invalid root node\n");
        xmlFreeDoc(doc);
        return;
    }

    devTiles->tilecount = xmlGetProp(root, (const xmlChar *)"tilecount");
    devTiles->columns = xmlGetProp(root, (const xmlChar *)"columns");

    xmlNode *imageNode = root->children;
    while (imageNode && xmlStrcmp(imageNode->name, (const xmlChar *)"image") != 0) {
        imageNode = imageNode->next;
    }

    if (imageNode) {
        devTiles->width = xmlGetProp(imageNode, (const xmlChar *)"width");
        devTiles->height = xmlGetProp(imageNode, (const xmlChar *)"height");
        devTiles->imgRef = xmlGetProp(imageNode, (const xmlChar *)"source");
    }

    xmlFreeDoc(doc);
}

void mapLoadClient(const char *path, ClientMap *pMap) {
    json_error_t error;
    json_t *root = json_load_file(path, 0, &error);

    if (!root) {
        fprintf(stderr, "Error loading TMJ: %s (line %d)\n", error.text, error.line);
        return;
    }

	pMap->width = json_integer_value(json_object_get(root, "width"));
	pMap->height = json_integer_value(json_object_get(root, "height"));

    json_t *layers = json_object_get(root, "layers");

    for (int i = 0; i < MAX_LAYERS; i++) {
        json_t *layer = json_array_get(layers, i);
        json_t *data = json_object_get(layer, "data");

        if (!json_is_array(data)) {
            fprintf(stderr, "Layer %d data is not an array\n", i);
            continue;
        }

        size_t size = json_array_size(data);
        pMap->layerSize[i] = size;
        pMap->layerData[i] = malloc(sizeof(int) * size);  // allocate normal C array

        for (size_t j = 0; j < size; j++) {
            json_t *tile = json_array_get(data, j);
            if (!json_is_integer(tile)) {
                fprintf(stderr, "Invalid tile at %zu in layer %d\n", j, i);
                pMap->layerData[i][j] = 0; // or handle error
            } else {
                pMap->layerData[i][j] = json_integer_value(tile);
            }
        }
    }
    json_decref(root);
}

void mapSetTileSheetPosition(ClientMap *pMap, int x, int y) {
    pMap->tileSheetPosition.x = x;
    pMap->tileSheetPosition.y = y;
}

SDL_Texture *mapGetTileTextures(Tileset *pTileset) {
    return pTileset->tileSheet;
}

xmlChar *mapGetColumns(Tileset *pTileset) {
    return pTileset->columns;
}

int mapGetWidth(ClientMap *pMap) {
    return pMap->width;
}

Tileset *mapGetTileset(ClientMap *pMap) {
    return pMap->layerTiles;
}

size_t mapGetLayerSize(ClientMap *pMap, int layer) {
    return pMap->layerSize[layer];
}

int mapGetLayerData(ClientMap *pMap, int layer, int index) {
    return pMap->layerData[layer][index];
}

SDL_Rect mapGetTileSheetPosition(ClientMap *pMap) {
    return pMap->tileSheetPosition;
}

void destroyMap(ClientMap *pMap) {
    if (pMap == NULL) { return; }

    SDL_DestroyTexture(pMap->layerTiles->tileSheet);
    free(pMap->layerTiles);
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (pMap->layerData[i]) { free(pMap->layerData[i]); }
    }
    free(pMap);

    return;
}

//-----------------Server:--------------------------

struct serverMap {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int width, height;                  //Width and Height of the entire "Map"   
};

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

void mapLoadServer(const char *path, ServerMap *pMap) {
    json_error_t error;
    json_t *root = json_load_file(path, 0, &error);

    if (!root) {
        fprintf(stderr, "Error loading TMJ: %s (line %d)\n", error.text, error.line);
        return;
    }

	pMap->width = json_integer_value(json_object_get(root, "width"));
	pMap->height = json_integer_value(json_object_get(root, "height"));


    
    json_t *layers = json_object_get(root, "layers");

    for (int i = 0; i < MAX_LAYERS; i++) {
        json_t *layer = json_array_get(layers, i);
        json_t *data = json_object_get(layer, "data");

        if (!json_is_array(data)) {
            fprintf(stderr, "Layer %d data is not an array\n", i);
            continue;
        }

        size_t size = json_array_size(data);
        pMap->layerSize[i] = size;
        pMap->layerData[i] = malloc(sizeof(int) * size);  // allocate normal C array

        for (size_t j = 0; j < size; j++) {
            json_t *tile = json_array_get(data, j);
            if (!json_is_integer(tile)) {
                fprintf(stderr, "Invalid tile at %zu in layer %d\n", j, i);
                pMap->layerData[i][j] = 0; // or handle error
            } else {
                pMap->layerData[i][j] = json_integer_value(tile);
            }
        }
    }
    json_decref(root);
}

int mapGetWidth_Server(ServerMap *pMap) {
    return pMap->width;
}

size_t mapGetLayerSize_Server(ServerMap *pMap, int layer) {
    return pMap->layerSize[layer];
}

int mapGetLayerData_Server(ServerMap *pMap, int layer, int index) {
    return pMap->layerData[layer][index];
}

void destroyMap_Server(ServerMap *pMap) {
    if (pMap == NULL) { return; }

    for (int i = 0; i < MAX_LAYERS; i++) {
        if (pMap->layerData[i]) { free(pMap->layerData[i]); }
    }
    free(pMap);
    
    return;
}