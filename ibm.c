#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32
#define SCALE 10

uint8_t font[80] = {
0xF0, 0x90, 0x90, 0x90, 0xF0,
0x20, 0x60, 0x20, 0x20, 0x70,
0xF0, 0x10, 0xF0, 0x80, 0xF0,
0xF0, 0x10, 0xF0, 0x10, 0xF0,
0x90, 0x90, 0xF0, 0x10, 0x10,
0xF0, 0x80, 0xF0, 0x10, 0xF0,
0xF0, 0x80, 0xF0, 0x90, 0xF0,
0xF0, 0x10, 0x20, 0x40, 0x40,
0xF0, 0x90, 0xF0, 0x90, 0xF0,
0xF0, 0x90, 0xF0, 0x10, 0xF0,
0xF0, 0x90, 0xF0, 0x90, 0x90,
0xE0, 0x90, 0xE0, 0x90, 0xE0,
0xF0, 0x80, 0x80, 0x80, 0xF0,
0xE0, 0x90, 0x90, 0x90, 0xE0,
0xF0, 0x80, 0xF0, 0x80, 0xF0,
0xF0, 0x80, 0xF0, 0x80, 0x80};

uint8_t V[16];
uint16_t I;
uint8_t delay_timer;
uint8_t sound_timer;
uint8_t sp;
jmp_buf stack[16];
uint8_t ram[4096];
uint8_t display[WINDOW_WIDTH][WINDOW_HEIGHT];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void draw_screen() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			if (display[j][i]) {
				SDL_Rect pixel = { j * SCALE, i * SCALE, SCALE, SCALE };
				SDL_RenderFillRect(renderer, &pixel);
			}
		}
	}
	SDL_RenderPresent(renderer);
}

uint32_t last_timer_tick;
void update_timers() {
	if (SDL_GetTicks() - last_timer_tick >= 16) {
		if (delay_timer > 0) delay_timer--;
		if (sound_timer > 0) sound_timer--;
		last_timer_tick = SDL_GetTicks();
	}
}
#define CALL(x) if (setjmp(stack[sp])) { sp--; } else { sp++; goto x; }
#define RET() longjmp(stack[sp - 1], sp);
#define CLS() memset(display, 0, sizeof(display));

int main(int argc, char* argv[]) {
last_timer_tick = SDL_GetTicks();

	memset(V, 0, sizeof(V));
	I = 0;
	delay_timer = 0;
	sound_timer = 0;
	sp = 0;
	memset(ram, 0, sizeof(ram));
	memcpy(ram + 0x050, font, sizeof(font));
	memset(display, 0, sizeof(display));

	FILE* f = fopen("ibm.ch8", "r");
	fseek(f, 0, SEEK_END);
	int rom_size = ftell(f);
	rewind(f);
	fread(ram + 0x200, 1, rom_size, f);
	fclose(f);

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("ibm logo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

SDL_Event event;
uint8_t x_cord;
uint8_t y_cord;

_200:
	CLS()
	update_timers();
	SDL_Delay(16);
_202:
	I = 0x22A;
	update_timers();
	SDL_Delay(16);
_204:
	V[0x00] = 0x0C;
	update_timers();
	SDL_Delay(16);
_206:
	V[0x01] = 0x08;
	update_timers();
	SDL_Delay(16);
_208:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_20A:
	V[0x00] += 0x09;
	update_timers();
	SDL_Delay(16);
_20C:
	I = 0x239;
	update_timers();
	SDL_Delay(16);
_20E:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_210:
	I = 0x248;
	update_timers();
	SDL_Delay(16);
_212:
	V[0x00] += 0x08;
	update_timers();
	SDL_Delay(16);
_214:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_216:
	V[0x00] += 0x04;
	update_timers();
	SDL_Delay(16);
_218:
	I = 0x257;
	update_timers();
	SDL_Delay(16);
_21A:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_21C:
	V[0x00] += 0x08;
	update_timers();
	SDL_Delay(16);
_21E:
	I = 0x266;
	update_timers();
	SDL_Delay(16);
_220:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_222:
	V[0x00] += 0x08;
	update_timers();
	SDL_Delay(16);
_224:
	I = 0x275;
	update_timers();
	SDL_Delay(16);
_226:
	x_cord = V[0x00] & 63;
	y_cord = V[0x01] & 31;
	V[0xF] = 0;
	for (int row = 0; row < 0x0F; row++) {
		uint8_t spryte = ram[I + row];
		for (int col = 0; col < 8; col++) {
			if((spryte & (0x80 >> col)) != 0) {
				int dx = (x_cord + col) & 63;
				int dy = (y_cord + row) & 31;
				if (display[dx][dy]) V[0xF] = 1;
				display[dx][dy] ^= 1;
			}
		}
	}
	draw_screen();
	update_timers();
	SDL_Delay(16);
_228:
	goto _228;
	update_timers();
	SDL_Delay(16);
_22A:
	return 0;
}
