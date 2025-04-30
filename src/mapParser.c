
#include "mapParser.h"

struct tileset{
    xmlChar *imgRef;
    xmlChar *tilecount;
    xmlChar *columns;
    xmlChar *width;
    xmlChar *height;

    SDL_Texture *tileSheet;
};

#define MAX_LAYERS 2

struct map {

    int *layerData[MAX_LAYERS]; 
    size_t layerSize[MAX_LAYERS];

    int width, height;

    Tileset *layerTiles;
    SDL_Rect tileSheetPosition;
};

Tileset *createTileset(SDL_Renderer *pRenderer) {
    Tileset *pTileset = malloc(sizeof(Tileset));

    pTileset->imgRef = NULL;
    pTileset->tilecount = NULL;
    pTileset->columns = NULL;
    pTileset->width = NULL;
    pTileset->height = NULL;

    pTileset->tileSheet = IMG_LoadTexture(pRenderer, "resources/mapData/devTiles.png");
    if (pTileset->tileSheet == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    return pTileset;
}

Map *createMap(SDL_Renderer *pRenderer) {
    Map *pMap = malloc(sizeof(Map));

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


void loadTileset(const char *filename, Tileset *devTiles) {
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

void loadMap(const char *path, Map *pMap) {
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

void setTileSheetPosition(Map *pMap, int x, int y) {
    pMap->tileSheetPosition.x = x;
    pMap->tileSheetPosition.y = y;
}

SDL_Texture *getTileTextures(Tileset *pTileset) {
    return pTileset->tileSheet;
}

xmlChar *getColumns(Tileset *pTileset) {
    return pTileset->columns;
}

int getMapWidth(Map *pMap) {
    return pMap->width;
}

Tileset *getTileset(Map *pMap) {
    return pMap->layerTiles;
}

size_t getLayerSize(Map *pMap, int layer) {
    return pMap->layerSize[layer];
}

int getLayerData(Map *pMap, int layer, int index) {
    return pMap->layerData[layer][index];
}

SDL_Rect getTileSheetPosition(Map *pMap) {
    return pMap->tileSheetPosition;
}

void destroyMap(Map *pMap) {
    if (pMap == NULL) { return; }

    SDL_DestroyTexture(pMap->layerTiles->tileSheet);
    free(pMap->layerTiles);
    free(pMap->layerData);
    free(pMap);

    return;
}