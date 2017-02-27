#include "rsdl.hpp"
#include <cstdlib>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>

#define WINDOW_WIDTH 800
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
 void draw_zombies(window& win,vector<Zombie*>& zombies){
    for (int i = 0; i < zombies.size(); ++i)
    {
        
        win.draw_png(string("./zombie.png"),zombies[i]->x,zombies[i]->y-40,60,100,0);    
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
    if((x-80)%80==0){
        i_index=(y-100)/100;
        j_index=(x-80)/80;
    }
    else{
        i_index=(y-100)/100;
        j_index=((x-80)/80);
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
int find_neighbor_zombie(Plant* plant,vector<Zombie*>& zombies){
    int result=-1;
    for (int i = 0; i < zombies.size(); ++i)
    {
        int zombie_y=zombies[i]->y;
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
void shoot_pea(vector<Plant*>& plants,vector<Zombie*>& zombies,vector<Pea*>& peas,Time* current_time,int counter){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(plants[i]->name=="pea_shooter"){
            if(plants[i]->seen_zombie==true){
                
                int zombie_index=find_neighbor_zombie(plants[i],zombies);
                int shooter_x= plants[i]->x;
                int zombie_x;
                if(zombie_index!=-1){
                    zombie_x=zombies[zombie_index]->x;
                }
                else{
                    plants[i]->seen_zombie=false;
                    continue;
                }
                int shooter_y=plants[i]->y;
                int pea_index;
                if (! find_pea(peas,shooter_y ,pea_index))
                {
                        
                        cout<<"find is false"<<endl;
                        Pea* new_pea=new Pea();
                        new_pea->x=shooter_x+30;
                        new_pea->y=shooter_y;
                        peas.push_back(new_pea);
                        
                    
                       
                }
                if(find_pea(peas,shooter_y ,pea_index)){
                    cout<<"find pea true!"<<endl;
                    zombie_index=find_neighbor_zombie(plants[i],zombies);
                    if(zombie_index!=-1){
                        zombie_x=zombies[zombie_index]->x;
                    }
                    else{
                        plants[i]->seen_zombie=false;
                        continue;
                    }
                
                    if (peas[pea_index]->x == zombie_x+30 && zombie_index!=-1)
                    {
                        cout<<"find is true"<<endl;
                        peas.erase(peas.begin()+pea_index);
                        cout<<"erase completely!"<<endl;
                        if(zombie_index != -1){
                            zombies[zombie_index]->lives-=1;
                            if(zombies[zombie_index]->lives==0){
                                plants[i]->seen_zombie=false;
                            }
                        }
                    }
                    else if(peas[pea_index]->x > 720 || zombie_index==-1){
                        peas.erase(peas.begin()+pea_index);
                    }
                    
                }
                if(find_pea(peas,shooter_y ,pea_index))
                { 
                        cout<<"find pea and increase x!"<<endl;  
                        peas[pea_index]->x+=ZOMBIE_SPEED;
                    
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
void change_zombies_state(vector<Zombie*>& zombies,vector<vector<char> >& board){
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i]->lives == 0){
            update_board(board,zombies[i]->x,zombies[i]->y,'.');
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
bool is_zombie_neighbor(vector<Zombie*>& zombies,Plant* plant){
    int plant_x=plant->x;
    int plant_y=plant->y;
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        
        if(zombies[i]->y==plant_y && zombies[i]->x==plant_x+80){
            result=true;
        }
    }
    return result;
}
bool seen_zombie(vector<Zombie*>& zombies,Plant* plant){
    int plant_y=plant->y;
    int plant_x=plant->x;
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i]->y==plant_y && zombies[i]->x > plant_x+80){
            result=true;
        }
    }
    return result;
}
void zombies_and_plants(vector<Zombie*>& zombies,vector<Plant*>& plants,Time* current_time){
    for (int i = 0; i < plants.size(); ++i)
    {
        if(is_zombie_neighbor(zombies,plants[i]) && plants[i]->is_zombie_alongside!=true){
            plants[i]->is_zombie_alongside=true;
            plants[i]->collision_time->second=current_time->second;
            plants[i]->collision_time->minute=current_time->minute;
            plants[i]->collision_time->hour=current_time->hour;
        
        }
        if( !is_zombie_neighbor(zombies,plants[i]) && plants[i]->is_zombie_alongside==true){
            plants[i]->is_zombie_alongside=false;
        }
        if(plants[i]->name=="pea_shooter"){
            if(seen_zombie(zombies, plants[i])){
                plants[i]->seen_zombie=true;
            }
            if (!seen_zombie(zombies, plants[i]))
            {
                plants[i]->seen_zombie=false;
            }
        }
    }
}
void generate_random_suns(vector<Sun*>& suns,vector<vector<char> >& board){
    srand(time(0));
    int probability=rand()%5+1;
    if(probability==1){
        int random_row=rand()%5;
        int random_column=rand()%9;
        if(board[random_row][random_column]=='.'){
            Sun* new_sun=new Sun();
            new_sun->x=80+(random_column*80);
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
}
void update_zombies_data(int row,int column ,int zombie_index, vector<Zombie*>& zombies,Time* current_time,vector<vector<char> >& board){
                if(board[row][column] != 'f' && board[row][column] != 'n' && board[row][column] != 'p' && (board[row][column-1] != 'z' || board[row][column] != 'z')){
                    
                    update_board(board,zombies[zombie_index]->x,zombies[zombie_index]->y,'.');
                    zombies[zombie_index]->x-=ZOMBIE_SPEED;
                    zombies[zombie_index]->produce_time->second=current_time->second;
                    zombies[zombie_index]->produce_time->minute=current_time->minute;
                    zombies[zombie_index]->produce_time->hour=current_time->hour;
                    update_board(board,zombies[zombie_index]->x,zombies[zombie_index]->y,'z');
                }
                else{
                    zombies[zombie_index]->produce_time->second=current_time->second;
                    zombies[zombie_index]->produce_time->minute=current_time->minute;
                    zombies[zombie_index]->produce_time->hour=current_time->hour;
                }
}
void move_zombies(vector<Zombie*>& zombies,Time* current_time,vector<vector<char> >& board,int counter){
    for (int i = 0; i < zombies.size(); ++i)
    {
    
        int time=time_left(current_time,zombies[i]->produce_time);
        if(time == 1){
            if(zombies[i]->x >80){
                int i_index,j_index;
                i_index=(zombies[i]->y-100)/100;
                j_index=(zombies[i]->x-80)/80;
                
                cout<<i_index<<' '<<j_index<<endl;
                if((zombies[i]->x-80)%80==0){
                    update_zombies_data(i_index,j_index-1,i,zombies,current_time,board);
                }
                else{
                    update_zombies_data(i_index,j_index,i,zombies,current_time,board);
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
bool loss_the_game(vector<Zombie*>& zombies){
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i]->x == 80){
            result=true;
            break;
        }
    }
    return result;
}
bool win_the_game(int& wave_number,int max_wave,vector<Zombie*>& zombies){
    bool result=false;
    if(wave_number== max_wave){
        if(zombies.size() == 0){
            result=true;
        }
    }
    return result;
}
void add_zombie(int row_index,vector<Zombie*>& zombies,vector<vector<char> >& board){
            Zombie* new_zombie=new Zombie();
            new_zombie->x=720;
            new_zombie->y=row_index*100+100;
            new_zombie->lives=10;
            time_t tim2;
            struct tm* second_time;
            time(&tim2);
            second_time=localtime(&tim2);
            new_zombie->produce_time=new Time();
            new_zombie->produce_time->second=second_time->tm_sec;
            new_zombie->produce_time->minute=second_time->tm_min;
            new_zombie->produce_time->hour=second_time->tm_hour;
            zombies.push_back(new_zombie);
            update_board(board,new_zombie->x,new_zombie->y,'z');
}
void genarate_random_zombies(vector<Zombie*>& zombies,vector<vector<char> >& board,int& produced_zombie_number){
    
        srand(time(0));
        int i_index=rand()%5;
        int j_index=8;
        
        if(board[i_index][j_index]!='z'){
            add_zombie(i_index,zombies,board);
            cout<<i_index<<endl;
            produced_zombie_number++;
        }
        else{
            for (int i = 0; i < 5; ++i)
            {
                if(board[i][j_index] != 'z'){
                    add_zombie(i,zombies,board);
                    produced_zombie_number++;
                    break;           
                }
            }
        }
}
void manage_level_waves(Time* start_time,Time* current_time, vector<Zombie*>& zombies,vector<vector<char> >& board,vector<int>& number_of_zombies,vector<int>& waves_length,int& wave_number,int max_wave, int& produced_zombie_number){
    
    if(wave_number == max_wave){
        return;
    }
    if(number_of_zombies[wave_number] != produced_zombie_number && produced_zombie_number==0){
        cout<<"hey"<<endl;
        start_time->second=current_time->second;
        start_time->minute=current_time->minute;
        start_time->hour=current_time->hour;
    }
    if(number_of_zombies[wave_number] != produced_zombie_number){
        clock_t clock_time=clock();
        if(clock_time%10==0){
            
                genarate_random_zombies(zombies,board,produced_zombie_number);
        }
        cout<<"produced_zombie_number :"<<produced_zombie_number<<endl;
    }
    
    int time=time_left(current_time,start_time);
    if(number_of_zombies[wave_number] == produced_zombie_number && time==waves_length[wave_number]){
        wave_number++;
        cout<<"wave :"<<wave_number<<' '<<"produced_zombie_number :"<<produced_zombie_number<<"time :"<<time<<endl;
        produced_zombie_number=0;
        start_time->second=current_time->second;
        start_time->minute=current_time->minute;
        start_time->hour=current_time->hour;
        return;
        
    }
    if(number_of_zombies[wave_number] == produced_zombie_number){
        cout<<"create zombies time :"<<time<<endl;
        return;
    }
}
void add_sunflower(int row,int column,vector<vector<char> >& board,vector<Plant*>& plants){
        struct tm* current_time;
        int i_index=(200-row)/100;
        int j_index=(-column)/80;
        if(i_index<0){
            i_index*=-1;
        }
        if(j_index<0){
            j_index*=-1;
        }
        if (board[i_index][j_index]=='.')
        {
            Plant* sunflower=new Plant();
            sunflower->x=80-column;
            sunflower->y=300-row;
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
            plants.push_back(sunflower);
        }
}
void add_wallnut(int row ,int column,vector<vector<char> >& board,vector<Plant*>& plants){
        struct tm* current_time;
        int i_index=(200-row)/100;
        int j_index=(-column)/80;
        if(i_index<0){
            i_index*=-1;
        }
        if(j_index<0){
            j_index*=-1;
        }
        if(board[i_index][j_index]=='.'){
            Plant* wallnut=new Plant();
            wallnut->x=80-column;
            wallnut->y=300-row;
            board[i_index][j_index]='n';
            wallnut->name="nut";
            wallnut->image_name="./nut1.png";
            wallnut->lives=72;
            wallnut->is_zombie_alongside=false;
            wallnut->seen_zombie=false;
            time_t tim;
            time(&tim);
            current_time=localtime(&tim);
            wallnut->produce_time=new Time();
            (wallnut->produce_time)->second=current_time->tm_sec;
            (wallnut->produce_time)->minute=current_time->tm_min;
            (wallnut->produce_time)->hour=current_time->tm_hour;
            wallnut->collision_time=new Time();
            (wallnut->collision_time)->second=current_time->tm_sec;
            (wallnut->collision_time)->minute=current_time->tm_min;
            (wallnut->collision_time)->hour=current_time->tm_hour;
            plants.push_back(wallnut);
        }
}
void add_peashooter(int row ,int column,vector<vector<char> >& board,vector<Plant*>& plants){
        struct tm* current_time;
        int i_index=(200-row)/100;
        int j_index=(-column)/80;
        if(i_index<0){
            i_index*=-1;
        }
        if(j_index<0){
            j_index*=-1;
        }
        if(board[i_index][j_index]=='.'){
            Plant* pea_shooter=new Plant();
            pea_shooter->x=80-column;
            pea_shooter->y=300-row;
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
}
int main(void){
    srand(0);
    int max_wave=6;
    Time* start_time=new Time();
    time_t tim2;
    struct tm* second_time;
    time(&tim2);
    second_time=localtime(&tim2);
    start_time->second=second_time->tm_sec;
    start_time->minute=second_time->tm_min;
    start_time->hour=second_time->tm_hour;
    int wave_number=0;
    int produced_zombie_number=0;
    
    vector<int> number_of_zombies;
    number_of_zombies.push_back(12);
    number_of_zombies.push_back(0);
    number_of_zombies.push_back(20);
    number_of_zombies.push_back(0);
    number_of_zombies.push_back(12);
    number_of_zombies.push_back(0);
    number_of_zombies.push_back(15);
    
    vector<vector<char> > board(5);
    
    vector<int> waves_length;
    waves_length.push_back(15);
    waves_length.push_back(15);
    waves_length.push_back(10);
    waves_length.push_back(15);
    waves_length.push_back(15);
    waves_length.push_back(15);
    waves_length.push_back(10);
    
    vector<Plant*> plants;
    vector<Sun*> suns;
    vector<Zombie*> zombies;
    vector<Pea*> peas;
    window win(WINDOW_WIDTH, WINDOW_HEIGHT,"PLANTS vs ZOMBIES");
    struct tm* current_time;
    int i = 0,j = 0;
    int x = 0;
    int sun_number=0;
    time_t time_m;
    int sgn = 1;
    int angle=0;
    clock_t t;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            board[i].push_back('.');
        }
    }
    
    genarate_random_zombies(zombies,board,produced_zombie_number);
    int counter=0;
    while(true){
        angle++;
        win.draw_bg(string("./Frontyard.png"),190,0);
        time_t tim2;
        struct tm* second_time;
        time(&tim2);
        second_time=localtime(&tim2);
        Time* correct_t=new Time();
        correct_t->second=second_time->tm_sec;
        correct_t->minute=second_time->tm_min;
        correct_t->hour=second_time->tm_hour;
        manage_level_waves(start_time, correct_t, zombies, board, number_of_zombies, waves_length,wave_number,max_wave,produced_zombie_number);
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
        
        win.draw_rect(80-j,300-i,60,60,WHITE);
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
        cout<<zombies[0]->x<<' '<<zombies[0]->y<<endl;
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
                if(80-j>80){
                    j += 80;
                }
            })
            KEY_PRESS(RIGHT, {
                if(80-j<720){
                    j -= 80;
                }
            })
            KEY_PRESS(q, {return 0;})
            KEY_PRESS(f,{
                add_sunflower( i, j, board, plants);
                continue;
            })
            KEY_PRESS(n,{
                add_wallnut( i, j, board, plants);
                continue;
            })
            KEY_PRESS(p,{
                add_peashooter(i,j,board,plants);
                continue;
            })
            KEY_PRESS(r,{
                remove_plant(plants,80-j,300-i);
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
        if(loss_the_game(zombies)){
            /*win.draw_bg(string("./Frontyard.png"),0,0);
            win.show_text("you lose the game!",510,300,BLACK);
            */
            DELAY(40);
            win.draw_bg(string("./game_over.png"),0,0);
            win.update_screen();

        }
        if(win_the_game(wave_number,max_wave,zombies)){
            win.draw_bg(string("./Frontyard.png"),0,0);
            win.show_text("you won this level",270,300,BLACK);
            win.update_screen();
            
        }
        win.update_screen();
        DELAY(15);
    }
}
