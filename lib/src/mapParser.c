#include "mapParser.h"
#include "main.h"

// nu berhöver vi inte libxml.h längre för att läsa filen <3



/// man  kan läsa filen två gången om man vill fixa allt outomatiska istället för att hårdkoda namn och typ m.m (Orkar inte fixa det ;) )
void loadMap(struct Game *pGame) { 

    pGame->pMap = (Map*)calloc(1, sizeof(Map));
    if (!pGame->pMap) {
        printf(" 1  \n");
        return; 
    }

    const char* image_path = "lib/resources/mapData2/platformSheetNr1.png";
    pGame->pMap->mapSheetImage.filePath = strdup(image_path);
    if (!pGame->pMap->mapSheetImage.filePath) {
        printf(" 2  \n");
    }

    FILE* file = fopen("lib/resources/mapData2/mapNr1.tmj", "r");

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileSize + 1);
    size_t readSize = fread(buffer, 1, fileSize, file);
    buffer[readSize] = '\0';
    fclose(file);
    json_t* root = json_loads(buffer, 0, NULL);
    
    free(buffer);
    json_t* layers = json_object_get(root, "layers"); 

    pGame->pMap->blocks = (Block*)calloc(MAX_BLOCKS, sizeof(Block));
    if (!pGame->pMap->blocks) {
        printf(" 3  \n");
        return;
    }

    pGame->pMap->layerNr1_tile.data = (int*)malloc(60 * 34 * sizeof(int)); 
    memset(pGame->pMap->layerNr1_tile.data, 0, 60 * 34 * sizeof(int)); 
    pGame->pMap->blockCount = 0;
  
    json_t* layer = json_array_get(layers, 0); // det är 0 för att jag vet att jag har bara 2. layer och objects folder
    const char* layerType = json_string_value(json_object_get(layer, "type")); 
    const char* layerName = json_string_value(json_object_get(layer, "name")); 
    if (layerType && strcmp(layerType, "tilelayer") == 0) { 
        pGame->pMap->layerNr1_tile.width = json_integer_value(json_object_get(layer, "width"));
        pGame->pMap->layerNr1_tile.height = json_integer_value(json_object_get(layer, "height"));

        size_t layerDataSize = (size_t)pGame->pMap->layerNr1_tile.width * pGame->pMap->layerNr1_tile.height;
        pGame->pMap->layerNr1_tile.data = (int*)malloc(layerDataSize * sizeof(int)); 
        memset(pGame->pMap->layerNr1_tile.data, 0, layerDataSize * sizeof(int)); 

        json_t* data = json_object_get(layer, "data");
        if (!json_is_array(data)) printf(" 4  \n"); 
        size_t dataSize = json_array_size(data); 
        if (layerName && strcmp(layerName, "layerNr1_tile") == 0) { 
            for (int radY = 0; radY < pGame->pMap->layerNr1_tile.height; radY++) {
                for (int kolumnX = 0; kolumnX < pGame->pMap->layerNr1_tile.width; kolumnX++) {
                    int DataArrayIndex = (( radY * pGame->pMap->layerNr1_tile.width ) + ( kolumnX));
                    json_t* DataArrayIndexJson = json_array_get(data, (size_t)(DataArrayIndex)); 
                    if (json_is_integer(DataArrayIndexJson)) 
                    { 
                        int sheetIndex = (int)json_integer_value(DataArrayIndexJson); 
                        pGame->pMap->layerNr1_tile.data[DataArrayIndex] = sheetIndex; 
                        if (sheetIndex > 0) {
                            if (pGame->pMap->blockCount < MAX_BLOCKS) { 
                                pGame->pMap->blocks[pGame->pMap->blockCount].blockId = sheetIndex; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].x = kolumnX * TILE_SIZE; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].y = radY * TILE_SIZE; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].origX = kolumnX * TILE_SIZE; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].origY = radY * TILE_SIZE; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].type = -1;         // läggas till när det loppar igenom object_folder
                                pGame->pMap->blocks[pGame->pMap->blockCount].direction = 1;      // ändras senare i röralse när den når max range
                                pGame->pMap->blocks[pGame->pMap->blockCount].speed = 120;      
                                pGame->pMap->blocks[pGame->pMap->blockCount].moveRange = 100; 
                                pGame->pMap->blocks[pGame->pMap->blockCount].active = -1;   // läggas till när det loppar igenom object_folder och hittar en matchande x och y
                                pGame->pMap->blocks[pGame->pMap->blockCount].groupId = -1;   // läggas till när det loppar igenom object_folder för att länka visa blovk tillsmans
                                pGame->pMap->blockCount++; 
                            }
                        }
                    }
                }
            }
        }
        json_t* Object_Folder = json_array_get(layers, 1);
        const char* Object_FolderType = json_string_value(json_object_get(Object_Folder, "type"));  // "Group"
        const char* Object_FolderName = json_string_value(json_object_get(Object_Folder, "name"));   // "Object_Folder"
        json_t* Object_Folder_Layers = json_object_get(Object_Folder, "layers");
        size_t Object_Folder_Layers_size = json_array_size(Object_Folder_Layers);
        char Object_Folder_Layers_name[7][50] ={"LayerNr2_objectStatic", "LayerNr3_objectHorizontal", "LayerNr4_objectVertical", "LayerNr5_objectLift", "LayerNr6_objectDrop", "LayerNr7_objectSwitch"}; 
        for (size_t i = 0; i < Object_Folder_Layers_size; i++) {
            json_t* Object_Folder_Layer_index = json_array_get(Object_Folder_Layers, i);
            const char* Object_Folder_Layer_index_name = json_string_value(json_object_get(Object_Folder_Layer_index, "name"));
            json_t* Object_Folder_Objects_array = json_object_get(Object_Folder_Layer_index, "objects"); 
            size_t Object_Folder_Size_array = json_array_size(Object_Folder_Objects_array);
            if (strcmp(Object_Folder_Layer_index_name, Object_Folder_Layers_name[i]) == 0) {
                int type = i;   // typen av platform hittas innan den hitar vilken group link som dem trigas tillsammans med
                for (size_t j = 0; j < Object_Folder_Size_array; j++) {   
                    json_t* Object_Folder_Object_array = json_array_get(Object_Folder_Objects_array, j);
                    int GroupId = atoi(json_string_value(json_object_get(Object_Folder_Object_array, "type")));

                    float x = json_number_value(json_object_get(Object_Folder_Object_array, "x"));
                    float y = json_number_value(json_object_get(Object_Folder_Object_array, "y"));

                    float width = json_number_value(json_object_get(Object_Folder_Object_array, "width"));
                    float height = json_number_value(json_object_get(Object_Folder_Object_array, "height"));

                    // Om en Polygon finns så kommer w och h inte ha någon värde
                    json_t* polygon = json_object_get(Object_Folder_Object_array, "polygon");

                    
                    if (json_is_array(polygon)) {
                        float minX = 0, maxX = 0, minY = 0, maxY = 0;
                        size_t polygonSize = json_array_size(polygon);
                        for (size_t k = 0; k < polygonSize; k++) {
                            json_t* point = json_array_get(polygon, k);
                            float px = json_number_value(json_object_get(point, "x"));
                            float py = json_number_value(json_object_get(point, "y"));

                            // här begresnar jag område som ska söka i om den har maatchamde x och y då man har en träff därför är det viktigt när man rittar mapensägg till att pologon hela hitboxen inte krokar med en annan annars kan ponkten läsas till en annanstans
                            if (k == 0 || px < minX) minX = px;
                            if (k == 0 || px > maxX) maxX = px;
                            if (k == 0 || py < minY) minY = py;
                            if (k == 0 || py > maxY) maxY = py;
                        }

                        float startX = minX + x;
                        float startY = minY + y;
                        float endX = maxX + x;
                        float endY = maxY + y;

                        for (float Xvalue = startX; Xvalue < endX; Xvalue += TILE_SIZE) {
                            for (float Yvalue = startY; Yvalue < endY; Yvalue += TILE_SIZE) {
                                for (int s = 0; s < pGame->pMap->blockCount; s++) {
                                    if (pGame->pMap->blocks[s].x == Xvalue && pGame->pMap->blocks[s].y == Yvalue && pGame->pMap->blocks[s].active < 0) {
                                        pGame->pMap->blocks[s].groupId = GroupId;
                                        pGame->pMap->blocks[s].type = type;
                                        pGame->pMap->blocks[s].active = 1;
                                    }
                                }
                            }
                        }
                    }else{
                        // om det inte finns en polygon så kommer loopen gå direkt efter w och h
                        for (float yi = y; yi < height + y; yi += TILE_SIZE) {
                            for (float xj = x; xj < width + x; xj += TILE_SIZE) {  
                                for (int i = 0; i < pGame->pMap->blockCount; i++) {
                                    if (pGame->pMap->blocks[i].x == xj && pGame->pMap->blocks[i].y == yi && pGame->pMap->blocks[i].active < 0) {
                                        pGame->pMap->blocks[i].groupId = GroupId;
                                        pGame->pMap->blocks[i].type = type;
                                        pGame->pMap->blocks[i].active = 1;
                                    }
                                }
                            }
                        }
                    }   
                }
            }
   
        }
    }

    json_decref(root);

}

//// server

Map* createMap_Server(const char* filePath) {

    Map* pMap = (Map*)calloc(1, sizeof(Map));
 
    FILE* file = fopen(filePath, "r");

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileSize + 1);
    size_t readSize = fread(buffer, 1, fileSize, file);
    buffer[readSize] = '\0';

    fclose(file);

    json_t* root = json_loads(buffer, 0, NULL);
    free(buffer);

    json_t* layers = json_object_get(root, "layers");


    json_t* tileLayerNode = NULL;
    size_t numLayers = json_array_size(layers);


    for (size_t i = 0; i < numLayers; ++i) {
        json_t* currentLayer = json_array_get(layers, i);
        const char* type = json_string_value(json_object_get(currentLayer, "type"));
        if (type && strcmp(type, "tilelayer") == 0) {
            tileLayerNode = currentLayer; 
            break;
        }
    }

    if (tileLayerNode) {
        pMap->layerNr1_tile.width = json_integer_value(json_object_get(tileLayerNode, "width"));
        pMap->layerNr1_tile.height = json_integer_value(json_object_get(tileLayerNode, "height"));
        size_t dataSize = (size_t)pMap->layerNr1_tile.width * pMap->layerNr1_tile.height;
        
        pMap->layerNr1_tile.data = (int*)malloc(dataSize * sizeof(int));

        memset(pMap->layerNr1_tile.data, 0, dataSize * sizeof(int));

        json_t* data = json_object_get(tileLayerNode, "data");

        if (json_is_array(data)) {
       
            for (size_t i = 0; i < dataSize; ++i) {
                json_t* tileValue = json_array_get(data, i);
                if (json_is_integer(tileValue)) {
                    pMap->layerNr1_tile.data[i] = (int)json_integer_value(tileValue);
                }
            }

        }
    }

    pMap->blocks = (Block*)calloc(MAX_BLOCKS, sizeof(Block));

    pMap->blockCount = 0;
    for (size_t i = 0; i < numLayers; ++i) {
        json_t* currentLayer = json_array_get(layers, i);
        const char* type = json_string_value(json_object_get(currentLayer, "type"));
        if (type && strcmp(type, "objectgroup") == 0) {

            json_t* objects = json_object_get(currentLayer, "objects");
            if (json_is_array(objects)) {
                size_t numObjects = json_array_size(objects);
                for (size_t j = 0; j < numObjects; ++j) {
                    if (pMap->blockCount >= MAX_BLOCKS) break;
                    json_t* object = json_array_get(objects, j);
                    // innan de rör sig så har dem samma värdet. server måste updateras efter movable x och y för att nu det rätter till efter orig x och y (Jag vet inte varför det är så)
                    // formodingen måste man också ändra så att server får tag i range och hastighet men min tanke var att server ska ha en kopia direkt av mapen istället för att inisaliter sitt eget
                    pMap->blocks[pMap->blockCount].origX = pMap->blocks[pMap->blockCount].x =  (float)json_number_value(json_object_get(object, "x"));
                    pMap->blocks[pMap->blockCount].origY = pMap->blocks[pMap->blockCount].y =  (float)json_number_value(json_object_get(object, "y"));
                }
            }

        }

    }
    json_decref(root);
    return pMap;
}

void destroyMap(Map* pMap) {
    if (!pMap) return;

    if (pMap->mapSheetImage.texture) {
        SDL_DestroyTexture(pMap->mapSheetImage.texture);
        pMap->mapSheetImage.texture = NULL;
    }
    if (pMap->layerNr1_tile.data) {
        free(pMap->layerNr1_tile.data);
        pMap->layerNr1_tile.data = NULL;
    }
    if (pMap->blocks) {
        free(pMap->blocks);
        pMap->blocks = NULL;
    }

    free(pMap);
}


