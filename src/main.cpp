#include "VertexBuffer.h"
#include "VertexArray.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <stb_image/stb_image.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/ext.hpp>

#include <exception>

int windowWidth = 640;
int windowHeight = 480;

int main(int argc, char *argv[])
{
  //-----------------------------------------------------------------------------------------------//

  ///SDL SET UP
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { throw std::exception(); }

  SDL_Window *window = SDL_CreateWindow("Triangle",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    windowWidth, windowHeight,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  if (!SDL_GL_CreateContext(window)) { throw std::exception(); }

  if (glewInit() != GLEW_OK) { throw std::exception(); }

  //-----------------------------------------------------------------------------------------------//

  //POSITIONS

  //VertexBuffer *positions = new VertexBuffer();
  //positions->add(glm::vec3(0.0f, 0.5f, 0.0f));
  //positions->add(glm::vec3(-0.5f, -0.5f, 0.0f));
  //positions->add(glm::vec3(0.5f, -0.5f, 0.0f));

  //-----------------------------------------------------------------------------------------------//

  //COLOURS

  //VertexBuffer *colors = new VertexBuffer();
  //colors->add(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  //colors->add(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
  //colors->add(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

  //-----------------------------------------------------------------------------------------------//
  
  //TEXTURE

  //VertexBuffer *texCoords = new VertexBuffer();
  //texCoords->add(glm::vec2(0.5f, 0.0f));
  //texCoords->add(glm::vec2(0.0f, 1.0f));
  //texCoords->add(glm::vec2(1.0f, 1.0f));

  //-----------------------------------------------------------------------------------------------//

  //CREATEING A NEW VERTEX ARRAY CALLED SHAPE
  //VertexArray *shape = new VertexArray();
  //shape->setBuffer("in_Position", positions);
  ////shape->setBuffer("in_Color", positions);
  //shape->setBuffer("in_TexCoord", texCoords);
  
  //-----------------------------------------------------------------------------------------------//

  VertexArray *hallShape = new VertexArray("../models/re_hall_baked.obj");
  Texture *hallTexture = new Texture("../images/re_hall_diffuse.png");

  VertexArray *shape = new VertexArray("../models/curuthers.obj");
  Texture *texture = new Texture("../images/curuthers_diffuse.png");

  ShaderProgram *shader = new ShaderProgram("../shaders/simple.vert", "../shaders/simple.frag");

  //-----------------------------------------------------------------------------------------------//

  //DEFINED FLOAT TO BE USED FOR THE ANGLE OF THE ROTATION
  float angle = 0;

  //-----------------------------------------------------------------------------------------------//
  
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  //-----------------------------------------------------------------------------------------------//

  ///WHILE LOOP FOR SDL
  bool quit = false;

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

    //-----------------------------------------------------------------------------------------------//
    
    //SET THE BACKGROUND TO WHITE
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    //-----------------------------------------------------------------------------------------------//
    
    ///DRAW WITH PERSPECTIVE PROJECTION MATRIX
    shader->setUniform("in_Projection", glm::perspective(glm::radians(45.0f),
      (float)windowWidth / (float)windowHeight, 0.1f, 100.f));

    //-----------------------------------------------------------------------------------------------//

    //DRAW THE : CAMERA
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
    shader->setUniform("in_View", glm::inverse(model));
    
    //-----------------------------------------------------------------------------------------------//

    //DRAW THE : MANSION
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, -2.0f, -16.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    shader->setUniform("in_Model", model);
    shader->setUniform("in_Texture", hallTexture);
    shader->draw(hallShape);
    
    //-----------------------------------------------------------------------------------------------//

    //DRAW THE : CAT
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, -2.1f, -16.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
    shader->setUniform("in_Model", model);
    shader->setUniform("in_Texture", texture);
    shader->draw(shape);

    //-----------------------------------------------------------------------------------------------//

    //INCREASE ANGLE BY VALUE
    angle += 0.1f;

    //-----------------------------------------------------------------------------------------------//
    
    SDL_GL_SwapWindow(window);
  }

  //-----------------------------------------------------------------------------------------------//
  
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
