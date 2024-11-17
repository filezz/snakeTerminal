#ifndef SNAKE_H
#define SNAKE_H 

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
   coordonates body; 
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