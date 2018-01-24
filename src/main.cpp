#include "main.h"
#include "timer.h"
#include "ball.h"
#include "ground.h"
#include "obstacle.h"
#include "mine.h"

#include "time.h"
using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/
color_t COLOR_OBSTACLE = { 153,102,255 };
color_t COLOR_TRAMPOLINE = {102, 153, 255 };
color_t COLOR_MINE = { 255, 0, 102 };

Ball ball;
Ground ground;
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
Timer t60(1.0 / 60);
int i1,i2,i3;
int moveLeft=0;
int moveRight=0;
int moveUp=0;
int moveDown=0;
float initL=0.0;
float initR=0.0;
float initS=0.0;
int no_obstacles=50;
int no_mines=10;
int mine_lives=3;
int jumplimit=2.0;

int obsflag=0;
int groundflag=0;
Obstacle obstacles[50];
float obstaclesOrigin[50];
int collision_flag[50]={0};
/* Render the scene with openGL */
/* Edit this function according to your assignment */
int firstjump=0;
int jumps_avail=3;
float maxReached;

Mine mines[10];
float minesOrigin[10];


void floatsleep ( float delay)
{
    clock_t start;
    clock_t stop;

    start = clock();
    stop = clock();
    while ( ( (float)( stop - start)/ CLOCKS_PER_SEC) < delay) {
        stop = clock();
    }
}

void draw() {

    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    ball.draw(VP);
    ground.draw(VP);
//    obstacles[0].draw(VP);
//    obstacles[1].draw(VP);
    for(int i=0;i<no_obstacles;i++)
        obstacles[i].draw(VP);
    for(int i=0;i<no_mines;i++)
        mines[i].draw(VP);
}

void foo(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key==GLFW_KEY_SPACE && action==GLFW_PRESS && firstjump==0)
    {
        ball.speedy=-0.05;
        initS=ball.position.y;
        moveUp=1;
    }
    if(key==GLFW_KEY_SPACE && action==GLFW_PRESS && firstjump==1 && jumps_avail)
    {
        jumps_avail--;
        ball.speedy=-0.05;
        initS=ball.position.y;
        moveUp=1;
    }
//    else if(key==GLFW_KEY_RIGHT && action==GLFW_PRESS)
    else if(key==GLFW_KEY_RIGHT)
    {
        ball.speedx=-0.03;
        initR=ball.position.x;
        moveRight=1;
        moveLeft=0;
    }
//    else if(key==GLFW_KEY_LEFT && action==GLFW_PRESS)
    else if(key==GLFW_KEY_LEFT )
    {
        ball.speedx=0.03;
        initL=ball.position.x;
        moveLeft=1;
        moveRight=0;
    }
    else if(key==GLFW_KEY_ESCAPE)
    {
        int score=0;
        for(int i=0;i<no_obstacles;i++)
            if(collision_flag[i]==1)score++;
        printf("%d\n",score);
        quit(window);
    }
}

void tick_elements() {
    ball.tick();
    if(ball.position.y>=screen_center_y){screen_center_y=ball.position.y;reset_screen();}
    if(ball.position.y<=screen_center_y && ball.position.y>=1.5){screen_center_y=ball.position.y;reset_screen();}
    for(int i=0;i<no_obstacles;i++)
        obstacles[i].tick();
    for(int i=0;i<no_mines;i++)
        mines[i].tick();
    //obstacles[1].tick();
    for(int i=0;i<no_obstacles;i++)
    {
    if(obstacles[i].position.x<=-3.2 || obstacles[i].position.x>=3.2 || abs(obstacles[i].position.x-obstaclesOrigin[i])>=1.25)
    {
        obstacles[i].speed*=-1;
    }
    }
    for(int i=0;i<no_mines;i++)
    {
    if(mines[i].position.x<=-3.2 || mines[i].position.x>=3.2 || abs(mines[i].position.x-minesOrigin[i])>=1.25)
    {
        mines[i].speed*=-1;
    }
    }
    if(moveLeft==1 && initL-0.8>=ball.position.x)
    {
        moveLeft=0;
        ball.speedx=0;
    }
    if(moveRight==1 && initR+0.8<=ball.position.x)
    {
        moveRight=0;
        ball.speedx=0;
    }
    if(moveUp==1 && initS+jumplimit<=ball.position.y)
    {
        moveUp=0;
        ball.speedy=-1.0*ball.speedy;
    }
    int gcoll=0;
//    if(detect_collision(ball.bounding_box(),obstacles[0].bounding_box()) && ball.position.y>obstacles[0].position.y)
    for(int i=0;i<no_obstacles;i++)
    {
    if(detect_collision(ball.bounding_box(),obstacles[i].bounding_box()) && ball.speedy>=0)
    {
        //printf("Collide");
//        ball.speedy=0;
//        ball.speedx=obstacles[i].speed;
        obsflag=1;
        groundflag=0;
        collision_flag[i]=1;
        firstjump=1;
        ball.speedy=-0.05;
        initS=ball.position.y;
        moveUp=1;
        obstacles[i].set_position(-5.0,-5.0);
        if(i==i1-1||i==i2+14||i==i3+29)
        {
            float theta=obstacles[i].rotation;
            float osx=ball.speedx;
            float osy=ball.speedy;
            ball.speedx=osx*(1-tan(M_PI_2-(theta*M_PI/180)))+osy*(-1+tan((theta*M_PI/180)));
            ball.speedy=osx*(1+tan(M_PI_2-(theta*M_PI/180)))+osy*(1+tan((theta*M_PI/180)));
            jumplimit=5.0;
        }
        else
        {
            jumplimit=2.0;
        }
        maxReached=(float)i;
        break;
    }
    else
    {
        obsflag=0;
    }
    }

    for(int i=0;i<no_mines;i++)
    {
    if(detect_collision(ball.bounding_box(),mines[i].bounding_box()) && ball.speedy>=0)
    {
        //printf("Collide");
//        ball.speedy=0;
//        ball.speedx=mines[i].speed;
        obsflag=1;
        groundflag=0;
        collision_flag[i]=1;
        firstjump=1;
        ball.speedy=-0.05;
        initS=ball.position.y;
        moveUp=1;
        mines[i].set_position(-5.0,-5.0);
        maxReached=(float)i;
        mine_lives--;
        break;
    }
    else
    {
        obsflag=0;
    }
    }

    if (detect_collision_ground()) {
        moveLeft=1;
        ball.speedx=0;
        ball.speedy=0;
        gcoll=1;
        groundflag=1;
        obsflag=0;
        if(collision_flag[0]==1)
        {
            int score=0;
            for(int i=0;i<no_obstacles;i++)
                if(collision_flag[i]==1)score++;
            printf("%d\n",score);
            floatsleep(0.25);
            quit(window);
        }
    }
    else if(groundflag==0 && obsflag==0 && moveUp==0)
    {

        ball.speedy=0.03;
    }
//    else if(obsflag==1)
//    {
//        obsflag=0;
//        ball.speedy=0.03;
//    }
    else if(int coll=detect_collision_side()!=0 && gcoll==0)
    {
        if(coll=1){moveLeft=1;moveRight=0;}
        else {moveRight=1;moveLeft=0;}
        ball.speedx=0;
        ball.speedy=0.03;
    }
    if(ball.position.y<=-2.0+1.9*(maxReached-2.5))
    {
        int score=0;
        for(int i=0;i<no_obstacles;i++)
            if(collision_flag[i]==1)score++;
        printf("%d\n",score);
        //floatsleep(0.25);
        quit(window);
    }
    if(mine_lives==0)
    {
        int score=0;
        for(int i=0;i<no_obstacles;i++)
            if(collision_flag[i]==1)score++;
        printf("%d\n",score);
        floatsleep(0.6);
        quit(window);
    }

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    ball       = Ball(0, -1.5, COLOR_RED);
    ball.speedy=0.03;
    ground = Ground(0,0,COLOR_GREEN);
//    obstacles[0] = Obstacle(-1.5,-2.0,COLOR_BLACK);
//    obstaclesOrigin[0]=-1.5;
//    obstacles[0].speed=0.02;
//    obstacles[1] = Obstacle(2.5,-0.1,COLOR_BLACK);
//    obstaclesOrigin[1]=2.5;
//    obstacles[1].speed=0.02;

    srand((unsigned int)time(NULL));
    i1=(int)((float)rand()/(float)(RAND_MAX/15.0));
    i2=(int)((float)rand()/(float)(RAND_MAX/15.0));
    i3=(int)((float)rand()/(float)(RAND_MAX/15.0));
    i1=1;
    float yinitial=-2.0;
    for(int i=0;i<no_obstacles;i++)
    {
        float x = (float)rand()/(float)(RAND_MAX/5.0);
        if(i==0)
        {
            while(x>=0.8 || x<=-0.8)x = (float)rand()/(float)(RAND_MAX/5.0);
        }
        if(i==i1-1||i==i2+14||i==29+i3)
        {
            obstacles[i] = Obstacle(x-2.5,yinitial,COLOR_TRAMPOLINE);
            if(x-2.5>=0)obstacles[i].rotation=-22.5;
            else obstacles[i].rotation=22.5;
        }
        else
        {
            obstacles[i] = Obstacle(x-2.5,yinitial,COLOR_OBSTACLE);
        }
        obstaclesOrigin[i]=x-2.5;
        float spd = (float)rand()/(float)(RAND_MAX/0.03);
        obstacles[i].speed=spd;
        yinitial+=1.9;
    }
    yinitial=5.0;
    for(int i=0;i<no_mines;i++)
    {
        float x = (float)rand()/(float)(RAND_MAX/5.0);
        if(i==0)
        {
            while(x>=0.8 || x<=-0.8)x = (float)rand()/(float)(RAND_MAX/5.0);
        }
        mines[i] = Mine(x-2.5,yinitial,COLOR_MINE);
        minesOrigin[i]=x-2.5;
        float spd = (float)rand()/(float)(RAND_MAX/0.03);
        mines[i].speed=spd;
        yinitial+=3.0;
    }


    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window,foo);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            //tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision_ground() {
    //printf("%f\n",ball.position.x);
    if(ball.position.y<=-2.8)
        return true;
    else return false;
    //return false;
}
int detect_collision_side() {
    //printf("%f\n",ball.position.x);
    if(ball.position.x>=3.8)
        return 1;
    else if(ball.position.x<=-3.8)
        return -1;
    else return 0;
    //return false;
}

bool detect_collision(bounding_box_t a,bounding_box_t b)
{
        return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
               (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
