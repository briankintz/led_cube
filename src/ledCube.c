#include "ledCube.h"
#include "plib.h"

extern void waitMicro(int msecs);

int cache[3];

// Write the the passed layer at vertical position i
void writeLayer(int i, const Layer *l){

    // activate the correct ground pin, all others to 0
    switch (i) {
        case 1:
            PORTWrite(IOPORT_G, BIT_0);
            break;
        case 2:
            PORTWrite(IOPORT_G, BIT_14);
            break;
        case 3:
            PORTWrite(IOPORT_G, BIT_12);
            break;
        case 4:
            PORTWrite(IOPORT_G, BIT_13);
            break;
        default:
            PORTWrite(IOPORT_G, 0);
    }

    // display all colors
    writeRed(l->R);
    writeGreen(l->G);
    writeBlue(l->B);

    // wait to allow the values to propogate to the IO pins
    waitMicro(1);
}

void writeRed(unsigned short value)     {

    // break out the bits of the red value to the proper IO port bits
    PORTWrite(IOPORT_D, (1 & value) << 9    |  // value[0] -> (1, 1)
                        BIT(1, value)       |  // value[1] -> (1, 2)
                        BIT(2, value) << 13 |  // value[2] -> (1, 3)
                        BIT(3, value) << 7  |  // value[3] -> (1, 4)
            
                        BIT(4, value) << 8  |  // value[4] -> (2, 1)
                        BIT(6, value) << 12 |  // value[6] -> (2, 3)
                        BIT(7, value) << 6  |  // value[7] -> (2, 4)
            
                        BIT(8, value) << 10 |  // value[8] -> (3, 1)
                        BIT(9, value) << 14 |  // value[9] -> (3, 2)
                        BIT(10, value) << 3 |  // value[10] -> (3, 3)
                        BIT(11, value) << 5 |  // value[11] -> (3, 4)
            
                        BIT(12, value) << 11 |  // value[12] -> (4, 1)
                        BIT(13, value) << 15 |  // value[13] -> (4, 2)
                        BIT(14, value) << 2  |  // value[14] -> (4, 3)
                        BIT(15, value) << 4     // value[15] -> (4, 4)
    );

    PORTWrite(IOPORT_C, BIT(5, value) << 13);
}
void writeGreen(unsigned short value)   {
    PORTWrite(IOPORT_B, (1 & value) << 15   |  // value[0] -> (1, 1)
                        BIT(1, value) << 11 |  // value[1] -> (1, 2)
                        BIT(2, value) << 3  |  // value[2] -> (1, 3)

                        BIT(4, value) << 14 |  // value[4] -> (2, 1)
                        BIT(5, value) << 10 |  // value[5] -> (2, 2)
                        BIT(6, value) << 4  |  // value[6] -> (2, 3)

                        BIT(8, value) << 13 |  // value[8] -> (3, 1)
                        BIT(9, value) << 9  |  // value[9] -> (3, 2)
                        BIT(10, value) << 5 |  // value[10] -> (3, 3)

                        BIT(12, value) << 12 |  // value[12] -> (4, 1)
                        BIT(13, value) << 8     // value[13] -> (4, 2)

    );

    PORTWrite(IOPORT_E, BIT(3, value) << 1  |  // value[3] -> (1, 4)
                        BIT(7, value) << 4  |  // value[7] -> (2, 4)
                        BIT(11, value) << 3 |  // value[11] -> (3, 4)
                        BIT(14, value)      |  // value[14] -> (4, 3)
                        BIT(15, value) << 2    // value[15] -> (4, 4)
    );
}
void writeBlue(unsigned short value)    {
    PORTWrite(IOPORT_A, (1 & value) << 4 |  // value[0] -> (1, 1)
                        BIT(3, value)    |  // value[3] -> (1, 4)
                        
                        BIT(4, value) << 5 |  // value[4] -> (2, 1)
                        BIT(7, value) << 9 |  // value[7] -> (2, 4)

                        BIT(8, value) << 14  |  // value[8] -> (3, 1)
                        BIT(9, value) << 2   |  // value[9] -> (3, 2)
                        BIT(11, value) << 10 |  // value[11] -> (3, 4)

                        BIT(12, value) << 15 |  // value[12] -> (4, 1)
                        BIT(13, value) << 3  |  // value[13] -> (4, 2)
                        BIT(15, value) << 1     // value[15] -> (4, 4)
    );

    PORTWrite(IOPORT_F, BIT(1, value) << 2  |  // value[1] -> (1, 2)
                        BIT(2, value) << 13 |  // value[2] -> (1, 3)
                        BIT(5, value) << 8  |  // value[5] -> (2, 2)
                        BIT(6, value) << 12 |  // value[6] -> (2, 3)
                        BIT(10, value) << 4 |  // value[10] -> (3, 3)
                        BIT(14, value) << 5    // value[14] -> (4, 3)
    );
}

/* Animation generation function
 *
 * All off randomly to all white
 */
void randB2W_GetNextFrame(const unsigned int i, Frame *current) {
    if(i == 0){ // clear display
        memset(current, 0, sizeof(Frame));
        current->DisplayMS = 50;
        return;
    }else if(i == 193){ // hold all white longer
        current->DisplayMS = 250;
        return;
    }

    // 50msec/frame
    current->DisplayMS = 50;

    // get a layer and a color
    int layer_offset = rand() % 4;
    int color_offset = rand() % 3;

    // get the address of the selected color in the selected layer
    unsigned short *color = &((Layer*)(&current->L1 + layer_offset))->R + color_offset;

    // if that color's all white, pick a new layer/color
    while(*color == ((i < 193) ? 65535 : 0)){
        layer_offset = rand() % 4;
        color_offset = rand() % 3;

        color = &((Layer*)(&current->L1 + layer_offset))->R + color_offset;
    }

    // pick an LED and make sure it isn't already set
    int led = 1 << (rand() % 16);
    while ((i < 193) ^ (!(*color & led))) {
        led = 1 << (rand() % 16);
    }

    // toggle the LED
    *color ^= led;
}

/* Animation generation function
 *
 * Display a single random LED in a random color
 */
void random_GetNextFrame(const unsigned int i, Frame *current) {
    // clear everything
    memset(current, 0, sizeof(Frame));

    // 50msec/led
    current->DisplayMS = 50;

    // leave the first frame blank
    if(i == 0) return;

    // pick a layer and get the base address (red) of the layer's colors
    int layer_offset = rand() % 4;
    unsigned short *target = &((Layer*)(&current->L1 + layer_offset))->R;

    // pick a color and an led
    int color = rand() % 6 + 1;
    int led = 1 << (rand() % 16);

    // set the colors of the selected led
    if (BIT(0, color)) *target |= led;       // R
    if (BIT(1, color)) *(target + 1) |= led; // G
    if (BIT(2, color)) *(target + 2) |= led; // B
}

/* Animation generation function
 *
 * Display a single random column in a random color
 */
void randCol_GetNextFrame(const unsigned int i, Frame *current){
    // clear everything
    memset(current, 0, sizeof(Frame));

    // 50msec/led
    current->DisplayMS = 50;

    // leave the first frame blank
    if(i == 0) return;

    int color = rand() % 6 + 1;
    int col = 1 << (rand() % 16);

    if(BIT(0, color)){
        current->L1.R |= col;
        current->L2.R |= col;
        current->L3.R |= col;
        current->L4.R |= col;
    }

    if(BIT(1, color)){
        current->L1.G |= col;
        current->L2.G |= col;
        current->L3.G |= col;
        current->L4.G |= col;
    }

    if(BIT(2, color)){
        current->L1.B |= col;
        current->L2.B |= col;
        current->L3.B |= col;
        current->L4.B |= col;
    }
}

/* Animation generation function
 *
 * Pulse empty->full->empty for each color
 */
void pulse_GetNextFrame(const unsigned int i, Frame *current){
    if(!(i % 129)){ // clear display, hold off longer
        memset(current, 0, sizeof(Frame));
        current->DisplayMS = 50;
        return;
    }

    // 10msec/frame, hold for 200ms if full
    current->DisplayMS = (i % 129) == 64 ? 200 : 10;

    // calculate pulse color
    int color = (i / 129) + 1;

    // random layer
    int layer_offset = rand() % 4;
    unsigned short *target = &((Layer*)(&current->L1 + layer_offset))->R;

    // pick an LED and make sure it isn't already set if filling or off if emptying
    int led = 1 << (rand() % 16);
    while((i % 129 > 64) ^ ((*target & led) || (*(target + 1) & led) || (*(target + 2) & led))){
        layer_offset = rand() % 4;

        target = &((Layer*)(&current->L1 + layer_offset))->R;

        led = 1 << (rand() % 16);
    }

    // set or clear the colors of the selected led
    if (BIT(0, color)) *target ^= led;       // R
    if (BIT(1, color)) *(target + 1) ^= led; // G
    if (BIT(2, color)) *(target + 2) ^= led; // B
}

void snake_GetNextFrame(const unsigned int i, Frame *current) {
    int phase, phase_last, color, x, y, z, dx = 0, dy = 0, dz = 0;

    if(i == 0) {
        memset(current, 0, sizeof(Frame));

        current->DisplayMS = 25;

        // initialize cache
        cache[0] = 0x0001;
        cache[1] = 0x0021;
        cache[2] = 0x0041;

        return;
    }

    phase = i % 3;
    phase_last = phase == 0 ? 2 : phase - 1;

    color = cache[phase_last] & 0x07;

    // pick a color
    if (i % 64 == 0) {
        int color_last = color;

        while ((color = rand() % 6 + 1) == color_last);
    }

    x = (cache[phase] >> 3) & 0x03;
    y = (cache[phase] >> 5) & 0x03;
    z = (cache[phase] >> 7) & 0x03;

    // clear last cached led
    unsigned short *target = &((Layer*)(&current->L1 + z))->R;
    unsigned int led = 1 << (4 * x + y);
    
    *target &= ~led;
    *(target + 1) &= ~led;
    *(target + 2) &= ~led;

    x = (cache[phase_last] >> 3) & 0x03;
    y = (cache[phase_last] >> 5) & 0x03;
    z = (cache[phase_last] >> 7) & 0x03;

    // find next movement
    target = &((Layer*)(&current->L1 + z + dz))->R;
    led = 1 << (4 * (x + dx) + y + dy);
    
    while ((*target | *(target + 1) | *(target + 2)) & led) {
        dx = dy = dz = 0;

        switch (rand() % 3) {
            case 0:
                dx = rand() % 3 - 1;
                while (x && x + dx > 3 || x + dx < 0) dx = rand() % 3 - 1;
                break;

            case 1:
                dy = rand() % 3 - 1;
                while (y && y + dy > 3 || y + dy < 0) dy = rand() % 3 - 1;
                break;

            case 2:
                dz = rand() % 3 - 1;
                while (z && z + dz > 3 || z + dz < 0) dz = rand() % 3 - 1;
                break;
        }

        target = &((Layer*)(&current->L1 + z + dz))->R;
        led = 1 << (4 * (x + dx) + y + dy);
    }

    // update cache
    cache[phase] = (z + dz << 7) | (y + dy << 5) | (x + dx << 3) | color;

    // set the colors of the selected led
    *target ^= (-BIT(0, color) ^ *target) & led;                // R
    *(target + 1) ^= (-BIT(1, color) ^ *(target + 1)) & led;    // G
    *(target + 2) ^= (-BIT(2, color) ^ *(target + 2)) & led;    // B
}