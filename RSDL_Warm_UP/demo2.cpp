#include "rsdl.hpp"

using namespace std;

int main(void){
    window win(800, 600);
    int x = 0;
    int sgn = 1;
    while(true){
        win.draw_bg("./frontyard.png", x, 0);
        x += 5*sgn;
        if(x >= 240)
        {
            DELAY(1000);
            sgn = -1;
        }
        if(x <= 0)
        {
            DELAY(1000);
            sgn = 1;
        }
        HANDLE({
            QUIT({return 0;})
            KEY_PRESS(q, {return 0;})
        })
        win.update_screen();
        DELAY(15);
    }
}
