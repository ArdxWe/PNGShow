//
// Created by ardxwe on 2021/1/31.
//

#ifndef DEMO_WINDOW_H
#define DEMO_WINDOW_H

#include <SDL2/SDL.h>

#include <cstdint>
#include <memory>
#include <string>

class Window {
 public:
  struct size {
    int w;
    int h;
  };
  Window(const std::string &title, int x, int y, int w, int h, uint32_t flags);
  [[nodiscard]] SDL_Window *get() const;
  size getSize();

 private:
  struct Deleter {
    void operator()(SDL_Window *x);
  };
  std::unique_ptr<SDL_Window, Deleter> window_;
  size size_;
};

#endif  // DEMO_WINDOW_H
