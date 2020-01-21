#include "SVGParser.h"
#include <string.h>
#include <stdlib.h>

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
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    xmlNode *cur_node;
    for (cur_node = root_element; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // Uncomment the code below if you want to see the content of every node.

        if (cur_node->content != NULL ){
            printf("  content: %s\n", cur_node->content);
        }
    }
    return NULL;
}

/** Function to create a string representation of an SVG object.
 *@pre SVGimgage exists, is not null, and is valid
 *@post SVGimgage has not been modified in any way, and a string representing the SVG contents has been created
 *@return a string contaning a humanly readable representation of an SVG object
 *@param obj - a pointer to an SVG struct
**/
char* SVGimageToString(SVGimage* img) {
    return NULL;
}

/** Function to delete image content and free all the memory.
 *@pre SVGimgage  exists, is not null, and has not been freed
 *@post SVSVGimgageG  had been freed
 *@return none
 *@param obj - a pointer to an SVG struct
**/
void deleteSVGimage(SVGimage* img) {
/* 6 freelist 1 free */
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
    return NULL;

}
// Function that returns a list of all circles in the image.  
List* getCircles(SVGimage* img) {
    return NULL;

}
// Function that returns a list of all groups in the image.  
List* getGroups(SVGimage* img) {
    return NULL;

}
// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img) {
    return NULL;

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
    return 0;
}
// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
    return 0;
}
// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {
    return 0;

}
// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len) {
    return 0;

}

/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.  
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img) {
    return 0;

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
    
    strcpy(stringAtt, "Attribute: Name=");
    strcat(stringAtt, attribute->name);
    strcat(stringAtt, " Value=");
    strcat(stringAtt, attribute->value);
    strcat(stringAtt, "\n");

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
    printf("deleting group1\n");
    freeList(group->groups);
    printf("deleting group\n");
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

    char *rectListString = toString(group->rectangles);
    char *circleListString = toString(group->circles);
    char *pathListString = toString(group->paths);
    char *groupListString = toString(group->groups);
    char *attListString = toString(group->otherAttributes);
    lengthString = strlen(rectListString) + strlen(circleListString) + strlen(pathListString) + strlen(groupListString) + strlen(attListString);

    groupString = malloc(30 + lengthString);

    strcpy(groupString, "Group:\n");
    strcat(groupString, rectListString);
    strcat(groupString, circleListString);
    strcat(groupString, pathListString);
    strcat(groupString, "Group Attributes:\n");
    strcat(groupString, attListString);
    strcat(groupString, "\t");
    strcat(groupString, groupListString);



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
    sprintf(rectString, "Rectangle: x=%.3f, y=%.3f, width=%.3f, height=%.3f, unit=%s\n%s", rect->x, rect->y, rect->width, rect->height, rect->units, attributeListString);
    
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
    sprintf(circleString, "Circle: cx=%.3f, cy=%.3f, r=%.3f, unit=%s\n\t%s", circle->cx, circle->cy, circle->r, circle->units, attributeListString);
    
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
    strcat(pathString, "\n\t");
    strcat(pathString, attributeListString);

    free(attributeListString);
    return pathString;

}

int comparePaths(const void *first, const void *second) {
    return 0;
}