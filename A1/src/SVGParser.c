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
        printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

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
    image->circles = initializeList(circleToString, deleteCircle, compareCircles);
    image->paths = initializeList(pathToString, deletePath, comparePaths);
    image->groups = initializeList(groupToString, deleteGroup, compareGroups);
    image->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    return image;
}

/* Loads content of xml into SVGimage */
void loadSVGimage(xmlNode * a_node, SVGimage *image) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        /* if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        } */

        /* if (cur_node->content != NULL ){
            printf("  content: %s, size%ld\n", (char *)cur_node->content, strlen((char *)cur_node->content));
        } */

        /* Writing title to svg image */
        if(strcasecmp((char *)cur_node->name,"title")==0 && strcasecmp((char *)cur_node->parent->name,"svg")==0) {
            /* printf("  content: %s\n", (char *)cur_node->children->content); */

            /* Truncates if > 255 */
            if(strlen((char *)cur_node->children->content) > 255) {
                char *tempString = malloc(256);
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
                char *tempString = malloc(256);
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
                    char *tempString = malloc(256);
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
    attribute->value = malloc(strlen(cont) + 1);

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
        } else if(strcasecmp((char *)attr->name, "y") == 0) {
            /* strcpy */
            rect->y= strtod((const char *)attr->children->content, &buffer);
        } else if(strcasecmp((char *)attr->name, "width") == 0) {
            /* strcpy */
            rect->width = strtod((const char *)attr->children->content, &buffer);
        } else if(strcasecmp((char *)attr->name, "height") == 0) {
            /* strcpy */
            rect->height = strtod((const char *)attr->children->content, &buffer);
        } else {
            Attribute *attribute = createAttribute(attr);
            insertBack(rect->otherAttributes, attribute);
        }
        strcpy(rect->units, buffer);
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
            
        } else if(strcasecmp((char *)attr->name, "cy") == 0) {
            circle->cy= strtod((const char *)attr->children->content, &buffer);

        } else if(strcasecmp((char *)attr->name, "r") == 0) {
            circle->r = strtod((const char *)attr->children->content, &buffer);

        } else {
            Attribute *attribute = createAttribute(attr);
            insertBack(circle->otherAttributes, attribute);
        }
        strcpy(circle->units, buffer);
    }
    if(circle->cx < 0 || circle->cy < 0 || circle->r < 0) {
        deleteCircle(circle);
    } else {
        insertBack(list, circle);
    }
}

void createPath(xmlNode *node, List *list) {
    Path *path = malloc(sizeof(Path));

    path->data = malloc(10);
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
    char *imageString = malloc(2048);
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
    List *rectList = initializeList(rectangleToString, deleteRectangle, compareRectangles);

    if(img == NULL) {
        return rectList;
    }

    /* Gets all rectangles from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->rectangles);

    while((elem = nextElement(&iter)) != NULL) {
        Rectangle *rect = (Rectangle *)elem;

        insertBack(rectList, &rect);
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
    List *circleList = initializeList(circleToString, deleteCircle, compareCircles);

    if(img == NULL) {
        return circleList;
    }

    /* Gets all circles from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->circles);

    while((elem = nextElement(&iter)) != NULL) {
        Circle *circle = (Circle *)elem;
        insertBack(circleList, &circle);
    }

    /* Searches down groups for all subsequent rectangles */
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
    List * groupList = initializeList(groupToString, deleteGroup, compareGroups);

    if(img == NULL) {
        return groupList;
    }

    /* Searches down groups for all subsequent groups */
    ListIterator iterGroups = createIterator(img->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        insertBack(groupList, &group);
        searchGroupGroups(groupList, group);
    }

    return groupList;
}

// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img) {
    List *pathList = initializeList(pathToString, deletePath, comparePaths);

    if(img == NULL) {
        return pathList;
    }

    /* Gets all paths from the svg node */
    void *elem;
    ListIterator iter = createIterator(img->paths);

    while((elem = nextElement(&iter)) != NULL) {
        Path *path = (Path *)elem;
        insertBack(pathList, &path);
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

    /* Creates iterator to go through list of rectangles in that group */
    ListIterator iterRects = createIterator(group->rectangles);
    while((elem = nextElement(&iterRects)) != NULL) {
        Rectangle *rect = (Rectangle *)elem;
        insertBack(list, &rect);
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

    /* Creates iterator to go through list of circles in that group */
    ListIterator iter = createIterator(group->circles);
    while((elem = nextElement(&iter)) != NULL) {
        Circle *circle = (Circle *)elem;
        insertBack(list, &circle);
    }

    /* Searches down groups for all subsequent circles */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupRect(list, group);
    }
}

void searchGroupPaths(List *list, Group *group) {
    void *elem;

    /* Creates iterator to go through list of paths in that group */
    ListIterator iter = createIterator(group->paths);
    while((elem = nextElement(&iter)) != NULL) {
        Path *path = (Path *)elem;
        insertBack(list, &path);
    }

    /* Searches down groups for all subsequent paths */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        searchGroupRect(list, group);
    }

}

void searchGroupGroups(List *list, Group *group) {
    void *elem;
    /* Searches down groups for all subsequent groups */
    ListIterator iterGroups = createIterator(group->groups);
    while((elem = nextElement(&iterGroups)) != NULL) {
        Group *group = (Group *)elem;
        insertBack(list, &group);
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

    char *stringAtt = malloc(strlen(attribute->name) + strlen(attribute->value) + 30);
    
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

    groupString = malloc(50 + lengthString);

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
    char *rectString = malloc(1024 + strlen(attributeListString));
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

    char *circleString = malloc(1024 + strlen(attributeListString));
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

    pathString = malloc(20 + strlen(path->data) + strlen(attributeListString));

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