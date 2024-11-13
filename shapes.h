// Define a structure to represent a shape
typedef struct {
    int x, y, width, height, type;  // Position, size, and type of the shape
    Color color;  // Color of the shape
    int selected;  // Flag to indicate if the shape is selected
} Shape;

// Create an empty list to store shapes
Shape *shapes = NULL;
int numShapes = 0;

// Create an empty list to store undone shapes
Shape *undoneShapes = NULL;
int numUndoneShapes = 0;

// Define a function that creates a new shape and adds it to the shapes array
void addShape(int x, int y, int w, int h, int t, Color c) {
    Shape newShape = { x - w * 6 + 16, y - h * 6 + 16, w * 12, h * 12, t, c, 0 };
    shapes = (Shape *)realloc(shapes, (numShapes + 1) * sizeof(Shape));
    shapes[numShapes] = newShape;
    numShapes++;
    
    // When a new shape is added, clear the undone shapes list
    free(undoneShapes);
    undoneShapes = NULL;
    numUndoneShapes = 0;
}

// Define a function that adds a shape to the undone shapes list
void addUndoneShape(Shape *shape) {
    undoneShapes = (Shape *)realloc(undoneShapes, (numUndoneShapes + 1) * sizeof(Shape));
    undoneShapes[numUndoneShapes] = *shape;
    numUndoneShapes++;
}

// Define a function to redo the last undone shape
void redoShape() {
    if (numUndoneShapes > 0) {
        // Increment the number of shapes
        numShapes++;
        
        // Resize the shapes array and add the last undone shape
        shapes = (Shape *)realloc(shapes, numShapes * sizeof(Shape));
        shapes[numShapes - 1] = undoneShapes[numUndoneShapes - 1];
        
        // Decrement the number of undone shapes and resize the undone shapes array
        numUndoneShapes--;
        undoneShapes = (Shape *)realloc(undoneShapes, numUndoneShapes * sizeof(Shape));
    }
}
