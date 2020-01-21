#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SVGParser.h"

int main (void) {
    char *buffer;

    /* 
     * Testing Attributes 
     */
    printf("===========================================\n\t\tTESTING ATTRIBUTES\n===========================================\n");
    Attribute *attribute1 = malloc (sizeof(Attribute));
    Attribute *attribute2 = malloc (sizeof(Attribute));
    Attribute *attribute3 = malloc (sizeof(Attribute));
    Attribute *attribute4 = malloc (sizeof(Attribute));


    char *name1 = malloc(5);
    strcpy(name1, "test");
    char *value1 = malloc(6);
    strcpy(value1, "value");

    attribute1->name = name1;
    attribute1->value = value1; 

    char *name2 = malloc(6);
    strcpy(name2, "test2");
    char *value2 = malloc(7);
    strcpy(value2, "value2");

    attribute2->name = name2;
    attribute2->value = value2; 

    char *name3 = malloc(6);
    strcpy(name3, "test3");
    char *value3 = malloc(7);
    strcpy(value3, "value3");

    attribute3->name = name3;
    attribute3->value = value3;

    char *name4 = malloc(6);
    strcpy(name4, "test4");
    char *value4 = malloc(7);
    strcpy(value4, "value4");

    attribute4->name = name4;
    attribute4->value = value4;

    char *attributeString = attributeToString((void *)attribute1);
    printf("%s\n", attributeString);
    free(attributeString);
    List *attributeList = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList, attribute1);

    List *attributeList2 = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList2, attribute2);

    List *attributeList3 = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList3, attribute3);

    List *attributeList4 = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList4, attribute4);

    printf("---FREEING LIST ATTRIBUTES---\n");
    /* freeList(attributeList); */


    /* Testing Rectangles */
    printf("\n\n");
    printf("===========================================\n\t\tTESTING RECTANGLES\n===========================================\n");
    Rectangle *rect1 = malloc (sizeof(Rectangle));

    rect1->x = 1.1;
    rect1->y = 2.1;
    rect1->width = 3.1;
    rect1->height = 4.1;
    strcpy(rect1->units, "cm");
    rect1->otherAttributes = attributeList;

    List *RectangleList = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    insertFront(RectangleList, rect1);

    buffer = toString(RectangleList);
    printf("%s\n", buffer);

    free(buffer);
    /* printf("---FREEING LIST RECTANGLES---\n");
    freeList(RectangleList); */

    /* Making Circle */
    printf("\n\n");
    printf("===========================================\n\t\tTESTING CIRCLE\n===========================================\n");
    Circle *circ1 = malloc (sizeof(Circle));

    circ1->cx = 1.234;
    circ1->cy = 12.234;
    circ1->r = 5.234;
    strcpy(circ1->units, "cm");
    circ1->otherAttributes = attributeList2;

    List *CircleList = initializeList(circleToString, deleteCircle, compareCircles);
    insertFront(CircleList, circ1);

    buffer = toString(CircleList);
    printf("%s\n", buffer);

    free(buffer);
    printf("---FREEING LIST CIRCLES---\n");
    /* freeList(CircleList); */

    

    /* Testing Paths */
    printf("\n\n");
    printf("===========================================\n\t\tTESTING PATHS\n===========================================\n");
    Path *path1 = malloc (sizeof(Path));

    char *data1 = malloc(5);
    strcpy(data1, "data");

    path1->data = data1;
    path1->otherAttributes = attributeList3;

    List *PathList = initializeList(pathToString, deletePath, comparePaths);
    insertFront(PathList, path1);

    buffer = toString(PathList);
    printf("%s\n", buffer);
    free(buffer);

    printf("---FREEING LIST PATHS---\n");
    /* freeList(PathList); */

    /* Testing Groups */
    printf("\n\n");
    printf("===========================================\n\t\tTESTING GROUPS\n===========================================\n");
    Group *group = malloc(sizeof(Group));
    group->rectangles = RectangleList;
    group->circles = CircleList;
    group->paths = PathList;
    group->otherAttributes = attributeList4;

    List *GroupList = initializeList(groupToString, deleteGroup, compareGroups);
    List *GroupList1 = initializeList(groupToString, deleteGroup, compareGroups);

    group->groups = GroupList1;
    insertFront(GroupList, group);
    
    /* group->groups = GroupList; */
    printf("printing group\n");

    buffer = toString(GroupList);
    printf("%s\n", buffer);
    free(buffer);

    printf("---FREEING LIST GROUPS---\n");
    freeList(GroupList);

    /* createSVGimage("../rect.svg"); */

    return 0;
}