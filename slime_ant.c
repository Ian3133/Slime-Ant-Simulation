#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <math.h>

#define WIDTH 600 // Width of the window
#define HEIGHT 500 // Height of the window
#define NUM_AGENTS 6000 // Number of agents
#define DELAY 0 // Delay between updates in milliseconds
float WOBBLE_P =  98.0; // percent larger = less random movemnt   // i liked .9

int bias = 1; 
#define AMP_FACTOR 1
float DARKEN_FACTOR =  0.2;  // LOWER  is equal to faster darkening
#define NUM2 1
#define SEN_MUL 2
#define TAIL_NUM 6
//#define FADE 2 
   


// next steps get a box for the sensor then an average of what is in the box is the new senosor



// Agent structure
typedef struct {
    float x; // x-coordinate
    float y; // y-coordinate
  
    int direction; // direction (0-7) think box with 8 sides
  int tail[TAIL_NUM][2][2];
  // int past_tail[2][2];
} Agent;

Agent agents[NUM_AGENTS];

float pixels[HEIGHT][WIDTH][3]; // array of all pixels
int sensors[NUM_AGENTS][7][2] ; // last one is positiong x and y

//int tail[NUM_AGENTS][TAIL_NUM][2] = {0}; // agent, amout of steps back , x and y pos

void initialize_pixels() {
    // Set all pixels to black
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
	  // pixels[y][x][0] = 0.0; // Red component
	  //pixels[y][x][1] = 0.0; // Green component
	     pixels[y][x][2] = 0.0; // Blue component
        }
    }
}


// Function to initialize agents with random positions and directions
void initialize_agents() {
    for (int i = 0; i < NUM_AGENTS; i++) {
       agents[i].x = (float)(rand() % WIDTH);
      agents[i].y = (float)(rand() % HEIGHT);
      agents[i].direction = rand() % 8;
      // agents[i].x = WIDTH /2;
      //agents[i].y = HEIGHT /2;
      //agents[i].direction = 0;

      //      for(int z = 0; z < TAIL_NUM; z++){/
      //	for(int k = 0; k < 2; k ++){
      //	agents[i].tail[z][k][0] = 0;
      //  agents[i].tail[z][k][1] = 0;
      //	}
      // }
      
    }
}


// used for wobble so moves at random
void shift_d_r(int i){
  if (((double)rand() / RAND_MAX) > .5){
      //left
      agents[i].direction += 1;
  if(agents[i].direction > 7){
    agents[i].direction = 0;
  }
  }
  else{   // right so it loops back around for the if statemnets in both of theses for l and r
    agents[i].direction -= 1;
    if(agents[i].direction < 0){
      agents[i].direction = 7;
    }
  }}



void shift_right(int i){
    agents[i].direction -= 1;
    if(agents[i].direction < 0){
      agents[i].direction = 7;
    }
}

void shift_left(int i){
     agents[i].direction += 1;
  if(agents[i].direction > 7){
    agents[i].direction = 0;
  }
}

void pheromones(int i){
     float p0;
      float p1;
       float p2;
    float sum0 =  0;
       sum0 += pixels[sensors[i][0][1]][sensors[i][0][0]][2];  // only detect blue //could change to green =  1
    sum0 += pixels[sensors[i][1][1]][sensors[i][1][0]][2];
    sum0 += pixels[sensors[i][2][1]][sensors[i][2][0]][2];
    sum0 += pixels[sensors[i][0][1]][sensors[i][0][0]][1];  // only detect blue //could change to green =  1
    sum0 += pixels[sensors[i][1][1]][sensors[i][1][0]][1];
    sum0 += pixels[sensors[i][2][1]][sensors[i][2][0]][1];
	 //  sum0 += pixels[sensors[i][0][1]][sensors[i][0][0]][1];
	 // sum0 += pixels[sensors[i][0][1]][sensors[i][0][0]][2];
    float sum1 =  0;
    sum1 += pixels[sensors[i][1][1]][sensors[i][1][1]][2];
     sum1 += pixels[sensors[i][3][1]][sensors[i][1][3]][2];
    	  sum1 += pixels[sensors[i][4][1]][sensors[i][1][4]][2];
     sum1 += pixels[sensors[i][1][1]][sensors[i][1][1]][1];
     sum1 += pixels[sensors[i][3][1]][sensors[i][1][3]][1];
	  sum1 += pixels[sensors[i][4][1]][sensors[i][1][4]][1];
	 //  sum1 += pixels[sensors[i][1][1]][sensors[i][1][0]][1];
	 // sum1 += pixels[sensors[i][1][1]][sensors[i][1][0]][2];
    float sum2 =  0;
    sum2 += pixels[sensors[i][2][1]][sensors[i][2][0]][2];
    	   sum2 += pixels[sensors[i][5][1]][sensors[i][5][0]][2];
    	   sum2 += pixels[sensors[i][6][1]][sensors[i][6][0]][2];

	       sum2 += pixels[sensors[i][2][1]][sensors[i][2][0]][1];
	   sum2 += pixels[sensors[i][5][1]][sensors[i][5][0]][1];
	   sum2 += pixels[sensors[i][6][1]][sensors[i][6][0]][1];
	 // sum2 += pixels[sensors[i][2][1]][sensors[i][2][0]][1];
	 //sum2 += pixels[sensors[i][2][1]][sensors[i][2][0]][2];

    float total = sum0 + sum1 + sum2; 
    if (total == 0.0){
      p0 = 0;
      p1 = 0;
      p2 = 0;
    }else{
    p0 = (sum0 / total);
    p1 = (sum1 / total);
    p2 = (sum2 / total);
    }    
    float random_num = (float)rand() / RAND_MAX;


    if(bias == 1){
      if (p2>p0 && p2>=(p1/3.0)) shift_right(i);
       if (p2<p1 && p1>p0) shift_left(i);
      } // play around this this next
    else{ if (p2>p0 && p2>=p1) shift_right(i);  
       if (p2<p1 && p1>p0) shift_left(i);
    }
}



void move_agents() {
    for (int i = 0; i < NUM_AGENTS; i++) {
        pheromones(i);

	for (int j = 0; j <( TAIL_NUM -1); j ++){
       	agents[i].tail[j][0][0] = agents[i].tail[j+1][0][0];
	agents[i].tail[j][0][1] = agents[i].tail[j+1][0][1];
		
      	agents[i].tail[j][1][0] = agents[i].tail[j+1][1][0];
       	agents[i].tail[j][1][1] = agents[i].tail[j+1][1][1];

	}

	
        switch (agents[i].direction) {
            case 0: // Move +2 y
	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y + 1;
	      
	      
                agents[i].y += 2;
                if (agents[i].y >= HEIGHT) {
                    agents[i].y = HEIGHT - 2; // Bounce off the bottom edge
                    agents[i].direction = 4; // Change direction to move upwards
                }

                sensors[i][0][0] = agents[i].x;                      
                sensors[i][0][1] = agents[i].y + (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][1][1] = agents[i].y + (2 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][2][1] = agents[i].y + (2 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][3][1] = agents[i].y + (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][4][1] = agents[i].y + (1 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][5][1] = agents[i].y + (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][6][1] = agents[i].y + (1 *SEN_MUL);

                break;
            case 1: // Move +1x +1y

	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y + 1;   

	      
                agents[i].x += 1;
                agents[i].y += 1;
                if (agents[i].x >= WIDTH) {
                    agents[i].x = WIDTH - 1; // Bounce off the right e
                    agents[i].direction = 7; // Change direction to move left and down
		  
                }
                if (agents[i].y >= HEIGHT) {
                    agents[i].y = HEIGHT - 1; // Bounce off the bottom edge
                    agents[i].direction = 3; // Change direction to move upwards
		 
                }

		sensors[i][0][0] = agents[i].x + (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y + (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][1][1] = agents[i].y + (2 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][2][1] = agents[i].y + (3 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][3][1] = agents[i].y + (1 *SEN_MUL);

		sensors[i][4][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][4][1] = agents[i].y;

		
		sensors[i][5][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][5][1] = agents[i].y + (3 *SEN_MUL);


                sensors[i][6][0] = agents[i].x;
                sensors[i][6][1] = agents[i].y + (3 *SEN_MUL);

                break;
            case 2: // Move +2x


	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x + 1;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y;
	      
                agents[i].x += 2;
                if (agents[i].x >= WIDTH) {
                    agents[i].x = WIDTH - 2; // Bounce off the right edge
                    agents[i].direction = 6; // Change direction to move left
		  
                }

                sensors[i][0][0] = agents[i].x + (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y;
		
		sensors[i][1][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][1][1] = agents[i].y - (1 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][2][1] = agents[i].y + (1 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][3][1] = agents[i].y - (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][4][1] = agents[i].y - (2 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][5][1] = agents[i].y + (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][6][1] = agents[i].y + (2 *SEN_MUL);

		

                break;
            case 3: // Move +1x -1y
	      
      	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x + 1;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y;


	      
                agents[i].x += 1;
                agents[i].y -= 1;
                if (agents[i].x >= WIDTH) {
                    agents[i].x = WIDTH - 1; // Bounce off the right edge
                    agents[i].direction = 5; // Change direction to move left and up
		    
		    
                }
                if (agents[i].y < 0) {
                    agents[i].y = 1; // Bounce off the top edge
                    agents[i].direction = 1; // Change direction to move downwards
		  
                }

                sensors[i][0][0] = agents[i].x + (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y - (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][1][1] = agents[i].y - (3 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][2][1] = agents[i].y - (2 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][3][1] = agents[i].y - (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x;
                sensors[i][4][1] = agents[i].y - (2 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][5][1] = agents[i].y - (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x + (3 *SEN_MUL);
                sensors[i][6][1] = agents[i].y - (1 *SEN_MUL);


		

                break;
            case 4: // Move -2y

	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y - 1;

	      
                agents[i].y -= 2;
                if (agents[i].y < 0) {
                    agents[i].y = 2; // Bounce off the top edge
                    agents[i].direction = 0; // Change direction to move downwards
		   
                }

                sensors[i][0][0] = agents[i].x;                      
                sensors[i][0][1] = agents[i].y - (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][1][1] = agents[i].y - (2 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][2][1] = agents[i].y - (2 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][3][1] = agents[i].y - (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][4][1] = agents[i].y - (1 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][5][1] = agents[i].y - (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x + (2 *SEN_MUL);
                sensors[i][6][1] = agents[i].y - (1 *SEN_MUL);

		

                break;
            case 5: // Move -x -y

	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x - 1;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y;
	      
                agents[i].x -= 1;
                agents[i].y -= 1;
                if (agents[i].x < 0) {
                    agents[i].x = 1; // Bounce off the left edge
                    agents[i].direction = 3; // Change direction to move right and up
		   
                }
                if (agents[i].y < 0) {
                    agents[i].y = 1; // Bounce off the top edge
                    agents[i].direction = 7; // Change direction to move downwards
		    break;
                }

		sensors[i][0][0] = agents[i].x - (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y - (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][1][1] = agents[i].y - (2 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][2][1] = agents[i].y - (3 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][3][1] = agents[i].y - (1 *SEN_MUL);

		sensors[i][4][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][4][1] = agents[i].y;

		
		sensors[i][5][0] = agents[i].x + (1 *SEN_MUL);
                sensors[i][5][1] = agents[i].y - (3 *SEN_MUL);


                sensors[i][6][0] = agents[i].x;
                sensors[i][6][1] = agents[i].y - (3 *SEN_MUL);

		

                break;
            case 6: // move - 2x
	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x -1;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y;

	      
                agents[i].x -= 2;
                if (agents[i].x < 0) {
                    agents[i].x = 2; // Bounce off the left edge
                    agents[i].direction = 2; // Change direction to move right
		    break;
                }

                sensors[i][0][0] = agents[i].x - (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y;
		
		sensors[i][1][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][1][1] = agents[i].y + (1 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][2][1] = agents[i].y - (1 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][3][1] = agents[i].y + (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][4][1] = agents[i].y + (2 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][5][1] = agents[i].y - (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][6][1] = agents[i].y - (2 *SEN_MUL);

                break;
            case 7: // move +y  -x

	      agents[i].tail[TAIL_NUM - 1][0][0] = agents[i].x;
	      agents[i].tail[TAIL_NUM - 1][0][1] = agents[i].y;

	      agents[i].tail[TAIL_NUM - 1][1][0] = agents[i].x -1;
	      agents[i].tail[TAIL_NUM - 1][1][1] = agents[i].y;

	      
                agents[i].x -= 1;
                agents[i].y += 1;
                if (agents[i].x < 0) {
                    agents[i].x = 1; // Bounce off the left edge
                    agents[i].direction = 1; // Change direction to move right and down
		  
                }
                if (agents[i].y >= HEIGHT) {
                    agents[i].y = HEIGHT - 1; // Bounce off the bottom edge
                    agents[i].direction = 5; // Change direction to move left and up

		   
                }

		sensors[i][0][0] = agents[i].x - (3 *SEN_MUL);                      
                sensors[i][0][1] = agents[i].y + (3 *SEN_MUL);
		
		sensors[i][1][0] = agents[i].x - (2 *SEN_MUL);
                sensors[i][1][1] = agents[i].y + (3 *SEN_MUL);
		
		sensors[i][2][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][2][1] = agents[i].y + (2 *SEN_MUL);

		

                sensors[i][3][0] = agents[i].x - (1 *SEN_MUL);
                sensors[i][3][1] = agents[i].y + (2 *SEN_MUL);

		sensors[i][4][0] = agents[i].x;
                sensors[i][4][1] = agents[i].y + (2 *SEN_MUL);

		

		sensors[i][5][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][5][1] = agents[i].y + (2 *SEN_MUL);


                sensors[i][6][0] = agents[i].x - (3 *SEN_MUL);
                sensors[i][6][1] = agents[i].y + (1 *SEN_MUL);


                break;
        }

   	        //wrap around for the senors for the agents  // need to change the wrap around
		for (int k = 0; k < 7; k ++){
		  if (sensors[i][k][0] >= WIDTH) sensors[i][k][0] -= WIDTH;
		  if (sensors[i][k][1] >= HEIGHT) sensors[i][k][1] -= HEIGHT;

		   if (sensors[i][k][0] < 0) sensors[i][k][0] += WIDTH;
		  if (sensors[i][k][1] < 0) sensors[i][k][1] += HEIGHT;

		}

		if(agents[i].tail[TAIL_NUM - 1][1][0] >= WIDTH) agents[i].tail[TAIL_NUM - 1][1][0] -=  1;
		if(agents[i].tail[TAIL_NUM - 1][1][0] < 0) agents[i].tail[TAIL_NUM - 1][1][0] += 1; 

		if(agents[i].tail[TAIL_NUM - 1][1][1] >= HEIGHT) agents[i].tail[TAIL_NUM - 1][1][1] -= 1;
		if(agents[i].tail[TAIL_NUM - 1][1][1] < 0) agents[i].tail[TAIL_NUM - 1][1][1] += 1; 
		

		
	  if (((double)rand()/RAND_MAX) > WOBBLE_P){
		shift_d_r(i);
	      }
    }
}




// Function to update pixel colors based on neighboring pixels


void update_pixels() {
    // Temporary array to store updated pixel colors
    float updated_pixels[HEIGHT][WIDTH][3] = {0};

    // Loop through all pixels
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Sum of color components of neighboring pixels and itself
	  float sum_b = 0.0, sum_g = 0.0; //sum_r = 0.0, sum_g = 0.0, sum_b = 0.0;
	     int count = 0;

            // Loop through neighboring pixels
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    // Skip out-of-bounds pixels
                    if (x + dx < 0 || x + dx >= WIDTH || y + dy < 0 || y + dy >= HEIGHT) {
                        continue;
                    }

                    // Accumulate color components
                    //sum_r += pixels[y + dy][x + dx][0];
                    sum_g += pixels[y + dy][x + dx][1];
                    sum_b += pixels[y + dy][x + dx][2];
                    count++;
                }
            }
	     
            // Average color components of neighboring pixels and itself mult by AMP_factor

	    
	    //   updated_pixels[y][x][0] = (pixels[y][x][0]*NUM2 + sum_r) / (count + 1) * AMP_FACTOR;
	    updated_pixels[y][x][1] = (pixels[y][x][1]*NUM2 + sum_g) / (count + 1) * AMP_FACTOR;  // play here for multiple colors?
	     updated_pixels[y][x][2] = (pixels[y][x][2]*NUM2 + sum_b) / (count + 1) * AMP_FACTOR/2;
        }
    }

     for (int i = 0; i < NUM_AGENTS; i++) {



       for (int j = 0; j < TAIL_NUM; j ++){
         for (int tail = 0; tail < 2; tail++){
	   float  x = (pixels[agents[i].tail[j][tail][1]][agents[i].tail[j][tail][0]][0] + (1.0 -((float)j+1)/TAIL_NUM+1));
	   //float y = (pixels[agents[i].tail[j][tail][1]][agents[i].tail[j][tail][0]][0] + (1.0 -((float)j+1)/TAIL_NUM+1))
	   if (x > 1.0) x = 1.0;
	   
	   //	   updated_pixels[agents[i].tail[j][tail][1]][agents[i].tail[j][tail][0]][0]  = x;
	   updated_pixels[agents[i].tail[j][tail][1]][agents[i].tail[j][tail][0]][1]  = x;
	   updated_pixels[agents[i].tail[j][tail][1]][agents[i].tail[j][tail][0]][2]  = x; 

											 
       }
       
       }
       
       ///// for (int tail = 0; tail < 2; tail++){
       // updated_pixels[agents[i].tail[tail][1]][agents[i].tail[tail][0]][0]  = 1.0;
       // updated_pixels[agents[i].tail[tail][1]][agents[i].tail[tail][0]][1]  = 1.0;
       // updated_pixels[agents[i].tail[tail][1]][agents[i].tail[tail][0]][2]  = 1.0;   

       //}
       
     int x = agents[i].x;
     int y = agents[i].y;

     //       updated_pixels[y][x][0] = 1.0; // Red component
     	     updated_pixels[y][x][1] = 0.65; // Green component
             updated_pixels[y][x][2] = 1.0; // Blue component

	     }

    
    for (int i = 0; i < NUM_AGENTS; i++) {
     int x = agents[i].x;
     int y = agents[i].y;

     //      updated_pixels[y][x][0] = 1.0; // Red component
     	     updated_pixels[y][x][1] = 0.65; // Green component
             updated_pixels[y][x][2] = 1.0; // Blue component


	     }
	  

    // Copy updated pixel colors to the original array
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
           for (int c = 1; c < 3; c++) {
                pixels[y][x][c] = updated_pixels[y][x][c];
		 }
        }
    }
}


void darken_pixels() {
    // Loop through all pixels
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Darken each color component by a factor
	   for (int c = 1; c < 3; c++) {
                pixels[y][x][c] *= DARKEN_FACTOR;
		 }
        }
    }
}





// Function to display agents on the window
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
   // Draw pixels
    glBegin(GL_POINTS);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            glColor3fv(pixels[y][x]); // Set pixel color
            glVertex2f(x, y);
        }
    }
    glColor3f(0.0, 0.65, 1.0); // color of agent

    // Draw agents
    // glBegin(GL_POINTS);
    for (int i = 0; i < NUM_AGENTS; i++) {
        glVertex2f(agents[i].x, agents[i].y);
	// glVertex2f(agents[i].x+1, agents[i].y);
	// glVertex2f(agents[i].x, agents[i].y+1);
	//  glVertex2f(agents[i].x+1, agents[i].y+1);
	

	for (int l; l < TAIL_NUM; l++){
	for(int k = 0; k < 2; k ++){
	  
	glVertex2f(agents[i].tail[l][k][0], agents[i].tail[l][k][1]);
	glVertex2f(agents[i].tail[l][k][0]+1, agents[i].tail[l][k][1]);
	glVertex2f(agents[i].tail[l][k][0], agents[i].tail[l][k][1]+1);
      	glVertex2f(agents[i].tail[l][k][0]+1, agents[i].tail[l][k][1]+1);
          
	
	}
}
    }

    // draw sensors
    //   for (int i = 0; i < NUM_AGENTS; i++) {
    //    glVertex2f(sensors[i][0][0],sensors[i][0][1]);
    //	 glVertex2f(sensors[i][1][0],sensors[i][1][1]);
    //Vertex2f(sensors[i][2][0],sensors[i][2][1]);
	
    // }
    
    glEnd();


    
     glutSwapBuffers();
     //  glFlush();
}

// Function to update agents and redraw the window
void update(int value) {
    update_pixels();


    darken_pixels();
    
    move_agents();
    glutPostRedisplay(); // Redraw the window
    glutTimerFunc(DELAY, update, 0); // Call update function after delay
}

int main(int argc, char **argv) {
   printf("\n"); printf("\n");
  printf("*DISCLAIMER* This program may needs to be run a few times.\nBecause of it's size you might see'Segmentation fault (core dumped)' try it 1/2 more times\n");
    printf("\n");

  int dif  = 0; 
  while(1){
  printf("Input a diffusion value:\nRange (10 - 98)\n");
  
  scanf("%d", &dif);
  DARKEN_FACTOR = dif/100.0;
  if (DARKEN_FACTOR >= .1 && DARKEN_FACTOR < 0.99) break;
  else printf("Not within range, try again\n");
  
  }


  printf("\n");
  
  int wob = 0;
  while(1){
  printf("Input a value for the wobble rate: \nRange (0 - 100) * If closer to 0 == more pheromones attraction \n");
  
  scanf("%d", &wob);
  
  WOBBLE_P = (1.0 - wob/100.0);
  
  if (WOBBLE_P >= 0.0 && WOBBLE_P <= 1.0) break;
  else printf("not within range, try again\n");
  
  }

    printf("\n");
    
//  while(1){
//  printf("Swiral Bias?\nInput 1 for yes 0 | 0 for no \n");
  
//  scanf("%d", &bias);
  
  
//  if (bias == 1 ||  bias == 0) break;
//  else printf("not within range, try again\n");
  
 // }
  

  


  
    // Initialize OpenGL and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Agent Simulation");

    // Set up coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    // Initialize agents
    initialize_pixels();
    initialize_agents();

    // Set up callback functions
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0); // Call update function immediately

    // Start the main loop
    glutMainLoop();

    return 0;
}
