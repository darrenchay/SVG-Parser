#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SVGParser.h"

int main (void) {
    Attribute *attribute1 = malloc (sizeof(Attribute));
    /* Attribute *attribute2 = malloc (sizeof(Attribute)); */

    Rectangle *rect1 = malloc (sizeof(Rectangle));
    /*Rectangle *rect2 = malloc (sizeof(Rectangle));

    Circle *circ1 = malloc (sizeof(Circle));
    Circle *circ2 = malloc (sizeof(Circle));

    Path *path1 = malloc (sizeof(Path));
    Path *path2 = malloc (sizeof(Path)); */

    char *name1 = malloc(5);
    strcpy(name1, "test");
    char *value1 = malloc(6);
    strcpy(value1, "value");

    attribute1->name = name1;
    attribute1->value = value1; 

    char *data1 = malloc(5);
    strcpy(data1, "data");

    char *attributeString = attributeToString((void *)attribute1);
    printf("%s\n", attributeString);

    rect1->x = 1.1;
    rect1->y = 2.1;
    rect1->width = 3.1;
    rect1->height = 4.1;
    strcpy(rect1->units, "cm");
    List *attributeList = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList, attribute1);
    rect1->otherAttributes = attributeList;

    printf("%s\n", rectangleToString((void *)rect1));

    return 0;
}