#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h> //getting terminal size for screen
#include<termios.h>

typedef enum direction
{
    UP, DOWN , LEFT , RIGHT 
} direction;

#define MAX_SNAKESIZE 100 
#define cursorXY(x,y) printf("\033[%d;%dH",(x),(y)) // moving cursor to a specific position 

typedef struct coordonates
{
    int x,y ;
}coordonates;

typedef struct player
{
   coordonates body; 
   int size;
   direction direction ;
}player;

typedef struct fruit
{
    int value; 
    coordonates position;
}fruit;

coordonates lastFruit = {-1,-1}; // for last position of a fruit 
int isFruit = 0;

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
void clearFruit(int **grid , int x, int y)
{
    if( x != -1 && y != -1)
        grid[y][x]=' '; 

}

void fruitposition(struct fruit *fruit,int height,int width, int **grid)
{
    
        clearFruit(grid,lastFruit.x,lastFruit.y);
        do{
            fruit->position.x = 1 + rand() % (width - 2); 
            fruit->position.y = 1 + rand() % (height - 2) ; 
        }while(grid[fruit->position.y][fruit->position.x] != ' ') ; 
    

    grid[fruit->position.y][fruit->position.x] = '@' ;
    isFruit = 1;

    lastFruit.x = fruit->position.x; 
    lastFruit.y = fruit->position.y; 

}

struct termios original_settings , new_settings; 
void setTerminal() // prepre terminal for continous input 
{
    tcgetattr(STDIN_FILENO , &original_settings) ; // copying the terminal settings to the original_settings variable
    new_settings = original_settings; 
    new_settings.c_lflag &= ~(ICANON | ECHO ); //disable canonical mode and echo & bit operation for AND , attribuing to new_settings the flag of NOT ICANON AND NOT ECHO ( ~ bitwise op for NOT )
    tcsetattr(STDIN_FILENO , TCSANOW , &new_settings); //TCASNOW is defined as changing the file descriptor ( our case terminal ) in this moment 
}
void restoreToDef()
{
    tcsetattr(STDIN_FILENO , TCSANOW ,  &original_settings) ; 
}
void movement(struct player *snake,int height,int width )
{
    switch(snake->direction){
        case UP : 
           if(snake->body.x < width && snake->body.x > 0)
                snake->body.x += 1;
            else 
                perror("Array out of boundries"),exit(1);
            break;

        case DOWN : 
            if(snake->body.x < width && snake->body.x > 0)
                snake->body.x -= 1 ;
            else 
                perror("Array out of boundries"),exit(1);
            break; 
    }
}
coordonates lastSnake = {0,0};

void setpos(struct player *snake,int **grid)
{
    grid[snake->body.x][snake->body.y] = '*';

    lastSnake.x = snake->body.x; 
    lastSnake.y = snake->body.y;    
}
void cleanTrail(struct coordonates lastSnake, int **grid)
{
    grid[lastSnake.x][lastSnake.y] = ' ';
}
int main()
{
    fruit fruit;
    int height , width ; 
    int isFruit = 0;

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
    setTerminal(); 
    char keyboard; 
    printf("press any key to play , WASD-control the snake, Q - exit the game\n") ; 
    while(1)
    {
        
        //system("clear"); //system('cls') for windows 
        drawBorder(grid,height,width);
        
        if(isFruit == 0){
            fruitposition(&fruit,height,width,grid);
            isFruit = 1 ;
        }

        keyboard = getchar(); 
        if(keyboard == 'q' || keyboard == 'Q'){
            restoreToDef(); //restore default setti ngs;
            break;
        }
       //grid[snake->body.x][snake->body.y] = '*';
       if(keyboard == 'w')
            snake->direction = UP;
       
        movement(snake,height,width);
        setpos(snake,grid);
        printGrid(grid,height,width);
        cleanTrail(lastSnake,grid);
        sleep(1);
    }
    restoreToDef() ; // restoring default settings 
    clean(grid,height);   
    return 0;   
}