#include <stdio.h>
#include "SVGParser.h"

int main (void) {
    Attribute *attribute1 = malloc (sizeof(Attribute));
    Attribute *attribute2 = malloc (sizeof(Attribute));

    Rectangle *rect1 = malloc (sizeof(Rectangle));
    Rectangle *rect2 = malloc (sizeof(Rectangle));

    Circle *circ1 = malloc (sizeof(Circle));
    Circle *circ2 = malloc (sizeof(Circle));

    Path *path1 = malloc (sizeof(Path));
    Path *path2 = malloc (sizeof(Path));

    char *name1 = malloc(5);
    strcpy(name1, "test");
    char value1 = malloc(6);
    strcpy(value1, "value");

    attribute1->name = name1;
    attribute1->value = value1; 

    char data1 = malloc(5);
    strcpy(data1, "data");

    return 0;
}