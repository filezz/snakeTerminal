#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h> //getting terminal size for screen
#include<termios.h>
#include"snake.h"


#define MAX_SNAKESIZE 100 
#define cursorXY(x,y) printf("\033[%d;%dH",(x),(y)) // moving cursor to a specific position 

int height,width;

void screenSize(int *height , int *width)
{
    struct winsize window; //col  = width , row = height
    ioctl(STDOUT_FILENO , TIOCGWINSZ,&window) ; 
    *height = window.ws_row ; 
    *width = window.ws_col;
}
void clean(int **grid,int height) //free everything
{
    for(int i = 0 ; i < height;i++)
        free(grid[i]);
    free(grid);   
}
void drawBorder(int **grid , int height,int width)
{ 
    for(int i = 0; i< width; i++) 
        grid[0][i] = '-' , grid[height-1][i] = '-';
    
    for(int i = 0; i< height; i++) 
        grid[i][0] = '|' , grid[i][width-1] = '|';
    
}
void printGrid(int **grid,int height, int width)
{
    for(int i = 0; i<height; i++,printf("\n"))
        for(int j = 0; j<width; j++)
            printf("%c" , grid[i][j]) ;
            
}
void cleanscreen()
{
    printf("\033[H\033[J");
}
void clearFruit(int **grid , int x, int y)
{
    if( x != -1 && y != -1)
        grid[y][x]=' '; 

}

void fruitposition(struct fruit *fruit,int height,int width, int **grid)
{
    
        clearFruit(grid,fruit->lastFruit.x,fruit->lastFruit.y);
        do{
            fruit->position.x = 1 + rand() % (width - 2); 
            fruit->position.y = 1 + rand() % (height - 2) ; 
        }while(grid[fruit->position.y][fruit->position.x] != ' ') ; 
    

    grid[fruit->position.y][fruit->position.x] = '@' ;
    fruit->active = 1;

    fruit->lastFruit.x = fruit->position.x; 
    fruit->lastFruit.y = fruit->position.y; 

}

struct termios original_settings , new_settings; 
void setTerminal() // prepre terminal for continous input 
{   if(height< 5 || width < 5 )
        fprintf(stderr, "terminal too small") ;

    tcgetattr(STDIN_FILENO , &original_settings) ; // copying the terminal settings to the original_settings variable
    new_settings = original_settings; 
    new_settings.c_lflag &= ~(ICANON | ECHO ); //disable canonical mode and echo & bit operation for AND , attribuing to new_settings the flag of NOT ICANON AND NOT ECHO ( ~ bitwise op for NOT )
    new_settings.c_cc[VMIN] = 0; //min number of chars to read
    new_settings.c_cc[VTIME] = 1 ;// timeout for the read funtioc
    tcsetattr(STDIN_FILENO , TCSANOW , &new_settings); //TCASNOW is defined as changing the file descriptor ( our case terminal ) in this moment 
}
void restoreToDef()
{
    tcsetattr(STDIN_FILENO , TCSANOW ,  &original_settings) ; 
}

void movement(struct player *snake, int height, int width) {
    snake->lastpos.x = snake->body.x;
    snake->lastpos.y = snake->body.y;

    switch (snake->direction) {
        case UP: 
            snake->body.y = (snake->body.y > 1) ? snake->body.y - 1 : height - 2;
            break;
        case DOWN: 
            snake->body.y = (snake->body.y < height - 2) ? snake->body.y + 1 : 1;
            break;
        case LEFT: 
            snake->body.x = (snake->body.x > 1) ? snake->body.x - 1 : width - 2;
            break;
        case RIGHT: 
            snake->body.x = (snake->body.x < width - 2) ? snake->body.x + 1 : 1;
            break;
    }
}

void setpos(struct player *snake,int **grid)
{
    grid[snake->body.y][snake->body.x] = '*';

    snake->lastpos.x = snake->body.x; 
    snake->lastpos.y = snake->body.y;    
}

void cleanTrail(struct coordonates lastSnake , int **grid)
{
    grid[lastSnake.y][lastSnake.x] = ' ';
}

int main()
{
    fruit fruit;
   
    fruit.active = 0;
    fruit.lastFruit.x = -1;
    fruit.lastFruit.y = -1;

    screenSize(&height,&width); //grid[height,width] height = Y axis , width = X axis
    //allocate memory for height pointers ( each pointers points to an array)
    int **grid = malloc(sizeof(int *) * height); //array of pointers to an array, reason to malloc with (int *) 
   
    if(!grid)
        perror("allocation error") , exit(1);

    for(int i = 0; i<height; i++){
        grid[i] = malloc(sizeof(int) * width); 
        if(!grid[i])
            perror("allocation in for error"),exit(1);
    }
    //printf("allocation succesful \n"); 

    for(int i = 0; i<height; i++)
        for(int j = 0; j<width; j++)
            grid[i][j] = ' ';

   
    player *snake = malloc(sizeof(player));

    if(!snake)
        perror("Bad allocation with snake") , exit(1);
    
    snake->body.x = height/2; 
    snake->body.y = width/2 ;
    snake->direction = UP;
    snake->lastpos.x = -1; 
    snake->lastpos.y = -1;

    setTerminal(); 
    char keyboard = '\0'; 
    printf("press any key to play , WASD-control the snake, Q - exit the game\n") ; 
    while(1)
    {   
        drawBorder(grid,height,width);
        
        if(fruit.active == 0){
            fruitposition(&fruit,height,width,grid);
        }
        
        read(STDIN_FILENO , &keyboard , 1);  
        if(keyboard == 'q' || keyboard == 'Q'){
            restoreToDef(); //restore default setti ngs;
            break;
        }
        else if (keyboard == 'w' || keyboard == 'a' || keyboard == 's' || keyboard == 'd')
            snake->direction = (keyboard == 'w') ? UP : 
                                (keyboard =='s') ? DOWN :
                                (keyboard =='a') ? LEFT : RIGHT;
            
        movement(snake,height,width);
        setpos(snake,grid);
        //cleanscreen();
        printGrid(grid,height,width);
        cleanTrail(snake->lastpos,grid);
        usleep(200000);
    }
    restoreToDef() ; // restoring default settings 
    clean(grid,height);   
    return 0;   
}