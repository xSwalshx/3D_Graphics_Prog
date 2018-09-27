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
  if (!SDL_GL_CreateContext(window) || glewInit() != GLEW_OK) { throw std::exception(); }



  //DRAWING WHILE QUIT IS FALSE
  

  //USING OPENGL TO SET THE WINDOW COLOUR


  //LIST OF POSITIONS THAT DESCRIBE THE SHAPE (X,Y,Z)
  const GLfloat positions[] = {
    0.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
  };

  GLuint positionsVboId = 0;

  ///CREATE A NEW VBO (Vertex Buffer Object) ON THE GPU AND BIND IT
  glGenBuffers(1, &positionsVboId);

  if (!positionsVboId) { throw std::exception(); } //CHECK FOR ERROR


  ///CREATE A VAO (Vertex Array Object) 
  GLuint vaoId = 0;
  glGenVertexArrays(1, &vaoId); //CREATE A NEW VAO ON THE GPU AND BIND IT

  if (!vaoId) { throw std::exception(); } //CHECK FOR ERROR

  glBindVertexArray(vaoId);
  glBindBuffer(GL_ARRAY_BUFFER, positionsVboId); //BIND THE POSITION VBO, ASSIGN IT TO THE POSITION 0 ON THE BOUND VAO AND FLAG IT TO BE USED
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); //UPLOAD A COPY OF THE DATA FROM MEMORY INTO THE VBO
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0); //RESET THE STATE
  glBindVertexArray(0);

  ///SIMPLE VERTEX SHADER
  const GLchar *vertexShaderSrc =
    "attribute vec3 in_Position;            " \
    "                                       " \
    "void main()                            " \
    "{                                      " \
    "  gl_Position = vec4(in_Position, 1.0); " \
    "}                                      ";

  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); //CREATE A NEW VERTEX SHADER
  glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShaderId);
  GLint success = 0;
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  ///FRAGMENT SHADER
  const GLchar *fragmentShaderSrc =
    "void main()                             " \
    "{                                       " \
    "  gl_FragColor = vec4(0, 0, 1, 1);     " \
    "}                                       ";

  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  ///CREATE NEW SHADER PROGRAM AND ATTACH OUR SHADER OBJECTS
  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);

  glBindAttribLocation(programId, 0, "in_Position"); //ENSURE THE VAO "position" ATTRIBUTE STREAM GETS SET AT THE FIRST POS DURING THE LINK

  glLinkProgram(programId); //PERFORM THE LINK
  glGetProgramiv(programId, GL_LINK_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  glDetachShader(programId, vertexShaderId);    //DETACH AND DESTROY THE SHADER OBJS
  glDeleteShader(vertexShaderId);               //THEY'RE NO LONGER NEEDED BECAUSE WE
  glDetachShader(programId, fragmentShaderId);  //NOW HAVE A COMPLETE SHADER PROGRAM
  glDeleteShader(fragmentShaderId);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  //CODE FROM LECTURE
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(GL_COLOR_BUFFER_BIT);
  glUseProgram(programId);
  glBindVertexArray(vaoId);

  glDrawArrays(GL_TRIANGLES, 0, 3);


  glUseProgram(programId); //INSTRUCT OPENGL TO USE OUR SHADER PROGRAM AND OUR VAO
  glBindVertexArray(vaoId);

  glDrawArrays(GL_TRIANGLES, 0, 3); //DRAW 3 VERTICES (A TRIANGLE

  //RESET THE STATE
  glBindVertexArray(0);
  glUseProgram(0);
  SDL_GL_SwapWindow(window);

  bool quit = false;


  SDL_Event event = { 0 };

  while (!quit)
  {
    //QUIT EVENT
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }

    }

  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
