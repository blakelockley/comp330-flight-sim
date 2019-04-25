/*
 Author: Blake Lockley
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#define ESC 27
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define CLAMP(x, lower, upper) (MIN(MAX(x, lower), upper))

enum camera_mode_t { GROUND, FIRST } c_mode = GROUND;

int previousTime = 0;
float heading, pitch, roll, x, y, z;

struct vec { double x, y, z; };

void reset(void) {
  heading = -180.0;
  pitch = 0.0;
  roll = 0.0;
  x = 0.0;
  y = 3.0;
  z = -5.0;
}

vec dof(void) {
  vec result;

  double dx = sin(heading * M_PI/180.0);
  double dy = -sin(pitch * M_PI/180.0);
  double dz = cos(heading * M_PI/180.0);

  double d = sqrt(dx*dx + dy*dy + dz*dz);

  result.x = dx / d;
  result.y = dy / d;
  result.z = dz / d;

  return result;
}

void plane(void) {
  static GLUquadric *quad = gluNewQuadric();

  glPushMatrix();

  glTranslatef(0.0, 0.0, 2.5);
  glColor3f(1.0, 1.0, 1.0);
  gluCylinder(quad, 0.5, 0.5, 5, 20, 20);

  glPushMatrix();
  glBegin(GL_TRIANGLES);
  //wings
  glVertex3f(0.5, 0.0, 3);
  glVertex3f(3.0, 0.0, 1);
  glVertex3f(0.5, 0.0, 1);
  glVertex3f(-0.5, 0.0, 3);
  glVertex3f(-3.0, 0.0, 1);
  glVertex3f(-0.5, 0.0, 1);
  //tail
  glVertex3f(0.0, 0.5, 1);
  glVertex3f(0.0, 2.0, 0);
  glVertex3f(0.0, 0.5, 0);
  glEnd();
  glPopMatrix();

  glPopMatrix();
}

void render(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  if (c_mode == GROUND)
  	gluLookAt(0, 5, 5,
              x, y, z,
              0, 1, 0);
  else {
    glTranslatef(-x, -y, -z);
    glRotatef(180 -heading, 0, 1, 0);
    glRotatef(roll, 0, 0, 1);
    glRotatef(pitch, 1, 0, 0);
  }

  if (roll != 0)
  	heading += roll / -90.0;

  glBegin(GL_QUADS);

  //grass
  glColor3f(0.7, 1.0, 0.7);
  glVertex3f(-100.0, -0.1, 0.0);
  glVertex3f(0.0, -0.1, -100.0);
  glVertex3f(100.0, -0.1, 0.0);
  glVertex3f(0.0, -0.1, 100.0);

  //runway
  glColor3f(0.7, 0.7, 0.7);
  glVertex3f(-10.0, 0.0, -100.0);
  glVertex3f(10.0, 0.0, -100.0);
  glVertex3f(10.0, 0.0, 100.0);
  glVertex3f(-10.0, 0.0, 100.0);
  glEnd();

  //towers
  glPushMatrix();
  glTranslatef(-30, 5, -50);
  glScalef(2, 10, 2);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-35, 5, -50);
  glScalef(2, 10, 2);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 5, -90);
  glScalef(2, 10, 2);
  glutSolidCube(1);
  glPopMatrix();

  //plane
  if (c_mode == GROUND) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(heading, 0, 1, 0);
  glRotatef(pitch, 1, 0, 0);
  glRotatef(roll, 0, 0, 1);
  plane();
  glPopMatrix();
  }

  glBegin(GL_LINES);
  //red - x
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-100.0, 0.0, 0.0);
  glVertex3f(100.0, 0.0, 0.0);

  //green - y
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, -100.0, 0.0);
  glVertex3f(0.0, 100.0, 0.0);

  //blue - z
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, -100.0);
  glVertex3f(0.0, 0.0, 100.0);
  glEnd();

  glutSwapBuffers();
}

void reshape(int width, int height) {
  float ratio =  width * 1.0 / height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glViewport(0, 0, width, height);
  gluPerspective(45.0f, ratio, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
  vec v = dof();
  x += v.x * 0.1;
  y += v.y * 0.1;
  z += v.z * 0.1;

  glutPostRedisplay();
}

void keyboard(unsigned char key, int mouse_x, int mouse_y) {
  if (key == ESC)
    exit(0);
  else if (key == ' ')
  	reset();
  else if (key == '1')
  	c_mode = GROUND;
  else if (key == '2')
  	c_mode = FIRST;

  glutPostRedisplay();
}

void special(int key, int x, int y) {
  if (key == GLUT_KEY_LEFT)
  	roll -= 2.0f;
  else if (key == GLUT_KEY_RIGHT)
  	roll += 2.0f;
  else if (key == GLUT_KEY_UP)
  	pitch -= 2.0f;
  else if (key == GLUT_KEY_DOWN)
  	pitch += 2.0f;

  roll = CLAMP(roll, -90.0, 90.0);
  pitch = CLAMP(pitch, -90.0, 90.0);

  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {

}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  //create window
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(400, 400);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Blake Lockley - 44587643");

  //callbacks
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);

  //opengl init
  glClearColor(0.4f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  reset();

  glutMainLoop();

  return 1;
}
