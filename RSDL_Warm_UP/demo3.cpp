#include "rsdl.hpp"
#include <cstdlib>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>

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
    int start_y;
    int y;
    bool is_related_to_sunflower;
    bool is_fixed;
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
        int x;
        int y;
        if(suns[i]->is_related_to_sunflower == false){
        
            x = suns[i]->x;
            y = suns[i]->start_y;
        }
        else{
            x = suns[i]->x;
            y = suns[i]->y;   
        }
        win.draw_png(string("./Sun2.png"), x,y,60, 60,angle);    
    }
    
 }
 void remove_plant(vector<Plant*> &plants,int x,int y,int& sun_number){
    for (int i = plants.size()-1; i >= 0 ;i--)
    {
        if(plants[i]->x==x && plants[i]->y==y){
            if(plants[i]->name == "pea_shooter"){
                sun_number+=100;
            }
            if(plants[i]->name == "sunflower"){
                sun_number+=50;
            }
            if(plants[i]->name == "nut"){
                sun_number+=50;
            }
            cout<<plants[i]->name<<endl;
            plants.erase(plants.begin()+i);
            break;

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
                cout<<"sun created"<<endl;
                Sun* new_sun=new Sun();
                new_sun->x=plants[i]->x;
                new_sun->y=plants[i]->y;
                new_sun->produce_time=current_time;
                new_sun->is_related_to_sunflower=true;
                new_sun->is_fixed = true;
                suns.push_back(new_sun);
                plants[i]->produce_time->second=current_time->second;
                plants[i]->produce_time->minute=current_time->minute;
                plants[i]->produce_time->hour=current_time->hour;
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
            cout<<"sun"<<endl;
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
void add_sun(int random_row,int random_column,vector<Sun*>& suns,vector<vector<char> >& board){
            Sun* new_sun=new Sun();
            new_sun->x=80+(random_column*80);
            new_sun->y=100+(random_row*100);
            new_sun->start_y=100;
            time_t tim2;
            struct tm* second_time;
            time(&tim2);
            second_time=localtime(&tim2);
            new_sun->produce_time=new Time();
            new_sun->produce_time->second=second_time->tm_sec;
            new_sun->produce_time->minute=second_time->tm_min;
            new_sun->produce_time->hour=second_time->tm_hour;
            new_sun->is_related_to_sunflower=false;
            new_sun->is_fixed=false;    
            suns.push_back(new_sun);
            board[random_row][random_column]='s';
}
void move_suns(vector<Sun*>& suns){
    for (int i = 0; i < suns.size(); ++i)
    {
        if(suns[i]->is_related_to_sunflower == false){
            if(suns[i]->is_fixed == false){
                suns[i]->start_y+=10;
            }
        }
    }

}
void generate_random_suns(Time* current_time,vector<Sun*>& suns,vector<vector<char> >& board){
    
    if(suns.size() !=0){
        int time_l=time_left(current_time,suns[suns.size()-1]->produce_time);
        if(time_l>=1){
            srand(time(0));
            int probability=rand()%5+1;
            if(probability==1){
                int random_row=rand()%5;
                int random_column=rand()%9;
                if(board[random_row][random_column]=='.'){
                    add_sun(random_row,random_column,suns,board);
                }
            }
        }
    }
    else{
        srand(time(0));
        int probability=rand()%5+1;
        if(probability==1){
            int random_row=rand()%5;
            int random_column=rand()%9;
            if(board[random_row][random_column]=='.'){
                add_sun(random_row,random_column,suns,board);
            }
        }   
    }
}
bool find_near_zombie(int zombie_index, vector<Zombie*>& zombies){
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i]->y == zombies[zombie_index]->y && zombies[i]->x+ZOMBIE_SPEED == zombies[zombie_index]->x ){
            return true;
        }
    }
    return false;
}
void update_zombies_data(int row,int column ,int zombie_index, vector<Zombie*>& zombies,Time* current_time,vector<vector<char> >& board){
                if(board[row][column] != 'f' && board[row][column] != 'n' && board[row][column] != 'p' && (board[row][column-1] != 'z' || board[row][column] != 'z') &&  !find_near_zombie( zombie_index, zombies)){
                    
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
            if(zombies[i]->x >=80){
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
        if(suns[i]->is_fixed == true){
            int time=time_left(current_time,suns[i]->produce_time);
            if(time==5){
                if(suns[i]->is_related_to_sunflower==false){
                    update_board(board,suns[i]->x,suns[i]->y,'.');
                }
                suns.erase(suns.begin()+i);
            }
        }
        if(suns[i]->is_fixed == false && suns[i]->start_y == suns[i]->y){
            suns[i]->is_fixed = true;
        }
    }
}
bool loss_the_game(vector<Zombie*>& zombies){
    bool result=false;
    for (int i = 0; i < zombies.size(); ++i)
    {
        if(zombies[i]->x < 80){
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
void genarate_random_zombies(vector<Zombie*>& zombies,vector<vector<char> >& board,int& produced_zombie_number,bool heavy_attack){
    
        srand(time(0));
        int i_index=rand()%5;
        int j_index=8;
        if(heavy_attack){
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
        else{
            if(board[i_index][j_index]!='z'){
                add_zombie(i_index,zombies,board);
                cout<<i_index<<endl;
                produced_zombie_number++;
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
                if(number_of_zombies[wave_number] *2 > waves_length[wave_number]){
                    genarate_random_zombies(zombies,board,produced_zombie_number,true);
                }
                else{
                    genarate_random_zombies(zombies,board,produced_zombie_number,false);   
                }
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
int add_plant(int row,int column,char plant_char,string plant_name,string image_name,int toughness,vector<vector<char> >& board,vector<Plant*>& plants){
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
            Plant* new_plant=new Plant();
            new_plant->x=80-column;
            new_plant->y=300-row;
            board[i_index][j_index]=plant_char;
            new_plant->name=plant_name;
            new_plant->image_name=image_name;
            new_plant->lives=toughness;
            new_plant->is_zombie_alongside=false;
            new_plant->seen_zombie=false;
            time_t tim;
            time(&tim);
            current_time=localtime(&tim);
            new_plant->produce_time=new Time();
            (new_plant->produce_time)->second=current_time->tm_sec;
            (new_plant->produce_time)->minute=current_time->tm_min;
            (new_plant->produce_time)->hour=current_time->tm_hour;
            new_plant->collision_time=new Time();
            (new_plant->collision_time)->second=current_time->tm_sec;
            (new_plant->collision_time)->minute=current_time->tm_min;
            (new_plant->collision_time)->hour=current_time->tm_hour;
            plants.push_back(new_plant);
            return 0;
        }
        return -1;
}

int read_level(string level_name,int& waves,vector<int>& number_of_zombies,vector<int>& waves_length){
    ifstream myfile;
    cout<<level_name<<endl;
    myfile.open(level_name.c_str());
    if(!myfile.is_open()){
        cerr<<"I can not open level file!"<<endl;
        return -1;
    }
    string line;
    int line_counter=1;
    while (getline(myfile,line))
    {
        if(line_counter == 1){
            int temp_int;
            string temp;
            istringstream iss(line);
            while(getline(iss,temp,' ')){
                if(stringstream(temp)>>temp_int){
                    waves=temp_int;               
                }
            }
        }
        if(line_counter == 2){
            int temp_int;
            string temp;
            istringstream iss(line);
            while(getline(iss,temp,' ')){
                if(stringstream(temp)>>temp_int){
                    number_of_zombies.push_back(temp_int);               
                }
            }
        }
        if(line_counter == 3){
            int temp_int;
            string temp;
            istringstream iss(line);
            while(getline(iss,temp,' ')){
                if(stringstream(temp)>>temp_int){
                    waves_length.push_back(temp_int);               
                }
            }
        }
        line_counter++;
    }
    myfile.close();
    return 0;
}

string read_savedata(){
    string result;
    ifstream myfile;
    myfile.open("savedata.txt");
    if(!myfile.is_open()){
        result="0.level";
        cout<<result<<endl;
        return result;
    }
    else{
        int line_counter=1;
        string line;
        while (getline(myfile,line)){
            if(line_counter == 2){
                result=line;

            }
            line_counter++;
        }   
    }
    return result;
    cout<<result<<endl;
}
void save_level_data(string current_level,string status){
    string new_level;
    if(status == "win"){
        for (int i = 0; i < current_level.size(); ++i)
        {
            if(i == 0){
                new_level.push_back(current_level[i]+1);
                continue;
            }
            new_level.push_back(current_level[i]);
        }
    }
    else{
        for (int i = 0; i < current_level.size(); ++i)
        {
            new_level.push_back(current_level[i]);
        }
    }
    ofstream myfile;
    cout<<current_level<<endl;
    myfile.open("savedata.txt");
    myfile<<"user :user"<<endl;
    myfile<< new_level;
    myfile.close();
}
void show_suns_number(window& win,int sun_number){
    string result="SUNS :";
    ostringstream convert;
    convert<< sun_number;
    result=result + convert.str();
    win.show_text(result,540,0,BLACK);
}
void check_start(int x,int y,bool& start_game){
    if(x>300 && x<500 && y<600 && y>550){
        start_game=true;
    }
    cout<<x<<' '<<y<<endl;
}
int main(void){
    srand(0);
    int max_wave;
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
    
    vector<vector<char> > board(5);
    
    vector<int> waves_length;
    
    
    vector<Plant*> plants;
    vector<Sun*> suns;
    vector<Zombie*> zombies;
    vector<Pea*> peas;
    window win(WINDOW_WIDTH, WINDOW_HEIGHT,"PLANTS vs ZOMBIES");
    struct tm* current_time;
    int i = 0,j = 0;
    int x = 0;
    int sun_number=50;
    time_t time_m;
    int sgn = 1;
    int angle=0;
    bool start_game=false;
    clock_t t;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            board[i].push_back('.');
        }
    }
    cout<<read_savedata()<<endl;
    int counter=0;
    bool update_data=true;
    bool next_level=true;
    while(true){
        angle++;

        if(start_game){
        cout<<"main game"<<endl;    
        win.draw_bg(string("./Frontyard.png"),190,0);
        string level_ID;
        if(next_level == true){
            level_ID=string(read_savedata());
            if(read_level(level_ID,max_wave,number_of_zombies,waves_length)== -1){
                win.show_text("you see all levels!",270,300,WHITE);
                HANDLE({
                    QUIT({return 0;})    
                })
                win.update_screen();
                cerr<<level_ID<<endl;
                DELAY(15);
                continue;
            }
            next_level=false;
        }
        time_t tim2;
        struct tm* second_time;
        time(&tim2);
        second_time=localtime(&tim2);
        Time* correct_t=new Time();
        correct_t->second=second_time->tm_sec;
        correct_t->minute=second_time->tm_min;
        correct_t->hour=second_time->tm_hour;
        manage_level_waves(start_time, correct_t, zombies, board, number_of_zombies, waves_length,wave_number,max_wave,produced_zombie_number);
        generate_random_suns(correct_t,suns,board);
        zombies_and_plants(zombies,plants,correct_t);
        change_nuts_states(plants, correct_t,board);
        change_sunflowers_states(plants,suns,correct_t,board);
        change_peashooter_states(plants,correct_t,board);
        move_zombies(zombies,correct_t,board,counter);
        shoot_pea(plants,zombies,peas,correct_t,counter);
        change_zombies_state(zombies,board);
        change_suns_state(suns,correct_t,board);
        move_suns(suns);
        // for (int j = 0; j < 200; j++)
        //     win.draw_bmp(string("./image.bmp"), rand()%600, rand()%600, 30, 30);
        
        win.draw_rect(80-j,300-i,60,60,WHITE);
        draw_plants(win,plants);
        draw_suns(win,suns,angle);
        draw_zombies(win,zombies);
        draw_peas(win,peas,angle);
        show_suns_number(win,sun_number);
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
            KEY_PRESS(s,{
                //if(sun_number >=50){
                    if(add_plant( i, j,'f',"sunflower","./sunflower.png",5, board, plants)==0){
                        sun_number-=50;
                    }
                //}
                continue;
            })
            KEY_PRESS(w,{
                //if(sun_number >=50){
                    if(add_plant( i, j,'n',"nut","./nut1.png",72, board, plants)==0){
                        sun_number-=50;
                    }
                //}
                continue;
            })
            KEY_PRESS(p,{
                //if(sun_number >= 100){
                    if(add_plant( i, j,'p',"pea_shooter","./peashooter.png",5, board, plants)==0){
                        sun_number-=100;
                    }
                //}
                continue;
            })
            KEY_PRESS(d,{
                remove_plant(plants,80-j,300-i,sun_number);
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
            
            DELAY(40);
            win.draw_bg(string("./game_over.png"),0,0);
            level_ID=read_savedata();
            save_level_data(level_ID,"loss");
            win.update_screen();

        }
        if(win_the_game(wave_number,max_wave,zombies)){
            win.draw_bg(string("./Frontyard.png"),0,0);
            win.show_text("you won this level",270,300,BLACK);
            if(update_data == true){
                level_ID=read_savedata();
                save_level_data(level_ID,"win");
                update_data=false;
            }
            cerr<<level_ID<<endl;
            win.update_screen();
            
        }
        win.update_screen();
        DELAY(15);
    }
    else{
        win.draw_bg(string("./h.png"),0,0);
        HANDLE({
            QUIT({return 0;})
            LCLICK({
                check_start(mouse_x,mouse_y,start_game);
                continue;
            })
        })
        win.update_screen();
        DELAY(15);
    }
    }
}

