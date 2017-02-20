#include "rsdl.hpp"
#include <cstdlib>
#include <cstdlib>
#include <vector>
#include <ctime>

using namespace std;
typedef struct zombie Zombie;
typedef struct sun Sun;
typedef struct plant Plant;

struct zombie
{
    int lives;
    int x;
    int y;
};
struct sun
{
    int x;
    int y;
};
struct plant
{
    string name;
    string image_name;
    int time;
    int lives;
    int x;
    int y;
};
 void draw_plants(window& win,vector<Plant*> plants){
    for (int i = 0; i < plants.size(); ++i)
    {
        cout<< plants[i]->x<<' '<<plants[i]->y<<endl;
        win.draw_png(string(plants[i]->image_name),plants[i]->x,plants[i]->y,60,60,0);    
    }
    
 }
 void remove_plant(vector<Plant*> &plants,int x,int y){
    for (int i = plants.size()-1; i >= 0 ;i--)
    {
        if(plants[i]->x==x && plants[i]->y==y){
            plants.erase(plants.begin()+i);
             return;

        }
    }
 }
 void change_nuts_states(vector<Plant*>& plants,bool is_zombie_alongside,int current_time){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(plants[i]->name=="nut"){
            if(is_zombie_alongside==true){
                int time=(current_time)-(plants[i]->time);
                plants[i].lives-=time;
            }
            if(plants[i]->lives<=48 && is_zombie_alongside==true){
                plants[i]->image_name="./nut2.png";
            }
            if(plants[i]->lives<=24 && is_zombie_alongside==true){
                plants[i]->image_name="./nut3.png";
            }
            if(plants[i]->lives<=0){
                plants.erase(plants.begin()+i);
            }
        }
    }
    
 }
int main(void){
    srand(0);
    vector<Plant*> plants;
    window win(1024, 600);
    struct tm* current_time;
    int i = 0,j = 0;
    int x = 0;
    time_t time_m;
    int sgn = 1;
    while(true){
        win.draw_bg(string("./frontyard.png"),0,0);
        x += 5*sgn;
        if(x == 500)
            sgn = -1;
        if(x == 0)
            sgn = 1;
        // for (int j = 0; j < 200; j++)
        //     win.draw_bmp(string("./image.bmp"), rand()%600, rand()%600, 30, 30);
        win.draw_rect(200-j,200-i,60,60,WHITE);
        draw_plants(win,plants);
        HANDLE({
            QUIT({return 0;})
            KEY_PRESS(w, {
                i += 100;
            })
            KEY_PRESS(s, {
                i -= 100;
            })
            KEY_PRESS(a, {
                j += 70;
            })
            KEY_PRESS(d, {
                j -= 70;
            })
            KEY_PRESS(q, {return 0;})
            KEY_PRESS(f,{
                Plant* sunflower=new Plant();
                sunflower->x=200-j;
                sunflower->y=200-i;
                sunflower->name="sunflower";
                sunflower->image_name="./sunflower.png";
                sunflower->lives=5;
                time_t tim;
                time(&tim);
                current_time=localtime(&tim);
                sunflower->time=current_time->tm_sec;
                cout<< sunflower->time <<endl;
                plants.push_back(sunflower);
                continue;
            })
            KEY_PRESS(n,{
                Plant* nut=new Plant();
                nut->x=200-j;
                nut->y=200-i;
                nut->name="nut";
                nut->image_name="./nut1.png";
                nut->lives=72;
                plants.push_back(nut);
                continue;
            })
            KEY_PRESS(p,{
                Plant* pea_shooter=new Plant();
                pea_shooter->x=200-j;
                pea_shooter->y=200-i;
                pea_shooter->name="pea_shooter";
                pea_shooter->image_name="./peashooter.png";
                pea_shooter->lives=5;
                plants.push_back(pea_shooter);
                continue;
            })
            KEY_PRESS(r,{
                remove_plant(plants,200-j,200-i);
                continue;
            })
            
        })
        
        win.update_screen();
        DELAY(15);
    }
}
