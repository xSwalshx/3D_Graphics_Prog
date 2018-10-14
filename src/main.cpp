#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <exception>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

///DEFINE GLOBAL VARIABLES

//LIST OF POSITIONS THAT DESCRIBE THE SHAPE (X,Y,Z)
const GLfloat positions[] = {
  0.0f, 0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f
};

//DEFINE THE COLOURS OF EACH POINT OF THE TRIANGLE (R/G/B/A)
const GLfloat colors[] = {
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f
};

//VERTEX SHADER
const GLchar *vertexShaderSrc =
  "attribute vec3 in_Position;             " \
  "attribute vec4 in_Color;                " \
  "                                        " \
  "varying vec4 ex_Color;                  " \
  "                                        " \
  "void main()                             " \
  "{                                       " \
  "  gl_Position = vec4(in_Position, 1.0); " \
  "  ex_Color = in_Color;                  " \
  "}                                       " \
  "                                        ";

//FRAGMENT SHADER
const GLchar *fragmentShaderSrc =
  "varying vec4 ex_Color;                  " \
  "void main()                             " \
  "{                                       " \
  "  gl_FragColor = ex_Color;              " \
  "}                                       " \
  "                                        ";

int main(int argc, char *argv[])
{

  //-----------------------------------------------------------------------------------------------//

  //CHECK IF SLD WAS INITIALISED
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { throw std::exception(); }

  //CREATE WINDOW FUNCTION
  SDL_Window *window = SDL_CreateWindow("Triangle",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  //ERROR CHECK
  if (!SDL_GL_CreateContext(window) || glewInit() != GLEW_OK) { throw std::exception(); }

  //-----------------------------------------------------------------------------------------------//

  ///CREATE A NEW VBO (Vertex Buffer Object) ON THE GPU AND BIND IT
  GLuint positionsVboId = 0;

  glGenBuffers(1, &positionsVboId);

  if (!positionsVboId) { throw std::exception(); } //CHECK FOR ERROR

  glBindBuffer(GL_ARRAY_BUFFER, positionsVboId); //BIND THE POSITION VBO, ASSIGN IT TO THE POSITION 0 ON THE BOUND VAO AND FLAG IT TO BE USED

  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); //UPLOAD A COPY OF THE DATA FROM MEMORY INTO THE VBO

  glBindBuffer(GL_ARRAY_BUFFER, 0); //RESET THE STATE

  //-----------------------------------------------------------------------------------------------//

  ///CREATE A COLOURS VBO (Vertex Buffer Object) ON THE GPU AND BIND IT
  GLuint colorsVboId = 0;

  glGenBuffers(1, &colorsVboId);

  if (!colorsVboId) { throw std::exception(); } //CHECK FOR ERROR

  glBindBuffer(GL_ARRAY_BUFFER, colorsVboId);

  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW); //UPLOAD A COPY OF THE DATA FROM MEMORY IN THE NEW VBO

  glBindBuffer(GL_ARRAY_BUFFER, 0); //RESET THE STATE

  //-----------------------------------------------------------------------------------------------//

  ///CREATE A VAO (Vertex Array Object) ON THE GPU AND BIND IT
  GLuint vaoId = 0;

  glGenVertexArrays(1, &vaoId); //CREATE A NEW VAO ON THE GPU AND BIND IT

  if (!vaoId) { throw std::exception(); } //CHECK FOR ERROR

  glBindVertexArray(vaoId);

  //-----------------------------------------------------------------------------------------------//

  //BIND THE POSITIONS VBO, ASSIGN IT TO POSITION 0 ON THE BOUND VAO AND FLAG IT TO BE USED
  glBindBuffer(GL_ARRAY_BUFFER, positionsVboId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
  glEnableVertexAttribArray(0);

  //-----------------------------------------------------------------------------------------------//

  //BIND THE COLOUR VBO, ASSIGN IT TO POSITION 0 ON THE BOUND VAO AND FLAG IT TO BE USED
  glBindBuffer(GL_ARRAY_BUFFER, colorsVboId);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(1);

  //-----------------------------------------------------------------------------------------------//

  //RESET THE STATE
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //-----------------------------------------------------------------------------------------------//

  ///CREATE A NEW VERTEX SHADER
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); 
  glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShaderId);
  GLint success = 0;
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR
  
  //-----------------------------------------------------------------------------------------------//

  ///CREATE A NEW FRAGMENT SHADER
  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  //-----------------------------------------------------------------------------------------------//

  ///CREATE NEW SHADER PROGRAM AND ATTACH OUR SHADER OBJECTS
  GLuint programId = glCreateProgram();

  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);

  //ENSURE THE VAO "position" ATTRIBUTE STREAM GETS SET AT THE FIRST POS DURING THE LINK
  glBindAttribLocation(programId, 0, "in_Position");
  glBindAttribLocation(programId, 1, "in_Color");

  if (glGetError() != GL_NO_ERROR) { throw std::exception(); } //ERROR CHECK

  //-----------------------------------------------------------------------------------------------//

  ///PERFORM THE LINK
  glLinkProgram(programId);
  glGetProgramiv(programId, GL_LINK_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  //-----------------------------------------------------------------------------------------------//

  ///DETACHING AND DELETING SHADERS
  /*DETACH AND DESTROY THE SHADER OBJS THEY'RE NO LONGER NEEDED BECAUSE WE NOW HAVE A COMPLETE SHADER PROGRAM*/

  //DETACH AND DELETE VERTEX SHADER
  glDetachShader(programId, vertexShaderId);
  glDeleteShader(vertexShaderId);

  //DETACH AND DELETE FRAGMENT SHADER
  glDetachShader(programId, fragmentShaderId);
  glDeleteShader(fragmentShaderId);

  //-----------------------------------------------------------------------------------------------//

  bool quit = false;


  //-----------------------------------------------------------------------------------------------//

  ///SDL WHILE LOOP
  while (!quit)
  {

    SDL_Event event = { 0 };

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
 
    //CLEARING THE SCREEN
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //INSTRUCT OPENGL TO USE OUR SHADER PROGRAM AND OUR VAO
    glUseProgram(programId);
    glBindVertexArray(vaoId);

    //DRAW 3 VERTICES (A TRIANGLE)
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //RESET THE STATE
    glBindVertexArray(0);
    glUseProgram(0);

    SDL_GL_SwapWindow(window);
      
    }

  //-----------------------------------------------------------------------------------------------//
  
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
