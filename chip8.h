#include <functional>


class chip8 {
public:
    chip8();
    ~chip8();

    bool drawFlag;

    void emulateCycle();
    void loadGame(const char * filename);
    void initialize();


    unsigned char gfx[64 * 32];
    unsigned char key[16];

private:
    unsigned short opcode;      // Current opcode
    unsigned char memory[4096]; // 4K memory
    unsigned char V[16];        // V-regs (V0-VF)
    unsigned short I;           // Index register
    unsigned short pc;          // Program counter

    unsigned char delay_timer;  // Delay timer
    unsigned char sound_timer;  // Sound timer

    unsigned short stack[16];   // Stack (16 levels)
    unsigned short sp;          // Stack pointer

    void case0x0000();
    void case0x1000();
    void case0x2000();
    void case0x3000();
    void case0x4000();
    void case0x5000();
    void case0x6000();
    void case0x7000();
    void case0x8000();
    void case0x9000();
    void case0xA000();
    void case0xB000();
    void case0xC000();
    void case0xD000();
    void case0xE000();
    void case0xF000();

    std::function<void()> opcodeFunction[16] = {
        std::bind(&chip8::case0x0000, this),
        std::bind(&chip8::case0x1000, this),
        std::bind(&chip8::case0x2000, this),
        std::bind(&chip8::case0x3000, this),
        std::bind(&chip8::case0x4000, this),
        std::bind(&chip8::case0x5000, this),
        std::bind(&chip8::case0x6000, this),
        std::bind(&chip8::case0x7000, this),
        std::bind(&chip8::case0x8000, this),
        std::bind(&chip8::case0x9000, this),
        std::bind(&chip8::case0xA000, this),
        std::bind(&chip8::case0xB000, this),
        std::bind(&chip8::case0xC000, this),
        std::bind(&chip8::case0xD000, this),
        std::bind(&chip8::case0xE000, this),
        std::bind(&chip8::case0xF000, this)
    };

    
    unsigned char chip8_fontset[80] =
    { 
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

};

