#ifndef __RSDL_H__
#define __RSDL_H__

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <map>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class RGB {
public:
	RGB(int r, int g, int b) {
		if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
			throw "Invalid RGB color!";
		red = r;
		green = g;
		blue = b;
	}
	int red;
	int green;
	int blue;
};

const RGB WHITE(255, 255, 255);
const RGB YELLOW(255, 255, 0);
const RGB MAGENTA(255, 0, 255);
const RGB RED(255, 0, 0);
const RGB CYAN(0, 255, 255);
const RGB GREEN(0, 255, 0);
const RGB BLUE(0, 0, 255);
const RGB BLACK(0, 0, 0);

class window {
public:
	window(int width=800, int height=400, std::string title="RSDL");
	~window();
	void draw_bmp(std::string filename, int x, int y, int width, int height);
	void draw_png(std::string filename, int x, int y, int width, int height);
	void draw_png(std::string filename, int x, int y, int width, int height, int angle);
    void show_text(std::string input, int x=0, int y=0, RGB color=WHITE, std::string font_addr="FreeSans.ttf", int size=24);
    void draw_bg(std::string filename, int x=0, int y=0);
	void update_screen();
    void fill_rect(int x, int y, int width, int height, RGB color=WHITE);
    void draw_line(int x1, int y1, int x2, int y2, RGB color=WHITE);
    void draw_point(int x, int y, RGB color=WHITE);
    void draw_rect(int x, int y, int width, int height, RGB color=WHITE);
private:
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;
	SDL_Window* win;
	SDL_Renderer* renderer;
	std::map<std::string, SDL_Texture*> texture_cache;
    std::map<std::string, TTF_Font*> fonts_cache;
	void set_color(RGB color);
	void dump_err() { std::cerr << SDL_GetError() << '\n'; }
	void init();
};

#define HANDLE(A)   SDL_Event e; while (SDL_PollEvent(&e) != 0) { A }
#define QUIT(Code) if (e.type == SDL_QUIT) { Code ; }
#define LCLICK(Code) if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) { int mouse_x = e.button.x; int mouse_y = e.button.y; Code ; }
#define RCLICK(Code) if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) { int mouse_x = e.button.x; int mouse_y = e.button.y; Code ; }
#define KEY_PRESS(Char, Code) if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_##Char) { Code ; }
#define LRELEASE(Code) if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) { int mouse_x = e.button.x; int mouse_y = e.button.y; Code ; }
#define RRELEASE(Code) if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT) { int mouse_x = e.button.x; int mouse_y = e.button.y; Code ; }

#define DELAY(Millis) SDL_Delay(Millis)

#endif
