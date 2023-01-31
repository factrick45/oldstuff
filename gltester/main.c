#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include <GL/glu.h>

#define PI 3.1415926535

GLFWwindow * window;

float ppos[2] = {127, 127};
float pyaw = 0;
float pfvec[2] = {0.0, 0.0}; // forward vector
float psvec[2] = {0.0, 0.0}; // side vector

double delta = 0.0;

double getDelta()
{
	double time = glfwGetTime();
	glfwSetTime(0.0);
	return time;
}

void phys()
{
	if (pyaw > 2*PI) { pyaw = 0; }
	if (pyaw < 0) { pyaw = 2 * PI; }

	pfvec[0] = cosf(pyaw);
	pfvec[1] = sinf(pyaw);
	psvec[0] = cosf(pyaw + PI/2);
	psvec[1] = sinf(pyaw + PI/2);
}

void input()
{
	if (glfwGetKey(window, GLFW_KEY_W)) {
		ppos[0] += pfvec[0] * delta * 128;
		ppos[1] += pfvec[1] * delta * 128;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		ppos[0] -= pfvec[0] * delta * 64;
		ppos[1] -= (pfvec[1] * delta * 64);
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		pyaw -= 3 * delta;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		pyaw += 3 * delta;
	}
}

int main()
{
	glfwInit();
	
	glfwWindowHint(GLFW_SAMPLES, 2); // 2x AA

	window = glfwCreateWindow( 640, 480, "window", NULL, NULL);

	glfwMakeContextCurrent(window);

	glClearColor(0.3, 0.3, 0.3, 1.0);

	gluOrtho2D(0, 640, 480, 0);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		delta = getDelta();

		phys();
		input();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPointSize(8);
		glBegin(GL_POINTS);

		// player
		glColor3f(1, 1, 0);
		glVertex2i(ppos[0], ppos[1]);

		glEnd();

		glLineWidth(2);
		glBegin(GL_LINES);

		// forward vector
		glColor3f(0, 0, 1);
		glVertex2i(ppos[0], ppos[1]);
		glVertex2i(ppos[0] + pfvec[0] * 32, ppos[1] + pfvec[1] * 32);

		glColor3f(0, 1, 0);
		glVertex2i(ppos[0], ppos[1]);
		glVertex2i(ppos[0] + psvec[0] * 32, ppos[1] + psvec[1] * 32);

		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0
	);
}
