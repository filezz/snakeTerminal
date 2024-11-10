#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h> //getting terminal size for screen

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
int main()
{
    int height , width ; 
    screenSize(&height,&width); //grid[height,width]
    printf("height :%d \n width : %d \n" ,height,width);
    //allocate memory for height pointers ( each pointers points to an array)
    int **grid = malloc(sizeof(int *) * height); //array of pointers to an array, reason to malloc with (int *) 
   
    if(!grid)
        perror("allocation error") , exit(1);

    for(int i = 0; i<height; i++){
        grid[i] = malloc(sizeof(int) * width); 
        if(!grid[i])
            perror("allocation in for error"),exit(1);
    }
    printf("allocation succesful \n"); 


    for(int i = 0; i<height; i++)
        for(int j = 0; j<width; j++)
            grid[i][j] = ' ';

    drawBorder(grid,height,width);
    printGrid(grid,height,width); 

    //cleanup
    clean(grid,height);   
    return 0;   
}