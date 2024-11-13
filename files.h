#include <dirent.h>
#include "shapes.h"

void saveFile(number) {
    // Get current date and time for creating a unique filename
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dateBuffer[20];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d_%H%M%S", &tm);
    char screenshotFilename[30];
    sprintf(screenshotFilename, "%s.png", dateBuffer);

    // Take a screenshot
    TakeScreenshot(screenshotFilename);
    Image screenshot = LoadImage(screenshotFilename);

    // Crop the image
    Rectangle crop = { 16, 16, 768, 480 };
    ImageCrop(&screenshot, crop);

    // Save the cropped image to a new file
    char croppedFilename[30];
    sprintf(croppedFilename, "%s", screenshotFilename);
    ExportImage(screenshot, croppedFilename);

    // Unload the image from memory
    UnloadImage(screenshot);

    // Create a "savedata" folder if it doesn't exist
    mkdir("savedata");
    char fileName[20];

    sprintf(fileName, "savedata\\%d.ug", number);

    // Open the file for writing
    FILE *fptr;
    fptr = fopen(fileName, "w");

    // Save shape information to the file
    for (int i = 0; i < numShapes; i++) {
        fprintf(fptr, "%d %d %d %d %d %d\n", shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height, shapes[i].type, ColorToInt(shapes[i].color));
    }

    // Close the file
    fclose(fptr);
}

void loadFile(number){
	while(numShapes > 0) {
        numShapes--;
		shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
	}
	char fileName[20];
	sprintf(fileName, "savedata\\%d.ug", number);
						
	FILE *fptr;
	fptr = fopen(fileName, "r");
	int x, y, width, height, type;
	unsigned int c;
	while (fscanf(fptr, "%d %d %d %d %d %ud", &x, &y, &width, &height, &type, &c) != EOF) {
        addShape((x+width/2)-16, (y+height/2)-16, width/12, height/12, type, GetColor(c));
	}
}