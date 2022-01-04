#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Cube.hpp>
#include <glimac/Cone.hpp>
#include <glimac/Cylinder.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>
#include <glm/gtc/random.hpp>
#include <glimac/Image.hpp>
#include <glimac/FreeFlyCamera.hpp>



#define NB_SPOTS 2

using namespace glimac;


struct BielleProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightPos;
    GLint uLightIntensities;

    GLint uSpotLightsPos[NB_SPOTS];
    GLint uSpotLightsIntensities[NB_SPOTS];
    GLint uSpotLightsDirections[NB_SPOTS];
    GLint uSpotLightsAngles[NB_SPOTS];

    BielleProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/spheres.vs.glsl",
                              applicationPath.dirPath() + "shaders/bielle.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        
        uLightPos = glGetUniformLocation( m_Program.getGLId(), "uLightDir_vs");
        uLightIntensities = glGetUniformLocation( m_Program.getGLId(), "uLightIntensity");
    

        for (int i = 0; i < NB_SPOTS; i++) {
            std::string number = std::to_string(i);
            uSpotLightsPos[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsPos[" + number + "]").c_str());
            uSpotLightsIntensities[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsIntensity[" + number + "]").c_str());
            uSpotLightsDirections[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsDirection[" + number + "]").c_str());
            uSpotLightsAngles[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsAngle[" + number + "]").c_str());
        }
        }
};

struct Warehouse {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uShininess;
    GLint uLightDir_vs;
    GLint uLightIntensity;

    GLint uKdTexture;
    GLint uKsTexture;


    GLint uSpotLightsPos[NB_SPOTS];
    GLint uSpotLightsIntensities[NB_SPOTS];
    GLint uSpotLightsDirections[NB_SPOTS];
    GLint uSpotLightsAngles[NB_SPOTS];



    Warehouse(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/spheres.vs.glsl",
                              applicationPath.dirPath() + "shaders/warehouse.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");

        uKdTexture = glGetUniformLocation( m_Program.getGLId(), "uKdTexture");
        uKsTexture = glGetUniformLocation( m_Program.getGLId(), "uKsTexture");
        
    

        for (int i = 0; i < NB_SPOTS; i++) {
            std::string number = std::to_string(i);
            uSpotLightsPos[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsPos[" + number + "]").c_str());
            uSpotLightsIntensities[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsIntensity[" + number + "]").c_str());
            uSpotLightsDirections[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsDirection[" + number + "]").c_str());
            uSpotLightsAngles[i] = glGetUniformLocation( m_Program.getGLId(), ("uSpotLightsAngle[" + number + "]").c_str());
        }
    }
};

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    FilePath applicationPath(argv[0]);
    BielleProgram bielleProgram(applicationPath);
    Warehouse warehouse(applicationPath);
    //MoonProgram moonProgram(applicationPath); earth

    float l = 8;
    float d = 6;
    float E = 0.7;
    float arbre_angle = 30.;
    float speed = 0.5;
    float pos_piston = 0.;
    bool mouse_rotation = false;
    float accumulator = 0;
    float tmp=0;


    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    //objets canoniques
    Sphere sphere(1, 32, 32);
    Cylinder cylinder(32);
    Cube cube(1);
    Cone cone(1,1,16,16);

    //camera
    FreeFlyCamera cam;


    /* Point Lights */
    bool flagPoint;

    glm::vec4 lightsPositions;
    glm::vec3 lightsIntensities;

    flagPoint = false;
    lightsIntensities = glm::vec3(0.,0.,0.);
    

    /* Spot Lights */
    bool flagSpot0;
    bool flagSpot1;

    glm::vec4 spotLightsPositions[NB_SPOTS-1];
    glm::vec3 spotLightsIntensities[NB_SPOTS-1];
    glm::vec4 spotLightsDirections[NB_SPOTS-1];
    float spotLightsAngles[NB_SPOTS-1];

    for (int i = 1; i < NB_SPOTS; i++) {
        spotLightsIntensities[i-1] = glm::vec3(0.,0.,0.);
    }


    /* Bielle Light */
    spotLightsIntensities[0] = glm::vec3(20,20,20);
    spotLightsAngles[0] = M_PI/10;

    spotLightsPositions[1] = glm::vec4(0,l,0,1);
    spotLightsAngles[1] = M_PI/10;
    spotLightsDirections[1] = glm::vec4(0,-1.,0, 0);//calculer le x a partir de pos_piston
    spotLightsIntensities[1] = glm::vec3(8,8,8);



    /* Camera Light */
    flagSpot0 = true;
    flagSpot1 = true;


    /* First PointLight */
    flagPoint = true;

    lightsPositions = glm::vec4(0, 10, 0, 0);
    lightsIntensities = glm::vec3(5,5,5);


////////////////////////VBO and VAO init/////////////////////////////////////////
    auto textureWarehouseWall = loadImage("../textures/warehouse.jpeg");

    GLuint texWallKd;
    glGenTextures(1,&texWallKd);
    glBindTexture(GL_TEXTURE_2D,texWallKd);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, textureWarehouseWall.get()->getWidth(), textureWarehouseWall.get()->getHeight()
            ,0,GL_RGBA,GL_FLOAT,textureWarehouseWall.get()->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);


    auto textureCrate = loadImage("../textures/crate.jpeg");

    GLuint texCrate;
    glGenTextures(1,&texCrate);
    glBindTexture(GL_TEXTURE_2D,texCrate);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, textureCrate.get()->getWidth(), textureCrate.get()->getHeight()
            ,0,GL_RGBA,GL_FLOAT,textureCrate.get()->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    GLuint spherevbo;
    glGenBuffers(1,&spherevbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, spherevbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount()*sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     GLuint spherevao;
     glGenVertexArrays(1,&spherevao);
     glBindVertexArray(spherevao);
     
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);
     
     glBindBuffer(GL_ARRAY_BUFFER, spherevbo);
     
     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, position)));
     glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, normal)) );
     glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, texCoords)) );

     
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     glBindVertexArray(0);/**/
//cube
    
    GLuint cubevbo;
    glGenBuffers(1,&cubevbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
    glBufferData(GL_ARRAY_BUFFER, cube.getVertexCount()*sizeof(ShapeVertex), cube.getDataPointer(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     GLuint cubevao;
     glGenVertexArrays(1,&cubevao);
     glBindVertexArray(cubevao);
     
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);
     
     glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
     
     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, position)));
     glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, normal)) );
     glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, texCoords)) );

     
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     glBindVertexArray(0);/**/
//cylinder
     GLuint cylvbo;
    glGenBuffers(1,&cylvbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, cylvbo);
    glBufferData(GL_ARRAY_BUFFER, cylinder.getVertexCount()*sizeof(ShapeVertex), cylinder.getDataPointer(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     GLuint cylvao;
     glGenVertexArrays(1,&cylvao);
     glBindVertexArray(cylvao);
     
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);
     
     glBindBuffer(GL_ARRAY_BUFFER, cylvbo);
     
     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, position)));
     glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, normal)) );
     glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, texCoords)) );

     
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     glBindVertexArray(0);
//cone
     GLuint conevbo;
    glGenBuffers(1,&conevbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, conevbo);
    glBufferData(GL_ARRAY_BUFFER, cone.getVertexCount()*sizeof(ShapeVertex), cone.getDataPointer(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     GLuint conevao;
     glGenVertexArrays(1,&conevao);
     glBindVertexArray(conevao);
     
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);
     
     glBindBuffer(GL_ARRAY_BUFFER, conevbo);
     
     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, position)));
     glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, normal)) );
     glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(ShapeVertex),(const GLvoid*) (offsetof(ShapeVertex, texCoords)) );

     
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     
     glBindVertexArray(0);/*
*/
    glEnable(GL_DEPTH_TEST); 
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//GL_LINE);
/////////////////////////////////END init /////////////////////////////////////////////////////

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f),(float)(800./600.),0.1f,10000.f);
    glm::mat4 MVMatrix = cam.getViewMatrix();
    glm::mat4 NormalMatrix;

    std::vector<glm::vec3> room;
    room.emplace_back(glm::vec3(1.,0.,0.));
    room.emplace_back(glm::vec3(-1.,0.,0.));
    room.emplace_back(glm::vec3(0.,1,0.));
    room.emplace_back(glm::vec3(0.,-1,0.));
    room.emplace_back(glm::vec3(0.,0.,1.));
    room.emplace_back(glm::vec3(0.,0.,-1.));

    std::vector<glm::vec3> crates;
    crates.emplace_back(glm::vec3(0.1,-1.,0.));
    crates.emplace_back(glm::vec3(-1.,-1,0.));
    crates.emplace_back(glm::vec3(0.,-0.7,0.));
    crates.emplace_back(glm::vec3(-0.8,-1.,0.2));
    crates.emplace_back(glm::vec3(0.,-1.,1.));
    crates.emplace_back(glm::vec3(0.,-1.,-1.));

        

    // Application loop:
    bool done = false;
    SDL_EnableKeyRepeat(100, SDL_DEFAULT_REPEAT_INTERVAL);
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            switch(e.type) {
                case SDL_QUIT:
                    done = true; // Leave the loop after this iteration
                    break;
                case SDL_MOUSEMOTION :
                    if (mouse_rotation){
                    cam.rotateLeft(e.motion.xrel/10.);
                    cam.rotateUp(-e.motion.yrel/10.);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN :
                    mouse_rotation = !mouse_rotation; break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_z:
                        cam.moveFrontSameHeight(0.1);
                        break;
                    case SDLK_s:
                        cam.moveFrontSameHeight(-0.1);
                        break;
                    case SDLK_d:
                        cam.moveLeft(-0.1);
                        break;
                    case SDLK_q:
                        cam.moveLeft(0.1);
                        break;
                    case SDLK_o:
                        cam.rotateUp(1.);
                        break;
                    case SDLK_l:
                        cam.rotateUp(-1.);
                        break;
                    case SDLK_m:
                        cam.rotateLeft(-1.);
                        break;
                    case SDLK_k:
                        cam.rotateLeft(1.);
                        break;
                    case SDLK_t:
                        d-=0.1;
                        if(d<0)d=0.;
                        break;
                    case SDLK_y:
                        d+=0.1;
                        if(d>l) d=l;
                        break;
                    case SDLK_g:
                        l-=0.1;
                        if (l<0) l=0.;
                        if(l<d) d=l;
                        break;
                    case SDLK_h:
                        l+=0.1;
                        break;
                    case SDLK_v:
                        E-=0.1;
                        if (E<0) E=0.2;
                        break;
                    case SDLK_b:
                        E+=0.1;
                        if (E*2>=d) E-=0.2;
                        break;
                    case SDLK_a:
                        speed-=0.5;
                        accumulator = 0;
                        break;
                    case SDLK_e:
                        speed+=0.5;
                        accumulator  =0;
                        break;
                    case SDLK_r:
                        flagPoint = !flagPoint;
                        //std::cout<< flagPoint<<std::endl;
                        break;
                    case SDLK_f:
                        flagSpot0 = !flagSpot0;
                        //std::cout<< "flagspot0:" << flagSpot0<<std::endl;
                        break;
                    case SDLK_c:
                        flagSpot1 = !flagSpot1;
                        //std::cout<< "flagspot1:" << flagSpot1<<std::endl;
                        break;
                    case SDLK_KP0:
                        flagPoint = !flagPoint;
                        break;
                    case SDLK_KP1:
                        flagSpot0 = !flagSpot0;
                        break;
                    case SDLK_KP2:
                        flagSpot1 = !flagSpot1;
                        break;
                    case SDLK_p:
                        tmp = speed;
                        speed = accumulator;
                        accumulator = tmp;
                        break;
/**/
                    case SDLK_ESCAPE:
                        done = true;
                        break;
                    
                    default:
                        break;
                    }
                    break;
            }
        }

        arbre_angle += speed;
        while(arbre_angle > 360){
            arbre_angle -= 360;
        }

        //pos piston 
        pos_piston = d*sin(glm::radians(arbre_angle)+M_PI/2)+sqrt(pow(l,2)-(pow(d,2)*pow(cos(glm::radians(arbre_angle)+M_PI/2),2)));
        // angle = pos_piston 
        spotLightsPositions[1].x = pos_piston;
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the display
        bielleProgram.m_Program.use();

        //ajouté ukd & co
        glUniform3fv(bielleProgram.uKs, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
        glUniform1f(bielleProgram.uShininess, 0.4);


        /* Points light */
        if (flagPoint) {
            glUniform3fv(bielleProgram.uLightPos, 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*lightsPositions)));
            glUniform3fv(bielleProgram.uLightIntensities, 1, glm::value_ptr(lightsIntensities));
        } else {
            glUniform3f(bielleProgram.uLightIntensities,0.,0.,0.);
        }
        

        /* Camera light */
        if (flagSpot0) {
            glUniform3f(bielleProgram.uSpotLightsPos[0], 0., 0., 0.);
            glUniform3f(bielleProgram.uSpotLightsIntensities[0], 15., 15., 15.);
            glUniform3f(bielleProgram.uSpotLightsDirections[0], 0., 0., -1);
            glUniform1f(bielleProgram.uSpotLightsAngles[0], glm::cos(M_PI/7));
        } else {
            glUniform3f(bielleProgram.uSpotLightsIntensities[0],0.,0.,0.);
        }
        /* Spots lights */
        if (flagSpot1) {
            glUniform3fv(bielleProgram.uSpotLightsPos[1], 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*glm::vec4(pos_piston,l,0,1))));
            glUniform3fv(bielleProgram.uSpotLightsIntensities[1], 1, glm::value_ptr(spotLightsIntensities[1]));
            glUniform3fv(bielleProgram.uSpotLightsDirections[1], 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*spotLightsDirections[1])));
            glUniform1f(bielleProgram.uSpotLightsAngles[1], glm::cos(spotLightsAngles[1]));
            //std::cout<< "position: " << spotLightsPositions[1]<<std::endl;
            //std::cout<< "direction :" << spotLightsDirections[1]<<std::endl;
        } else {
            glUniform3f(bielleProgram.uSpotLightsIntensities[1],0.,0.,0.);
        }
        

        //bielle
         //rail

        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.5,0.,0.)));
         glBindVertexArray(cubevao);

         MVMatrix = glm::scale(
             glm::translate(
                 cam.getViewMatrix(), glm::vec3((l+d)/2,0,-E))
            ,glm::vec3(d+l+2*E,2*E,E)
        );
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());


            //bras
                //poignée
        
        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
         MVMatrix = glm::scale(
             glm::translate(
                 glm::rotate(
                    cam.getViewMatrix(),(float) glm::radians(arbre_angle) ,glm::vec3(0,0,1))
                    , glm::vec3(-d,0,0))
            ,glm::vec3(d/2,d/2,2*E)
         );
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());

         //branche

        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.,0.5,0.5)));
         MVMatrix = 
             glm::scale(
                 glm::rotate(cam.getViewMatrix(),(float) glm::radians(arbre_angle)  ,glm::vec3(0,0,1)),
                 glm::vec3(2*d,2*E,E));
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());
        glBindVertexArray(0);/**/
         
        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
         //ecrous
         glBindVertexArray(cylvao);
        MVMatrix = 
            glm::rotate(
                glm::scale(
                cam.getViewMatrix(), glm::vec3(E/2,E/2,2*E)),
            (float) glm::radians(90.),glm::vec3(1,0,0)
        );
         NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

         MVMatrix = glm::scale(
             glm::rotate(
                 glm::translate(
                     glm::rotate(
                        cam.getViewMatrix(),(float) glm::radians(arbre_angle)  ,glm::vec3(0,0,1)), glm::vec3(d,0,0))
                ,(float) glm::radians(90.),glm::vec3(1,0,0))
            , glm::vec3(E/2,2*E,E/2)
        );
         NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

            //bout
         MVMatrix = 
         glm::scale(
             glm::rotate(
                 glm::translate(
                     glm::rotate(
                        cam.getViewMatrix(),(float) glm::radians(arbre_angle)  ,glm::vec3(0,0,1)), glm::vec3(d,0,0))
             ,(float) glm::radians(90.),glm::vec3(1,0,0))
         ,glm::vec3(E,E,E)
        );

         NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());
         glBindVertexArray(0);/**/

         //arbre

        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.)));

         glBindVertexArray(cubevao);

         MVMatrix = 
         glm::scale(
             glm::translate(
                glm::rotate(
                        glm::translate(
                            glm::rotate(cam.getViewMatrix(),(float) glm::radians(arbre_angle) ,glm::vec3(0,0,1)), glm::vec3(d,0,E))
                    ,(float) (glm::radians(-arbre_angle)-M_PI/2+acos((d*sin(glm::radians(arbre_angle)))/l)) ,glm::vec3(0,0,1))
                , glm::vec3(l/2,0,0))
        ,glm::vec3(l,E,E/2));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());
        //accroches

        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.5)));

         MVMatrix = 
         glm::scale(
             glm::translate(glm::rotate(cam.getViewMatrix(),(float) glm::radians(arbre_angle) ,glm::vec3(0,0,1))
             , glm::vec3(d,0,E))
        ,glm::vec3(E,E,E));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());

         MVMatrix = 
         glm::scale(
             glm::translate(cam.getViewMatrix(), glm::vec3(pos_piston,0,E))
        ,glm::vec3(E,E,E));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());

        //piston


        glUniform3fv(bielleProgram.uKd, 1, glm::value_ptr(glm::vec3(0.,0.,0.5)));

        MVMatrix = 
         glm::scale(
             glm::translate(cam.getViewMatrix(), glm::vec3(pos_piston,0,0))
        ,glm::vec3(2*E,2*E,E));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());

        MVMatrix = 
         glm::scale(
             glm::translate(cam.getViewMatrix(), glm::vec3(pos_piston,E,0))
        ,glm::vec3(2*E,E/4,3*E));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());

        MVMatrix = 
         glm::scale(
             glm::translate(cam.getViewMatrix(), glm::vec3(pos_piston,-E,0))
        ,glm::vec3(2*E,E/4,3*E));
        
         NormalMatrix = cam.getViewMatrix();
         glUniformMatrix4fv(bielleProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
         glUniformMatrix4fv(bielleProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
         glUniformMatrix4fv(bielleProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
         glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());


        //warehouse

        warehouse.m_Program.use();

        /* Points light */
        if (flagPoint) {
            glUniform3fv(warehouse.uLightDir_vs, 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*lightsPositions)));
            glUniform3fv(warehouse.uLightIntensity, 1, glm::value_ptr(lightsIntensities));
        } else {
            glUniform3f(warehouse.uLightIntensity,0.,0.,0.);
        }
        

        /* Camera light */
        if (flagSpot0) {
            glUniform3fv(warehouse.uSpotLightsPos[0], 1, glm::value_ptr(cam.getViewMatrix()));
            glUniform3fv(warehouse.uSpotLightsIntensities[0], 1, glm::value_ptr(glm::vec3(1,1,1)));
            glUniform3fv(warehouse.uSpotLightsDirections[0], 1, glm::value_ptr(glm::vec3(0,0,-1)));
            glUniform1f(warehouse.uSpotLightsAngles[0], glm::cos(M_PI/7));
        } else {
            glUniform3f(warehouse.uSpotLightsIntensities[0],0.,0.,0.);
        }
        /* Spots lights */
        if (flagSpot1) {
            glUniform3fv(warehouse.uSpotLightsPos[1], 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*glm::vec4(pos_piston,l,0,1))));
            glUniform3fv(warehouse.uSpotLightsIntensities[1], 1, glm::value_ptr(spotLightsIntensities[1]));
            glUniform3fv(warehouse.uSpotLightsDirections[1], 1, glm::value_ptr(glm::vec3(cam.getViewMatrix()*spotLightsDirections[1])));
            glUniform1f(warehouse.uSpotLightsAngles[1], glm::cos(spotLightsAngles[1]));
        } else {
            glUniform3f(warehouse.uSpotLightsIntensities[1],0.,0.,0.);
        }
        


         /*RAJOUTER DU DECOR*/

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texWallKd);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,texWallKd);
        glUniform1i(warehouse.uKdTexture,0);
        glUniform1i(warehouse.uKsTexture,1);
        glUniform1f(warehouse.uShininess,2);

        glm::mat4 MVMatrix2;
        for (int i = 0; i < 6 ; ++i){
            MVMatrix2 = cam.getViewMatrix(); 
            MVMatrix2 = glm::translate(MVMatrix2, room[i]*(4*(l+d))); 
            MVMatrix2 = glm::scale(MVMatrix2, glm::vec3(4*(l+d), 4*(l+d), 4*(l+d))); // Translation * Rotation * Translation * Scale
            NormalMatrix = glm::transpose(glm::inverse(MVMatrix2));
            glUniformMatrix4fv(warehouse.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix2));
            glUniformMatrix4fv(warehouse.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix2));
            glUniformMatrix4fv(warehouse.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix2))));
            glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());
         }

         //for decor
         glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texCrate);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,texCrate);
        glUniform1i(warehouse.uKdTexture,0);
        glUniform1i(warehouse.uKsTexture,1);
        glUniform1f(warehouse.uShininess, 1);

        for (int i = 0; i < 6 ; ++i){
            MVMatrix2 = cam.getViewMatrix(); 
            MVMatrix2 = glm::translate(MVMatrix2, crates[i]*(l+d)); 
            MVMatrix2 = glm::scale(MVMatrix2, glm::vec3(d/2, d/2, d/2)); // Translation * Rotation * Translation * Scale
            NormalMatrix = glm::transpose(glm::inverse(MVMatrix2));
            glUniformMatrix4fv(warehouse.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix2));
            glUniformMatrix4fv(warehouse.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix2));
            glUniformMatrix4fv(warehouse.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix2))));
            glDrawArrays(GL_TRIANGLES, 0, cube.getVertexCount());
         }


         
         glBindVertexArray(0);
         
        windowManager.swapBuffers();
    }
    glDeleteBuffers(1, &spherevbo);
    glDeleteVertexArrays(1, &spherevao);
    glDeleteBuffers(1, &cubevbo);
    glDeleteVertexArrays(1, &cubevao);
    glDeleteBuffers(1, &cylvbo);
    glDeleteVertexArrays(1, &cylvao);
    glDeleteBuffers(1, &conevbo);
    glDeleteVertexArrays(1, &conevao);
    return EXIT_SUCCESS;
}
