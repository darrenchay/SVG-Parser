/* Name: Darren Chay Loong
 * ID: 1049254
 * Email: tchayloo@uoguelph.ca
 */

#include "SVGParser.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>



/* Function definitions */
SVGimage *initSVGimage(); 
void loadSVGimage(xmlNode * a_node, SVGimage *image);
Attribute *createAttribute(xmlAttr *attributeNode); 
void createRect(xmlNode *node, List *list);
void createCircle(xmlNode *node, List *list);
void createPath(xmlNode *node, List *list);
void createGroup(xmlNode *node, List *list); 
void searchGroupRect(List *list, Group *group);
void searchGroupCircle(List *list, Group *group);
void searchGroupPaths(List *list, Group *group);
void searchGroupGroups(List *list, Group *group);
int findAttributesFromGroups(List *listGroup);
int findAttributesFromElement(List *list, int caseNo);
int searchGroup (List *list, void *area, int type);
bool compareLen(const void *first, const void *second);
int search(List* list, void *area, int type);
void deleteListGetter(void *data);

bool checkAttributes(List* list);
bool checkRect(List* list);
bool checkCircle(List* list);
bool checkPath(List* list);
bool checkGroup(List* list);

xmlDoc* convertSVGimageToXMLdoc (SVGimage* image);
bool validateXMLtree(xmlDoc* doc, char* schemaFile);

void addAttributesToXMLnode (List* attributeList, xmlNode* node);
void addRectanglesToXMLnode(List* rectList, xmlNode* rootNode);
void addPathsToXMLnode(List* pathList, xmlNode* rootNode);
void addCirclesToXMLnode(List* circList, xmlNode* rootNode);
void addGroupsToXMLnode(List* groupList, xmlNode* rootNode); 
void addAttribute(List* attributes, Attribute* newAttribute);

/* Getters to JSON */
char* readSVGtoJSON(char* fileName, char* schemaFile);
char* SVGdetailsToJSON(char* fileName, char* schemaFile);
char* getRectJSONlist(char* fileName, char* schemaFile);
char* getCircJSONlist(char* fileName, char* schemaFile);
char* getPathJSONlist(char* fileName, char* schemaFile);
char* getGroupJSONlist(char* fileName, char* schemaFile);
char* getAttrJSONlist(char* fileName, char* schemaFile, int index, int type);
int writeJSONSVGtoSVGFile(char* svgJSON, char* fileName, char* schemaFile);
int addShapeToSVGFile(char* fileName, char* schemaFile, char* JSONstring, int type);
int writeExistingShapeFromJSON(char* fileName, char* schemaFile, char* JSONstring, int type, int index);
int writeAttrFromJSON(char* fileName, char* schemaFile, int index, int type, char* JSONattr); 
Attribute* JSONtoAttr(const char* JSONstring);
int updateCircle(Circle* oldCirc, Circle* newCirc);
int updateRect(Rectangle* oldRect, Rectangle* newRect);
int scaleShape(char* fileName, char* schemaFile, char* scaleFactorString, int type);

/** Function to create an SVG object based on the contents of an SVG file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createSVGimage(char* fileName) {
    if(fileName == NULL) {
        return NULL;
    }
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *image = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        //printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    if(root_element == NULL) {
        return NULL;
    }

    image = initSVGimage();

    loadSVGimage(root_element, image);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return image;
}

/* Creates an SVGimage struct */
SVGimage *initSVGimage() {
    SVGimage *image = malloc(sizeof(SVGimage));

    image->namespace[0] = '\0';
    image->title[0] = '\0';
    image->description[0] = '\0';

    image->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    if(image->rectangles == NULL) {
        return NULL;
    }
    image->circles = initializeList(circleToString, deleteCircle, compareCircles);
    if(image->circles == NULL) {
        return NULL;
    }
    image->paths = initializeList(pathToString, deletePath, comparePaths);
    if(image->paths == NULL) {
        return NULL;
    }
    image->groups = initializeList(groupToString, deleteGroup, compareGroups);
    if(image->groups == NULL) {
        return NULL;
    }
    image->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    if(image->otherAttributes == NULL) {
        return NULL;
    }

    return image;
}

/* Loads content of xml into SVGimage */
void loadSVGimage(xmlNode * a_node, SVGimage *image) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        /* Writing title to svg image */
        if(strcasecmp((char *)cur_node->name,"title")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) {
            /* Truncates if > 255 */
            if(strlen((char *)cur_node->children->content) > 255) {
                char *tempString = malloc(sizeof(char) * 256);
                strncpy(tempString, (char *)cur_node->children->content, 255);
                tempString[255] = '\0';
                strcpy(image->title, tempString);
                free(tempString);
            } else {
                strcpy(image->title, (char *)cur_node->children->content);
            }
        } else if(strcasecmp((char *)cur_node->name,"desc")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) { /* Writing desc to svg image */
            /* Truncates if > 255 */
            if(strlen((char *)cur_node->children->content) > 255) {
                char *tempString = malloc(sizeof(char) * 256);
                strncpy(tempString, (char *)cur_node->children->content, 255);
                tempString[255] = '\0';
                strcpy(image->description, tempString);
                free(tempString);
            } else {
                strcpy(image->description, (char *)cur_node->children->content);
            }
        } else if(strcasecmp((char *)cur_node->name,"rect")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) { /* Adding rect to svg image */
            createRect(cur_node, image->rectangles);

        } else if (strcasecmp((char *)cur_node->name,"circle")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) { /* Adding circle to svg image */
            createCircle(cur_node, image->circles);

        } else if (strcasecmp((char *)cur_node->name,"path")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) { /* Adding path to svg image */
            createPath(cur_node, image->paths);

        } else if (strcasecmp((char *)cur_node->name,"g")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) { /* Adding group to svg image */
            createGroup(cur_node, image->groups);

        } else if (strcasecmp((char *)cur_node->name,"svg")==0) { /* Adding svg attributes */
            /* Checks if there is a namespace, if there is add it to SVG struct */
            if(cur_node->ns) {
                if(strlen((char *)cur_node->ns->href) > 255) {
                    char *tempString = malloc(sizeof(char) * 256);
                    strncpy(tempString, (char *)cur_node->ns->href, 255);
                    tempString[255] = '\0';
                    strcpy(image->namespace, tempString);
                    free(tempString);
                } else {
                    strcpy(image->namespace, (char *)cur_node->ns->href);
                }
            }
            
            xmlAttr *attr;
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                Attribute *attribute = createAttribute(attr);
                insertBack(image->otherAttributes, attribute);
            }
        } 
        
        loadSVGimage(cur_node->children, image);
    }
}

Attribute *createAttribute(xmlAttr *attributeNode) {
    Attribute *attribute = malloc(sizeof(Attribute));

    xmlNode *value = attributeNode->children;
    char *attrName = (char *)attributeNode->name;
    char *cont = (char *)(value->content);

    attribute->name = malloc(strlen(attrName) + 1);
    if(attribute->name == NULL) {
        return NULL;
    }
    attribute->value = malloc(strlen(cont) + 1);
    if(attribute->value == NULL) {
        return NULL;
    }

    strcpy(attribute->name, attrName);
    strcpy(attribute->value, cont);
    /* printf("\tattribute name: %s, attribute value = %s\n", attrName, cont); */

    return attribute;

}

void createRect(xmlNode *node, List *list) {
    Rectangle *rect = malloc(sizeof(Rectangle));

    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    rect->units[0] = '\0';

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        char *buffer;
        if(strcasecmp((char *)attr->name, "x") == 0) {
            /* strcpy */
            rect->x = strtod((const char *)attr->children->content, &buffer);
            if(strlen(rect->units) == 0) {
                strcpy(rect->units, buffer);
            }
        } else if(strcasecmp((char *)attr->name, "y") == 0) {
            /* strcpy */
            rect->y= strtod((const char *)attr->children->content, &buffer);
            if(strlen(rect->units) == 0) {
                strcpy(rect->units, buffer);
            }
        } else if(strcasecmp((char *)attr->name, "width") == 0) {
            /* strcpy */
            rect->width = strtod((const char *)attr->children->content, &buffer);
            if(strlen(rect->units) == 0) {
                strcpy(rect->units, buffer);
            }
        } else if(strcasecmp((char *)attr->name, "height") == 0) {
            /* strcpy */
            rect->height = strtod((const char *)attr->children->content, &buffer);
            if(strlen(rect->units) == 0) {
                strcpy(rect->units, buffer);
            }
        } else {
            Attribute *attribute = createAttribute(attr);
            insertBack(rect->otherAttributes, attribute);
        }
    }
    insertBack(list, rect);
}

void createCircle(xmlNode *node, List *list) {
    Circle *circle = malloc(sizeof(Circle));

    circle->cx = 0;
    circle->cy = 0;
    circle->r = 0;
    circle->units[0] = '\0';
    circle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        char *buffer;
        if(strcasecmp((char *)attr->name, "cx") == 0) {
            circle->cx = strtod((const char *)attr->children->content, &buffer);
            if(strlen(circle->units) == 0) {
                strcpy(circle->units, buffer);
            }
        } else if(strcasecmp((char *)attr->name, "cy") == 0) {
            circle->cy= strtod((const char *)attr->children->content, &buffer);
            if(strlen(circle->units) == 0) {
                strcpy(circle->units, buffer);
            }
        } else if(strcasecmp((char *)attr->name, "r") == 0) {
            circle->r = strtod((const char *)attr->children->content, &buffer);
            if(strlen(circle->units) == 0) {
                strcpy(circle->units, buffer);
            }
        } else {
            Attribute *attribute = createAttribute(attr);
            insertBack(circle->otherAttributes, attribute);
        }
    }
    if(circle->cx < 0 || circle->cy < 0 || circle->r < 0) {
        deleteCircle(circle);
    } else {
        insertBack(list, circle);
    }
}

void createPath(xmlNode *node, List *list) {
    Path *path = malloc(sizeof(Path));

    path->data = malloc(sizeof(char) * 10);
    strcpy(path->data, "\0");
    path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        if(strcasecmp((char *)attr->name, "d") == 0) {
            path->data = realloc(path->data, strlen((char *)attr->children->content) + 1);
            strcpy(path->data, (char *)attr->children->content);

        } else {
            Attribute *attribute = createAttribute(attr);
            insertBack(path->otherAttributes, attribute);
        }
    }
    insertBack(list, path);
}

void createGroup(xmlNode *node, List *list) {
    Group *group = malloc(sizeof(Group));

    group->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    group->circles = initializeList(circleToString, deleteCircle, compareCircles);
    group->paths = initializeList(pathToString, deletePath, comparePaths);
    group->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    group->groups = initializeList(groupToString, deleteGroup, compareGroups);

    xmlNode *cur_node = NULL;
    for (cur_node = node->children; cur_node != NULL; cur_node = cur_node->next) {
        if(strcasecmp((char *)cur_node->name,"rect")==0) { /* Adding rect to svg image */
            createRect(cur_node, group->rectangles);

        } else if (strcasecmp((char *)cur_node->name,"circle")==0) { /* Adding circle to svg image */
            createCircle(cur_node, group->circles);

        } else if (strcasecmp((char *)cur_node->name,"path")==0) { /* Adding path to svg image */
            createPath(cur_node, group->paths);

        } else if (strcasecmp((char *)cur_node->name,"g")==0) { /* Adding group to svg image */
            createGroup(cur_node, group->groups);
        }
    }
    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        Attribute *attribute = createAttribute(attr);
        insertBack(group->otherAttributes, attribute);
    }
    insertBack(list, group);
}




/** Function to create a string representation of an SVG object.
 *@pre SVGimgage exists, is not null, and is valid
 *@post SVGimgage has not been modified in any way, and a string representing the SVG contents has been created
 *@return a string contaning a humanly readable representation of an SVG object
 *@param obj - a pointer to an SVG struct
**/
char* SVGimageToString(SVGimage* img) {
    if(img == NULL) {
        return "";
    }
    char *imageString = malloc(sizeof(char) * 2048);
    int bufferLen = 2048;

    strcpy(imageString, "Namespace: ");
    strcat(imageString, img->namespace);
    strcat(imageString, "\nTitle: ");
    strcat(imageString, img->title);
    strcat(imageString, "\nDescription: ");
    strcat(imageString, img->description);

    strcat(imageString, "\n\n");
    char *rectString = toString(img->rectangles);
    bufferLen += strlen(rectString);
    imageString = realloc(imageString, bufferLen);
    strcat(imageString, rectString);
    free(rectString);

    strcat(imageString, "\n");
    char *circleString = toString(img->circles);
    bufferLen += strlen(circleString);
    imageString = realloc(imageString, bufferLen);
    strcat(imageString, circleString);
    free(circleString);

    strcat(imageString, "\n");
    char *pathString = toString(img->paths);
    bufferLen += strlen(pathString);
    imageString = realloc(imageString, bufferLen);
    strcat(imageString, pathString);
    free(pathString);

    strcat(imageString, "\n");
    char *groupString = toString(img->groups);
    bufferLen += strlen(groupString);
    imageString = realloc(imageString, bufferLen);
    strcat(imageString, groupString);
    free(groupString);

    strcat(imageString, "\nOther Attributes: \n");
    char *attributeString = toString(img->otherAttributes);
    bufferLen += strlen(attributeString);
    imageString = realloc(imageString, bufferLen);
    strcat(imageString, attributeString);
    free(attributeString);
    
    return imageString;
}

/** Function to delete image content and free all the memory.
 *@pre SVGimgage  exists, is not null, and has not been freed
 *@post SVGimgage  had been freed
 *@return none
 *@param obj - a pointer to an SVG struct
**/
void deleteSVGimage(SVGimage* img) {
    if(img == NULL) {
        return;
    }
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    freeList(img->otherAttributes);
    free(img);
}

/* For the four "get..." functions below, make sure you return a list of opinters to the existing structs 
 - do not allocate new structs.  They all share the same format, and only differ in the contents of the lists 
 they return.
 
 *@pre SVGimgage exists, is not null, and has not been freed
 *@post SVGimgage has not been modified in any way
 *@return a newly allocated List of components.  While the List struct itself is new, the components in it are just pointers
  to the ones in the image.

 The list must me empty if the element is not found - do not return NULL

 *@param obj - a pointer to an SVG struct
 */

// Function that returns a list of all rectangles in the image.  
List* getRects(SVGimage* img) {
    List *rectList = initializeList(rectangleToString, deleteListGetter, compareRectangles);

    if(img == NULL || img->rectangles == NULL) {
        return rectList;
    }

    /* Gets all rectangles from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->rectangles);

    while((elem = nextElement(&iter)) != NULL) {
        Rectangle *rect = (Rectangle *)elem;

        insertBack(rectList, rect);
    }

    /* Searches down groups for all subsequent rectangles */
    ListIterator iterGroups = createIterator(img->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupRect(rectList, group);

    }

    return rectList;
}

// Function that returns a list of all circles in the image.  
List* getCircles(SVGimage* img) {
    List *circleList = initializeList(circleToString, deleteListGetter, compareCircles);

    if(img == NULL || img->circles == NULL) {
        return circleList;
    }

    /* Gets all circles from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->circles);

    while((elem = nextElement(&iter)) != NULL) {
        Circle *circle = (Circle *)elem;
        insertBack(circleList, circle);
    }

    /* Searches down groups for all subsequent circles */
    ListIterator iterGroups = createIterator(img->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupCircle(circleList, group);

    }

    return circleList;
}

// Function that returns a list of all groups in the image.  
List* getGroups(SVGimage* img) {
    void *elem;
    List * groupList = initializeList(groupToString, deleteListGetter, compareGroups);

    if(img == NULL || img->groups == NULL) {
        return groupList;
    }
    /* Searches down groups for all subsequent groups */
    ListIterator iterGroups = createIterator(img->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        insertBack(groupList, group);
        searchGroupGroups(groupList, group);
    }

    return groupList;
}

// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img) {
    List *pathList = initializeList(pathToString, deleteListGetter, comparePaths);

    if(img == NULL || img->paths == NULL) {
        return pathList;
    }

    /* Gets all paths from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->paths);

    while((elem = nextElement(&iter)) != NULL) {
        Path *path = (Path *)elem;
        insertBack(pathList, path);
    }

    /* Searches down groups for all subsequent paths */
    ListIterator iterGroups = createIterator(img->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupPaths(pathList, group);

    }
    return pathList;
}

/* Recursively goes through groups to find elements and add to list */
void searchGroupRect(List *list, Group *group) {
    void *elem;
    if(group->rectangles == NULL) {
        return;
    }
    /* Creates iterator to go through list of rectangles in that group */
    ListIterator iterRects = createIterator(group->rectangles);
    while((elem = nextElement(&iterRects)) != NULL) {
        Rectangle *rect = (Rectangle *)elem;
        insertBack(list, rect);
    }

    /* Searches down groups for all subsequent rectangles */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupRect(list, group);
    }
}

void searchGroupCircle(List *list, Group *group) {
    void *elem;
    if(group->circles == NULL) {
        return;
    }
    /* Creates iterator to go through list of circles in that group */
    ListIterator iter = createIterator(group->circles);
    while((elem = nextElement(&iter)) != NULL) {
        Circle *circle = (Circle *)elem;
        insertBack(list, circle);
    }

    /* Searches down groups for all subsequent circles */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupCircle(list, group);
    }
}

void searchGroupPaths(List *list, Group *group) {
    void *elem;
    if(group->paths == NULL) {
        return;
    }
    /* Creates iterator to go through list of paths in that group */
    ListIterator iter = createIterator(group->paths);
    while((elem = nextElement(&iter)) != NULL) {
        Path *path = (Path *)elem;
        insertBack(list, path);
    }

    /* Searches down groups for all subsequent paths */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupPaths(list, group);
    }

}

void searchGroupGroups(List *list, Group *group) {
    void *elem;
    if(group->groups == NULL) {
        return;
    }
    /* Searches down groups for all subsequent groups */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        insertBack(list, group);
        searchGroupGroups(list, group);
    }
}

/* For the four "num..." functions below, you need to search the SVG image for components  that match the search 
  criterion.  You may wish to write some sort of a generic searcher fucntion that accepts an image, a predicate function,
  and a dummy search record as arguments.  We will discuss such search functions in class

 NOTE: For consistency, use the ceil() function to round the floats up to the nearest integer once you have computed 
 the number you need.  See A1 Module 2 for details.

 *@pre SVGimgage exists, is not null, and has not been freed.  The search criterion is valid
 *@post SVGimgage has not been modified in any way
 *@return an int indicating how many objects matching the criterion are contained in the image
 *@param obj - a pointer to an SVG struct
 *@param 2nd - the second param depends on the function.  See details below
 */   

// Function that returns the number of all rectangles with the specified area
int numRectsWithArea(SVGimage* img, float area) {
    int count = 0;

    if(img == NULL || area < 0) {
        return 0;
    }

    count += search(img->rectangles, (void *)&area, 1);
    count += searchGroup(img->groups, (void *)&area, 1);
    
    return count;
}

// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
    int count = 0;

    if(img == NULL || area < 0) {
        return 0;
    }

    count += search(img->circles, (void *)&area, 2);
    count += searchGroup(img->groups, (void *)&area, 2);
    return count;
}
// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {
    int count = 0;
    if(img == NULL || data == NULL || strlen(data) == 0) {
        return 0;
    }

    count += search(img->paths, (void *)data, 3);
    count += searchGroup(img->groups, (void *)data, 3);
    return count;

}
// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len) {
    int count = 0;
    if(img == NULL || len < 0) {
        return 0;
    }

    count += search(img->groups, (void *)&len, 4);
    count += searchGroup(img->groups, (void *)&len, 4);
    return count;
}



int searchGroup (List *list, void *key, int type) {
    int count = 0;
    void *elem;

    /* Searches down groups for all subsequent groups */
    ListIterator iterGroups = createIterator(list);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        if(type == 1) {
            count += search(group->rectangles, key, type);
        } else if(type == 2) {
            count += search(group->circles, key, type);
        } else if (type == 3) {
            count += search(group->paths, key, type);
        } else {
            count += search(group->groups, key, type);
        }
        count += searchGroup(group->groups, key, type);
    }
    return count;
}

int search(List* list, void *key, int type){
    int count = 0;
    void *elem;

    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        if(type == 1) {
            /* Counting rectangles with same area */
            Rectangle *rect = (Rectangle *)elem;
            int areaRect = ceil(rect->width * rect->height);
            if(areaRect == ceil(*(float *)key)) {
                count ++;
            }
        } else if(type == 2){
            /* Counting circles with same area */
            Circle *circle = (Circle *)elem;
            int areaCircle = ceil(circle->r * circle->r * 3.14159265);
            int areaKey = ceil(*(float *)key);
            if(areaCircle == areaKey) {
                count++;
            }
        } else if (type == 3) {
            /* Counting paths with data equal to key */
            Path *path = (Path *)elem;
            //printf("======================\nPath data: |%s| \n key: |%s|\n======================\n", path->data, (char *)key);
            if(strcasecmp(path->data, (char *)key) == 0) {
                count ++;
            }
        } else {
            Group *group = (Group *)elem;
            int tempCount = 0;
            tempCount += getLength(group->rectangles);
            tempCount += getLength(group->circles);
            tempCount += getLength(group->groups);
            tempCount += getLength(group->paths);
            //printf("======================\nGroup count: %d \n key: %d\n======================\n", tempCount, *(int *)key);
            if(tempCount == *(int *)key) {
                count ++;
            }

        }
    }
    return count;
}


/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.  
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img) {
    int count = 0;
    if(img == NULL) {
        return 0;
    }

    //printf("|%s|\n\n=========================================\n\n", SVGimageToString(img));

    /* Gets num of attributes for each root element and svg element then recursively checks groups */
    count += findAttributesFromElement(img->rectangles, 1);
    count += findAttributesFromElement(img->circles, 2);
    count += findAttributesFromElement(img->paths, 3);
    count += getLength(img->otherAttributes);
    count += findAttributesFromGroups(img->groups);
    return count;
}

int findAttributesFromGroups(List *listGroup) {
    int count = 0;
    void *elem;
    
    /* Searches down groups for all subsequent groups, incrementing count for each element's other attributes list */
    ListIterator iterGroups = createIterator(listGroup);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;

        count += findAttributesFromElement(group->rectangles, 1);
        count += findAttributesFromElement(group->circles, 2);
        count += findAttributesFromElement(group->paths, 3);
        count += getLength(group->otherAttributes);
        count += findAttributesFromGroups(group->groups);
    }
    return count;
}

int findAttributesFromElement(List *list, int caseNo) {
    int count = 0;
    void *elem;
    
    /* Iterates through the list of elements and gets the no of other attributes for that element */
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        /* Finds num attributes for respective element and increments count */
        if(caseNo == 1) {
            Rectangle *rect = (Rectangle *)elem;
            count += getLength(rect->otherAttributes);
            //printf("Rect Attributes: %s\n", toString(rect->otherAttributes));
        } else if(caseNo == 2) {
            Circle *circle = (Circle *)elem;
            count += getLength(circle->otherAttributes);
            //printf("Circ Attributes: %s\n", toString(circle->otherAttributes));
        } else {
            Path *path = (Path *)elem;
            count += getLength(path->otherAttributes);
            //printf("path Attributes: %s\n", toString(path->otherAttributes));
        }
    }
    return count;
}

/** Function to validating an existing a SVGimage object against a SVG schema file
 *@pre 
    SVGimage object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post SVGimage has not been modified in any way
 *@return the boolean aud indicating whether the SVGimage is valid
 *@param obj - a pointer to a GPXSVGimagedoc struct
 *@param obj - the name iof a schema file
 **/
bool validateSVGimage(SVGimage* image, char* schemaFile) {
    if (image == NULL || schemaFile == NULL) {
        return false;
    }

    xmlDoc* doc = convertSVGimageToXMLdoc(image);
    if(validateXMLtree(doc, schemaFile) == false) {
        return false;
    }

    bool isValid = checkRect(getRects(image));
    if(isValid == false) {
        return false;
    }
    isValid = checkCircle(getCircles(image));
    if(isValid == false) {
        return false;
    }
    isValid = checkPath(getPaths(image));
    if(isValid == false) {
        return false;
    }
    isValid = checkGroup(getGroups(image));
    if(isValid == false) {
        return false;
    }

    return true;
}

bool checkAttributes(List* list) {
    if(list == NULL) {
        return false;
    }
    void *elem;
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        Attribute* attribute = (Attribute *)elem;
        //printf("checking attribute\n");

        if(attribute->name == NULL || attribute->value == NULL || strlen(attribute->name) == 0 || strlen(attribute->value) == 0 ) {
            return false;
        }
    }
    return true;
}

bool checkRect(List* list) {
    void *elem;
    
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        Rectangle* rect = (Rectangle *)elem;
        //printf("checking rect\n");
        if(rect->x < 0 || rect->y < 0 || rect->width < 0 || rect->height < 0) {
            return false;
        }
        if(!checkAttributes(rect->otherAttributes)) {

            return false;
        }
    }
    
    freeList(list);

    return true;
}

bool checkCircle(List* list) {
    void *elem;
    
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        Circle* circle = (Circle *)elem;
        if(circle->cx < 0 || circle->cy < 0 || circle->r < 0) {
            return false;
        }
        if(!checkAttributes(circle->otherAttributes)) {
            return false;
        }
    }
    freeList(list);
    return true;
}

bool checkPath(List* list) {
    void *elem;
    
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        Path* path = (Path *)elem;
        if(path->data == NULL || strlen(path->data) == 0 || !checkAttributes(path->otherAttributes)) {
            return false;
        }
    }
    freeList(list);
    return true;
}

bool checkGroup(List* list) {
    void *elem;
    
    ListIterator iter = createIterator(list);
    while((elem = nextElement(&iter)) != NULL) {
        Group* group = (Group *)elem;
        if(!checkAttributes(group->otherAttributes)) {
            return false;
        }
    }
    freeList(list);
    return true;
}

/** Function to create an SVG object based on the contents of an SVG file.
 * This function must validate the XML tree generated by libxml against a SVG schema file
 * before attempting to traverse the tree and create an SVGimage struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
       Schema file name is not NULL/empty, and represents a valid schema file
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, or SVG file was invalid, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {
    if(fileName == NULL  || schemaFile == NULL) {
        return NULL;
    }
    xmlDoc *doc = NULL;
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        xmlCleanupParser();
        return NULL;
    }
    if(validateXMLtree(doc, schemaFile) == false) {
        /*free the document */
        //xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    //printf("done validating file\n");
    SVGimage* img = createSVGimage(fileName);
    if(img == NULL) {
        return NULL;
    }
    //printf("created image\n");
    if(validateSVGimage(img, schemaFile) == false) {
        deleteSVGimage(img);
        return NULL;
    }
    //printf("image validated\n");
    return img;
}

/** Function to writing a SVGimage into a file in SVG format.
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post SVGimage has not been modified in any way, and a file representing the
    SVGimage contents in SVG format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a SVGimage struct
 	fileName - the name of the output file
 **/
bool writeSVGimage(SVGimage* image, char* fileName) {
    if(image == NULL || fileName == NULL) {
        return false;
    }
    //convert struct to xmlDoc
    xmlDoc* docToFile = convertSVGimageToXMLdoc(image);
    if(docToFile == NULL) {
        return false;
    }
    //saving doc to file
    int success = xmlSaveFormatFileEnc(fileName, docToFile, "UTF-8", 1);

    //freeing data
    xmlFreeDoc(docToFile);
    xmlCleanupParser();

    if(success == -1) {
        return false;
    } else {
        return true;
    }
    
}

xmlDoc* convertSVGimageToXMLdoc (SVGimage* image) {
    if(image == NULL) {
        return NULL;
    }
    xmlDoc* doc = NULL;
    xmlNode* rootNode = NULL;


    LIBXML_TEST_VERSION;
    
    doc = xmlNewDoc(BAD_CAST "1.0");
    if(doc == NULL) {
        return NULL;
    }
    rootNode = xmlNewNode(NULL, BAD_CAST "svg");
    if(rootNode == NULL) {
        return NULL;
    }
    xmlDocSetRootElement(doc, rootNode);

    //write out the root svg node and adding title, desc and namespace
    if(image->namespace !=NULL) {
        xmlSetNs(rootNode, xmlNewNs(rootNode, (const xmlChar*)image->namespace, NULL));
    }

    if(strlen(image->title) > 1) {
        xmlNewChild(rootNode, NULL, BAD_CAST "title", BAD_CAST image->title);
    }
    if(strlen(image->description) > 1) {
        xmlNewChild(rootNode, NULL, BAD_CAST "desc", BAD_CAST image->description);
    } 
    if(image->otherAttributes != NULL) {
        addAttributesToXMLnode(image->otherAttributes, rootNode);
    }
    //write out any rects that are in the root node
    if(image->rectangles != NULL) {
        addRectanglesToXMLnode(image->rectangles, rootNode);
    }

    //write out any circles in the root
    if(image->circles != NULL) {    
        addCirclesToXMLnode(image->circles, rootNode);
    }

    //write out any paths
    if(image->paths != NULL) {
        addPathsToXMLnode(image->paths, rootNode);
    }
    
    //write out any groups
    if(image->groups != NULL) {
        addGroupsToXMLnode(image->groups, rootNode);
    }
    return doc;
}

bool validateXMLtree(xmlDoc* doc, char* schemaFile) {
    if(doc == NULL || schemaFile == NULL) {
        return false;
    }
    xmlSchema* schemaPtr = NULL;
    xmlSchemaParserCtxt* ctxt = NULL;

    //parse schema file
    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schemaPtr = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xmlSchemaValidCtxtPtr ctxtValid;
    int returnVal;
    ctxtValid = xmlSchemaNewValidCtxt(schemaPtr);
    xmlSchemaSetValidErrors(ctxtValid, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    returnVal = xmlSchemaValidateDoc(ctxtValid, doc);

    //freeing schema and doc
    xmlSchemaFreeValidCtxt(ctxtValid);
    xmlFreeDoc(doc);
    if(schemaPtr != NULL) {
        xmlSchemaFree(schemaPtr);
    }
    xmlSchemaCleanupTypes();
    xmlCleanupParser();

    if(returnVal == 0) {
        return true;
    } else {
        return false;
    }

}

//adds the attributes of an xmlNode
void addAttributesToXMLnode (List* attributeList, xmlNode* node) {
    if(attributeList == NULL || getLength(attributeList) == 0) {
        return;
    }
    void *elem = NULL;
    ListIterator iter = createIterator(attributeList);
    while((elem = nextElement(&iter)) != NULL) {
        Attribute* att = (Attribute *)elem;
        xmlNewProp(node, BAD_CAST att->name, BAD_CAST att->value);
    }
}

void addRectanglesToXMLnode(List* rectList, xmlNode* rootNode) {
    if(rectList == NULL || getLength(rectList) == 0) {
        return;
    }
    void *elem = NULL;
    xmlNode* node = NULL;
    char buffer[1024];
    ListIterator iter = createIterator(rectList);
    while((elem = nextElement(&iter)) != NULL) {
        Rectangle* rect = (Rectangle *)elem;
        node = xmlNewChild(rootNode, NULL, BAD_CAST "rect", NULL);
        sprintf(buffer, "%f%s", rect->x, rect->units);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST buffer);
        sprintf(buffer, "%f%s", rect->y, rect->units);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST buffer);
        sprintf(buffer, "%f%s", rect->width, rect->units);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST buffer);
        sprintf(buffer, "%f%s", rect->height, rect->units);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST buffer);

        addAttributesToXMLnode(rect->otherAttributes, node);
    }
    //printf("done adding rects\n");

}

void addPathsToXMLnode(List* pathList, xmlNode* rootNode) {
    if(pathList == NULL || getLength(pathList) == 0) {
        return;
    }
    void *elem = NULL;
    xmlNode* node = NULL;
    ListIterator iter = createIterator(pathList);
    while((elem = nextElement(&iter)) != NULL) {
        Path* path = (Path *)elem;
        node = xmlNewChild(rootNode, NULL, BAD_CAST "path", NULL);
        xmlNewProp(node, BAD_CAST "d", BAD_CAST path->data);
        addAttributesToXMLnode(path->otherAttributes, node);
    }
    //printf("done adding paths\n");

}

void addCirclesToXMLnode(List* circList, xmlNode* rootNode) {
    if(circList == NULL || getLength(circList) == 0) {
        return;
    }
    ListIterator iter = createIterator(circList);
    void *elem = NULL;
    char buffer[1024];
    xmlNode* node = NULL;

    while((elem = nextElement(&iter)) != NULL) {
        Circle* circle = (Circle *)elem;
        node = xmlNewChild(rootNode, NULL, BAD_CAST "circle", NULL);
        sprintf(buffer, "%f%s", circle->cx, circle->units);
        xmlNewProp(node, BAD_CAST "cx", BAD_CAST buffer);
        sprintf(buffer, "%f%s", circle->cy, circle->units);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST buffer);
        sprintf(buffer, "%f%s", circle->r, circle->units);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST buffer);

        addAttributesToXMLnode(circle->otherAttributes, node);
    }
    //printf("done adding circles\n");

}

void addGroupsToXMLnode(List* groupList, xmlNode* rootNode) {
    if(groupList == NULL || getLength(groupList) == 0) {
        return;
    }
    ListIterator iter = createIterator(groupList);
    void *elem = NULL;
    xmlNode* node = NULL;

    while((elem = nextElement(&iter)) != NULL) {
        Group* group = (Group *)elem;
        node = xmlNewChild(rootNode, NULL, BAD_CAST "g", NULL);
        addRectanglesToXMLnode(group->rectangles, node);
        addCirclesToXMLnode(group->circles, node);
        addPathsToXMLnode(group->paths, node);
        addGroupsToXMLnode(group->groups, node);
        addAttributesToXMLnode(group->otherAttributes, node);
    }
    //printf("done adding groups\n");


}

/** Function to setting an attribute in an SVGimage or component
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newAttribute is not NULL
 *@post The appropriate attribute was set corectly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    elemIndex - index of thje lement to modify
    newAttribute - struct containing name and value of the updated attribute
 **/
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if(elemType == SVG_IMAGE) {
        elemIndex = 0;
    }
    if(image == NULL || elemType < SVG_IMAGE || elemType > GROUP || newAttribute == NULL || elemIndex < 0 || newAttribute->name == NULL || newAttribute->value == NULL) {
        return;
    }

    void* elem;
    char* buffer;

    if(elemType == SVG_IMAGE) {
        addAttribute(image->otherAttributes, newAttribute);
    } else if(elemType == RECT) {
        //check if index is out of bounds
        if(getLength(image->rectangles) <= elemIndex) {
            return;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(image->rectangles);
        for(int i = 0; i <= elemIndex; i++) {
            elem = nextElement(&iter);
        }
        Rectangle* rect = (Rectangle *)elem;
        if(strcmp(newAttribute->name, "x") == 0) {
            rect->x = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);

        } else if(strcmp(newAttribute->name, "y") == 0) {
            rect->y = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);

        } else if(strcmp(newAttribute->name, "width") == 0) {
            rect->width = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);

        } else if(strcmp(newAttribute->name, "height") == 0) {
            rect->height = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);

        } else {
            addAttribute(rect->otherAttributes, newAttribute);
        }
    } else if(elemType == CIRC) {
        //check if index is out of bounds
        if(getLength(image->circles) <= elemIndex) {
            return;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(image->circles);
        for(int i = 0; i <= elemIndex; i++) {
            elem = nextElement(&iter);
        }
        Circle* circle = (Circle *)elem;
        if(strcmp(newAttribute->name, "cx") == 0) {
            circle->cx = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);
        } else if(strcmp(newAttribute->name, "cy") == 0) {
            circle->cy = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);
        } else if(strcmp(newAttribute->name, "r") == 0) {
            //printf("updating: val: %s\n", newAttribute->value);
            circle->r = strtod(newAttribute->value, &buffer);
            deleteAttribute(newAttribute);
        } else {
            addAttribute(circle->otherAttributes, newAttribute);
        }
    } else if(elemType == PATH) {
        //check if index is out of bounds
        if(getLength(image->paths) <= elemIndex) {
            return;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(image->paths);
        for(int i = 0; i <= elemIndex; i++) {
            elem = nextElement(&iter);
        }
        Path* path = (Path *)elem;
        if(strcmp(newAttribute->name, "d") == 0) {
            //overriding current data
            free(path->data);
            path->data = malloc(strlen(newAttribute->value) * sizeof(char));
            strcpy(path->data, newAttribute->value);

            //deleting newAttribute
            deleteAttribute(newAttribute);
        } else {
            addAttribute(path->otherAttributes, newAttribute);
        }
    } else if(elemType == GROUP) {
        //check if index is out of bounds
        if(getLength(image->groups) <= elemIndex) {
            return;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(image->groups);
        for(int i = 0; i <= elemIndex; i++) {
            elem = nextElement(&iter);
        }
        Group* group = (Group *)elem;
        addAttribute(group->otherAttributes, newAttribute);

    }
}
void addAttribute(List* attributes, Attribute* newAttribute) {
    void* elem;

    //getting the element at the right index
    ListIterator iter = createIterator(attributes);
    while((elem = nextElement(&iter)) != NULL) {
        Attribute* attribute = (Attribute *)elem;
        //found an already existing attribute with same name so update this one
        if(strcmp(attribute->name, newAttribute->name) == 0) {
            //overriding current value of that attribute
            free(attribute->value);
            attribute->value = malloc(strlen(newAttribute->value) * sizeof(char));
            strcpy(attribute->value, newAttribute->value);

            //deleting newAttribute
            deleteAttribute(newAttribute);
            return;
        }
    }

    //if no attribute was found, append to the end of list
    insertBack(attributes, newAttribute);
}


/** Function to adding an element - Circle, Rectngle, or Path - to an SVGimage
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newElement is not NULL
 *@post The appropriate element was added correctly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    newElement - pointer to the element sgtruct (Circle, Rectngle, or Path)
 **/
void addComponent(SVGimage* image, elementType type, void* newElement) {
    if(image == NULL || newElement == NULL || type < CIRC || type > PATH ) {
        return;
    }
    if(type == CIRC) {
        insertBack(image->circles, newElement);
    } else if (type == RECT) {
        insertBack(image->rectangles, newElement);
    } else if (type == PATH) {
        insertBack(image->paths, newElement);
    }
}

int writeJSONSVGtoSVGFile(char* svgJSON, char* fileName, char* schemaFile) {
    SVGimage* img = JSONtoSVG(svgJSON);
    if(validateSVGimage(img, schemaFile) == true) {
        if(writeSVGimage(img, fileName) == true) {
            /* Successfully written */
            deleteSVGimage(img);
            return 0;
        } else {
            /* Error writing to file */
            deleteSVGimage(img);
            return -1;
        }
    } else {
        /* Invalid SVG format */
        deleteSVGimage(img);
        return -2;
    }
    
}

/* Creates an SVGimage from file and converts it to a JSON string */
char* readSVGtoJSON(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        char *temp = calloc(10, sizeof(char));
        //printf("test\n");
        strcpy(temp, "{}");
        return temp;
    } else {
        char* JSONstring = SVGtoJSON(img);
        deleteSVGimage(img);
        return JSONstring;
    }
    
}

/* Returns a JSON string containing the title and description of the img */
char* SVGdetailsToJSON(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        char *temp = calloc(10, sizeof(char));
        strcpy(temp, "{}");
        return temp;
    } else {
        char* JSONstring = calloc(strlen(img->title) + strlen(img->description) + 30, 1);

        strcpy(JSONstring ,"{\"title\":\"");
        strcat(JSONstring, img->title);
        strcat(JSONstring, "\",\"desc\":\"");
        strcat(JSONstring, img->description);
        strcat(JSONstring, "\"}");

        deleteSVGimage(img);
        return JSONstring;
    }    
}

int addShapeToSVGFile(char* fileName, char* schemaFile, char* JSONstring, int type) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        return -1;
    }

    if(type == 1) {
        Rectangle* rect = JSONtoRect(JSONstring);
        if(rect == NULL) {
            return -2;
        }
        addComponent(img, RECT, rect);

    } else if(type == 2) {
        Circle* circle = JSONtoCircle(JSONstring);
        if(circle == NULL) {
            return -2;
        }
        addComponent(img, CIRC, circle);
    }
    

    if(validateSVGimage(img, schemaFile) == false) {
        deleteSVGimage(img);
        return -3;
    } else {
        if(writeSVGimage(img, fileName) == false) {
            deleteSVGimage(img);
            return -4;
        } else {
            deleteSVGimage(img);
            return 0;
        }
    }

    
}

char* getRectJSONlist(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);

    char* JSONstring = rectListToJSON(img->rectangles);
    deleteSVGimage(img);
    return JSONstring;
}

char* getCircJSONlist(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);

    char* JSONstring = circListToJSON(img->circles);
    deleteSVGimage(img);
    return JSONstring;
}

char* getPathJSONlist(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    printf("%s\n", toString(img->paths));

    char* JSONstring = pathListToJSON(img->paths);

    deleteSVGimage(img);
    return JSONstring;
}

char* getGroupJSONlist(char* fileName, char* schemaFile) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);

    char* JSONstring = groupListToJSON(img->groups);
    deleteSVGimage(img);
    return JSONstring;
}

char* getAttrJSONlist(char* fileName, char* schemaFile, int index, int type) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);

    /* Check if index is valid */
    if(index < 0 || type < 0 || type > 4 || img == NULL) {
        char *temp = calloc(30, sizeof(char));
        strcpy(temp, "invalid input");
        return temp;
    }
    void* elem;
    char* JSONstring = calloc(1000, sizeof(char));
    strcpy(JSONstring, "[");

    if(type == 1) {
        if(getLength(img->rectangles) <= index) {
            char *temp = calloc(10, sizeof(char));
            strcpy(temp, "index");
            return temp;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->rectangles);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Rectangle* rect = (Rectangle *)elem;
        //printf("[%s]\n", rectangleToString(rect));
        //return rectangleToString(rect);
        strcat(JSONstring, rectToJSON(rect));
        strcat(JSONstring, ",");
        strcat(JSONstring, attrListToJSON(rect->otherAttributes));
    } else if (type == 2) {
        if(getLength(img->circles) <= index) {
            char *temp = calloc(10, sizeof(char));
            strcpy(temp, "{}");
            return temp;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->circles);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Circle* circle = (Circle *)elem;
        strcat(JSONstring, circleToJSON(circle));
        strcat(JSONstring, ",");
        strcat(JSONstring, attrListToJSON(circle->otherAttributes));
    } else if (type == 3) {
        if(getLength(img->paths) <= index) {
            char *temp = calloc(10, sizeof(char));
            strcpy(temp, "{}");
            return temp;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->paths);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Path* path = (Path *)elem;
        strcat(JSONstring, pathToJSON(path));
        strcat(JSONstring, ",");
        strcat(JSONstring, attrListToJSON(path->otherAttributes));
    } else if (type == 4) {
        if(getLength(img->groups) <= index) {
            char *temp = calloc(10, sizeof(char));
            strcpy(temp, "{}");
            return temp;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->groups);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Group* group = (Group *)elem;
        /* strcat(JSONstring, groupToJSON(group));
        strcat(JSONstring, ",");
        strcat(JSONstring, attrListToJSON(group->otherAttributes)); */
        strcat(JSONstring, attrListToJSON(group->otherAttributes));
        //JSONstring = attrListToJSON(group->otherAttributes);
    } else if (type == 0) {
        strcat(JSONstring, attrListToJSON(img->otherAttributes));
    }
    strcat(JSONstring, "]");
    //printf("%s\n", JSONstring);

    deleteSVGimage(img);
    return JSONstring;
}

/** Function to converting an Attribute into a JSON string
*@pre Attribute is not NULL
*@post Attribute has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an Attribute struct
**/
char* attrToJSON(const Attribute *a) {
    if(a == NULL) {
        char *temp = calloc(10, sizeof(char));
        strcpy(temp, "{}");
        return temp;
    }
    char *string = calloc(strlen(a->value) + strlen(a->name) + 50, sizeof(char));
    
    strcpy(string, "{\"name\":\"");
    strcat(string, a->name);
    strcat(string, "\",\"value\":\"");
    strcat(string, a->value);
    strcat(string, "\"}");
    return string;
}

/** Function to converting a Circle into a JSON string
*@pre Circle is not NULL
*@post Circle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Circle struct
**/
char* circleToJSON(const Circle *c) {
    char* string = calloc(1000, sizeof(char));
    if(c == NULL) {
        strcpy(string, "{}");
        return string;
    }
     
    char buffer[100];

    strcpy(string, "{\"cx\":");
    sprintf(buffer, "%.2f", c->cx);
    strcat(string, buffer);
    strcat(string, ",\"cy\":");
    sprintf(buffer, "%.2f", c->cy);
    strcat(string, buffer);
    strcat(string, ",\"r\":");
    sprintf(buffer, "%.2f", c->r);
    strcat(string, buffer);
    strcat(string, ",\"numAttr\":");
    sprintf(buffer, "%d", getLength(c->otherAttributes));
    strcat(string, buffer);
    strcat(string, ",\"units\":\"");
    strcat(string, c->units);
    strcat(string, "\"}");

    return string;
}


/** Function to converting a Rectangle into a JSON string
*@pre Rectangle is not NULL
*@post Rectangle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Rectangle struct
**/
char* rectToJSON(const Rectangle *r){
    char* string = calloc(1000, sizeof(char));
    if(r == NULL) {
        strcpy(string, "{}");
        return string;
    }
     
    char buffer[100];

    strcpy(string, "{\"x\":");
    sprintf(buffer, "%.2f", r->x);
    strcat(string, buffer);
    strcat(string, ",\"y\":");
    sprintf(buffer, "%.2f", r->y);
    strcat(string, buffer);
    strcat(string, ",\"w\":");
    sprintf(buffer, "%.2f", r->width);
    strcat(string, buffer);
    strcat(string, ",\"h\":");
    sprintf(buffer, "%.2f", r->height);
    strcat(string, buffer);
    strcat(string, ",\"numAttr\":");
    sprintf(buffer, "%d", getLength(r->otherAttributes));
    strcat(string, buffer);
    strcat(string, ",\"units\":\"");
    strcat(string, r->units);
    strcat(string, "\"}");

    return string;
}


/** Function to converting a Path into a JSON string
*@pre Path is not NULL
*@post Path has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Path struct
**/
char* pathToJSON(const Path *p){
    char* string = calloc(100, sizeof(char));
    if(p == NULL) {
        strcpy(string, "{}");
        return string;
    }
    
    char buffer[100];

    strcpy(string, "{\"d\":\"");
    strncat(string, p->data, 64);
    strcat(string, "\",\"numAttr\":\"");
    sprintf(buffer, "%d", getLength(p->otherAttributes));
    strcat(string, buffer);
    strcat(string, "\"}");
    printf("%ld\n", strlen(string));
    return string;
}


/** Function to converting a Group into a JSON string
*@pre Group is not NULL
*@post Group has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Group struct
**/
char* groupToJSON(const Group *g){
    char* string = calloc(1000, sizeof(char));
    if(g == NULL) {
        strcpy(string, "{}");
        return string;
    }
    
    char buffer[100];

    strcpy(string, "{\"children\":");
    sprintf(buffer, "%d", (getLength(g->circles) + getLength(g->rectangles) + getLength(g->paths) + getLength(g->groups)));
    strcat(string, buffer);
    strcat(string, ",\"numAttr\":");
    sprintf(buffer, "%d", getLength(g->otherAttributes));
    strcat(string, buffer);
    strcat(string, "}");

    return string;
}


/** Function to converting a list of Attribute structs into a JSON string
*@pre Attribute list is not NULL
*@post Attribute list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* attrListToJSON(const List *list){
    char *string = calloc(500, sizeof(char));
    if(list == NULL || getLength((List *)list) == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *buffer = NULL;
    int length = 0;
    bool isFirstElem = true;
    
    strcpy(string, "[");
    void *elem;
    ListIterator iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Attribute* attribute = (Attribute *)elem;

        //getting attributeJSON string
        buffer = attrToJSON(attribute);
        length += strlen(buffer);
        if(length > 500) { //realloc if length > size of string
            string = realloc(buffer, length * sizeof(char));
        }

        //checking if first att element, if it is, don't put a ,
        if(!isFirstElem) {
            strcat(string, ",");
        } else {
            isFirstElem = false;
        }
        strcat(string, buffer);
        free(buffer);

    }
    strcat(string, "]");
    return string;
}


/** Function to converting a list of Circle structs into a JSON string
*@pre Circle list is not NULL
*@post Circle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* circListToJSON(const List *list){
    char *string = calloc(500, sizeof(char));
    if(list == NULL || getLength((List *)list) == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *buffer = NULL;
    int length = 0;
    bool isFirstElem = true;
    
    strcpy(string, "[");
    void *elem;
    ListIterator iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Circle* circle = (Circle *)elem;

        //getting circle JSON string
        buffer = circleToJSON(circle);
        length += strlen(buffer);
        if(length > 500) { //realloc if length > size of string
            string = realloc(buffer, length * sizeof(char));
        }

        //checking if first att element, if it is, don't put a ,
        if(!isFirstElem) {
            strcat(string, ",");
        } else {
            isFirstElem = false;
        }
        strcat(string, buffer);
        free(buffer);

    }
    strcat(string, "]");
    return string;
}


/** Function to converting a list of Rectangle structs into a JSON string
*@pre Rectangle list is not NULL
*@post Rectangle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* rectListToJSON(const List *list){
    char *string = calloc(500, sizeof(char));
    if(list == NULL || getLength((List *)list) == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *buffer = NULL;
    int length = 0;
    bool isFirstElem = true;
    
    strcpy(string, "[");
    void *elem;
    ListIterator iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Rectangle* rect = (Rectangle *)elem;

        //getting rect JSON string
        buffer = rectToJSON(rect);
        length += strlen(buffer);
        if(length > 500) { //realloc if length > size of string
            string = realloc(buffer, length * sizeof(char));
        }

        //checking if first att element, if it is, don't put a ,
        if(!isFirstElem) {
            strcat(string, ",");
        } else {
            isFirstElem = false;
        }
        strcat(string, buffer);
        free(buffer);

    }
    strcat(string, "]");
    return string;
}


/** Function to converting a list of Path structs into a JSON string
*@pre Path list is not NULL
*@post Path list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* pathListToJSON(const List *list){
    if(list == NULL || getLength((List *)list) == 0) {
        char *string = malloc(10 * sizeof(char));
        strcpy(string, "[]");
        return string;
    }

    char *buffer;
    int length = 0;
    bool isFirstElem = true;
    void *elem;

    ListIterator iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Path* path = (Path *)elem;

        //getting path JSON string
        buffer = pathToJSON(path);
        length += strlen(buffer);
        free(buffer);
        //printf("%d\n", length);
    }
    
    char *string = calloc(length + 100, sizeof(char));
    strcpy(string, "[");
    iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Path* path = (Path *)elem;

        //getting path JSON string
        buffer = pathToJSON(path);
        //length += strlen(buffer);
        /* printf("[%d] + [%ld]\n", length, length * sizeof(char));
        if(length > 500) { //realloc if length > size of string
            string = realloc(string, length * sizeof(char));
        } */
        //checking if first att element, if it is, don't put a ,
        if(!isFirstElem) {
            strcat(string, ",");
        } else {
            isFirstElem = false;
        }
        strcat(string, buffer);
        free(buffer);
        //printf("%s\n", string);

    }
    strcat(string, "]");
    //printf("%s\n", string);
    return string;
}


/** Function to converting a list of Group structs into a JSON string
*@pre Group list is not NULL
*@post Group list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* groupListToJSON(const List *list){
    char *string = calloc(500, sizeof(char));
    if(list == NULL || getLength((List *)list) == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *buffer = NULL;
    int length = 0;
    bool isFirstElem = true;
    
    strcpy(string, "[");
    void *elem;
    ListIterator iter = createIterator((List *)list);
    while((elem = nextElement(&iter)) != NULL) {
        Group* group = (Group *)elem;

        //getting group JSON string
        buffer = groupToJSON(group);
        length += strlen(buffer);
        if(length > 500) { //realloc if length > size of string
            string = realloc(buffer, length * sizeof(char));
        }

        //checking if first att element, if it is, don't put a ,
        if(!isFirstElem) {
            strcat(string, ",");
        } else {
            isFirstElem = false;
        }
        strcat(string, buffer);
        free(buffer);

    }
    strcat(string, "]");
    return string;
}


/** Function to converting an SVGimage into a JSON string
*@pre SVGimage is not NULL
*@post SVGimage has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an SVGimage struct
**/
char* SVGtoJSON(const SVGimage* imge){
    char* string = calloc(1000, sizeof(char));
    if(imge == NULL) {
        strcpy(string, "{}");
        return string;
    }

    SVGimage* tempSVG = (SVGimage *)imge;
    char buffer[20];
    List* tempList = NULL;

    //getting numRects
    strcpy(string, "{\"numRect\":");
    tempList = getRects(tempSVG);
    sprintf(buffer, "%d", getLength(tempList));
    strcat(string, buffer);
    freeList(tempList);
    
    //getting numCircles
    strcat(string, ",\"numCirc\":");
    tempList = getCircles(tempSVG);
    sprintf(buffer, "%d", getLength(tempList));
    strcat(string, buffer);
    freeList(tempList);

    //getting numPaths
    strcat(string, ",\"numPaths\":");
    tempList = getPaths(tempSVG);
    sprintf(buffer, "%d", getLength(tempList));
    strcat(string, buffer);
    freeList(tempList);

    //getting numGroups
    strcat(string, ",\"numGroups\":");
    tempList = getGroups(tempSVG);
    sprintf(buffer, "%d", getLength(tempList));
    strcat(string, buffer);
    freeList(tempList);
    strcat(string, "}");

    return string;
}

/** Function to converting a JSON string into an SVGimage struct
*@pre JSON string is not NULL
*@post String has not been modified in any way
*@return A newly allocated and initialized SVGimage struct
*@param str - a pointer to a string
**/
SVGimage* JSONtoSVG(const char* svgString) {
    if(svgString == NULL) {
        return NULL;
    }
    SVGimage *img = initSVGimage();
    
    sscanf(svgString, "{\"title\":\"%256[^\",\"descr\":\"]s", img->title);
    sscanf(svgString, "{\"title\":\"%256[^\"]\",\"descr\":\"%[^\n]s", img->title, img->description);
    img->description[strlen(img->description) - 2] = '\0';

    //printf("title:[%s]\ndesc:[%s]\nval = %d\n", img->title, img->description, val);
    strcpy(img->namespace, "http://www.w3.org/2000/svg");

    return img;
}

/** Function to converting a JSON string into a Rectangle struct
*@pre JSON string is not NULL
*@post Rectangle has not been modified in any way
*@return A newly allocated and initialized Rectangle struct
*@param str - a pointer to a string
**/
Rectangle* JSONtoRect(const char* svgString) {
    if(svgString == NULL) {
        return NULL;
    }
    Rectangle *rect = calloc(1, sizeof(Rectangle));
    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    rect->units[0] = '\0';

    sscanf(svgString, "{\"x\":%f,\"y\":%f,\"w\":%f,\"h\":%f,\"units\":\"%[^\"]s\"}", &rect->x, &rect->y, &rect->width, &rect->height, rect->units);

    if(rect->x < 0) {
        rect->x = 0;
    }
    if(rect->y < 0) {
        rect->y = 0;
    }
    if(rect->width < 0) {
        rect->width = 0;
    }
    if(rect->height < 0) {
        rect->height = 0;
    }
    return rect;
}

/** Function to converting a JSON string into a Circle struct
*@pre JSON string is not NULL
*@post Circle has not been modified in any way
*@return A newly allocated and initialized Circle struct
*@param str - a pointer to a string
**/
Circle* JSONtoCircle(const char* svgString) {
    if(svgString == NULL) {
        return NULL;
    }
     Circle *circle = malloc(sizeof(Circle));

    circle->cx = 0;
    circle->cy = 0;
    circle->r = 0;
    circle->units[0] = '\0';
    circle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    sscanf(svgString, "{\"cx\":%f,\"cy\":%f,\"r\":%f,\"units\":\"%[^\"]s\"}", &circle->cx, &circle->cy, &circle->r, circle->units);
    
    if(circle->cx < 0) {
        circle->cx = 0;
    }
    if(circle->cy < 0) {
        circle->cy = 0;
    }
    if(circle->r < 0) {
        circle->r = 0;
    }
    //printf("[%s]\n", circleToString(circle));
    return circle;
}

Attribute* JSONtoAttr(const char* JSONstring) {
    if(JSONstring == NULL) {
        return NULL;
    }

    Attribute* attr = calloc(1, sizeof(Attribute));

    char bufferName[1024];
    char bufferVal[1024];

    sscanf(JSONstring, "{\"name\":\"%[^\",\",\"value\":\"]s", bufferName);
    sscanf(JSONstring, "{\"name\":\"%[^\"]\",\"value\":\"%[^\"]s\"}", bufferName, bufferVal);

    attr->name = calloc(strlen(bufferName), sizeof(char));
    attr->value = calloc(strlen(bufferVal), sizeof(char));
    printf("[%s],[%s]\n", bufferName, bufferVal);

    strcpy(attr->name, bufferName);
    strcpy(attr->value, bufferVal);

    return attr;
}

int writeAttrFromJSON(char* fileName, char* schemaFile, int index, int type, char* JSONattr) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        return -1;
    }

    if(index < 0 || type < 0 || type > 4) {
        return -2;
    }

    Attribute* attr = JSONtoAttr(JSONattr);
    if(attr == NULL) {
        return -3;
    }
    if(type == 0) {
        type = SVG_IMAGE; 
    } else if (type == 1) {
        type = RECT;
    } else if (type == 2) {
        type = CIRC;
    } else if (type == 3) {
        type = PATH;
    } else if (type == 4) {
        type = GROUP;
    }
    setAttribute(img, type, index, attr);

    if(validateSVGimage(img, schemaFile) == true) {
        if(writeSVGimage(img, fileName) == true) {
            deleteSVGimage(img);
            return 0;
        } else {
            deleteSVGimage(img);
            return -4;
        }
    } else {
        deleteSVGimage(img);
        return -5;
    }
    
}

int writeExistingShapeFromJSON(char* fileName, char* schemaFile, char* JSONstring, int type, int index) {
    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        return -1;
    } 
    if(type < 0 || index < 0 || type > 4) {
        return -1;
    }
    void* elem;

    if(type == 1) {
        Rectangle* newRect = JSONtoRect(JSONstring);
        if(newRect == NULL) {
            return -2;
        }
        if(getLength(img->rectangles) <= index) {
            return -1;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->rectangles);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Rectangle* oldRect = (Rectangle *)elem;
        updateRect(oldRect, newRect);
    } else if (type == 2) {
        Circle* newCirc = JSONtoCircle(JSONstring);
        if(newCirc == NULL) {
            return -2;
        }
        if(getLength(img->circles) <= index) {
            return -1;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->circles);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Circle* oldCirc = (Circle *)elem;
        updateCircle(oldCirc, newCirc);
    } else if (type == 3) {
        if(getLength(img->paths) <= index) {
            return -1;
        }
        //getting the element at the right index
        ListIterator iter = createIterator(img->circles);
        for(int i = 0; i <= index; i++) {
            elem = nextElement(&iter);            
        }
        Path* path = (Path *)elem;
        free(path->data);
        path->data = malloc(strlen(JSONstring) * sizeof(char));
        strcpy(path->data, JSONstring);
    }

    if(validateSVGimage(img, schemaFile) == true) {
        if(writeSVGimage(img, fileName) == true) {
            deleteSVGimage(img);
            return 0;
        } else {
            deleteSVGimage(img);
            return -4;
        }
    } else {
        deleteSVGimage(img);
        return -5;
    }
}

int updateCircle(Circle* oldCirc, Circle* newCirc) {
    if(oldCirc == NULL || newCirc == NULL) {
        return -1;
    } 

    oldCirc->cx = newCirc->cx;
    oldCirc->cy = newCirc->cy;
    oldCirc->r = newCirc->r;

    deleteCircle(newCirc);
    return 0;
}

int updateRect(Rectangle* oldRect, Rectangle* newRect) {
    if(oldRect == NULL || newRect == NULL) {
        return -1;
    }

    oldRect->x = newRect->x;
    oldRect->y = newRect->y;
    oldRect->width = newRect->width;
    oldRect->height = newRect->height;

    deleteRectangle(newRect);
    return 0;
}

int scaleShape(char* fileName, char* schemaFile, char* scaleFactorString, int type) {
    if(scaleFactorString == NULL || type < 1 || type > 2 || fileName == NULL || schemaFile == NULL) {
        return -1;
    }
    char* buffer;
    double scaleFactor = strtod(scaleFactorString, &buffer);

    printf("scale: %lf\n", scaleFactor);

    SVGimage* img = createValidSVGimage(fileName, schemaFile);
    if(img == NULL) {
        return -1;
    }

    if(type == 1) {
        List* rectList = getRects(img);
        if(rectList == NULL) {
            return -2;
        }
        void* elem;
        printf("len: %d\n", getLength(rectList));

        //getting the element at the right index
        ListIterator iter = createIterator(rectList);
        while((elem = nextElement(&iter)) != NULL) {
            Rectangle* rect = (Rectangle *)elem;
            printf("old width: %lf\n", rect->width);
            rect->width = rect->width * scaleFactor;
            printf("new: %lf\n", rect->width);
            rect->height = rect->height * scaleFactor;
        }
        freeList(rectList);
    } else if(type == 2) {
        List* circleList = getCircles(img);
        if(circleList == NULL) {
            return -2;
        }
        void* elem;

        //getting the element at the right index
        ListIterator iter = createIterator(circleList);
        while((elem = nextElement(&iter)) != NULL) {
            Circle* circ = (Circle *)elem;
            printf("old: %lf\n", circ->r);
            circ->r = circ->r * scaleFactor;
            printf("new: %lf\n", circ->r);
        }
        freeList(circleList);
    }

    if(validateSVGimage(img, schemaFile) == true) {
        if(writeSVGimage(img, fileName) == true) {
            deleteSVGimage(img);
            return 0;
        } else {
            deleteSVGimage(img);
            return -4;
        }
    } else {
        deleteSVGimage(img);
        return -5;
    }

}

/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteAttribute(void* data) {
    Attribute *attribute;

    if(data == NULL) {
        return;
    }

    attribute = (Attribute *)data;
    /* printf("Freeing name\n"); */
    free(attribute->name);
    /* printf("Freeing value\n"); */
    free(attribute->value);
    free(attribute);
    /* printf("deleted\n"); */

}

char* attributeToString(void* data) {
    Attribute *attribute;

    if(data == NULL) {
        return NULL;
    }

    attribute = (Attribute *)data;

    char *stringAtt = malloc(strlen(attribute->name) + strlen(attribute->value) + (sizeof(char) * 30));
    
    strcpy(stringAtt, "\tAttribute name:");
    strcat(stringAtt, attribute->name);
    strcat(stringAtt, ", Value=");
    strcat(stringAtt, attribute->value);

    return stringAtt;
}

int compareAttributes(const void *first, const void *second) {
    return 0;
}


void deleteGroup(void* data) {
    Group *group;

    if(data == NULL) {
        return;
    }

    group = (Group *)data;

    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    freeList(group->otherAttributes);
    freeList(group->groups);
    free(group);
}

char* groupToString( void* data) {
    Group *group;
    char *groupString;
    int lengthString = 0;

    if(data == NULL) {
        return NULL;
    }

    group = (Group *)data;

    /* Getting all the lists of items as strings */
    char *rectListString = toString(group->rectangles);
    char *circleListString = toString(group->circles);
    char *pathListString = toString(group->paths);
    char *attListString = toString(group->otherAttributes);
    char *groupListString = toString(group->groups);

    /* Mallocing enough space for group string */
    lengthString = strlen(rectListString) + strlen(circleListString) + strlen(pathListString) + strlen(groupListString) + strlen(attListString);

    groupString = malloc((sizeof(char) * 50) + lengthString);

    /* Generating group string */
    strcpy(groupString, "Group Start:\n");
    strcat(groupString, rectListString);
    strcat(groupString, circleListString);
    strcat(groupString, pathListString);
    strcat(groupString, "\t");
    if(getLength(group->otherAttributes) > 0) {
        strcat(groupString, "\nGroup Attributes:");
    }
    strcat(groupString, attListString);
    strcat(groupString, groupListString);
    strcat(groupString, "\n==Group End==\n");

    //Freeing list strings
    free(rectListString);
    free(circleListString);
    free(pathListString);
    free(groupListString);
    free(attListString);

    return groupString;

}

int compareGroups(const void *first, const void *second) {
    return 0;
}


void deleteRectangle(void* data) {
    Rectangle *rect;

    if(data == NULL) {
        return;
    }
    /* Initializes rect struct */
    rect = (Rectangle *)data;

    /* Frees list of attributes first then rectangle struct itself */
    freeList(rect->otherAttributes);
    free(rect);
}

char* rectangleToString(void* data) {
    Rectangle *rect;

    if(data == NULL) {
        return NULL;
    }
    /* Initializes rect struct */
    rect = (Rectangle *)data;


    /* Gets string of list of attributes */
    char *attributeListString = toString(rect->otherAttributes);
    
    /* Creates the string to return */
    char *rectString = malloc(sizeof(char) * (1024 + strlen(attributeListString)));
    sprintf(rectString, "Rectangle: x=%.3f, y=%.3f, width=%.3f, height=%.3f, unit=%s%s", rect->x, rect->y, rect->width, rect->height, rect->units, attributeListString);
    
    /* Frees attribute list string */
    free(attributeListString);
    return rectString;

}

int compareRectangles(const void *first, const void *second) {
    return 0;
}


void deleteCircle(void* data) {
    Circle *circle;

    if(data == NULL) {
        return;
    }

    circle = (Circle *)data;

    /* Frees list of attributes then circle struct */
    freeList(circle->otherAttributes);
    free(circle);

}

char* circleToString(void* data) {
    Circle *circle;

    if(data == NULL) {
        return NULL;
    }

    circle = (Circle *)data;

    char *attributeListString = toString(circle->otherAttributes);

    char *circleString = malloc(sizeof(char) * (1024 + strlen(attributeListString)));
    sprintf(circleString, "Circle: cx=%.3f, cy=%.3f, r=%.3f, unit=%s\t%s", circle->cx, circle->cy, circle->r, circle->units, attributeListString);
    
    free(attributeListString);
    return circleString;
}

int compareCircles(const void *first, const void *second) {
    return 0;
}


void deletePath(void* data) {
    Path *path;

    if(data == NULL) {
        return;
    }

    path = (Path *)data;

    /* Frees all content of a path struct then the struct itself */
    free(path->data);
    freeList(path->otherAttributes);
    free(path);
}

char* pathToString(void* data) {
    Path *path;
    char *pathString;

    if(data == NULL) {
        return NULL;
    }

    path = (Path *)data;

    char *attributeListString = toString(path->otherAttributes);

    pathString = malloc( (sizeof(char) * 20) + strlen(path->data) + strlen(attributeListString));

    strcpy(pathString, "Path: Data=");
    strcat(pathString, path->data);
    strcat(pathString, "\t");
    strcat(pathString, attributeListString);

    free(attributeListString);
    return pathString;

}

int comparePaths(const void *first, const void *second) {
    return 0;
}

void deleteListGetter(void *data) {
    
}