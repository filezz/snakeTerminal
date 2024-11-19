#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h> //getting terminal size for screen
#include<termios.h>
#include"snake.h"

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

    for(int i = snake->size - 1 ; i>0 ; i--)
        snake->body[i] = snake->body[i-1]; 
    
    switch(snake->direction){
        case UP :snake->body[0].y -= 1;break;
        case DOWN : snake->body[0].y +=1 ; break;
        case LEFT: snake->body[0].x -=1 ; break;
        case RIGHT: snake->body[0].x += 1 ; break;
    }
    
}


void growsnake(struct player *snake)
{
    int size = snake->size ;
    if(size < MAX_SNAKE) {
        coordonates new_seg;
        switch(snake->direction){
            case UP : 
                new_seg.x = snake->body[size-1].x; 
                new_seg.y = snake->body[size-1].y + 1 ;  
            break;

            case DOWN: 
                new_seg.x = snake->body[size-1].x ;
                new_seg.y = snake->body[size-1].y - 1; 
            break ;

            case LEFT : 
                new_seg.x = snake->body[size-1].x + 1 ;
                new_seg.y = snake->body[size-1].y;
            break;
            
            case RIGHT : 
                new_seg.x = snake->body[size-1].x - 1;
                new_seg.y = snake->body[size-1].y;
            break;
                
        }
    snake->body[size] = new_seg ; 
    snake->size ++;

    }
}
void checkCol(struct player *snake, struct fruit *fruit , int **grid,int height,int width)
{
    if(snake->body[0].x == fruit->position.x && snake->body[0].y == fruit->position.y )
        growsnake(snake), fruitposition(fruit,height,width,grid);
    
    for(int i=1;i<snake->size;i++)
        if(snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            printf("You lost .. try again "), restoreToDef(),exit(0);

    if(snake->body[0].x <= 0 || snake->body[0].x >= width -1  || snake->body[0].y <= 0 || snake->body[0].y >= height-1)
        restoreToDef() ,printf("You lost...\n"), exit(0);
}
void printSnake(struct player *snake, int **grid) {
    for (int i = 0; i < snake->size; i++)
        if (snake->body[i].y >= 0 && snake->body[i].y < height && snake->body[i].x >= 0 && snake->body[i].x < width)
            grid[snake->body[i].y][snake->body[i].x] = '*';
}

int main()
{
    fruit fruit;
   
    fruit.active = 0;
    fruit.lastFruit.x = -1;
    fruit.lastFruit.y = -1;
    
    screenSize(&height,&width); //grid[height,width] height = Y axis , width = X axis
    //allocate memory for height pointers ( each pointers points to an array)
    int **grid = malloc(sizeof(int *)*height); //array of pointers to an array, reason to malloc with (int *) 

    if(!grid)
        perror("allocation error") , exit(1);

    for(int i = 0; i<height; i++){
        grid[i] = malloc(sizeof(int) * width); 
        if(!grid[i])
            perror("allocation in for error"),exit(1);
    }

    for(int i = 0; i<height; i++)
        for(int j = 0; j<width; j++)
            grid[i][j] = ' ';

   
    player *snake = malloc(sizeof(player));

    if(!snake)
        perror("Bad allocation with snake") , exit(1);
    
    snake->body[0].x = width/2; 
    snake->body[0].y = height/2 ;
    snake->direction = UP;
    snake->lastpos.x = -1; 
    snake->lastpos.y = -1;
    snake->size = 1 ;

    setTerminal(); 
    char keyboard = '\0'; 
    printf("press any key to play , WASD-control the snake, Q - exit the game\n") ; 

    while(1)
    {   

        for(int i = 0;i<height;i++)
            for(int j = 0; j< width;j++){
               if(grid[i][j] == '@')
                    j ++; 
                else 
                grid[i][j] = ' ';
            }
        drawBorder(grid,height,width);
      
        if(fruit.active == 0){
            fruitposition(&fruit,height,width,grid);
        }
        
        read(STDIN_FILENO , &keyboard , 1);  
        if(keyboard == 'q' || keyboard == 'Q'){
            restoreToDef(); //restore default settings;
            break;
        }
        else if (keyboard == 'w' && snake->direction != DOWN)
            snake->direction = UP;
        else if(keyboard == 's' && snake->direction != UP)
            snake->direction = DOWN;
        else if(keyboard == 'a' && snake->direction != RIGHT)
            snake->direction = LEFT;
        else if(keyboard == 'd' && snake->direction != LEFT)
            snake->direction = RIGHT; 
           
           //change in normal if , if snake  dir up and keyboard is S do not change snake dir 
            
        
        checkCol(snake,&fruit,grid,height,width);
        movement(snake,height,width);

        printSnake(snake,grid);
        printGrid(grid,height,width);
      
        usleep(20000);
    }
    restoreToDef() ; // restoring default settings 
    free(snake);
    clean(grid,height);   
    return 0;   
}