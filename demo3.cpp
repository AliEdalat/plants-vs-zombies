#include "rsdl.hpp"
#include <cstdlib>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 600
#define IMAGE_WIDTH 60
#define IMAGE_HEIGHT 60
#define ZOMBIE_SPEED 20


using namespace std;

typedef struct zombie Zombie;
typedef struct sun Sun;
typedef struct plant Plant;
typedef struct produce_time Time;
typedef struct pea Pea;

struct produce_time{
    int second;
    int minute;
    int hour;
};
struct zombie
{
    int lives;
    Time* produce_time;
    int x;
    int y;
};
struct sun
{
    Time* produce_time;
    int x;
    int y;
    bool is_related_to_sunflower;
};
struct pea
{
    Time* produce_time;
    int x;
    int y;   
};
struct plant
{
    string name;
    string image_name;
    Time* produce_time;
    Time* collision_time;
    int lives;
    int x;
    int y;
    bool is_zombie_alongside;
    bool seen_zombie;  
};
 void draw_plants(window& win,vector<Plant*>& plants){
    for (int i = 0; i < plants.size(); ++i)
    {
        
        win.draw_png(string(plants[i]->image_name),plants[i]->x,plants[i]->y,60,60,0);    
    }
    
 }
 void draw_zombies(window& win,vector<Zombie>& zombies){
    for (int i = 0; i < zombies.size(); ++i)
    {
        
        win.draw_png(string("./zombie.png"),zombies[i].x,zombies[i].y-40,60,100,0);    
    }
    
 }
 void draw_peas(window& win,vector<Pea*>& peas,int angle){
    for (int i = 0; i < peas.size(); ++i)
    {
        win.draw_png(string("./pea.png"), peas[i]->x,peas[i]->y+10,10, 10,angle);
    }
 }
 void draw_suns(window& win,vector<Sun*>& suns,int angle){
    for (int i = 0; i < suns.size(); ++i)
    {
        
        int x = suns[i]->x;
        int y = suns[i]->y;
        win.draw_png(string("./Sun2.png"), x,y,60, 60,angle);    
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
 int time_left(Time* current_time,Time* produce_time){
    int second_left;
    int minute_left;
    int hour_left;
    int result=0;
    if(current_time->second < produce_time->second){
        second_left=(current_time->second+60)-produce_time->second;
        minute_left=-1;
    }
    if (current_time->second >= produce_time->second)
    {
        second_left=(current_time->second)-produce_time->second;
        minute_left=0;
    }
    if(current_time->minute < produce_time->minute){
        minute_left+=(current_time->minute+60)-produce_time->minute;
        hour_left=-1;
    }
    if (current_time->minute >= produce_time->minute)
    {
        minute_left+=(current_time->minute)-produce_time->minute;
        hour_left=0;   
    }
    
    result=second_left+minute_left*60;
    
    return result;
 }
 void update_board(vector<vector<char> >& board,int x,int y,char new_character){
    int i_index,j_index;
    if((x-270)%80==0){
        i_index=(y-100)/100;
        j_index=(x-270)/80;
    }
    else{
        i_index=(y-100)/100;
        j_index=((x-270)/80)+1;
    }
    cerr<<y<<' '<<x<<endl;
    cerr<<i_index<<' '<<j_index<<endl;
    board[i_index][j_index]=new_character;
 }
 void change_nuts_states(vector<Plant*>& plants,Time* current_time,vector<vector<char> >& board){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(plants[i]->name=="nut"){
            bool is_zombie_alongside=plants[i]->is_zombie_alongside;
            if(is_zombie_alongside==true){
                int time=time_left(current_time,plants[i]->collision_time);
                plants[i]->lives-=time;
                plants[i]->collision_time=current_time;
                
            }
            if(plants[i]->lives<=48 && is_zombie_alongside==true){
                plants[i]->image_name="./nut2.png";
            }
            if(plants[i]->lives<=24 && is_zombie_alongside==true){
                plants[i]->image_name="./nut3.png";
            }
            if(plants[i]->lives<=0){
                update_board(board,plants[i]->x,plants[i]->y,'.');
                plants.erase(plants.begin()+i);
            }
        }
    }
    
 }
 void change_sunflowers_states(vector<Plant*>& plants,vector<Sun*>& suns,Time* current_time,vector<vector<char> >& board){
    for (int i = 0; i < plants.size(); ++i){
        if(plants[i]->name=="sunflower"){
            bool is_zombie_alongside=plants[i]->is_zombie_alongside;
            int time=time_left(current_time,plants[i]->produce_time);
            if(time%25==0 && time!=0){
                Sun* new_sun=new Sun();
                new_sun->x=plants[i]->x;
                new_sun->y=plants[i]->y;
                new_sun->produce_time=current_time;
                new_sun->is_related_to_sunflower=true;
                suns.push_back(new_sun);
            }
            if(is_zombie_alongside==true){
                int time=time_left(current_time,plants[i]->collision_time);
                plants[i]->lives-=time;
                plants[i]->collision_time=current_time;
            }
            if (plants[i]->lives <= 0)
            {
                update_board(board,plants[i]->x,plants[i]->y,'.');
                plants.erase(plants.begin()+i);   
            }

        }
    }
}
int find_neighbor_zombie(Plant* plant,vector<Zombie>& zombies){
    int result=-1;
    for (int i = 0; i < zombies.size(); ++i)
    {
        int zombie_y=zombies[i].y;
        if(plant->y == zombie_y){
            result=i;
            break;
        }
    }
    return result;

}
bool find_pea(vector<Pea*>& peas,int shooter_y ,int& pea_index){
    bool result=false;
    for (int i = 0; i < peas.size(); ++i)
    {
        if(peas[i]->y == shooter_y){
            result=true;
            pea_index=i;
            break;
        }
    }
    return result;
}
void shoot_pea(vector<Plant*>& plants,vector<Zombie>& zombies,vector<Pea*>& peas,Time* current_time,int counter){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(plants[i]->name=="pea_shooter"){
            if(plants[i]->seen_zombie==true){
                /*int time=time_left(current_time,plants[i]->produce_time);*/
                int zombie_index=find_neighbor_zombie(plants[i],zombies);
                int shooter_x= plants[i]->x;
                int zombie_x=zombies[zombie_index].x;
                int shooter_y=plants[i]->y;
                int pea_index;
                if (! find_pea(peas,shooter_y ,pea_index))
                {
                    
                        cout<<"find is false"<<endl;
                        Pea* new_pea=new Pea();
                        new_pea->x=shooter_x+30;
                        new_pea->y=shooter_y;
                        peas.push_back(new_pea);
                        /*plants[i]->produce_time->second=current_time->second;
                        plants[i]->produce_time->minute=current_time->minute;
                        plants[i]->produce_time->hour=current_time->hour;

                        new_pea->produce_time=current_time;
                        */
                    
                       
                }
                if(find_pea(peas,shooter_y ,pea_index)){
                    zombie_index=find_neighbor_zombie(plants[i],zombies);
                    zombie_x=zombies[zombie_index].x;
                    cout<< zombie_x<<' '<<peas[pea_index]->x<<endl;
                    if (peas[pea_index]->x == zombie_x+30)
                    {
                        cout<<"find is true"<<endl;
                        peas.erase(peas.begin()+pea_index);
                        plants[i]->seen_zombie=false;
                        /*plants[i]->produce_time->second=current_time->second;
                        plants[i]->produce_time->minute=current_time->minute;
                        plants[i]->produce_time->hour=current_time->hour;
                        */
                        if(zombie_index != -1){
                            zombies[zombie_index].lives-=1;
                        }
                    }
                    if(peas[pea_index]->x > 910){
                        peas.erase(peas.begin()+pea_index);
                    }
                    
                }
                if(find_pea(peas,shooter_y ,pea_index))
                {
                    //int time=time_left(current_time,peas[pea_index]->produce_time);
                    //if(time == 0){
                    //if(counter%1==0){
                        peas[pea_index]->x+=ZOMBIE_SPEED;
                        //peas[pea_index]->produce_time=current_time;
                    //}
                }
            }
        }
    }

}
void change_peashooter_states(vector<Plant*>& plants,Time* current_time,vector<vector<char> >& board){
    for (int i = 0; i < plants.size(); ++i){
        if(plants[i]->name=="pea_shooter"){
            bool is_zombie_alongside=plants[i]->is_zombie_alongside;
            if(is_zombie_alongside==true){
                int time=time_left(current_time,plants[i]->collision_time);
                plants[i]->lives-=time; 
                plants[i]->collision_time=current_time;  
            }
            if (plants[i]->lives <= 0)
            {
                update_board(board,plants[i]->x,plants[i]->y,'.');
                plants.erase(plants.begin()+i);   
            }
        }
    }

}
void change_zombies_state(vector<Zombie>& zombies,vector<vector<char> >& board){
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i].lives == 0){
            update_board(board,zombies[i].x,zombies[i].y,'.');
            zombies.erase(zombies.begin()+i);
        }
    }
}    
void save_suns(int x,int y,vector<Sun*> &suns,int& sun_number,vector<vector<char> >& board){

    for (int i = suns.size()-1; i >=0; i--)
    {
        
        int minimum_x=suns[i]->x+10;
        int maximum_x=suns[i]->x+50;
        int minimum_y=suns[i]->y+10;
        int maximum_y=suns[i]->y+50;
        if(x>=minimum_x && x<=maximum_x && y>=minimum_y && y<=maximum_y){
            if(suns[i]->is_related_to_sunflower==false){
                cerr<<"update_board"<<endl;
                update_board(board,suns[i]->x,suns[i]->y,'.');
            }
            suns.erase(suns.begin()+i);
            sun_number+=25;
            
        }
    }
    
}
bool is_zombie_neighbor(vector<Zombie>& zombies,Plant* plant){
    int plant_x=plant->x;
    int plant_y=plant->y;
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        
        if(zombies[i].y==plant_y && zombies[i].x==plant_x+80){
            result=true;
        }
    }
    return result;
}
bool seen_zombie(vector<Zombie>& zombies,Plant* plant){
    int plant_y=plant->y;
    int plant_x=plant->x;
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i].y==plant_y && zombies[i].x > plant_x+80){
            result=true;
        }
    }
    return result;
}
void zombies_and_plants(vector<Zombie>& zombies,vector<Plant*>& plants,Time* current_time){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(is_zombie_neighbor(zombies,plants[i]) && plants[i]->is_zombie_alongside!=true){
            plants[i]->is_zombie_alongside=true;
            plants[i]->collision_time->second=current_time->second;
            plants[i]->collision_time->minute=current_time->minute;
            plants[i]->collision_time->hour=current_time->hour;
        
        }
        if(plants[i]->name=="pea_shooter"){
            if(seen_zombie(zombies, plants[i])){
                plants[i]->seen_zombie=true;
            }
        }
    }
}
void generate_random_suns(vector<Sun*>& suns,vector<vector<char> >& board){
    srand(time(0));
    int random_row=rand()%5;
    int random_column=rand()%9;
    if(board[random_row][random_column]=='.'){
        Sun* new_sun=new Sun();
        new_sun->x=270+(random_column*80);
        new_sun->y=100+(random_row*100);
        time_t tim2;
        struct tm* second_time;
        time(&tim2);
        second_time=localtime(&tim2);
        new_sun->produce_time=new Time();
        new_sun->produce_time->second=second_time->tm_sec;
        new_sun->produce_time->minute=second_time->tm_min;
        new_sun->produce_time->hour=second_time->tm_hour;
        new_sun->is_related_to_sunflower=false;    
        suns.push_back(new_sun);
        board[random_row][random_column]='s';
    }
}
void move_zombies(vector<Zombie>& zombies,Time* current_time,vector<vector<char> >& board,int counter){
    for (int i = 0; i < zombies.size(); ++i)
    {
    
        int time=time_left(current_time,zombies[i].produce_time);
        if(time == 1){
            if(zombies[i].x >270){
                int i_index,j_index;
                if((zombies[i].x-270)%80==0){
                    i_index=(zombies[i].y-100)/100;
                    j_index=(zombies[i].x-270)/80;
                }
                else{
                    i_index=(zombies[i].y-100)/100;
                    j_index=((zombies[i].x-270)/80)+1;

                }
                cout<<i_index<<' '<<j_index<<endl;
                if(board[i_index][j_index-1] != 'f' && board[i_index][j_index-1] != 'n' && board[i_index][j_index-1] != 'p'){
                    
                    update_board(board,zombies[i].x,zombies[i].y,'.');
                    zombies[i].x-=ZOMBIE_SPEED;
                    zombies[i].produce_time->second=current_time->second;
                    zombies[i].produce_time->minute=current_time->minute;
                    zombies[i].produce_time->hour=current_time->hour;
                    update_board(board,zombies[i].x,zombies[i].y,'z');
                }
                else{
                    zombies[i].produce_time->second=current_time->second;
                    zombies[i].produce_time->minute=current_time->minute;
                    zombies[i].produce_time->hour=current_time->hour;
                }
            }
        }
    }
}
void change_suns_state(vector<Sun*>& suns,Time* current_time,vector<vector<char> >& board){
    for (int i = 0; i < suns.size(); ++i)
    {
        int time=time_left(current_time,suns[i]->produce_time);
        if(time==5){
            if(suns[i]->is_related_to_sunflower==false){
                update_board(board,suns[i]->x,suns[i]->y,'.');
            }
            suns.erase(suns.begin()+i);
        }
    }
}
bool loss_the_game(vector<vector<char> >& board){
    bool result=false;
    for (int i = 0; i < 5; ++i)
    {
        if(board[i][0]=='z'){
            result=true;
            break;
        }
    }
    return result;
}
void genarate_random_zombies(vector<Zombie>& zombies,vector<vector<char> >& board){
    for (int i = 0; i < 10; ++i)
    {
        
        srand(time(0));
        int i_index=rand()%5;
        int j_index=rand()%9;
        if(board[i_index][j_index]=='.' && j_index!=0){
            board[i_index][j_index]='z';
            Zombie new_zombie;
            
            new_zombie.x=j_index*80+270;
            new_zombie.y=i_index*100+100;
            new_zombie.lives=10;
            time_t tim2;
            struct tm* second_time;
            time(&tim2);
            second_time=localtime(&tim2);
            Time* correct_t=new Time();
            correct_t->second=second_time->tm_sec;
            correct_t->minute=second_time->tm_min;
            correct_t->hour=second_time->tm_hour;
            new_zombie.produce_time=correct_t;
            zombies.push_back(new_zombie);
            
        }
        
    }
}
int main(void){
    srand(0);
    int waves=0;
    vector<vector<char> > board(5);
    vector<Plant*> plants;
    vector<Sun*> suns;
    vector<Zombie> zombies;
    vector<Pea*> peas;
    window win(WINDOW_WIDTH, WINDOW_HEIGHT,"PLANTS vs ZOMBIES");
    struct tm* current_time;
    int i = 0,j = 0;
    int x = 0;
    int sun_number=0;
    time_t time_m;
    int sgn = 1;
    int angle=0;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            board[i].push_back('.');
        }
    }
    /*Zombie zombie;
    zombie.x=910;
    zombie.y=200;
    zombie.lives=10;
    time_t tim2;
    struct tm* second_time;
    time(&tim2);
    second_time=localtime(&tim2);
    Time* correct_t=new Time();
    correct_t->second=second_time->tm_sec;
    correct_t->minute=second_time->tm_min;
    correct_t->hour=second_time->tm_hour;
    zombie.produce_time=correct_t;
    zombies.push_back(zombie);
    update_board(board,zombie.x,zombie.y,'z');
    */
    genarate_random_zombies(zombies,board);
    int counter=0;
    while(true){
        angle++;
        win.draw_bg(string("./Frontyard.png"),0,0);
        time_t tim2;
        struct tm* second_time;
        time(&tim2);
        second_time=localtime(&tim2);
        Time* correct_t=new Time();
        correct_t->second=second_time->tm_sec;
        correct_t->minute=second_time->tm_min;
        correct_t->hour=second_time->tm_hour;
        generate_random_suns(suns,board);
        zombies_and_plants(zombies,plants,correct_t);
        change_nuts_states(plants, correct_t,board);
        change_sunflowers_states(plants,suns,correct_t,board);
        change_peashooter_states(plants,correct_t,board);
        move_zombies(zombies,correct_t,board,counter);
        shoot_pea(plants,zombies,peas,correct_t,counter);
        change_zombies_state(zombies,board);
        change_suns_state(suns,correct_t,board);
        
        // for (int j = 0; j < 200; j++)
        //     win.draw_bmp(string("./image.bmp"), rand()%600, rand()%600, 30, 30);
        
        win.draw_rect(270-j,300-i,60,60,WHITE);
        draw_plants(win,plants);
        draw_suns(win,suns,angle);
        draw_zombies(win,zombies);
        draw_peas(win,peas,angle);
        for (int row = 0; row < 5; ++row)
        {
            for (int column = 0; column < 9; ++column)
            {
                cout<<board[row][column]<<' ';
            }
            cout<<endl;
        }
        cout<<endl;
        cout<<sun_number<<endl;
        cout<<zombies[0].x<<' '<<zombies[0].y<<endl;
        HANDLE({
            QUIT({return 0;})
            LCLICK({
              save_suns(mouse_x, mouse_y,suns,sun_number,board);
              continue;
            })
            KEY_PRESS(UP, {
                if(300-i>100){
                    i += 100;
                }
            })
            KEY_PRESS(DOWN, {
                if(300-i<500){
                    i -= 100;
                }
            })
            KEY_PRESS(LEFT, {
                if(270-j>270){
                    j += 80;
                }
            })
            KEY_PRESS(RIGHT, {
                if(270-j<910){
                    j -= 80;
                }
            })
            KEY_PRESS(q, {return 0;})
            KEY_PRESS(f,{
                int i_index=(200-i)/100;
                int j_index=(-j)/80;
                if(i_index<0){
                    i_index*=-1;
                }
                if(j_index<0){
                    j_index*=-1;
                }
                if (board[i_index][j_index]=='.')
                {
                    Plant* sunflower=new Plant();
                    sunflower->x=270-j;
                    sunflower->y=300-i;
                    board[i_index][j_index]='f';
                    sunflower->name="sunflower";
                    sunflower->image_name="./sunflower.png";
                    sunflower->lives=5;
                    sunflower->is_zombie_alongside=false;
                    sunflower->seen_zombie=false;
                    time_t tim;
                    time(&tim);
                    current_time=localtime(&tim);
                    sunflower->produce_time=new Time();
                    (sunflower->produce_time)->second=current_time->tm_sec;
                    (sunflower->produce_time)->minute=current_time->tm_min;
                    (sunflower->produce_time)->hour=current_time->tm_hour;
                    sunflower->collision_time=new Time();
                    (sunflower->collision_time)->second=current_time->tm_sec;
                    (sunflower->collision_time)->minute=current_time->tm_min;
                    (sunflower->collision_time)->hour=current_time->tm_hour;
                    cout<< (sunflower->produce_time)->second <<' '<<(sunflower->produce_time)->minute<<' '<<(sunflower->produce_time)->hour<<endl;
                    plants.push_back(sunflower);
                }
                continue;
            })
            KEY_PRESS(n,{
                int i_index=(200-i)/100;
                int j_index=(-j)/80;
                if(i_index<0){
                    i_index*=-1;
                }
                if(j_index<0){
                    j_index*=-1;
                }
                if(board[i_index][j_index]=='.'){
                    Plant* nut=new Plant();
                    nut->x=270-j;
                    nut->y=300-i;
                    board[i_index][j_index]='n';
                    nut->name="nut";
                    nut->image_name="./nut1.png";
                    nut->lives=72;
                    nut->is_zombie_alongside=false;
                    nut->seen_zombie=false;
                    time_t tim;
                    time(&tim);
                    current_time=localtime(&tim);
                    nut->produce_time=new Time();
                    (nut->produce_time)->second=current_time->tm_sec;
                    (nut->produce_time)->minute=current_time->tm_min;
                    (nut->produce_time)->hour=current_time->tm_hour;
                    nut->collision_time=new Time();
                    (nut->collision_time)->second=current_time->tm_sec;
                    (nut->collision_time)->minute=current_time->tm_min;
                    (nut->collision_time)->hour=current_time->tm_hour;
                    plants.push_back(nut);
                }
                continue;
            })
            KEY_PRESS(p,{
                int i_index=(200-i)/100;
                int j_index=(-j)/80;
                if(i_index<0){
                    i_index*=-1;
                }
                if(j_index<0){
                    j_index*=-1;
                }
                if(board[i_index][j_index]=='.'){
                    Plant* pea_shooter=new Plant();
                    pea_shooter->x=270-j;
                    pea_shooter->y=300-i;
                    board[i_index][j_index]='p';
                    pea_shooter->name="pea_shooter";
                    pea_shooter->image_name="./peashooter.png";
                    pea_shooter->lives=5;
                    pea_shooter->is_zombie_alongside=false;
                    pea_shooter->seen_zombie=false;
                    time_t tim;
                    time(&tim);
                    current_time=localtime(&tim);
                    pea_shooter->produce_time=new Time();
                    (pea_shooter->produce_time)->second=current_time->tm_sec;
                    (pea_shooter->produce_time)->minute=current_time->tm_min;
                    (pea_shooter->produce_time)->hour=current_time->tm_hour;
                    pea_shooter->collision_time=new Time();
                    (pea_shooter->collision_time)->second=current_time->tm_sec;
                    (pea_shooter->collision_time)->minute=current_time->tm_min;
                    (pea_shooter->collision_time)->hour=current_time->tm_hour;
                    plants.push_back(pea_shooter);
                }
                continue;
            })
            KEY_PRESS(r,{
                remove_plant(plants,270-j,300-i);
                int i_index=(200-i)/100;
                int j_index=(-j)/80;
                if(i_index<0){
                    i_index*=-1;
                }
                if(j_index<0){
                    j_index*=-1;
                }
                if(board[i_index][j_index]!='s' && board[i_index][j_index]!='z'){
                    board[i_index][j_index]='.';
                }
                continue;
            })
            
        })
        counter++;
        if(loss_the_game(board)){
            win.draw_bg(string("./Frontyard.png"),0,0);
            win.show_text("you lose the game!",510,300,BLACK);
            win.update_screen();

        }
        win.update_screen();
        DELAY(15);
    }
}
