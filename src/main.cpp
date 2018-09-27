#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <exception>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { throw std::exception(); }

  //CREATE WINDOW FUNCTION
  SDL_Window *window = SDL_CreateWindow("Triangle",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  //ERROR CHECK
  if (!SDL_GL_CreateContext(window)) { throw std::exception(); }
  if (glewInit() != GLEW_OK) { throw std::exception(); }

  bool quit = false;

  //DRAWING WHILE QUIT IS FALSE
  while(!quit)
  {
    SDL_Event event = {0};

    //USING OPENGL TO SET THE WINDOW COLOUR
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);

    //LIST OF POSITIONS THAT DESCRIBE THE SHAPE (X,Y,Z)
    const GLfloat positions[] = {
      0.0f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      0.5f - 0.5f, 0.0f
    };

    GLuint positionsVboId = 0;

    //CREATE A NEW VBO (Vertex Buffer Object) ON THE GPU AND BIND IT
    glGenBuffers(1, &positionsVboId);

    if (!positionsVboId) { throw std::exception(); } //CHECK FOR ERROR
    
    glBindBuffer(GL_ARRAY_BUFFER, positionsVboId); //OPENS THE BUFFER WITH THE POSITIONS INFORMATION
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); //UPLOAD A COPY OF THE DATA FROM MEMORY INTO THE VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); //RESET THE STATE (CLOSING THE BUFFER)

    //CREATE A VAO (Vertex Array Object) 
    GLuint vaoId = 0;
    glGenVertexArrays(1, &vaoId); //CREATE A NEW VAO ON THE GPU AND BIND IT
    
    if (!vaoId) { throw std::exception(); } //CHECK FOR ERROR

    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, positionsVboId); //BIND THE POSITION VBO, ASSIGN IT TO THE POSITION 0 ON THE BOUND VAO AND FLAG IT TO BE USED
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); //RESET THE STATE
    glBindVertexArray(0);



    
    //QUIT EVENT
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
