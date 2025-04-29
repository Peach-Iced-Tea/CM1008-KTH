
#include "mapParser.h"

typedef struct tileset{
    xmlChar *imgRef;
    xmlChar *tilecount;
    xmlChar *columns;
    xmlChar *width;
    xmlChar *height;
};

typedef struct map {
    Tileset *layerTiles;
    
    json_t *data;
    size_t size;
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

    pMap->data = NULL;  //array for the positions in map
    pMap->size = NULL;  //size of the array
    
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
        return 1;
    }

	int width = json_integer_value(json_object_get(root, "width"));
	int heigth = json_integer_value(json_object_get(root, "height"));
    // The "layers" array
    json_t *layers = json_object_get(root, "layers");
    if (!json_is_array(layers)) {
        fprintf(stderr, "\"layers\" is not an array\n");
        json_decref(root);
        return 1;
    }

	for (int i = 0; i < 2; i++){
		// Get first layer (index 0)
		json_t *layer = json_array_get(layers, i);
		if (!json_is_object(layer)) {
			fprintf(stderr, "Layer[0] is not an object\n");
			json_decref(root);
			return 1;
		}
	
		// Get the "data" array inside that layer
		pMap->data = json_object_get(layer, "data");
		if (!json_is_array(pMap->data)) {
			fprintf(stderr, "\"data\" is not an array\n");
			json_decref(root);
			return 1;
		}
	
		pMap->size = json_array_size(pMap->data);
		printf("Tile data (%zu tiles):\n", pMap->size);
	
		for (size_t j = 0; j < pMap->size; j++) {
			json_t *tile = json_array_get(pMap->data, j);
			if (!json_is_integer(tile)) {
				printf("Invalid value at %zu\n", j);
				continue;
			}




       /*      if ((int)json_integer_value(tile) >= 15){
                // printf("%2d ", (int)json_integer_value(tile));
                int index = (int)json_integer_value(tile) - 15;
                int columns = atoi((char *)devTiles.columns);

                int tileX = (index % columns) * 32;
                int tileY = (index / columns) * 32;

                printf("indx: %d -> Xoff: %d, Yoff: %d\n", index, tileX, tileY);

            }  */



            // else {
            //     printf("   ");
            // }
			// if ((j + 1) % width == 0) printf("\n");
		}
	}

    json_decref(root);
}

Tileset *getTileset(Map *pMap) {
    return pMap->layerTiles;
}

//------------Mind Vomit------------------

xmlChar *getColumns(Tileset *pTileset) {
    return pTileset->columns;
}

SDL_Rect getTilePosition(Map *pMap) {


    for (size_t j = 0; j < pMap->size; j++) {

        if ((int)json_integer_value(json_array_get(pMap->data, j)) >= 15) {
            int index = (int)json_integer_value(json_array_get(pMap->data, j)) - 15;
            int columns = atoi((char *)getColumns(getTileset(pMap)));

            int tileX = (index % columns) * 32;
            int tileY = (index / columns) * 32;
        }
    }
}

SDL_Rect getMapPosition() {

}

/* 
int main() {
    Tileset devTiles;
    parseTileset("resources/mapData/devTiles.tsx", &devTiles);

    return 0;
} 
    ss*/
