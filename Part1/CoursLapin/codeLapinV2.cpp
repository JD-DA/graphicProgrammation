//
// Created by Jean-Daniel de Ambrogi on 28/01/2022.
//

#include <iostream>
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <time.h>

#include <math.h>

#include "../Common/shaders_utilities.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_FORCE_RADIANS


#include "modele.hpp"

using namespace std;



 GLfloat* coordonnees;
 GLfloat* couleurs;
 GLuint *indices;
GLfloat* normales;

GLuint vboid[4];
GLuint programID;
GLuint vaoID;

// Pour les interactions, gestion des events.
GLFWwindow* window;
float stepTrans = 0.01;
int mouseXOld, mouseYOld;
double Xpos, Ypos;
bool leftbutton = false;
bool rightbutton = false;
bool middlebutton = false;

// Un identifiant encore ....
GLuint MatrixID;
GLint MID;
GLint lightID;

// Des matrices ...
// On va utiliser pour les construire et les manipuler glm (OpenGL Mathematics)
glm::mat4 Projection; // une matrice de projection par rapport à l'écran
glm::mat4 View; // une matrice pour se placer par rapport à la caméra
glm::mat4 Model; // une matrice Model pour déplacer dans le repère universel les objets
glm::mat4 MVP; // Et le produit des trois

glm::mat4 translation; // une matrice de translation qui va nous permettre de déplacer le cube
glm::mat4 trans_initial; // une matrice de translation pour centrer le cube
glm::mat4 rotation; // une matrice pour construire une rotation appliquée sur le cube

int nbFaces;
int nbSommets;

GLfloat ambiant[4] = {0.1,0.1,0.1,1.0};
GLfloat lumiere[3] = {0.1,0.1,0.25};
GLfloat diffuse[4] = {10,10,10,10};

// L'identifiant supplémentaire pour transmettre ce vecteur à la carte graphique

GLint ambiantID;
GLint diffuseID;

void init() {
    srand (time(NULL));
  glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    initialisation("lapin.obj",&nbSommets,&nbFaces);
    coordonnees=new GLfloat [3*nbSommets];

    //random colors
    couleurs=new GLfloat [3*nbSommets];
    for (int i = 0; i < nbSommets; ++i) {
        /*couleurs[i*3] = rand() % 10 + 1;
        couleurs[i*3+1] = rand() % 10 + 1;
        couleurs[i*3+2] = rand() % 10 + 1;*/
        couleurs[i*3] = 1;
        couleurs[i*3+1] = 1;
        couleurs[i*3+2] = 1;
    }
    indices=new GLuint [3*nbFaces];

    printf("nb faces : %i nb sommets : %i \n",nbFaces,nbSommets);

    GLfloat bbox[6];
    lecture("lapin.obj",nbSommets,nbFaces,coordonnees,indices,bbox);
    printf("bbox : %f %f %f %f %f %f\n",bbox[0],bbox[1],bbox[2],bbox[3],bbox[4],bbox[5]);

    glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  

  glUseProgram(programID);

  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

    //programID = LoadShaders("../Shaders/TransformVertexShader.vert", "../Shaders/ColorFragShader.frag");
    //programID = LoadShaders( "../Shaders/AmbiantVertexShader.vert", "../Shaders/AmbiantFragmentShader.frag" );
    programID = LoadShaders( "../Shaders/DiffuseVertexShader.vert", "../Shaders/DiffuseFragmentShader.frag" );

  glGenBuffers(4, vboid);
  glBindBuffer(GL_ARRAY_BUFFER, vboid[0]);
  glBufferData(GL_ARRAY_BUFFER,3 * nbSommets * sizeof(GLfloat),coordonnees,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, vboid[1]);
  glBufferData(GL_ARRAY_BUFFER,3 * nbSommets * sizeof(GLfloat),couleurs,GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(1);
//calcul des normales
    float rabbitCenter[3]={bbox[0]+bbox[1],bbox[3]-bbox[2],bbox[5]+bbox[4]};
    printf("rabbit center : %f, %f, %f\n",rabbitCenter[0],rabbitCenter[1],rabbitCenter[2]);
    normales=new GLfloat [3*nbSommets];
    for (int i = 0; i < nbSommets; ++i) {
        GLuint leSommet = indices[i*3] ;
        GLuint voisinA = indices[i*3+1];
        GLuint voisinB = indices[i*3+2];
        /*normales[i*3]=leSommet%2;
        normales[i*3+1]=voisinA%2;
        normales[i*3+2]=voisinB%2;*/
        /*normales[i*3]=rand()%2;
        normales[i*3+1]=rand()%2;
        normales[i*3+2]=rand()%2;*/
        normales[i*3]=coordonnees[i*3]-rabbitCenter[0];
        normales[i*3+1]=coordonnees[i*3+1]-rabbitCenter[1];
        normales[i*3+2]=coordonnees[i*3+2]-rabbitCenter[2];

        //printf("%f, %f, %f\n",normales[i*3],normales[i*3+1],normales[i*3+2]);

    }
  glBindBuffer(GL_ARRAY_BUFFER, vboid[2]);
  glBufferData(GL_ARRAY_BUFFER,3 * nbSommets * sizeof(GLfloat),normales,GL_STATIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboid[3]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,3*nbFaces*sizeof(GLuint),indices,GL_STATIC_DRAW);
  
  

  // Génération des identifiants pour les variables Uniform vers le vertex shader
  MatrixID = glGetUniformLocation(programID, "MVP");
  MID = glGetUniformLocation(programID, "M");

  ambiantID = glGetUniformLocation(programID, "ambiant");
  diffuseID = glGetUniformLocation(programID, "diffuse");
    lightID =  glGetUniformLocation(programID, "light_position");

    //Avant on était en projection orthographique
    // Maintenant on est en perspective !
    Projection = glm::perspective(70.0f, 1.f, 0.1f, 100.0f);
    // En projection orthographique pas besoin de caméra
    // Ici il faut la placer
  View = glm::lookAt(glm::vec3(0,0,1.0), glm::vec3(0,0,0), glm::vec3(0,1,0));

    //  std::cout<<glm::to_string(View)<<std::endl;

    // Création d'une matrice de translation pour déplacer le cube de -0.25 en X, Y et Z
  trans_initial = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
  

    // On initialise Model à l'identité
  Model = glm::mat4(1.0f);
  
    // Au final l'objet est placé dans le repère universel soit x un point en coordonnées homogènes
    // Model*x : permet d'appliquer des transformations toujours dans le repère universel
    // View*Model*x : on se place dans le repère de la caméra
    // Projection*View*Model*x : on projette sur l'écran 2D
    MVP = Projection * View * Model;

    // Initialisation d'une rotation et d'une translation à l'identité.
  rotation = glm::mat4(1.0f);
  
  translation = glm::mat4(1.0f);

}

void Display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

  Model = translation * rotation * trans_initial;

  MVP =  Projection * View * Model;


  // Envoi des matrices Model et View et de la lumière
  glUniformMatrix4fv(MID, 1, GL_FALSE, &Model[0][0]);

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform4fv(diffuseID,1,diffuse);
  glUniform4fv(ambiantID,1,ambiant);
  glUniform3fv (lightID,1,lumiere) ;

  glDrawElements(GL_TRIANGLES, (unsigned int)nbFaces*3, GL_UNSIGNED_INT, NULL);
  glfwSwapBuffers(window);

}


// Les touches F-H-T-V déplacent la lampe en x et y
// J et L déplacent la lampe sur l'axe des z
// C et B pour l'intensité de la lumière ambiante
// D et G pour l'intensité de la lumière diffuse
static void Clavier(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        lumiere[0] -= 0.5;
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
        lumiere[0] += 0.5;
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        lumiere[1] -= 0.5;
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        lumiere[1] += 0.5;
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
        lumiere[2] -= 0.5;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        lumiere[2] += 0.5;
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        for (int i = 0; i < 3; i++)
            ambiant[i] -= 0.1;
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        for (int i = 0; i < 3; i++)
            ambiant[i] += 0.1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        for (int i = 0; i < 3; i++)
            diffuse[i] -= 0.1;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        for (int i = 0; i < 3; i++)
            diffuse[i] += 0.1;
    }

    if (key ==  GLFW_KEY_UP)
        translation = glm::translate(translation, glm::vec3(0.f, stepTrans, 0.f));
    if (key ==  GLFW_KEY_DOWN)
        translation = glm::translate(translation, glm::vec3(0.f, -stepTrans, 0.f));
    if (key ==  GLFW_KEY_RIGHT)
        translation = glm::translate(translation, glm::vec3(-stepTrans, 0.f, 0.f));
    if (key ==  GLFW_KEY_LEFT)
        translation = glm::translate(translation, glm::vec3(stepTrans, 0.f, 0.f));
    if (key ==  GLFW_KEY_PAGE_UP)
        translation = glm::translate(translation, glm::vec3(0.f, 0.f, stepTrans));
    if (key ==  GLFW_KEY_PAGE_DOWN)
        translation = glm::translate(translation, glm::vec3(0.f, 0.f, -stepTrans));

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    glfwSwapBuffers(window);
}

void Souris(GLFWwindow *window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftbutton=true;
            mouseXOld = Xpos;
            mouseYOld = Ypos;
        } else {
            leftbutton=false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            middlebutton=true;
            mouseXOld = Xpos;
            mouseYOld = Ypos;
        } else {
            middlebutton=false;
        }
    }

    if (button== GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mouseXOld = Xpos;
            mouseYOld = Ypos;
            rightbutton=true;
        }
        else {
            rightbutton=false;
        }
    }
}

void Motion(GLFWwindow *window, double xpos, double ypos)
{
    Xpos = xpos;
    Ypos = ypos;
    if (leftbutton==true) {
        rotation = glm::rotate(rotation, (float)(xpos-mouseXOld)*stepTrans/10, glm::vec3(0.f, 1.f, 0.f));
    }
    if (middlebutton==true) {
        translation = glm::translate(translation, glm::vec3(0.f, 0.f, (ypos - mouseYOld) * stepTrans / 10));
        mouseXOld = xpos;
        mouseYOld = ypos;
    }
    if (rightbutton==true) {
        translation = glm::translate(translation, glm::vec3((xpos-mouseXOld)*stepTrans/10, -(ypos-mouseYOld)*stepTrans/10, 0.f));
        mouseXOld = xpos;
        mouseYOld = ypos;
    }
}

int main(int argc, char** argv)
{

    glfwInit();

    // Quelle est la compatibilité OpenGL et contexte créé : ici OpenGL 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


    // CORE_PROFILE on n'admet pas les fonctions OpenGL deprecated dans les versions précédentes  (même si elles sont encore disponibles)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // FORWARD_COMPATIBLE on n'admet pas les fonctions OpenGL qui vont devenir deprecated dans les futures versions ?
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Création de la fenetre
    window = glfwCreateWindow(500, 500, "The rabbit is a lie", NULL, NULL);

    glfwSetKeyCallback(window, Clavier);
    glfwSetMouseButtonCallback(window, Souris);
    glfwSetCursorPosCallback(window, Motion);

    if (!window) {
        std::cout << "Impossible de créer la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Taille et position à l'écran de la fenêtre créée
    glfwSetWindowPos(window, 100, 100);
    glfwSetWindowSize(window, 500, 500);

    // On attache le contexte actuel OpenGL à la fenêtre
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    init();
    while (!glfwWindowShouldClose(window)) {
        Display();
        glfwPollEvents();
    }

    glfwTerminate();
  return 0 ;
  
}
