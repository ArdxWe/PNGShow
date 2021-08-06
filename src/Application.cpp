//
// Created by ardxwe on 2021/2/6.
//

#include "Application.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using std::async;
using std::cout;
using std::endl;
using std::future;
using std::future_status;
using std::getenv;
using std::launch;
using std::localtime;
using std::move;
using std::put_time;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;
using namespace std::string_literals;
using Rect = Renderer::Rect;

constexpr int FPS = 80;
constexpr double FADE_OUT_TIME = 1;
constexpr double FADE_IN_TIME = 2;
constexpr double ON_SHOW_TIME = 3;

constexpr const char *PNG_ENV = "PNG";
constexpr const char *PNG_CMD = "find /usr/share/backgrounds -name '*.png'";
constexpr const char *TTF_CMD = "find /usr/share/fonts -name '*.ttf'";

stringstream executeCmd(const string &cmd) {
  struct Close {
    void operator()(FILE* file) {
      pclose(file);
    }
  };
  unique_ptr<FILE, Close> pipe{popen(cmd.c_str(), "r")};

  vector<char> buff(0x100);
  size_t n;
  stringstream stream;

  while ((n = fread(buff.data(), sizeof(buff[0]), buff.size(), pipe.get())) >
         0) {
    stream.write(buff.data(), static_cast<std::streamsize>(n));
  }
  return stream;
}

vector<string> getImagePaths() {
  const char *cmd;
  if ((cmd = getenv(PNG_ENV)) == nullptr) {
    cmd = PNG_CMD;
  }

  stringstream stream = executeCmd(string{cmd});

  vector<string> res;
  string path;
  while (getline(stream, path)) {
    res.push_back(move(path));
  }
  if (res.size() < 4) {
    throw runtime_error{"less than four pictures."s};
  }
  return res;
}

Window createWindow() {
  Window window{string(), 0x1FFF0000, 0x1FFF0000,
                0,        0,          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
  return window;
}

Texture createTextureFromSurface(Renderer &renderer, Surface &surface) {
  return Texture{SDL_CreateTextureFromSurface(renderer.get(), surface.get())};
}

future<Surface> nextImage(const string &path) {
  return async(launch::async, [&path]() { return Surface{path}; });
}

Font creatFont(int size) {
  stringstream stream = executeCmd(TTF_CMD);
  string path;
  getline(stream, path);
  if (path.empty()) {
    throw runtime_error{"find no fonts."s};
  }
  return Font{path, size};
}
}  // namespace

Application::Application()
    : paths_{getImagePaths()},
      window_{createWindow()},
      renderer_{window_},
      image_{nextImage(paths_[0]).get()},
      next_image_{nextImage(paths_[1])},
      size_{window_.getSize()} {
  renderer_.setColor(0, 0, 0, 0xFF);
  for (const auto &path : paths_) {
    cout << path << endl;
  }
}

void Application::run() {
  cout << "screen width: " << size_.w << endl;
  cout << "screen height: " << size_.h << endl;

  Font small{creatFont(72)};
  Font big{creatFont(96)};

  bool quit = false;
  SDL_Event e;
  int size = static_cast<int>(paths_.size()), i = 0;
  double alpha, tm, frame_tm;
  double time_long = FADE_IN_TIME;
  Rect src, dst;
  auto start = std::chrono::high_resolution_clock::now();
  decltype(start) now;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT || e.type == SDL_MOUSEBUTTONDOWN) {
        quit = true;
      }
    }
    now = std::chrono::high_resolution_clock::now();
    tm = time_long -
         std::chrono::duration_cast<std::chrono::duration<double>>(now - start)
             .count();

    switch (state_) {
      case State::FADE_IN:
        alpha = 1 - tm / time_long;
        if (tm < 0) {
          state_ = State::ON_SHOW;
          time_long = ON_SHOW_TIME;
          start = std::chrono::high_resolution_clock::now();
        }
        break;
      case State::FADE_OUT:
        alpha = tm / time_long;
        if (tm < 0 && next_image_.wait_for((std::chrono::seconds)0) ==
                          future_status::ready) {
          state_ = State::FADE_IN;
          time_long = FADE_IN_TIME;
          i++;
          if (i == size) {
            i = 0;
          }
          image_ = next_image_.get();
          next_image_ = nextImage(paths_[i]);
          start = std::chrono::high_resolution_clock::now();
        }
        break;
      case State::ON_SHOW:
        alpha = 1;
        if (tm < 0) {
          state_ = State::FADE_OUT;
          time_long = FADE_OUT_TIME;
          start = std::chrono::high_resolution_clock::now();
        }
        break;
    }
    if (alpha < 0)
      alpha = 0;
    else if (alpha > 1)
      alpha = 1;

    {
      auto time = std::chrono::system_clock::to_time_t(
          std::chrono::system_clock::now());
      stringstream stream;
      stream << put_time(localtime(&time), " %H:%M:%S");

      Surface surface{small, stream.str(), {0xFF, 0xFF, 0xFF, 0xFF}};

      src = {0, 0, surface.getWidth(), surface.getHeight()};
      dst = {size_.w / 8, 6 * size_.h / 8, surface.getWidth(),
             surface.getHeight()};
      Texture t{createTextureFromSurface(renderer_, surface)};
      renderer_.clear();
      renderer_.copyTexture(t, src, dst);

      stringstream new_stream;
      new_stream << put_time(localtime(&time), "%A %F");

      Surface new_surface{big, new_stream.str(), {0xFF, 0xFF, 0xFF, 0xFF}};

      src = {0, 0, new_surface.getWidth(), new_surface.getHeight()};
      dst = {size_.w / 8, 6 * size_.h / 8 + surface.getHeight(),
             new_surface.getWidth(), new_surface.getHeight()};
      Texture new_t{createTextureFromSurface(renderer_, new_surface)};
      renderer_.copyTexture(new_t, src, dst);
    }

    current_texture_ = createTextureFromSurface(renderer_, image_);
    current_texture_.setBlendMode(SDL_BLENDMODE_BLEND);
    current_texture_.setAlpha(static_cast<uint8_t>(alpha * 255));
    renderer_.copyAllTexture(current_texture_);
    renderer_.renderPresent();

    frame_tm = std::chrono::duration_cast<std::chrono::duration<double>>(
                   std::chrono::high_resolution_clock::now() - now)
                   .count();

    if (frame_tm < 1000.0 / FPS) {
      SDL_Delay(static_cast<Uint32>(1000.0 / FPS - frame_tm));
    }
  }
}
