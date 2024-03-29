#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SVGParser.h"
xmlDoc* convertSVGimageToXMLdoc (SVGimage* image) ;
int main (int argc, char **argv) {
    //char *buffer;

    //SVGimage* img = createSVGimage("beer.svg");
    if (argc != 3) {
        return(1);
    }

    SVGimage* img = createValidSVGimage(argv[1], "svg.xsd");
    validateSVGimage(img, "svg.xsd");
    writeSVGimage(img, argv[2]);

    Rectangle *rect1 = malloc (sizeof(Rectangle));

    rect1->x = 1.1;
    rect1->y = 2.1;
    rect1->width = 3.1;
    rect1->height = 4.1;
    strcpy(rect1->units, "cm");

    List *attributeList = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect1->otherAttributes = attributeList;
    
    Attribute *attribute1 = malloc (sizeof(Attribute));
    char *name1 = malloc(5);
    strcpy(name1, "x");
    char *value1 = malloc(6);
    strcpy(value1, "15cm");

    attribute1->name = name1;
    attribute1->value = value1; 

    char *string = SVGimageToString(img);
    printf("==================before==================\n%s\n====================================\n", string);

    free(string);
    //ToJSON functions testing
    string = SVGtoJSON(img);
    printf("======================================SVG======================================\n%s\n============================================================================\n", string);
    free(string);
    string = attrListToJSON(img->otherAttributes);
    printf("======================================attribute======================================\n%s\n============================================================================\n", string);
    free(string);
    addComponent(img, RECT, rect1);
    setAttribute(img, RECT, 2, attribute1);
    
    string = SVGimageToString(img);
    printf("==================after==================\n%s\n====================================\n", string);
    free(string);

    //ToJSON functions testing
    string = SVGtoJSON(img);
    printf("======================================SVG======================================\n%s\n============================================================================\n", string);
    free(string);
    rect1 = getFromFront(img->rectangles);
    string = attrListToJSON(rect1->otherAttributes);
    printf("======================================attribute======================================\n%s\n============================================================================\n", string);
    free(string);
    string = circListToJSON(img->circles);
    printf("======================================circles======================================\n%s\n============================================================================\n", string);
    free(string);
    //Group *group = getFromFront(img->groups);
    string = pathListToJSON(img->paths);
    printf("======================================paths======================================\n%s\n============================================================================\n", string);
    free(string);
    string = rectListToJSON(img->rectangles);
    printf("======================================rects======================================\n%s\n============================================================================\n", string);
    free(string);
    string = groupListToJSON(img->groups);
    printf("======================================groups======================================\n%s\n============================================================================\n", string);
    free(string);
    deleteSVGimage(img);

    char buffer[1000];
    strcpy(buffer, "{\"x\":1,\"y\":2,\"w\":19,\"h\":15,\"units\":\"\"}");
    rect1 = JSONtoRect(buffer);
    string = rectangleToString(rect1);
    printf("%s\n", string);
    free(string);
    deleteRectangle(rect1);

    strcpy(buffer, "{\"cx\":32,\"cy\":32,\"r\":30,\"units\":\"\"}");
    Circle *circ = JSONtoCircle(buffer);
    string = circleToString(circ);
    printf("%s\n", string);
    free(string);
    deleteCircle(circ);

    strcpy(buffer, "{\"title\":\"Example quad01 - quadratic Bézier commands in path data\",\"descr\":\"Picture showing a \"Q\" a \"T\" command, along with annotations showing the control points   and end points\"}");
    SVGimage *testimg = JSONtoSVG(buffer);
    validateSVGimage(testimg, "svg.xsd");
    string = SVGimageToString(testimg);
    printf("%s\n", string);
    free(string);
    deleteSVGimage(testimg);
    /*printf("===========================================\n\t\tTESTING ATTRIBUTES\n===========================================\n");



    Attribute *attribute1 = malloc (sizeof(Attribute));
    char *name1 = malloc(5);
    strcpy(name1, "test");
    char *value1 = malloc(6);
    strcpy(value1, "value");

    attribute1->name = name1;
    attribute1->value = value1; 

    
    Attribute *attribute2 = malloc (sizeof(Attribute));
    char *name2 = malloc(6);
    strcpy(name2, "test2");
    char *value2 = malloc(7);
    strcpy(value2, "value2");

    attribute2->name = name2;
    attribute2->value = value2; 

    Attribute *attribute3 = malloc (sizeof(Attribute));
    char *name3 = malloc(6);
    strcpy(name3, "test3");
    char *value3 = malloc(7);
    strcpy(value3, "value3");

    attribute3->name = name3;
    attribute3->value = value3;

    Attribute *attribute4 = malloc (sizeof(Attribute));
    char *name4 = malloc(6);
    strcpy(name4, "test4");
    char *value4 = malloc(7);
    strcpy(value4, "value4");

    attribute4->name = name4;
    attribute4->value = value4;

    Attribute *attribute5 = malloc (sizeof(Attribute));
    char *name5 = malloc(6);
    strcpy(name5, "test5");
    char *value5 = malloc(7);
    strcpy(value5, "value5");

    attribute5->name = name5;
    attribute5->value = value5;

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

    List *attributeList5 = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    insertFront(attributeList5, attribute5);

    printf("---FREEING LIST ATTRIBUTES---\n");
    //freeList(attributeList);

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
    printf("---FREEING LIST RECTANGLES---\n");
    freeList(RectangleList);

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
    freeList(CircleList);

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
    freeList(PathList); 

    printf("\n\n");
    printf("===========================================\n\t\tTESTING GROUPS\n===========================================\n");
    Group *group = malloc(sizeof(Group));
    group->rectangles = RectangleList;
    group->circles = CircleList;
    group->paths = PathList;
    group->otherAttributes = attributeList4;
    group->groups = initializeList(groupToString, deleteGroup, compareGroups);

    Group *group2 = malloc(sizeof(Group));
    group2->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    group2->circles = initializeList(circleToString, deleteCircle, compareCircles);
    group2->paths = initializeList(pathToString, deletePath, comparePaths);
    group2->otherAttributes = attributeList5;
    group2->groups = initializeList(groupToString, deleteGroup, compareGroups);

    insertFront(group->groups, group2);

    List *GroupList = initializeList(groupToString, deleteGroup, compareGroups);
    insertFront(GroupList, group);
    
    printf("printing group\n");

    buffer = toString(GroupList);
    printf("%s\n", buffer);
    free(buffer);

    printf("---FREEING LIST GROUPS---\n");
    freeList(GroupList);
 */
    /* if (argc != 2)
        return(1);

    SVGimage *image = createSVGimage(argv[1]);
    char *string = SVGimageToString(image);
    int val = numAttr(image);

    printf("%s\n=======================================\nNum attributes: %d\n", string, val);
    printf("Rects: %d\n", numRectsWithArea(image, 716404));
    printf("Circ: %d\n", numCirclesWithArea(image, 40.72));
    printf("groups: %d\n", numGroupsWithLen(image, 2));
    free(string);

    List *listGroup = getGroups(image);
    List *listPaths = getPaths(image);
    List *listCircles = getCircles(image);
    List *listRects = getRects(image);
    printf("len group: %d, paths: %d, circles: %d, rects: %d\n", getLength(listGroup), getLength(listPaths), getLength(listCircles), getLength(listRects));
    buffer = toString(listGroup);
    printf("======Groups=====\n%s\n", buffer);
    free(buffer);

    printf("======listPaths=====\n");

    buffer = toString(listPaths);
    if(buffer == NULL) {
        printf("ERROR\n");
    }
    printf("%s\n", buffer);
    free(buffer);

    char *buffer2 = toString(listCircles);
    printf("======listCircles=====\n%s\n", buffer2);
    free(buffer2);

    buffer = toString(listRects);
    printf("======listRects=====\n%s\n", buffer);
    free(buffer);

    freeList(listGroup);
    freeList(listRects);
    freeList(listCircles);
    freeList(listPaths);

    
    deleteSVGimage(image); */
    return 0;
}