#include "mapParser.h"

struct tileset{
    xmlChar *imgRef;
    xmlChar *tilecount;
    xmlChar *columns;
    xmlChar *width;
    xmlChar *height;
};

struct map {
    int *layerData[MAX_LAYERS];         //Array of data, loaded from .tmj file
    size_t layerSize[MAX_LAYERS];       //Size of the array
    int width, height;                  //Width and Height of the entire "Map"
    Vec2 mapSize;
    int tileWidth, tileHeight;
    Tileset *layerTiles;
};

Tileset *createTileset() {
    Tileset *pTileset = malloc(sizeof(Tileset));
    pTileset->imgRef = NULL;
    pTileset->tilecount = NULL;
    pTileset->columns = NULL;
    pTileset->width = NULL;
    pTileset->height = NULL;

    return pTileset;
}

Map *createMap() {
    Map *pMap = malloc(sizeof(Map));
    pMap->layerTiles = createTileset();

    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerData[i] = NULL;
    }
    for (int i = 0; i < MAX_LAYERS; i++) {
        pMap->layerSize[i] = 0;
    }

    pMap->width = 0;
    pMap->height = 0;
    pMap->mapSize = createVector(0.0f, 0.0f);
    pMap->tileWidth = 0;
    pMap->tileHeight = 0;
    return pMap;
}


void mapLoadTileset(Tileset *devTiles, const char *pFilePath) {
    xmlDoc *doc = xmlReadFile(pFilePath, NULL, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse %s\n", pFilePath);
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

void mapLoadDataFromFile(Map *pMap, const char *pFilePath) {
    json_error_t error;
    json_t *root = json_load_file(pFilePath, 0, &error);
    if (!root) {
        fprintf(stderr, "Error loading TMJ: %s (line %d)\n", error.text, error.line);
        return;
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
    }
    json_decref(root);
}

xmlChar *mapGetColumns(Tileset const *pTileset) {
    return pTileset->columns;
}

int mapGetWidth(Map const *pMap) {
    return pMap->width;
}

Tileset *mapGetTileset(Map const *pMap) {
    return pMap->layerTiles;
}

size_t mapGetLayerSize(Map const *pMap, int layer) {
    return pMap->layerSize[layer];
}

int mapGetLayerData(Map const *pMap, int layer, int index) {
    return pMap->layerData[layer][index];
}

Vec2 mapGetSize(Map const *pMap) {
    return pMap->mapSize;
}

bool mapGetTileInfo(Map const *pMap, int index, Entity *pTileToFill) {
    int gid = mapGetLayerData(pMap, LAYER_TILE_TEXTURES, index) - 15;

    if (gid > 0) {
        int columns = atoi((char *)mapGetColumns(mapGetTileset(pMap)));
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

    free(pMap->layerTiles);
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (pMap->layerData[i]) { free(pMap->layerData[i]); }
    }
    free(pMap);

    return;
}