#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

typedef struct tileset{
    xmlChar *imgRef;
    xmlChar *tilecount;
    xmlChar *columns;
    xmlChar *width;
    xmlChar *height;

} tileSet;

void parse_tileset(const char *filename, tileSet *devTiles) {
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

int main() {
    tileSet devTiles;
    parse_tileset("devTiles.tsx", &devTiles);
    json_error_t error;
    json_t *root = json_load_file("map.tmj", 0, &error);

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
		json_t *data = json_object_get(layer, "data");
		if (!json_is_array(data)) {
			fprintf(stderr, "\"data\" is not an array\n");
			json_decref(root);
			return 1;
		}
	
		size_t size = json_array_size(data);
		printf("Tile data (%zu tiles):\n", size);
	
		for (size_t j = 0; j < size; j++) {
			json_t *tile = json_array_get(data, j);
			if (!json_is_integer(tile)) {
				printf("Invalid value at %zu\n", j);
				continue;
			}
            if ((int)json_integer_value(tile) >= 15){
                // printf("%2d ", (int)json_integer_value(tile));
                int index = (int)json_integer_value(tile) - 15;
                int columns = atoi((char *)devTiles.columns);

                int tileX = (index % columns) * 32;
                int tileY = (index / columns) * 32;

                printf("indx: %d -> Xoff: %d, Yoff: %d\n", index, tileX, tileY);

            } 
            // else {
            //     printf("   ");
            // }
			// if ((j + 1) % width == 0) printf("\n");
		}
	}

    json_decref(root);
    return 0;
}
