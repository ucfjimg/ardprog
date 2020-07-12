// Signal definitions
#define ACEN 2                 // Enable the Arduino clock (disable the 555 clock)
#define ACLK 3                 // Arduino clock
#define UROMENB 4              // Rom enable, active low. High disables outputs on 
                               // microcode ROM's.

#define MI 5                   // Load the memory address register
#define RI 6                   // Load bus into RAM at current MAR address

#define RCLK 7                 // Rising edge: clock shift register data onto bus
#define SRCLK 8                // Rising edge: shift SDATA into shift register
#define SDATA 9                // Data for shift register
#define BUSENB 10              // When HIGH, shift register outputs drive bus

#define RESET 11               // Reset the computer

// Take control of computer from microcode.
//
void takeOverBus() {
  digitalWrite(ACEN, HIGH);
  digitalWrite(ACLK, LOW);
  digitalWrite(UROMENB, HIGH);
  digitalWrite(BUSENB, LOW);
  digitalWrite(RCLK, LOW);
  digitalWrite(SRCLK, LOW);
  digitalWrite(MI, LOW);
  digitalWrite(RI, LOW);
  digitalWrite(RESET, LOW);
  
  pinMode(ACEN, OUTPUT);
  pinMode(ACLK, OUTPUT);
  pinMode(UROMENB, OUTPUT);
  pinMode(BUSENB, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  pinMode(SDATA, OUTPUT);
  pinMode(RI, OUTPUT);
  pinMode(MI, OUTPUT);  
  pinMode(RESET, OUTPUT);
}

// Return control of computer to microcode
//
void releaseBus() {
  pinMode(ACEN, INPUT);
  pinMode(ACLK, INPUT);
  pinMode(UROMENB, INPUT);
  pinMode(BUSENB, INPUT);
  pinMode(RCLK, INPUT);
  pinMode(SRCLK, INPUT);
  pinMode(SDATA, INPUT);
  pinMode(RI, INPUT);
  pinMode(MI, INPUT);  
  pinMode(RESET, INPUT);
}

// Use the shift register to put a byte of data onto the bus
//
void putDataOnBus(int data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(SDATA, data & 0x80);
    data <<= 1;
    digitalWrite(SRCLK, HIGH);
    digitalWrite(SRCLK, LOW);
  }

  digitalWrite(RCLK, HIGH);
  digitalWrite(RCLK, LOW);
}

// Temporarily set the specified computer control signal and pulse
// the computer's clock
//
void clockControlLine(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(ACLK, HIGH);
  delay(10);
  digitalWrite(ACLK, LOW);
  digitalWrite(pin, LOW);
}

// Reset the computer
//
void reset() {
  digitalWrite(RESET, HIGH);
  delay(10);
  digitalWrite(RESET, LOW);
}

// Macros to help w/ writing computer assembly programs
//
#define NOP          (0b00000000)
#define LDA(addr)    (0b00010000 | (addr))
#define ADD(addr)    (0b00100000 | (addr))
#define SUB(addr)    (0b00110000 | (addr))
#define STA(addr)    (0b01000000 | (addr))
#define LDI(imm)     (0b01010000 | (imm)) 
#define JMP(addr)    (0b01100000 | (addr))
#define JC(addr)     (0b01110000 | (addr))
#define JZ(addr)     (0b10000000 | (addr))
#define OUT          (0b11100000)
#define HALT         (0b11110000)

// Test program: count up by powers of 2
//
static int program[16] = {
  /*  0 */ LDI(1),            // start:  LDI 1     Start at 1
  /*  1 */ STA(15),           //         STA 15    Save A 
  /*  2 */ OUT,               // again:  OUT       Display a
  /*  3 */ ADD(15),           //         ADD 15    Double A
  /*  4 */ JC(0),             //         JC start  Reset when we overflow
  /*  5 */ STA(15),           //         STA 15    Save doubled value
  /*  6 */ JMP(2),            //         JMP again Keep doubling
  /*  7 */ NOP,
  /*  8 */ NOP, 
  /*  9 */ NOP, 
  /* 10 */ NOP, 
  /* 11 */ NOP, 
  /* 12 */ NOP, 
  /* 13 */ NOP, 
  /* 14 */ NOP, 
  /* 15 */ 0,                 // temporary storage
};

void setup() {
  // Main routine: upload program, reset computer, and relinquish control back
  // to microcode.
  //
  takeOverBus();

  for (int i = 0;  i < 16; i++) {
    putDataOnBus(i);
    clockControlLine(MI);
    putDataOnBus(program[i]);
    clockControlLine(RI); 
  }
  
  reset();
  releaseBus();
}

void loop() {
}
