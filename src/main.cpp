#include <GL/openglut.h>
#include "definitions.h"

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;

    GLEC(glClearColor(0.19f, 0.65f, 0.32f, 1.0f));
    GLEC(glClear(GL_COLOR_BUFFER_BIT));

    glutSwapBuffers();
}

int main() {
    glutInitWindowSize(640,480);
    glutInitWindowPosition(40,40);
    glutInit(nullptr, nullptr);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("OpenGLUT Window");

    glutDisplayFunc(display);

   /*  glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle); */

    GLEC(glClearColor(0.19f, 0.65f, 0.32f, 1.0f));
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    //glEnable(GL_LIGHT0);
    //glEnable(GL_NORMALIZE);
    //glEnable(GL_COLOR_MATERIAL);

    //glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    //glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    //glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();

}