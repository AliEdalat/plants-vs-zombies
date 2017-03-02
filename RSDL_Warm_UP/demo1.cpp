#include "rsdl.hpp"

using namespace std;

int main(void){
    window win(800, 700);
    win.draw_bg("./h.png", 0, 0);
    while(true){
        HANDLE({
            QUIT({return 0;})
            KEY_PRESS(q, {return 0;})
        })
        win.update_screen();
        DELAY(15);
    }
}
