#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define RAM_SIZE (4 * 1042)

uint8_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t ram[RAM_SIZE];
uint16_t pc = 0x200;
uint16_t opcode;
uint16_t nnn;
uint8_t n;
uint8_t x;
uint8_t y;
uint8_t kk;

// int fprintf(FILE *stream, const char *format, ...);

int main(int argc, char* argv[]) {
    // first we load ther rom & font
    FILE *rom = fopen(argv[1], "r");
    if (rom == NULL) { printf("[ERROR]: could not open rom\n"); exit(-1); }
    fseek(rom, 0, SEEK_END);
    int rom_size = ftell(rom);
    rewind(rom);
    fread(ram + 0x200, 1, rom_size, rom);
    fclose(rom);
    memcpy(ram + 0x050, font, sizeof(font));


    FILE* f = fopen("ibm.c", "w");
    if (f == NULL) { printf("[ERROR]: could not create .c file\n"); exit(-1); }

    // libraries
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <setjmp.h>\n");
    fprintf(f, "#include <stdint.h>\n");
    fprintf(f, "#include <string.h>\n");
    fprintf(f, "#include <SDL2/SDL.h>\n\n");

    // some macros
    fprintf(f, "#define WINDOW_WIDTH 64\n");
    fprintf(f, "#define WINDOW_HEIGHT 32\n");
    fprintf(f, "#define SCALE 10\n\n");

    // font
    fprintf(f,
        "uint8_t font[80] = {\n"
        "0xF0, 0x90, 0x90, 0x90, 0xF0,\n"
        "0x20, 0x60, 0x20, 0x20, 0x70,\n"
        "0xF0, 0x10, 0xF0, 0x80, 0xF0,\n"
        "0xF0, 0x10, 0xF0, 0x10, 0xF0,\n"
        "0x90, 0x90, 0xF0, 0x10, 0x10,\n"
        "0xF0, 0x80, 0xF0, 0x10, 0xF0,\n"
        "0xF0, 0x80, 0xF0, 0x90, 0xF0,\n"
        "0xF0, 0x10, 0x20, 0x40, 0x40,\n"
        "0xF0, 0x90, 0xF0, 0x90, 0xF0,\n"
        "0xF0, 0x90, 0xF0, 0x10, 0xF0,\n"
        "0xF0, 0x90, 0xF0, 0x90, 0x90,\n"
        "0xE0, 0x90, 0xE0, 0x90, 0xE0,\n"
        "0xF0, 0x80, 0x80, 0x80, 0xF0,\n"
        "0xE0, 0x90, 0x90, 0x90, 0xE0,\n"
        "0xF0, 0x80, 0xF0, 0x80, 0xF0,\n"
        "0xF0, 0x80, 0xF0, 0x80, 0x80};\n\n");

    // global variables
    fprintf(f, "uint8_t V[16];\n");
    fprintf(f, "uint16_t I;\n");
    fprintf(f, "uint8_t delay_timer;\n");
    fprintf(f, "uint8_t sound_timer;\n");
    fprintf(f, "uint8_t sp;\n");
    fprintf(f, "jmp_buf stack[16];\n"); // it's not necesary to initialize
    fprintf(f, "uint8_t ram[4096];\n");
    fprintf(f, "uint8_t display[WINDOW_WIDTH][WINDOW_HEIGHT];\n\n");

    // sdl variables
    fprintf(f, "SDL_Window* window = NULL;\n");
    fprintf(f, "SDL_Renderer* renderer = NULL;\n\n");

    // aux function
    fprintf(f, "void draw_screen() {\n");
    fprintf(f, "\tSDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);\n");
    fprintf(f, "\tSDL_RenderClear(renderer);\n");
    fprintf(f, "\tSDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);\n");
    fprintf(f, "\tfor (int i = 0; i < 32; i++) {\n");
    fprintf(f, "\t\tfor (int j = 0; j < 64; j++) {\n");
    fprintf(f, "\t\t\tif (display[j][i]) {\n");
    fprintf(f, "\t\t\t\tSDL_Rect pixel = { j * SCALE, i * SCALE, SCALE, SCALE };\n");
    fprintf(f, "\t\t\t\tSDL_RenderFillRect(renderer, &pixel);\n");
    fprintf(f, "\t\t\t}\n");
    fprintf(f, "\t\t}\n");
    fprintf(f, "\t}\n");
    fprintf(f, "\tSDL_RenderPresent(renderer);\n");
    fprintf(f, "}\n\n");

    fprintf(f, "uint32_t last_timer_tick;\n");

    fprintf(f, "void update_timers() {\n");
    fprintf(f,
        "\tif (SDL_GetTicks() - last_timer_tick >= 16) {\n"
        "\t\tif (delay_timer > 0) delay_timer--;\n"
        "\t\tif (sound_timer > 0) sound_timer--;\n"
        "\t\tlast_timer_tick = SDL_GetTicks();\n"
        "\t}\n"
    );
    fprintf(f, "}\n");


    // macros for isntructions
    fprintf(f, "#define CALL(x) if (setjmp(stack[sp])) { sp--; } else { sp++; goto x; }\n");
    fprintf(f, "#define RET() longjmp(stack[sp - 1], sp);\n");
    fprintf(f, "#define CLS() memset(display, 0, sizeof(display));\n");
    fprintf(f, "\n");

    fprintf(f, "int main(int argc, char* argv[]) {\n");
    fprintf(f, "last_timer_tick = SDL_GetTicks();\n\n");

    // initialize global variables
    fprintf(f, "\tmemset(V, 0, sizeof(V));\n");
    fprintf(f, "\tI = 0;\n");
    fprintf(f, "\tdelay_timer = 0;\n");
    fprintf(f, "\tsound_timer = 0;\n");
    fprintf(f, "\tsp = 0;\n");
    fprintf(f, "\tmemset(ram, 0, sizeof(ram));\n");
    fprintf(f, "\tmemcpy(ram + 0x050, font, sizeof(font));\n");
    fprintf(f, "\tmemset(display, 0, sizeof(display));\n\n");

    // load the rom in ram
    fprintf(f, "\tFILE* f = fopen(\"%s\", \"r\");\n", argv[1]);
    fprintf(f, "\tfseek(f, 0, SEEK_END);\n");
    fprintf(f, "\tint rom_size = ftell(f);\n");
    fprintf(f, "\trewind(f);\n");
    fprintf(f, "\tfread(ram + 0x200, 1, rom_size, f);\n");
    fprintf(f, "\tfclose(f);\n\n");

    // initialize SDL
    fprintf(f, "\tSDL_Init(SDL_INIT_VIDEO);\n");
    fprintf(f, "\twindow = SDL_CreateWindow(\"ibm logo\", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, 0);\n");
    fprintf(f, "\trenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);\n\n");
    fprintf(f, "SDL_Event event;\n");
    // aux variables for display
    fprintf(f, "uint8_t x_cord;\n");
    fprintf(f, "uint8_t y_cord;\n\n");


    while (pc <= 0x230) { // TODO: fixme [only works for ibm.ch8]
        uint16_t opcode = (ram[pc] << 8) | ram[pc+1];
        nnn = opcode & 0xFFF;
        n = opcode & 0xF;
        x = (opcode & 0x0F00) >> 8;
        y = (opcode & 0x00F0) >> 4;
        kk = opcode & 0xFF;
        fprintf(f, "_%03X:\n", pc);
        pc += 2;

        switch((opcode & 0xF000) >> 12) {
            case 0x0: {
                switch(opcode) {
                    case 0x00E0: { // CLS
                        fprintf(f, "\tCLS()\n");
                    } break;
                    case 0x00EE: { printf("0x00EE\n"); exit(-1); } break;
                    default: { printf("0x0nnn\n"); exit(-1); } break;
                }
            } break;
            case 0x1: { // JP addr
                fprintf(f, "\tgoto _%03X;\n", nnn);
            } break;
            case 0x6: { // LD Vx, byte
                fprintf(f, "\tV[0x%02X] = 0x%02X;\n", x, kk);
            } break;
            case 0x7: { // ADD Vx, byte
                fprintf(f, "\tV[0x%02X] += 0x%02X;\n", x, kk);
            } break;
            case 0xA: { // LD I, addr
                fprintf(f, "\tI = 0x%03X;\n", nnn);
            } break;
            case 0xD: { // DRW Vx, Vy, nibble
                fprintf(f, "\tx_cord = V[0x%02X] & 63;\n", x);
                fprintf(f, "\ty_cord = V[0x%02X] & 31;\n", y);
                fprintf(f, "\tV[0xF] = 0;\n");
                fprintf(f, "\tfor (int row = 0; row < 0x%02X; row++) {\n", n);
                fprintf(f, "\t\tuint8_t spryte = ram[I + row];\n");
                fprintf(f, "\t\tfor (int col = 0; col < 8; col++) {\n");
                fprintf(f, "\t\t\tif((spryte & (0x80 >> col)) != 0) {\n");
                fprintf(f, "\t\t\t\tint dx = (x_cord + col) & 63;\n");
                fprintf(f, "\t\t\t\tint dy = (y_cord + row) & 31;\n");
                fprintf(f, "\t\t\t\tif (display[dx][dy]) V[0xF] = 1;\n");
                fprintf(f, "\t\t\t\tdisplay[dx][dy] ^= 1;\n");
                fprintf(f, "\t\t\t}\n");
                fprintf(f, "\t\t}\n");
                fprintf(f, "\t}\n");
                fprintf(f, "\tdraw_screen();\n");
            } break;
            default: {
                printf("[ERROR]: opcode not yet mapped 0x%01Xxx%01X\n", (opcode & 0xF000) >> 12, n);
                fprintf(f, "\treturn 0;\n");
                fprintf(f, "}\n");
                fclose(f);
                exit(-1);
            }
        }
        fprintf(f, "\tupdate_timers();\n");
        fprintf(f, "\tSDL_Delay(16);\n");
    }

    fprintf(f, "SDL_DestroyRenderer(renderer);\n");
    fprintf(f, "SDL_DestroyWindow(window);\n");
    fprintf(f, "SDL_Quit();\n");
    fprintf(f, "\treturn 0;\n");
    fprintf(f, "}\n");
    fclose(f);
    return 0;
}
