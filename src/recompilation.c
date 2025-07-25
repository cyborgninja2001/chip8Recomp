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
    fprintf(f, "#include <stdint.h>\n\n");

    // global variables
    fprintf(f, "uint8_t V[16];\n");
    fprintf(f, "uint16_t I;\n");
    fprintf(f, "uint8_t delay_timer;\n");
    fprintf(f, "uint8_t sound_timer;\n");
    fprintf(f, "uint8_t sp;\n");
    fprintf(f, "uint16_t stack[16];\n");
    fprintf(f, "uint8_t display[64][32];\n\n");


    // first i should fprintf() all macros instructions to use later i think

    fprintf(f, "int main(int argc, char* argv[]) {\n");

    while (pc <= 0xFFFF) {
        uint16_t opcode = (ram[pc] << 8) | ram[pc+1];
        uint16_t nnn = opcode & 0xFFF;
        uint8_t n = opcode & 0xF;
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;
        uint8_t kk = opcode & 0xFF;
        pc += 2;

        switch(opcode & 0x8000) {
            case 0x0: {
                switch(nnn) {
                    case 0x00E0: {} break;
                    case 0x00EE: {} break;
                    default: { // SYS addr
                        fprintf(f, "_%04X:\n", pc);
                        fprintf(f, "\tgoto _%04X;\n", nnn);
                    } break;
                }
            } break;
            default: {
                printf("[ERROR]: opcode not yet mapped 0x%01X\n", (opcode & 0x8000) >> 12);
                fprintf(f, "\treturn 0;\n");
                fprintf(f, "}\n");
                fclose(f);
                exit(-1);
            }
        }
    }

    fprintf(f, "\treturn 0;\n");
    fprintf(f, "}\n");
    fclose(f);
    return 0;
}
