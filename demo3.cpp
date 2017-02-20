#include "rsdl.hpp"
#include <cstdlib>
#include <cstdlib>
#include <vector>
#include <ctime>

using namespace std;
typedef struct zombie Zombie;
typedef struct sun Sun;
typedef struct plant Plant;
typedef struct produce_time Time;

struct produce_time{
    int second;
    int minute;
    int hour;
};
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
    Time* produce_time;
    int lives;
    int x;
    int y;
};
 void draw_plants(window& win,vector<Plant*> plants){
    for (int i = 0; i < plants.size(); ++i)
    {
        
        win.draw_png(string(plants[i]->image_name),plants[i]->x,plants[i]->y,60,60,0);    
    }
    
 }
 void draw_suns(window& win,vector<Sun*> suns){
    for (int i = 0; i < suns.size(); ++i)
    {
        
        win.draw_png(string("Sun2.png"),suns[i]->x,suns[i]->y,60,60,0);    
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
 void change_nuts_states(vector<Plant*>& plants,bool is_zombie_alongside,Time* current_time){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(plants[i]->name=="nut"){
            if(is_zombie_alongside==true){
                int time_left=(current_time->second)-(plants[i]->produce_time->second);
                plants[i]->lives-=time_left;
                cout<<time_left<<endl;
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
 void change_sunflowers_states(vector<Plant*>& plants,vector<Sun*>& suns,bool is_zombie_alongside,Time* current_time){
    for (int i = 0; i < plants.size(); ++i){
        if(plants[i]->name=="sunflower"){
            int time_left=(current_time->second)-(plants[i]->produce_time->second);
            if(time_left==25){
                Sun* new_sun=new Sun();
                new_sun->x=plants[i]->x+10;
                new_sun->y=plants[i]->y+10;
                suns.push_back(new_sun);
            }
        }
    }
}    
int main(void){
    srand(0);
    vector<Plant*> plants;
    vector<Sun*> suns;
    window win(1024, 600);
    struct tm* current_time;
    int i = 0,j = 0;
    int x = 0;
    time_t time_m;
    int sgn = 1;
    while(true){
        win.draw_bg(string("./frontyard.png"),0,0);
        time_t tim2;
        struct tm* second_time;
        time(&tim2);
        second_time=localtime(&tim2);
        Time* correct_t=new Time();
        correct_t->second=second_time->tm_sec;
        correct_t->minute=second_time->tm_min;
        correct_t->hour=second_time->tm_hour;
        change_nuts_states(plants,true, correct_t);
        change_sunflowers_states(plants,suns,true,correct_t);
        x += 5*sgn;
        if(x == 500)
            sgn = -1;
        if(x == 0)
            sgn = 1;
        // for (int j = 0; j < 200; j++)
        //     win.draw_bmp(string("./image.bmp"), rand()%600, rand()%600, 30, 30);
        win.draw_rect(200-j,200-i,60,60,WHITE);
        draw_plants(win,plants);
        draw_suns(win,suns);
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
                sunflower->produce_time=new Time();
                (sunflower->produce_time)->second=current_time->tm_sec;
                (sunflower->produce_time)->minute=current_time->tm_min;
                (sunflower->produce_time)->hour=current_time->tm_hour;
                cout<< (sunflower->produce_time)->second <<' '<<(sunflower->produce_time)->minute<<' '<<(sunflower->produce_time)->hour<<endl;
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
                time_t tim;
                time(&tim);
                current_time=localtime(&tim);
                nut->produce_time=new Time();
                (nut->produce_time)->second=current_time->tm_sec;
                (nut->produce_time)->minute=current_time->tm_min;
                (nut->produce_time)->hour=current_time->tm_hour;
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
