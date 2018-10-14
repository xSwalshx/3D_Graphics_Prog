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

  //LIST OF POSITIONS THAT DESCRIBE THE SHAPE (X,Y,Z)
  const GLfloat positions[] = {
    0.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
  };

  GLuint positionsVboId = 0;

  //DEFINE THE COLOURS OF EACH POINT OF THE TRIANGLE (R/G/B)
  const GLfloat colors[] = {
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f
  };

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

  ///VERTEX SHADER
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

  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); //CREATE A NEW VERTEX SHADER
  glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShaderId);
  GLint success = 0;
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  ///FRAGMENT SHADER
  const GLchar *fragmentShaderSrc =
    "varying vec4 ex_Color;                  " \
    "void main()                             " \
    "{                                       " \
    "  gl_FragColor = ex_Color;              " \
    "}                                       " \
    "                                        ";

  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR
  
  ///CREATE NEW SHADER PROGRAM AND ATTACH OUR SHADER OBJECTS
  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);

  //ENSURE THE VAO "position" ATTRIBUTE STREAM GETS SET AT THE FIRST POS DURING THE LINK
  glBindAttribLocation(programId, 0, "in_Position");
  
  glLinkProgram(programId); //PERFORM THE LINK
  glGetProgramiv(programId, GL_LINK_STATUS, &success);

  if (!success) { throw std::exception(); } //CHECK FOR ERROR

  ///UNIFORM STUFF
  ////STORE LOCATION OF THE COLOR UNIFORM
  //GLint colorUniformId = glGetUniformLocation(programId, "in_Color");
  //if (colorUniformId == -1) { throw std::exception(); } //CHECK IF IT FOUND UNIFORM LOCATION
  /////BING SHADER TO CHANGE THE UNIFORM
  ////glUseProgram(programId);
  ////glUniform4f(colorUniformId, 0, 1, 0, 1);  //SET THE UNIFORM
  ////glUseProgram(0);                          //RESET THE STATE

  ///CREATE THE NEW VBO, UPLOAD THE DATA AND ASSIGN IT TO POS. 1 OF VAO
  GLuint colorsVboId = 0;
  glGenBuffers(1, &colorsVboId);
  if (!colorsVboId) { throw std::exception(); } //CHECK FOR ERROR
  glBindBuffer(GL_ARRAY_BUFFER, colorsVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW); //UPLOAD A COPY OF THE DATA FROM MEMORY IN THE NEW VBO
  glBindBuffer(GL_ARRAY_BUFFER, colorsVboId); //BIND THE COLOUR VBO, ASSIGN IT TO POSITION 1 ON THE BOUND VBO AND FLAG IT TO BE USED
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  glBindAttribLocation(programId, 1, "in_Color"); 
  glEnableVertexAttribArray(1);

  //  //CODE FROM LECTURE
  //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT);
  //glUseProgram(GL_COLOR_BUFFER_BIT);
  //glUseProgram(programId);
  //glBindVertexArray(vaoId);
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  
  //INSTRUCT OPENGL TO USE OUR SHADER PROGRAM AND OUR VAO
  glUseProgram(programId); 
  glBindVertexArray(vaoId);
  
  //RESET THE STATE
  glBindVertexArray(0);
  glUseProgram(0);

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
 
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //DRAW 3 VERTICES (A TRIANGLE)
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);
      
    }

  //DETACHING AND DELETING SHADERS
  glDetachShader(programId, vertexShaderId);    //DETACH AND DESTROY THE SHADER OBJS
  glDeleteShader(vertexShaderId);               //THEY'RE NO LONGER NEEDED BECAUSE WE
  glDetachShader(programId, fragmentShaderId);  //NOW HAVE A COMPLETE SHADER PROGRAM
  glDeleteShader(fragmentShaderId);
  
  SDL_DestroyWindow(window);
  SDL_Quit();


  return 0;
}
