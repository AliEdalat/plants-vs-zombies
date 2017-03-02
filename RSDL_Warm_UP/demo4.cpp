#include "rsdl.hpp"
#include <cstdlib>
#include <vector>

using namespace std;
const int sun_width = 116;
const int sun_heigth = 116;
struct Sun{
  int x,y;
};
vector<Sun*> initialize_suns(int number_of_suns)
{
  vector<Sun*> suns;
  srand(0);
  for (int i = 0; i < number_of_suns; i++) {
    Sun* s = new Sun();
    s->x = rand()%400;
    s->y = rand()%400;
    suns.push_back(s);
  }
  return suns;
}

void check_suns(int x,int y,vector<Sun*> &suns)
{
	cout<<'('<< x<<','<<y<<')'<<endl;	
  for (int i = suns.size()-1; i >= 0 ;i--)
  {
    int left_threshold = suns[i]->x;
    int right_threshold = suns[i]->x + sun_width - 10;
    int top_threshold = suns[i]->y ;
    int bottom_threshold = suns[i]->y + sun_heigth - 10;
    if(x>left_threshold&&x<right_threshold
      &&y<bottom_threshold&&y>top_threshold)
    {
      suns.erase(suns.begin()+i);
      return;
    }
  }
}
void draw_sun(window &win,Sun* sun,int tick)
{
  int x = sun->x;
  int y = sun->y;
  win.draw_png(string("./Sun3.png"), x,y,sun_width, sun_heigth,tick);
  win.draw_png(string("./Sun2.png"), x+20,y+20,77, 79,-1*tick%360);
  win.draw_png(string("./Sun1.png"), x+43,y+43,32, 32);
}
int main(void){
    vector<Sun*> suns = initialize_suns(10);
    window win(800, 600);
    int x = 0;
    int tick = 0;
    int sgn = 1;
    while(true){
        tick++;
        win.draw_bg("./Frontyard.png", 0, 0);

        for (int i = 0; i < suns.size(); i++) {
          draw_sun(win, suns[i], tick);
        }
        x += 5*sgn;
        if(x >= 240)
        {
            sgn = -1;
        }
        if(x <= 0)
        {
            sgn = 1;
        }
        HANDLE({
            QUIT({return 0;})
            KEY_PRESS(q, {return 0;})
            LCLICK({
              check_suns(mouse_x, mouse_y,suns);
            })
        })
        win.update_screen();
        DELAY(15);
    }
}
