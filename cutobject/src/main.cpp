#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include "camera.h"

using namespace std::literals;

static constexpr float dAxes = 100.0f;
static constexpr float wAxes = 0.01f;

static int width = 1200;
static int height = 600;


float kube[] = { 0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1 }; // ������ ������ 
GLuint kubeInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,2,5, 6,7,3, 0,1,5, 5,4,0, // ������ ������
                    1,2,6, 6,5,1, 0,3,7, 7,4,0};
float axes[] = { 
    0, wAxes,-dAxes, wAxes,-wAxes,-dAxes, -wAxes,-wAxes,-dAxes, 
    0, wAxes, dAxes, wAxes,-wAxes, dAxes, -wAxes,-wAxes, dAxes};
GLuint axesInd[] = { 0,3,4, 4,1,0, 1,4,5, 5,2,1, 2,5,3, 3,0,2 }; 

float plainInSegment[] = {-1, -1, -1, 1};

struct TPoint {
    int x, y;
};

struct FPoint {
    float x, y, z;
};

struct TColor {
    GLubyte r, g, b;
};

struct Object {
    std::vector<float> xyz;
    float x, y, z;
};

void ResizeWindow(int w, int h)
{
    glViewport(0, 0, w, h);
    glLoadIdentity();
    float k = (float)w / (float)h;
    float sz = 0.2;
    glFrustum(-k * sz, k * sz, -sz, sz, sz * 2, 10000);
}

void ObjectShow()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, kube);
    for (int i = 0; i < 1/*objectcnt*/; i++)
    {
        glPushMatrix();
        glColor3ub(244, 60, 43);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, kubeInd);
        glPopMatrix();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}


void PlainShow()
{
    auto p = plainInSegment;
    std::vector<float> plain;
    //1
    plain.push_back(0.0f);
    plain.push_back(dAxes);
    plain.push_back((-p[3]-p[1]*dAxes)/p[2]);
    //6
    plain.push_back(-dAxes);
    plain.push_back((-p[3]+ p[0] * dAxes) / p[1]);
    plain.push_back(0.0f);
    //2
    plain.push_back((-p[3] - p[2] * dAxes) / p[0]);
    plain.push_back(0.0f);
    plain.push_back(dAxes);
    //4
    plain.push_back(0.0f);
    plain.push_back(-dAxes);
    plain.push_back((-p[3] + p[1] * dAxes) / p[2]);
    //3
    plain.push_back(dAxes);
    plain.push_back((-p[3] - p[0] * dAxes) / p[1]);
    plain.push_back(0.0f);
    //5
    plain.push_back((-p[3] + p[2] * dAxes) / p[0]);
    plain.push_back(0.0f);
    plain.push_back(-dAxes);

    glEnable(GL_BLEND);
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3ub(54, 100, 234);
        glVertexPointer(3, GL_FLOAT, 0, plain.data());
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, plain.size()/3);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    glDisable(GL_BLEND);
}

void SpaceInit()
{
    glEnable(GL_DEPTH_TEST);
    ResizeWindow(width, height);
}
void AxesShow()
{
    std::vector<TColor> colors = {{255,222,0}, {0,255,0}, {0,0,255}};
    std::vector<FPoint> rot = {{0,0,0}, {0,1,0}, {1,0,0}};
    glVertexPointer(3, GL_FLOAT, 0, &axes);
    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        if (i) // fix in windows
            glRotatef(90, rot[i].x, rot[i].y, rot[i].z);
        glColor3ub(colors[i].r, colors[i].g, colors[i].b);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, axesInd);
        glPopMatrix();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}

void SpaceShow(Camera& camera)
{
    glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        camera.Apply();
        AxesShow();
        PlainShow();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, kube);
        glPushMatrix();
        {
            glColor3ui(234, 142, 13);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, kubeInd);
        }
        glPopMatrix();
        glDisableClientState(GL_VERTEX_ARRAY);
        ObjectShow();
    }
    glPopMatrix();
}

int main()
{
    sf::ContextSettings window_settings;
    window_settings.depthBits = 24; // Request a 24-bit depth buffer
    window_settings.stencilBits = 8;  // Request a 8 bits stencil buffer
    window_settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing
    // create the window
    sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, window_settings);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorGrabbed(true);
    // activate the window
    window.setActive(true);

    // load resources, initialize the OpenGL states, ...
    SpaceInit();
    Camera camera(window);
    // run the main loop
    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                running = false;
                break;
            case sf::Event::Resized: {
                width = event.size.width;
                height = event.size.height;
                ResizeWindow(width, height);
                break; 
            }
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    running = false;
                break;
            case sf::Event::MouseWheelScrolled:
                break;
            case sf::Event::MouseButtonPressed: {
                break;
            }
            case sf::Event::MouseEntered: {
                //window.setMouseCursorVisible(false);
            }
            case sf::Event::MouseMoved: {
                //std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
                //std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
            }
            default:
                break;
            }
        }

        // clear the buffers
        camera.Move();
        SpaceShow(camera);


        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    // release resources...

    return 0;
}
