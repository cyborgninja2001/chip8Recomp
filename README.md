# chip8Recomp
An experimental tool to statically recompile .ch8 programs into C programs

this proyect is just an experiment to learn more about static recompilation.
at the moment is incomplete and a mess and still i have to fix some opcodes, so for now you can only recompile the ibm logo program

<img width="649" height="350" alt="Captura de pantalla 2025-07-25 131955" src="https://github.com/user-attachments/assets/7281915f-ef3f-409c-8921-7fd315717f44" />

# usage
compile with: gcc src/recompilation.c -o main
then run: ./main "roms/ibm.ch8" on linux

a .c file named program.c will be create on folder output/.

then you can normally compile and run that file using sdl2. (you'll need to install it first then add flag -lSDL2 when compiling)
