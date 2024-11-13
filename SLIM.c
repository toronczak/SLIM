#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "files.h"

void screen() {
	DrawRectangle(16, 16, 768, 480, WHITE);
	for (int i = 0; i < numShapes; i++) {
		if (shapes[i].type == 0) DrawRectangle(shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height, shapes[i].color);
		else if (shapes[i].type == 10) DrawEllipse(shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height, shapes[i].color);
		else if (shapes[i].type == 20) {
			DrawTriangle(
				(Vector2) {shapes[i].x, shapes[i].y},
				(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
				shapes[i].color
			);
		}
		else if (shapes[i].type == 21) {
			DrawTriangle(
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
				(Vector2) {shapes[i].x, shapes[i].y},
				(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
				shapes[i].color
			);
		}
		else if (shapes[i].type == 22) {
			DrawTriangle(
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
				(Vector2) {shapes[i].x, shapes[i].y},
				shapes[i].color
			);
		}
		else if (shapes[i].type == 23) {
			DrawTriangle(
				(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
				(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
				shapes[i].color
			);
		}
	}
}

bool CheckCollisionTriangleRec(Vector2 p1, Vector2 p2, Vector2 p3, Rectangle rec) {
	// Check if any vertex of the triangle is inside the rectangle
	if (CheckCollisionPointRec(p1, rec) || CheckCollisionPointRec(p2, rec) || CheckCollisionPointRec(p3, rec)) {
		return true;
	}

	// Check if any edge of the rectangle intersects with any edge of the triangle
	if (CheckCollisionLines(p1, p2, (Vector2){rec.x, rec.y}, (Vector2){rec.x + rec.width, rec.y}, NULL) || // Top edge of rec
		CheckCollisionLines(p1, p2, (Vector2){rec.x, rec.y + rec.height}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL) || // Bottom edge of rec
		CheckCollisionLines(p1, p2, (Vector2){rec.x, rec.y}, (Vector2){rec.x, rec.y + rec.height}, NULL) || // Left edge of rec
		CheckCollisionLines(p1, p2, (Vector2){rec.x + rec.width, rec.y}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL) || // Right edge of rec
		CheckCollisionLines(p2, p3, (Vector2){rec.x, rec.y}, (Vector2){rec.x + rec.width, rec.y}, NULL) ||
		CheckCollisionLines(p2, p3, (Vector2){rec.x, rec.y + rec.height}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL) ||
		CheckCollisionLines(p2, p3, (Vector2){rec.x, rec.y}, (Vector2){rec.x, rec.y + rec.height}, NULL) ||
		CheckCollisionLines(p2, p3, (Vector2){rec.x + rec.width, rec.y}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL) ||
		CheckCollisionLines(p3, p1, (Vector2){rec.x, rec.y}, (Vector2){rec.x + rec.width, rec.y}, NULL) ||
		CheckCollisionLines(p3, p1, (Vector2){rec.x, rec.y + rec.height}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL) ||
		CheckCollisionLines(p3, p1, (Vector2){rec.x, rec.y}, (Vector2){rec.x, rec.y + rec.height}, NULL) ||
		CheckCollisionLines(p3, p1, (Vector2){rec.x + rec.width, rec.y}, (Vector2){rec.x + rec.width, rec.y + rec.height}, NULL)) {
		return true;
	}

	// No collision detected
	return false;
}

void drawTextValue(char *label, char *format, float value, int x, int y) {
	DrawText(label, x, y, 20, WHITE);
	DrawText(TextFormat(format, value), x, y + 20, 50, WHITE);
}

int main(void) {
	const int screenWidth = 800;
	const int screenHeight = 600;
	
	InitWindow(screenWidth, screenHeight, "SLIM ~sometimes less is more~");
	SetTargetFPS(60);

	int cursorX = 32;
	int cursorY = 20;
	int width = 2;
	int height = 2;
	int shape = 0;
	int subShape = 0;
	int previousShape;
	int tempShape = 3;
	int pickedColor = 15;
	int saveButtonTime = 0;
	int colorPickFlag = 0;
	int redScroll = 0;
	int greenScroll = 0;
	int blueScroll = 0;
	int previousFrame = 0;
	int grab = 0;
	int loadMenuFlag = 0;
	int saveMenuFlag = 0;
	int menuPage = 0;
	int fileOpenFlag[1000] = {0};
	int customColorPicked = 0;
	int selectionStartX = 999;
	int selectionStartY = 999;
	int selectionX;
	int selectionY;
	int selectionWidth;
	int selectionHeight;
	Color color = BLACK;
	
	Texture2D undo = LoadTexture("icons/undo.png");
	Texture2D redo = LoadTexture("icons/redo.png");
	
	Texture2D save = LoadTexture("icons/save.png");
	Texture2D new = LoadTexture("icons/new.png");
	Texture2D load = LoadTexture("icons/load.png");

	Texture2D select = LoadTexture("icons/select.png");
	Texture2D horiflip = LoadTexture("icons/horiflip.png");
	Texture2D vertflip = LoadTexture("icons/vertflip.png");
	Texture2D eraser = LoadTexture("icons/eraser.png");


	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))) {
			ToggleFullscreen();
		}
		
		BeginDrawing();
		ClearBackground(BLACK);
		
		screen();
		
		if(width > 2 && IsKeyPressed(KEY_Z)) width -= 2;
		if(width < 99 && IsKeyPressed(KEY_X)) width += 2;
		if(height > 2 && (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_C))) height -= 2;
		if(height < 99 && (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_V))) height += 2;
				  
		if(GetMouseX() < 784) cursorX = (GetMouseX()-8)/12;
		if(GetMouseY() < 496) cursorY = (GetMouseY()-8)/12;
		if(GetMouseY() >= 496) cursorY = 40;

		if (((GetMouseX() < 722 || GetMouseX() > 749) || (GetMouseY() < 533 || GetMouseY() > 560) || colorPickFlag == 1 || loadMenuFlag == 1) && saveMenuFlag == 0) {
			if(shape < 3){
				DrawRectangle(16+cursorX*12-width*6, 16+cursorY*12-height*6, width*12, height*12, ColorAlpha(PURPLE, 0.625f));
				DrawEllipse(16+cursorX*12, 16+cursorY*12, width*6, height*6, ColorAlpha(RED, 0.375f));
				DrawLine(16+cursorX*12-width*6, 16+cursorY*12-height*6, 16+cursorX*12+width*6, 16+cursorY*12+height*6, MAGENTA);
				DrawLine(16+cursorX*12+width*6, 16+cursorY*12-height*6, 16+cursorX*12-width*6, 16+cursorY*12+height*6, MAGENTA);
			}
			for(int i = 0; i < 32; i++) {
				for(int j = 0; j < 20; j++) {
					
					DrawRectangleLines(16+i*24, 16+j*24, 24, 24, ColorAlpha(GRAY, 0.375f));
				}
			}
		}
		
		// Check if the left mouse button is pressed and the cursor is within the drawing area
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseY() < 507) {
			// Check the selected shape and add a shape to the list based on the shape type
			if (shape == 0) addShape(cursorX * 12 + width * 3, cursorY * 12 + height * 3, width / 2, height / 2, 10, color);
			else if (shape == 1) addShape(cursorX * 12, cursorY * 12, width, height, 20 + subShape, color);
			else if (shape == 2) addShape(cursorX * 12, cursorY * 12, width, height, 0, color);
		}

		// Check if the left mouse button is pressed, the selected shape is the selection tool (shape == 3)
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && shape == 3) {
			// Record the starting point of the selection
			selectionStartX = GetMouseX();
			selectionStartY = GetMouseY();
		}
		
		// Check if the left mouse button is held down, the selected shape is the selection tool (shape == 3), and the starting point is within the drawing area	
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && shape == 3 && selectionStartY < 507) {
			if(GetMouseX()-selectionStartX >= 0 && GetMouseY()-selectionStartY >= 0){
				selectionX = selectionStartX;
				selectionY = selectionStartY;
				selectionWidth = GetMouseX()-selectionStartX;
				selectionHeight = GetMouseY()-selectionStartY;
			}
			else if(GetMouseX()-selectionStartX < 0 && GetMouseY()-selectionStartY >= 0){
				selectionX = GetMouseX();
				selectionY = selectionStartY;
				selectionWidth = selectionStartX-GetMouseX();
				selectionHeight = GetMouseY()-selectionStartY;
			}
			else if(GetMouseX()-selectionStartX >= 0 && GetMouseY()-selectionStartY < 0){
				selectionX = selectionStartX;
				selectionY = GetMouseY();
				selectionWidth = GetMouseX()-selectionStartX;
				selectionHeight = selectionStartY-GetMouseY();
			}
			else {
				selectionX = GetMouseX();
				selectionY = GetMouseY();
				selectionWidth = selectionStartX-GetMouseX();
				selectionHeight = selectionStartY-GetMouseY();
			}
			DrawRectangle(selectionX, selectionY, selectionWidth, selectionHeight, (Color){0, 102, 204, 70});
			DrawRectangleLines(selectionX, selectionY, selectionWidth, selectionHeight, (Color){0, 120, 215, 255});
			
			for (int i = 0; i < numShapes; i++) shapes[i].selected = 0;
		}
		
		if (shape != 3) {
			selectionX = 0;
			selectionY = 0;
			selectionWidth = 0;
			selectionHeight = 0;
		}

		for (int i = 0; i < numShapes; i++) {
			if (shapes[i].type == 0 && CheckCollisionRecs(
				(Rectangle){selectionX, selectionY, selectionWidth, selectionHeight},
				(Rectangle){shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height}
				)) {
				DrawRectangle(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 102, 204, 70});
				DrawRectangleLines(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 120, 215, 255});
				shapes[i].selected = 1;
			}
			
			else if (shapes[i].type == 10){
				float ratio = (float)shapes[i].width/shapes[i].height;
				if(CheckCollisionCircleRec(
					(Vector2){shapes[i].x, shapes[i].y*ratio},
					(float)(shapes[i].width),
					(Rectangle){selectionX, selectionY*ratio, selectionWidth, selectionHeight*ratio}
					)) {
					DrawRectangle(shapes[i].x-6-shapes[i].width, shapes[i].y-6-shapes[i].height, shapes[i].width*2+12, shapes[i].height*2+12, (Color){0, 102, 204, 70});
					DrawRectangleLines(shapes[i].x-6-shapes[i].width, shapes[i].y-6-shapes[i].height, shapes[i].width*2+12, shapes[i].height*2+12, (Color){0, 120, 215, 255});
					shapes[i].selected = 1;
				}
			}
			else if (shapes[i].type == 20){
				if(CheckCollisionTriangleRec(
					(Vector2) {shapes[i].x, shapes[i].y},
					(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
					(Rectangle){selectionX, selectionY, selectionWidth, selectionHeight}
					)){
						DrawRectangle(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 102, 204, 70});
						DrawRectangleLines(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 120, 215, 255});
						shapes[i].selected = 1;
					}
				}
			else if (shapes[i].type == 21){
				if(CheckCollisionTriangleRec(
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
					(Vector2) {shapes[i].x, shapes[i].y},
					(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
					(Rectangle){selectionX, selectionY, selectionWidth, selectionHeight}
					)){
						DrawRectangle(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 102, 204, 70});
						DrawRectangleLines(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 120, 215, 255});
						shapes[i].selected = 1;
					}
			}
			else if (shapes[i].type == 22){
				if(CheckCollisionTriangleRec(
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
					(Vector2) {shapes[i].x, shapes[i].y},
					(Rectangle){selectionX, selectionY, selectionWidth, selectionHeight}
					)){
						DrawRectangle(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 102, 204, 70});
						DrawRectangleLines(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 120, 215, 255});
						shapes[i].selected = 1;
					}
			}
			else if (shapes[i].type == 23){
				if(CheckCollisionTriangleRec(
					(Vector2) {shapes[i].x, shapes[i].y + shapes[i].height},
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y + shapes[i].height},
					(Vector2) {shapes[i].x + shapes[i].width, shapes[i].y},
					(Rectangle){selectionX, selectionY, selectionWidth, selectionHeight}
					)){
						DrawRectangle(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 102, 204, 70});
						DrawRectangleLines(shapes[i].x-6, shapes[i].y-6, shapes[i].width+12, shapes[i].height+12, (Color){0, 120, 215, 255});
						shapes[i].selected = 1;
				}
			}
		}
	 
		DrawRectangle(0, 0, 800, 16, BLACK);
		DrawRectangle(0, 16, 16, 480, BLACK);
		DrawRectangle(784, 16, 16, 480, BLACK);
		DrawRectangle(0, 496, 800, 104, BLACK);
		
		DrawTexture(undo, 288, 532, ColorAlpha(WHITE, 0.375f));
		if(GetMouseX() >= 288 && GetMouseX() <= 307 && GetMouseY() >= 532 && GetMouseY() <= 553) {
			DrawTexture(undo, 288, 532, WHITE);
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && numShapes > 0) {
				addUndoneShape(&shapes[numShapes - 1]);
				// Remove the last shape from the shapes list
				numShapes--;
				shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
			}
		}
		
		if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
			if (IsKeyPressed(KEY_Z)) {
				if (numShapes > 0) {
					addUndoneShape(&shapes[numShapes - 1]);
					// Remove the last shape from the shapes list
					numShapes--;
					shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
				}
			}
		}
		
		DrawTexture(redo, 288, 561, ColorAlpha(WHITE, 0.375f));
		if(GetMouseX() >= 288 && GetMouseX() <= 307 && GetMouseY() >= 561 && GetMouseY() <= 582) {
			DrawTexture(redo, 288, 561, WHITE);
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				redoShape();
			}
		}
		
		if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
			if (IsKeyPressed(KEY_Y)) {
				redoShape();
			}
		}
		
		// Check if the right mouse button is pressed and the cursor is within the drawing area
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && GetMouseY() < 507) {
			// Check the selected shape and add a shape to the list based on the shape type
			if(shape == 0) addShape(cursorX*12+width*3, cursorY*12+height*3 , width/2 , height/2 , 10 , WHITE);  
			else if(shape == 1) addShape(cursorX*12 , cursorY*12 , width , height , 20+subShape , WHITE);
			else if(shape == 2) addShape(cursorX*12 , cursorY*12 , width , height , 0 , WHITE);
		}
		
		drawTextValue("width", "%.0f", width, 72, 514);

		DrawTriangle(
			(Vector2) {60, 542},
			(Vector2) {40, 522},
			(Vector2) {20, 542},
			WHITE);
		
		if(GetMouseX() >= 20 && GetMouseX() <= 59 && GetMouseY() >= 522 && GetMouseY() <= 541) {
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && width <= 99) width += 2;
			DrawTriangle(
			(Vector2) {60, 542},
			(Vector2) {40, 522},
			(Vector2) {20, 542},
			ColorAlpha(BLACK, 0.625f));
		}
			
		DrawTriangle(
			(Vector2) {20, 552},
			(Vector2) {40, 572},
			(Vector2) {60, 552},
			WHITE);
			
		if(GetMouseX() >= 20 && GetMouseX() <= 59 && GetMouseY() >= 552 && GetMouseY() <= 571) {
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && width > 2) width -= 2;
			DrawTriangle(
			(Vector2) {20, 552},
			(Vector2) {40, 572},
			(Vector2) {60, 552},
			ColorAlpha(BLACK, 0.625f));
		}
		
		drawTextValue("height", "%.0f", height, 216, 514);
		
		DrawTriangle(
			(Vector2) {204, 542},
			(Vector2) {184, 522},
			(Vector2) {164, 542},
			WHITE);
		
		if(GetMouseX() >= 164 && GetMouseX() <= 204 && GetMouseY() >= 522 && GetMouseY() <= 541) {
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && height <= 99) height += 2;
			DrawTriangle(
			(Vector2) {204, 542},
			(Vector2) {184, 522},
			(Vector2) {164, 542},
			ColorAlpha(BLACK, 0.625f));
		}
		
		DrawTriangle(
			(Vector2) {164, 552},
			(Vector2) {184, 572},
			(Vector2) {204, 552},
			WHITE);
			
		if(GetMouseX() >= 164 && GetMouseX() <= 204 && GetMouseY() >= 552 && GetMouseY() <= 571) {
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && height > 2) height -= 2;
			DrawTriangle(
			(Vector2) {164, 552},
			(Vector2) {184, 572},
			(Vector2) {204, 552},
			ColorAlpha(BLACK, 0.625f));
		}
		
		for (int i = 0; i < 12; i++) {
			DrawRectangle(323+i*26-i/6*156, 510+i/6*26, 25, 25, ColorFromHSV(-30+i/2*60, 1, 0.9-i%2*0.3));
			if(GetMouseX() >= 323+i*26-i/6*156 && GetMouseX() <= 347+i*26-i/6*156 && GetMouseY() >= 510+i/6*26 && GetMouseY() <= 534+i/6*26) {
				DrawRectangle(322+i*26-i/6*156, 509+i/6*26, 27, 27, ColorAlpha(WHITE, 0.625f));
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					color = ColorFromHSV(-30+i/2*60, 1, 0.9-i%2*0.3);
					pickedColor = i;
					customColorPicked = 0;
				}
			}
		}
		
		for (int i = 0; i < 4; i++) {
			DrawRectangle(323+i*26, 562, 25, 25, ColorAlpha(WHITE, 1-0.335*i));
			if(GetMouseX() >= 323+i*26 && GetMouseX() <= 347+i*26 && GetMouseY() >= 562 && GetMouseY() <= 586) {
				DrawRectangle(322+i*26, 561, 27, 27, ColorAlpha(WHITE, 0.625f));
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					color = (Color) { 255-i*85 , 255-i*85 , 255-i*85 , 255 };
					pickedColor = 12+i;
					customColorPicked = 0;
				}
			}
		}
		if(customColorPicked == 0) {
			for (int i = 0; i < 3; i++) {
				DrawRectangle(328+i*2+pickedColor*26-pickedColor/6*156, 515+i*2+pickedColor/6*26, 15-i*4, 15-i*4, (Color) { 255-i%2*255 , 255-i%2*255 , 255-i%2*255 , 224 });
			}
		}
		else {
			DrawRectangle(426, 562, 52, 25, color);
			for (int i = 0; i < 3; i++) {
				DrawRectangle(328+i*2+117, 515+i*2+52, 15-i*4, 15-i*4, (Color) { 255-i%2*255 , 255-i%2*255 , 255-i%2*255 , 224 });
			}
		}

		DrawRectangle(426, 562, 1, 25, WHITE);
		DrawRectangle(426, 562, 52, 1, WHITE);
		if(customColorPicked == 0) DrawText("+", 445, 561, 30, WHITE);
		DrawRectangleLines(320, 507, 161, 83, WHITE);
		DrawRectangleLines(321, 508, 159, 81, WHITE);
		if(GetMouseX() >= 426 && GetMouseX() <= 477 && GetMouseY() >= 562 && GetMouseY() <= 586) {
			DrawRectangle(426, 562, 52, 25, WHITE);
			DrawText("+", 445, 561, 30, BLACK);
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				colorPickFlag = 1;
			}
		}
		
		if(previousFrame > 0) previousFrame--;
		
		// Check if the color picker flag is set
		if (colorPickFlag == 1) {
			// Iterate over the red, green, and blue color sliders
			for (int i = 0; i <= 255; i++) {
				// Check if left mouse button is pressed on the red color slider
				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetMouseX() == 495 + i && 
					(GetMouseY() >= 512 && GetMouseY() <= 528 || grab == 1) && grab != 2 && grab != 3) {
					redScroll = i;
					previousFrame = 2;
					grab = 1;
				}
				// Check if left mouse button is pressed on the green color slider
				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetMouseX() == 495 + i && 
					(GetMouseY() >= 532 && GetMouseY() <= 548 || grab == 2) && grab != 1 && grab != 3) {
					greenScroll = i;
					previousFrame = 2;
					grab = 2;
				}
				// Check if left mouse button is pressed on the blue color slider
				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetMouseX() == 495 + i && 
					(GetMouseY() >= 552 && GetMouseY() <= 568 || grab == 3) && grab != 1 && grab != 2) {
					blueScroll = i;
					previousFrame = 2;
					grab = 3;
				}
		
				// Draw the red, green, and blue color sliders
				DrawRectangle(495 + i, 508, 1, 15, (Color) { i, 0, 0, 255 } );
				DrawRectangle(495 + i, 528, 1, 15, (Color) { 0, i, 0, 255 } );
				DrawRectangle(495 + i, 548, 1, 15, (Color) { 0, 0, i, 255 } );
			}
		
			// Handle edge cases for color sliders
			if (previousFrame == 1) {
				if (GetMouseX() <= 495) {
					if (grab == 1) redScroll = 0;
					else if (grab == 2) greenScroll = 0;
					else if (grab == 3) blueScroll = 0;
				}
				if (GetMouseX() >= 750) {
					if (grab == 1) redScroll = 255;
					else if (grab == 2) greenScroll = 255;
					else if (grab == 3) blueScroll = 255;
				}
			}
		
			// Draw indicators for selected color values
			DrawRectangle(493 + redScroll, 505, 5, 21, WHITE);
			DrawRectangle(493 + greenScroll, 525, 5, 21, WHITE);
			DrawRectangle(493 + blueScroll, 545, 5, 21, WHITE);
		
			// Draw outlines for the color sliders
			DrawRectangleLines(494, 507, 258, 17, WHITE);
			DrawRectangleLines(494, 527, 258, 17, WHITE);
			DrawRectangleLines(494, 547, 258, 17, WHITE);
		
			// Display the current values of the color sliders
			DrawText(TextFormat("%i", redScroll), 759, 506, 20, WHITE);
			DrawText(TextFormat("%i", greenScroll), 759, 526, 20, WHITE);
			DrawText(TextFormat("%i", blueScroll), 759, 546, 20, WHITE);
		
			// Draw a rectangle representing the selected color
			DrawRectangle(495, 568, 255, 24, (Color){redScroll, greenScroll, blueScroll, 255});
			DrawRectangleLines(494, 567, 257, 26, WHITE);
		
			// Check if the mouse is over the "OK" button and handle the selection
			if(GetMouseX() >= 758 && GetMouseX() <= 793 && GetMouseY() >= 567 && GetMouseY() <= 592) {
				DrawRectangle(758, 567, 36, 26, ColorAlpha(WHITE, 0.375f));
				DrawText("OK", 763, 571, 20, BLACK);
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					color = (Color) { redScroll , greenScroll , blueScroll , 255 };
					customColorPicked = 1;
					colorPickFlag = 0;
				}
			} else {
				DrawRectangle(758, 567, 36, 26, WHITE);
				DrawText("OK", 763, 571, 20, BLACK);
			}
		}
		
		// If the loadMenuFlag is set
		else if(loadMenuFlag == 1) {
			// Iterate over the file slots
			for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
				// Check if file slot is open
				if(fileOpenFlag[i] == 1) {
				
					// Draw rectangle and text for file slot
					DrawRectangle(497+(i%5)*43, 509+((i/5)%2)*43, 36, 36, WHITE);
					DrawText(TextFormat("%d", i), 514+(i%5)*43-MeasureText(TextFormat("%d", i), 30)/2, 514+((i/5)%2)*43, 30, BLACK);
					
					// Check if mouse is over the file slot
					if(GetMouseX() >= 497+(i%5)*43 && GetMouseX() <= 532+(i%5)*43 && 
					   GetMouseY() >= 509+((i/5)%2)*43 && GetMouseY() <= 544+((i/5)%2)*43) {
						// Draw selection rectangle and handle file loading
						DrawRectangle(497+(i%5)*43, 509+((i/5)%2)*43, 36, 36, ColorAlpha(BLACK, 0.625f));
						if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
							selectionX = 0;
							selectionY = 0;
							selectionWidth = 0;
							selectionHeight = 0;
							loadFile(i);
						}
					}
				}
			}
			

			if(menuPage > 0){
				// Check if mouse is over the button and handle the selection
				if(GetMouseX() >= 758 && GetMouseX() <= 773 && GetMouseY() >= 538 && GetMouseY() <= 560) {
					DrawRectangle(758, 538, 16, 23, ColorAlpha(WHITE, 0.375f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						menuPage--;
						for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
							char fileName[20];
							sprintf(fileName, "savedata\\%d.ug", i);
					
							FILE *fptr;
							if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
						}
					}
				} else {
					DrawRectangle(758, 538, 16, 23, WHITE);
				}
				DrawText("<", 763, 540, 20, BLACK);
			}
			
			
			char fileName[20];
			sprintf(fileName, "savedata\\%d.ug", 10+menuPage*10);
					
			FILE *fptr;
			if(fptr = fopen(fileName, "r")) fileOpenFlag[10+menuPage*10] = 1;
			
			
			if(fileOpenFlag[10+menuPage*10] == 1){
				// Check if mouse is over the button and handle the selection
				if(GetMouseX() >= 778 && GetMouseX() <= 793 && GetMouseY() >= 538 && GetMouseY() <= 560) {
					DrawRectangle(778, 538, 16, 23, ColorAlpha(WHITE, 0.375f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						menuPage++;
						for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
							char fileName[20];
							sprintf(fileName, "savedata\\%d.ug", i);
					
							FILE *fptr;
							if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
						}
					}
				} else {
					DrawRectangle(778, 538, 16, 23, WHITE);
				}
				DrawText(">", 783, 540, 20, BLACK);
			}

			drawTextValue("page", "%.0f", menuPage+1, 718, 514);
			// Check if mouse is over the "OK" button and handle the selection
			if(GetMouseX() >= 758 && GetMouseX() <= 793 && GetMouseY() >= 567 && GetMouseY() <= 592) {
				DrawRectangle(758, 567, 36, 26, ColorAlpha(WHITE, 0.375f));
				DrawText("OK", 763, 571, 20, BLACK);
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					loadMenuFlag = 0;
				}
			} else {
				DrawRectangle(758, 567, 36, 26, WHITE);
				DrawText("OK", 763, 571, 20, BLACK);
			}
		}
		
		// If the saveMenuFlag is set
		else if(saveMenuFlag == 1) {
			int largestI = -1;
			int saveFlag = -1;
			
			// Iterate over the file slots
			for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
				// Check if file slot is open
				if(fileOpenFlag[i] == 1) {
					// Record the largest file index
					largestI = i-menuPage*10;
					
					// Draw rectangle and text for file slot
					DrawRectangle(497+(i%5)*43, 509+((i/5)%2)*43, 36, 36, WHITE);
					DrawText(TextFormat("%d", i), 514+(i%5)*43-MeasureText(TextFormat("%d", i), 30)/2, 514+((i/5)%2)*43, 30, BLACK);
					
					// Check if mouse is over the file slot
					if(GetMouseX() >= 497+(i%5)*43 && GetMouseX() <= 532+(i%5)*43 && 
					   GetMouseY() >= 509+((i/5)%2)*43 && GetMouseY() <= 544+((i/5)%2)*43) {
						// Draw selection rectangle and handle file saving
						DrawRectangle(497+(i%5)*43, 509+((i/5)%2)*43, 36, 36, ColorAlpha(BLACK, 0.625f));
						if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
							saveFlag = i;
						}
					}
				}
			}
			
			// Increment the largest index for a new save slot
			largestI++;
			if(largestI < 10) {
				// Draw rectangle and text for the new save slot
				DrawRectangleLines(497+(largestI%5)*43, 509+((largestI/5)%2)*43, 36, 36, WHITE);
				DrawText("+", 507+(largestI%5)*43, 514+((largestI/5)%2)*43, 30, WHITE);
				
				// Check if mouse is over the new save slot
				if(GetMouseX() >= 497+(largestI%5)*43 && GetMouseX() <= 532+(largestI%5)*43 && 
				   GetMouseY() >= 509+((largestI/5)%2)*43 && GetMouseY() <= 544+((largestI/5)%2)*43) {
					// Draw selection rectangle and handle file saving
					DrawRectangle(497+(largestI%5)*43, 509+((largestI/5)%2)*43, 36, 36, WHITE);
					DrawText("+", 507+(largestI%5)*43, 514+((largestI/5)%2)*43, 30, BLACK);
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						saveFlag = largestI+menuPage*10;
					}
				}
			}
            
			if(menuPage > 0){
				// Check if mouse is over the button and handle the selection
				if(GetMouseX() >= 758 && GetMouseX() <= 773 && GetMouseY() >= 538 && GetMouseY() <= 560) {
					DrawRectangle(758, 538, 16, 23, ColorAlpha(WHITE, 0.375f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						menuPage--;
						for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
							char fileName[20];
							sprintf(fileName, "savedata\\%d.ug", i);
					
							FILE *fptr;
							if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
						}
					}
				} else {
					DrawRectangle(758, 538, 16, 23, WHITE);
				}
				DrawText("<", 763, 540, 20, BLACK);
			}
			if(largestI == 10){
				// Check if mouse is over the button and handle the selection
				if(GetMouseX() >= 778 && GetMouseX() <= 793 && GetMouseY() >= 538 && GetMouseY() <= 560) {
					DrawRectangle(778, 538, 16, 23, ColorAlpha(WHITE, 0.375f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						menuPage++;
						for (int i = 0+menuPage*10; i < 10+menuPage*10; i++) {
							char fileName[20];
							sprintf(fileName, "savedata\\%d.ug", i);
					
							FILE *fptr;
							if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
						}
					}
				} else {
					DrawRectangle(778, 538, 16, 23, WHITE);
				}
				DrawText(">", 783, 540, 20, BLACK);
			}
			// Check if mouse is over the "OK" button and handle the selection
			if(GetMouseX() >= 758 && GetMouseX() <= 793 && GetMouseY() >= 567 && GetMouseY() <= 592) {
				DrawRectangle(758, 567, 36, 26, ColorAlpha(WHITE, 0.375f));
				DrawText("OK", 763, 571, 20, BLACK);
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					saveMenuFlag = 0;
				}
			} else {
				DrawRectangle(758, 567, 36, 26, WHITE);
				DrawText("OK", 763, 571, 20, BLACK);
			}
			
			drawTextValue("page", "%.0f", menuPage+1, 718, 514);
			// If a file index is selected, save the file and reset the saveMenuFlag
			if(saveFlag > -1){
				EndDrawing();
				saveFile(saveFlag);
				BeginDrawing();
				saveMenuFlag = 0;
			}
		}
		
		
		else if(colorPickFlag == 0 && loadMenuFlag == 0 && saveMenuFlag == 0) {
			menuPage = 0;
			if(shape < 3){
				// Circle button
				DrawRectangle(497, 516, 64, 64, WHITE);
				if(shape == 0) DrawRectangle(497, 516, 64, 64, ColorAlpha(RED, 0.375f));
				if(GetMouseX() >= 497 && GetMouseX() <= 560 && GetMouseY() >= 516 && GetMouseY() <= 579) DrawRectangle(497, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
				if(GetMouseX() >= 497 && GetMouseX() <= 560 && GetMouseY() >= 516 && GetMouseY() <= 579 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) shape = 0;
				DrawCircle(475+54, 548, 20, BLACK);
				
				// triangle button
				DrawRectangle(571, 516, 64, 64, WHITE);
				if(shape == 1) DrawRectangle(571, 516, 64, 64, ColorAlpha(RED, 0.375f));
				if(GetMouseX() >= 571 && GetMouseX() <= 634 && GetMouseY() >= 516 && GetMouseY() <= 579) DrawRectangle(571, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
				if(GetMouseX() >= 571 && GetMouseX() <= 634 && GetMouseY() >= 516 && GetMouseY() <= 579 && shape == 1) {
					if(subShape == 3) {
					DrawTriangle(
						(Vector2) {583, 568},
						(Vector2) {623, 568},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 0) {
					DrawTriangle(
						(Vector2) {583, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 1) {
					DrawTriangle(
						(Vector2) {623, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 2) {
					DrawTriangle(
						(Vector2) {623, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 568},
						BLACK);
					}
				}
				else {
					if(subShape == 0) {
					DrawTriangle(
						(Vector2) {583, 568},
						(Vector2) {623, 568},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 1) {
					DrawTriangle(
						(Vector2) {583, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 2) {
					DrawTriangle(
						(Vector2) {623, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 528},
						BLACK);
					}
					else if(subShape == 3) {
					DrawTriangle(
						(Vector2) {623, 568},
						(Vector2) {623, 528},
						(Vector2) {583, 568},
						BLACK);
					}
				}
					
				if(GetMouseX() >= 571 && GetMouseX() <= 634 && GetMouseY() >= 516 && GetMouseY() <= 579 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					if (shape == 1) subShape = (subShape+1)%4;
					shape = 1;
				}
				
				// Rectangle button
				DrawRectangle(645, 516, 64, 64, WHITE);
				if(shape == 2) DrawRectangle(645, 516, 64, 64, ColorAlpha(RED, 0.375f));
				if(GetMouseX() >= 645 && GetMouseX() <= 708 && GetMouseY() >= 516 && GetMouseY() <= 579) DrawRectangle(645, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
				if(GetMouseX() >= 645 && GetMouseX() <= 708 && GetMouseY() >= 516 && GetMouseY() <= 579 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) shape = 2;
				DrawRectangle(657, 528, 40, 40, BLACK);
			}
			else{
				DrawRectangle(497, 516, 64, 64, WHITE);
				DrawTexture(horiflip, 508, 527, BLACK);
				if(GetMouseX() >= 497 && GetMouseX() <= 560 && GetMouseY() >= 516 && GetMouseY() <= 579) {
					DrawRectangle(497, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						for (int i = 0; i < numShapes; i++) {
							if(shapes[i].selected == 1) {
								if(shapes[i].type == 20) shapes[i].type = 23;
								else if(shapes[i].type == 23) shapes[i].type = 20;
								else if(shapes[i].type == 21) shapes[i].type = 22;
								else if(shapes[i].type == 22) shapes[i].type = 21;
							}
						}
					}
				}
				DrawRectangle(571, 516, 64, 64, WHITE);
				DrawTexture(vertflip, 582, 527, BLACK);
				if(GetMouseX() >= 571 && GetMouseX() <= 634 && GetMouseY() >= 516 && GetMouseY() <= 579) {
					DrawRectangle(571, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						for (int i = 0; i < numShapes; i++) {
							if(shapes[i].selected == 1) {
								if(shapes[i].type == 20) shapes[i].type = 21;
								else if(shapes[i].type == 21) shapes[i].type = 20;
								else if(shapes[i].type == 22) shapes[i].type = 23;
								else if(shapes[i].type == 23) shapes[i].type = 22;
							}
						}
					}
				}
				DrawRectangle(645, 516, 64, 64, WHITE);
				DrawTexture(eraser, 656, 527, BLACK);
				if(GetMouseX() >= 645 && GetMouseX() <= 708 && GetMouseY() >= 516 && GetMouseY() <= 579) {
					DrawRectangle(645, 516, 64, 64, ColorAlpha(BLACK, 0.625f));
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						for (int i = 0; i < numShapes; i++) {
							if(shapes[i].selected == 1) {
								// Mark the shape for removal (optional)
								shapes[i].type = -1; // or another flag to indicate removal
							}
						}
				
						// Filter out shapes marked for removal
						int newNumShapes = 0;
						for (int i = 0; i < numShapes; i++) {
							if(shapes[i].type != -1) {
								// Keep this shape in the array
								shapes[newNumShapes] = shapes[i];
								newNumShapes++;
							}
						}
				
						// Update the number of shapes
						numShapes = newNumShapes;
				
						// Resize the array if necessary
						shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
					}
				}
			}
			
			DrawRectangle(724, 571, 24, 24, WHITE);
			if(shape == 3) DrawRectangle(724, 571, 24, 24, ColorAlpha(RED, 0.375f));
			if(GetMouseX() >= 724 && GetMouseX() <= 747 && GetMouseY() >= 571 && GetMouseY() <= 594){
				DrawRectangle(724, 571, 24, 24, ColorAlpha(BLACK, 0.625f));
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					previousShape = shape;
					shape = tempShape;
					tempShape = previousShape;
				}
			}
			DrawTexture(select, 728, 575, BLACK);
			
			// Save/new/load buttons
			DrawTexture(save, 722, 533, ColorAlpha(WHITE, 0.375f));
			DrawTexture(new, 757, 516, ColorAlpha(WHITE, 0.375f));
			DrawTexture(load, 757, 552, ColorAlpha(WHITE, 0.375f));
			if(GetMouseX() >= 722 && GetMouseX() <= 749 && GetMouseY() >= 533 && GetMouseY() <= 560) DrawTexture(save, 722, 533, WHITE);
			DrawRectangle(719, 530, 34, 34, ColorAlpha(WHITE, 0.05f*saveButtonTime));
			DrawTexture(save, 722, 533, ColorAlpha(BLACK, 0.05f*saveButtonTime));
			if(GetMouseX() >= 757 && GetMouseX() <= 784 && GetMouseY() >= 516 && GetMouseY() <= 543) {
				DrawTexture(new, 757, 516, WHITE);
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					while(numShapes > 0) {
						numShapes--;
						shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
					}
				}
			}
			if(GetMouseX() >= 757 && GetMouseX() <= 784 && GetMouseY() >= 552 && GetMouseY() <= 579) {
				DrawTexture(load, 757, 552, WHITE);
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					loadMenuFlag = 1;
					for (int i = 0; i < 10; i++) {
						char fileName[20];
						sprintf(fileName, "savedata\\%d.ug", i);
					
						FILE *fptr;
						if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
					}
				}
			}
		}
		
		if(IsMouseButtonUp(MOUSE_BUTTON_LEFT)) grab = 0;
		
		EndDrawing();

		if ((GetMouseX() >= 722 && GetMouseX() <= 749 && GetMouseY() >= 533 && GetMouseY() <= 560) && colorPickFlag == 0 && loadMenuFlag == 0 && saveMenuFlag == 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			saveMenuFlag = 1;
			for (int i = 0; i < 10; i++) {
				char fileName[20];
				sprintf(fileName, "savedata\\%d.ug", i);
					
				FILE *fptr;
				if(fptr = fopen(fileName, "r")) fileOpenFlag[i] = 1;
			}
		}
	}
	
	UnloadTexture(undo);
	UnloadTexture(redo);
	UnloadTexture(select);
	UnloadTexture(save);
	UnloadTexture(new);
	UnloadTexture(load);
	UnloadTexture(horiflip);
	UnloadTexture(vertflip);
	UnloadTexture(eraser);
	CloseWindow();
	return 0;
}