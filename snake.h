#ifndef SNAKE_H
#define SNAKE_H 
#define MAX_SNAKE 100 
#include<stdio.h>

typedef enum direction
{
    UP, DOWN , LEFT , RIGHT 
} direction;

typedef struct coordonates
{
    int x,y ;
}coordonates;

typedef struct player
{   
   coordonates body[100]; 
   coordonates lastpos ;
   int size;
   direction direction ;

}player;

typedef struct fruit
{
    int value; 
    coordonates position;
    coordonates lastFruit;
    int active;
}fruit;

#endif