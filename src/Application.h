//
// Created by ardxwe on 2021/2/6.
//

#ifndef DEMO_APPLICATION_H
#define DEMO_APPLICATION_H

#include <SDL2/SDL.h>

#include <future>
#include <string>
#include <vector>

#include "App.h"
#include "Image.h"
#include "Renderer.h"
#include "Surface.h"
#include "Texture.h"
#include "Ttf.h"
#include "Window.h"

class Application {
 public:
  Application();
  void run();

 private:
  App app_{};
  Image image_app_{};
  Ttf ttf_app_{};
  std::vector<std::string> paths_;
  Window window_;
  Renderer renderer_;
  Surface image_;
  std::future<Surface> next_image_;
  Texture current_texture_{};

  enum class State { FADE_IN, FADE_OUT, ON_SHOW };

  State state_ = State::FADE_IN;
  Window::size size_;
};

#endif  // DEMO_APPLICATION_H
