#include "chip8.h"
#include <stdio.h>
#include <stdlib.h> 




chip8::chip8()
{
}

chip8::~chip8()
{
}




void chip8::initialize()
{
    pc     = 0x200; 
    opcode = 0;      
    I      = 0;      
    sp     = 0;
    for(int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        V[i] = 0;
    }
    for(int i = 0; i < 4096; ++i)
        memory[i] = 0;
    for(int i = 0; i < 64*32; ++i)
        gfx[i] = 0;
    for(int i = 0; i < 16; ++i)
        key[i] = 0;
    for(int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = true;
}


void chip8::loadGame(const char* filename) 
{
    FILE * pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        fputs("File error", stderr); 
        exit(1);
    }
 
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int)lSize);
 
    char * buffer = (char*)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        fputs("Memory error", stderr); 
        exit(2);
    }
 
    size_t result = fread(buffer, 1, lSize, pFile);
    if (result != lSize)
    {
        fputs("Reading error", stderr); 
        exit(3);
    }
 
    if((4096-512) > lSize)
    {
        for(int i = 0; i < lSize; ++i)
            memory[i + 512] = buffer[i];			
    }
    else
        printf("Error: ROM too big for memory");
 
    fclose(pFile);
    free(buffer);

}

void chip8::case0x0000()
{
    switch(opcode & 0x000F)
    {
    case 0x0000:  
        for(int i = 0; i < 2048; ++i)
            gfx[i] = 0x0;
        drawFlag = true;
        pc += 2;
    break;

    case 0x000E:        
        --sp;
        pc = stack[sp];				
        pc += 2;		
    break;

    default:
        printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);          
    }
}

void chip8::case0x1000()
{
    pc = opcode & 0x0FFF;
}

void chip8::case0x2000()
{
    stack[sp] = pc;
    ++sp;
    pc = opcode & 0x0FFF;
}

void chip8::case0x3000()
{
    if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 4;
    else
        pc += 2;
}

void chip8::case0x4000()
{
    if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 4;
    else
        pc += 2;
}

void chip8::case0x5000()
{
    if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        pc += 4;
    else
        pc += 2;
}

void chip8::case0x6000()
{
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
    pc += 2;
}

void chip8::case0x7000()
{
    V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
    pc += 2;
}

void chip8::case0x8000()
{
    switch (opcode & 0x000F)
    {
        case 0x0000: // 0x8XY0
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0001: // 0x8XY1
            V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0002: // 0x8XY2
            V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0003: // 0x8XY3
            V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;

        case 0x0004: // 0x8XY4
            if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1; 
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            pc += 2;  
        break;
        case 0x0005: // 0x8XY5
            if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 0; 
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;

        case 0x0006: // 0x8XY6
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
        break;

        case 0x0007: // 0x8XY7
            if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                V[0xF] = 0; 
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            pc += 2;
        break;

        case 0x000E: // 0x8XYE
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
        break;
        default:
            printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
    }
}

void chip8::case0x9000()
{
    if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        pc += 4;
    else
        pc += 2;
}

void chip8::case0xA000()
{
    I = opcode & 0x0FFF;
    pc += 2;
}

void chip8::case0xB000()
{
    pc = (opcode & 0x0FFF) + V[0];
}

void chip8::case0xC000()
{
    V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
    pc += 2;
}

void chip8::case0xD000()
{
    unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;
    
    V[0xF] = 0;

    for (int i = 0; i < height; i++) {
        pixel = memory[I + i];
        for (int j = 0; j < 8; j++) {
            if ((pixel & (0x80 >> j)) != 0) {
                if (gfx[(x + j + ((y + i) * 64))] == 1)
                    V[0xF] = 1;
                gfx[x + j + ((y + i) * 64)] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
}

void chip8::case0xE000()
{
    switch(opcode & 0x00FF)
    {

        case 0x009E:
            if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                pc += 4;
            else
                pc += 2;
        break;
        case 0x00A1:
            if(key[V[(opcode & 0x0F00) >> 8]] == 0)
                pc += 4;
            else
                pc += 2;
        break;
        default:
            printf ("Unknown opcode [0xE000]: 0x%X\n", opcode);
    }
}

void chip8::case0xF000()
{
    switch(opcode & 0x00FF)
    {
        case 0x0007:
            V[(opcode & 0x0F00) >> 8] = delay_timer;
            pc += 2;
        break;

        case 0x000A:
        {
            bool keyPress = false;
            for(int i = 0; i < 16; ++i)
            {
                if(key[i] != 0)
                {
                    V[(opcode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }
            if(!keyPress)						
                return;	
            pc += 2;
        }
        break;

        case 0x0015:
            delay_timer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
        break;

        case 0x0018:
            sound_timer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
        break;

        case 0x001E:
            I += V[(opcode & 0x0F00) >> 8];
            pc += 2;
        break;

        case 0x0029:
            I = V[(opcode & 0x0F00) >> 8] * 0x5;
            pc += 2;
        break;

        case 0x0033:
            memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
            pc += 2;
        break;

        case 0x0055:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                memory[I + i] = V[i];
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
        break;

        case 0x0065:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                V[i] = memory[I + i];
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
        break;
        


    }
}

void chip8::emulateCycle()
{
    opcode = memory[pc] << 8 | memory[pc + 1];
    opcodeFunction[(opcode & 0xF000) >> 12]();
 
    if(delay_timer > 0)
        --delay_timer;
    
    if(sound_timer > 0)
    {
        if(sound_timer == 1)
        printf("BEEP!\n");
        --sound_timer;
    }  
}


