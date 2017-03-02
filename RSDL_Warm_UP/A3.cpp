#include <iostream>

using namespace std;
typedef struct zombie Zombie;
typedef struct sun Sun;
typedef struct plant Plant;

struct zombie
{
	int lives;
	int row;
	int column;
};
struct sun
{
	int row;
	int column;
};
struct plant
{
	string name;
	int lives;
	int column;
	int row;
};
int main(){


	return 0;
}