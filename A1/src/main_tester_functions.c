#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SVGParser.h"

int main (void) {
    Attribute *attribute1 = malloc (sizeof(Attribute));
    Attribute *attribute2 = malloc (sizeof(Attribute));

    Rectangle *rect1 = malloc (sizeof(Rectangle));
    Circle *circ1 = malloc (sizeof(Circle));

    /*Rectangle *rect2 = malloc (sizeof(Rectangle));

    
    Circle *circ2 = malloc (sizeof(Circle));

    Path *path1 = malloc (sizeof(Path));
    Path *path2 = malloc (sizeof(Path)); */

    char *name1 = malloc(5);
    strcpy(name1, "test");
    char *value1 = malloc(6);
    strcpy(value1, "value");

    char *name2 = malloc(6);
    strcpy(name2, "test2");
    char *value2 = malloc(7);
    strcpy(value2, "value2");

    attribute1->name = name1;
    attribute1->value = value1; 

    attribute2->name = name2;
    attribute2->value = value2; 


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
    insertFront(attributeList, attribute2);
    rect1->otherAttributes = attributeList;

    printf("%s\n", rectangleToString((void *)rect1));

    /* clearList(attributeList); */

    printf("making circle\n");
    circ1->cx = 1.234;
    circ1->cy = 12.234;
    circ1->r = 5.234;
    strcpy(circ1->units, "cm");
    /* printf("created attribute list\n");
    insertFront(attributeList, attribute1);
    printf("created attribute list item 1\n");
    insertFront(attributeList, attribute2);
     */
     printf("created attribute list item 2\n");
    circ1->otherAttributes = attributeList;

    printf("%s\n", circleToString((void *)circ1));

    return 0;
}