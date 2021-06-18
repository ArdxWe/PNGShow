//
// Created by ardxwe on 2021/1/31.
//

#include "Window.h"

#include <SDL2/SDL_video.h>

#include <stdexcept>
#include <string>

Window::Window(const std::string &title, int x, int y, int w, int h,
               uint32_t flags)
    : window_{SDL_CreateWindow(title.c_str(), x, y, w, h, flags)}, size_{w, h} {
  SDL_SetWindowFullscreen(window_.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window_ == nullptr) {
    using namespace std::string_literals;
    throw std::runtime_error{"error call SDL_CreateWindow"s + SDL_GetError()};
  }
}

SDL_Window *Window::get() const { return window_.get(); }

void Window::Deleter::operator()(SDL_Window *x) { SDL_DestroyWindow(x); }

Window::size Window::getSize() {
  SDL_GetWindowSize(window_.get(), &size_.w, &size_.h);
  return size_;
}