# chip8Recomp
An experimental tool to statically recompile .ch8 programs into C programs

this proyect is just an experiment to learn more about static recompilation. For now you can only recompile the ibm logo program
<img width="651" height="351" alt="Captura de pantalla 2025-07-25 131933" src="https://github.com/user-attachments/assets/c3e49359-4e6c-4d8a-a8ab-81d74140cdcc" />

# usage
compile with: gcc src/recompilation.c -o main
then run: ./main "roms/ibm.ch8"

a .c file named program.c will be create on output/.

then you can normally compile and run that file using sdl2.
