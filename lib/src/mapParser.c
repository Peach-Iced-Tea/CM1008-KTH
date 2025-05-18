#include "mapParser.h"

#define MAX_PATH_LENGTH 64

struct tileset {
    char *source;
    int tilecount;
    int columns;
    int width;
    int height;
    char filePath[MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)];
};

struct map {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int layerCount;
    int width, height;                  //Width and Height of the entire "Map"
    Vec2 mapSize;
    int tileWidth, tileHeight;
    Tileset *pTileset;
    char folderPath[MAX_PATH_LENGTH];
    char tilesetPath[MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)];
};

Tileset *createTileset() {
    Tileset *pTileset = malloc(sizeof(Tileset));
    pTileset->source = NULL;
    pTileset->tilecount = 0;
    pTileset->columns = 0;
    pTileset->width = 0;
    pTileset->height = 0;
    pTileset->filePath[0] = '\0';

    return pTileset;
}

Map *createMap() {
    Map *pMap = malloc(sizeof(Map));
    pMap->pTileset = createTileset();

    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerData[i] = NULL;
    }
    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerSize[i] = 0;
    }

    pMap->layerCount = 0;
    pMap->width = 0;
    pMap->height = 0;
    pMap->mapSize = createVector(0.0f, 0.0f);
    pMap->tileWidth = 0;
    pMap->tileHeight = 0;
    pMap->folderPath[0] = '\0';
    pMap->tilesetPath[0] = '\0';

    return pMap;
}

char *mapGetFilePath(char *pFolderPath, MapIndex index) {
    switch (index) {
        case MAP_DEMO:
            strncat(pFolderPath, "lib/resources/mapData/demo/", MAX_PATH_LENGTH-1);
            return "lib/resources/mapData/demo/map.tmj";
    }

    return "";
}

int mapLoadTileset(Tileset *pTileset, char *tilesetPath) {
    xmlDoc *doc = xmlReadFile(tilesetPath, NULL, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse %s\n", tilesetPath);
        return 1;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, (const xmlChar *)"tileset") != 0) {
        fprintf(stderr, "Invalid root node\n");
        xmlFreeDoc(doc);
        return 1;
    }

    xmlChar *tmp = xmlGetProp(root, (const xmlChar *)"tilecount");
    if (tmp == NULL) { return 1; }
    pTileset->tilecount = atoi((char *)tmp);

    tmp = xmlGetProp(root, (const xmlChar *)"columns");
    if (tmp == NULL) { return 1; }
    pTileset->columns = atoi((char *)tmp);

    xmlNode *imageNode = root->children;
    while (imageNode && xmlStrcmp(imageNode->name, (const xmlChar *)"image") != 0) {
        imageNode = imageNode->next;
    }

    if (imageNode) {
        tmp = xmlGetProp(imageNode, (const xmlChar *)"width");
        if (tmp == NULL) { return 1; }
        pTileset->width = atoi((char *)tmp);

        tmp = xmlGetProp(imageNode, (const xmlChar *)"height");
        if (tmp == NULL) { return 1; }
        pTileset->height = atoi((char *)tmp);

        tmp = xmlGetProp(imageNode, (const xmlChar *)"source");
        if (tmp == NULL) { return 1; }
        pTileset->source = (char *)tmp;
    }

    xmlFreeDoc(doc);
    return 0;
}

int mapLoadDataFromFile(Map *pMap, MapIndex index) {
    json_error_t error;
    char *pFilePath = mapGetFilePath(pMap->folderPath, index);
    json_t *root = json_load_file(pFilePath, 0, &error);
    if (!root) {
        fprintf(stderr, "Error loading TMJ: %s (line %d)\n", error.text, error.line);
        return 1;
    }

	pMap->width = json_integer_value(json_object_get(root, "width"));
	pMap->height = json_integer_value(json_object_get(root, "height"));
    pMap->tileWidth = json_integer_value(json_object_get(root, "tilewidth"));
	pMap->tileHeight = json_integer_value(json_object_get(root, "tileheight"));

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

        if (pMap->mapSize.x == 0.0f) { pMap->mapSize.x = (float)json_integer_value(json_object_get(layer, "width"))*32.0f; }
	    if (pMap->mapSize.y == 0.0f) { pMap->mapSize.y = (float)json_integer_value(json_object_get(layer, "height"))*32.0f; }

        for (size_t j = 0; j < size; j++) {
            json_t *tile = json_array_get(data, j);
            if (!json_is_integer(tile)) {
                fprintf(stderr, "Invalid tile at %zu in layer %d\n", j, i);
                pMap->layerData[i][j] = 0; // or handle error
            } else {
                pMap->layerData[i][j] = json_integer_value(tile);
            }
        }

        pMap->layerCount++;
    }

    char tilesetName[MAX_PATH_LENGTH>>1];
    json_t *tilesets = json_object_get(root, "tilesets");
    for (int i = 0; i < 2; i++) {
        json_t *tileset = json_array_get(tilesets, i);
        json_t *source = json_object_get(tileset, "source");

        strncpy(tilesetName, json_string_value(source), (MAX_PATH_LENGTH>>1)-1);
    }

    strncpy(pMap->tilesetPath, pMap->folderPath, MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)-1);
    strncat(pMap->tilesetPath, tilesetName, MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)-1);
    if (mapLoadTileset(pMap->pTileset, pMap->tilesetPath)) { return 1; }

    strncpy(pMap->pTileset->filePath, pMap->folderPath, MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)-1);
    strncat(pMap->pTileset->filePath, pMap->pTileset->source, MAX_PATH_LENGTH+(MAX_PATH_LENGTH>>1)-1);
    json_decref(root);
    return 0;
}

int mapGetWidth(Map const *pMap) {
    return pMap->width;
}

char const *mapGetFolderPath(Map const *pMap) {
    return pMap->folderPath;
}

char const *mapGetTilesetPath(Map const *pMap) {
    return pMap->pTileset->filePath;
}

int mapGetLayerCount(Map const *pMap) {
    return pMap->layerCount;
}

size_t mapGetLayerSize(Map const *pMap, int layer) {
    return pMap->layerSize[layer];
}

int mapGetLayerData(Map const *pMap, int layer, int index, Vec2 *pPosition) {
    if (pPosition != NULL) {
        pPosition->x = (index % pMap->width) * pMap->tileWidth;
        pPosition->y = (index / pMap->width) * pMap->tileHeight;
    }

    return pMap->layerData[layer][index];
}

Vec2 mapGetSize(Map const *pMap) {
    return pMap->mapSize;
}

int mapGetTileWidth(Map const *pMap) {
    return pMap->tileWidth;
}

int mapGetTileHeight(Map const *pMap) {
    return pMap->tileHeight;
}

bool mapGetTileInfo(Map const *pMap, int index, Entity *pTileToFill) {
    int gid = mapGetLayerData(pMap, LAYER_TILE_TEXTURES, index, NULL) - 15;

    if (gid > 0) {
        int columns = pMap->pTileset->columns;
        pTileToFill->source.x = (gid % columns) * 32;
        pTileToFill->source.y = (gid / columns) * 32;
        pTileToFill->source.w = pMap->tileWidth;
        pTileToFill->source.h = pMap->tileHeight;

        pTileToFill->frame.x = (index % pMap->width) * pMap->tileWidth;
        pTileToFill->frame.y = (index / pMap->width) * pMap->tileHeight;
        pTileToFill->frame.w = pMap->tileWidth;
        pTileToFill->frame.h = pMap->tileHeight;
        return true;
    }
    else {
        return false;
    }
}

void destroyMap(Map *pMap) {
    if (pMap == NULL) { return; }

    free(pMap->pTileset);
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (pMap->layerData[i]) { free(pMap->layerData[i]); }
    }
    free(pMap);

    return;
}