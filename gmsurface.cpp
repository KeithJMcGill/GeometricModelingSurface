#include "Angel.h"
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

#define RANGE_MIN -0.75
#define RANGE_MAX 0.75
#define WIN_HEIGHT 500
#define WIN_WIDTH 500

using namespace std;

int numControl = 0, numControlSq;
int sizeOfSurfaceControlPoints, sizeOfSurfaceControlLines, sizeOfNormalSurface;
double t[] = {0, 1, 2, 3, 4, 5, 6, 7};
vec4 **surfaceControl;
vec4 *surfaceControlPoints;
vec4 *surfaceControlLines;
vec4 **arraySurfacePoints;
vec3 **normalSurfacePoints;
vec4 *surfacePoints;
vec3 *normalSurface;

const float degToRad = M_PI / 180.0;
float angle = 0.01 * degToRad;
int leftButton = 0, rightButton = 0;

// Forward declarations
void normalizeMesh();
void normalizeSurface();

void readFile(string filename)
{
	ifstream infile;
	infile.open(filename.c_str());
	infile >> numControl;
	numControlSq = sqrt(numControl);
	surfaceControl = new vec4*[numControlSq];
	surfaceControlPoints = new vec4[numControl];
	for (int i = 0; i < numControlSq; i++)
	{
		surfaceControl[i] = new vec4[numControlSq];
	}
	int countPoints = 0;
	for (int i = 0; i < numControlSq; i++)
	{
		for (int j = 0; j < numControlSq; j++)
		{
			infile >> surfaceControl[i][j].x >> surfaceControl[i][j].y >> surfaceControl[i][j].z;
			surfaceControlPoints[countPoints] = vec4(surfaceControl[i][j].x, surfaceControl[i][j].y, surfaceControl[i][j].z, 1);
			countPoints++;
		}
	}

	//Control Box Lines
	surfaceControlLines = new vec4[3 * numControl];

	//Bottom Left
	surfaceControlLines[0] = surfaceControlPoints[0];
	surfaceControlLines[1] = surfaceControlPoints[1];
	surfaceControlLines[2] = surfaceControlPoints[1];
	surfaceControlLines[3] = surfaceControlPoints[5];
	surfaceControlLines[4] = surfaceControlPoints[5];
	surfaceControlLines[5] = surfaceControlPoints[4];
	surfaceControlLines[6] = surfaceControlPoints[4];
	surfaceControlLines[7] = surfaceControlPoints[0];

	//Bottom Right
	surfaceControlLines[8] = surfaceControlPoints[2];
	surfaceControlLines[9] = surfaceControlPoints[3];
	surfaceControlLines[10] = surfaceControlPoints[3];
	surfaceControlLines[11] = surfaceControlPoints[7];
	surfaceControlLines[12] = surfaceControlPoints[7];
	surfaceControlLines[13] = surfaceControlPoints[6];
	surfaceControlLines[14] = surfaceControlPoints[6];
	surfaceControlLines[15] = surfaceControlPoints[2];

	//Top Left
	surfaceControlLines[16] = surfaceControlPoints[8];
	surfaceControlLines[17] = surfaceControlPoints[9];
	surfaceControlLines[18] = surfaceControlPoints[9];
	surfaceControlLines[19] = surfaceControlPoints[13];
	surfaceControlLines[20] = surfaceControlPoints[13];
	surfaceControlLines[21] = surfaceControlPoints[12];
	surfaceControlLines[22] = surfaceControlPoints[12];
	surfaceControlLines[23] = surfaceControlPoints[8];

	//Top Right
	surfaceControlLines[24] = surfaceControlPoints[10];
	surfaceControlLines[25] = surfaceControlPoints[11];
	surfaceControlLines[26] = surfaceControlPoints[11];
	surfaceControlLines[27] = surfaceControlPoints[15];
	surfaceControlLines[28] = surfaceControlPoints[15];
	surfaceControlLines[29] = surfaceControlPoints[14];
	surfaceControlLines[30] = surfaceControlPoints[14];
	surfaceControlLines[31] = surfaceControlPoints[10];

	//Bottom
	surfaceControlLines[32] = surfaceControlPoints[1];
	surfaceControlLines[33] = surfaceControlPoints[2];
	surfaceControlLines[34] = surfaceControlPoints[5];
	surfaceControlLines[35] = surfaceControlPoints[6];

	//Left
	surfaceControlLines[36] = surfaceControlPoints[4];
	surfaceControlLines[37] = surfaceControlPoints[8];
	surfaceControlLines[38] = surfaceControlPoints[5];
	surfaceControlLines[39] = surfaceControlPoints[9];

	//Right
	surfaceControlLines[40] = surfaceControlPoints[7];
	surfaceControlLines[41] = surfaceControlPoints[11];
	surfaceControlLines[42] = surfaceControlPoints[6];
	surfaceControlLines[43] = surfaceControlPoints[10];

	//Top
	surfaceControlLines[44] = surfaceControlPoints[13];
	surfaceControlLines[45] = surfaceControlPoints[14];
	surfaceControlLines[46] = surfaceControlPoints[9];
	surfaceControlLines[47] = surfaceControlPoints[10];

	infile.close();
}

double N(int i, int k, double u)
{
	if (k == 0 && u >= t[i] && u < t[i + 1])
	{
		return 1;
	}
	else if (k == 0 && (u < t[i] || u >= t[i + 1]))
	{
		return 0;
	}
	else
	{
		return((((u - t[i]) * N(i, k - 1, u)) / (t[i + k] - t[i])) 
			+ (((t[i + k + 1] - u) * N(i + 1, k - 1, u)) / (t[i + k + 1] - t[i + 1])));
	}
}

void createSurface()
{
	int numPoints = 0;
	double valueX = 0, valueY = 0, valueZ = 0, u = 0, w = 0;

	//Surface Points
	arraySurfacePoints = new vec4*[10];
	surfacePoints = new vec4[100];
	for (int i = 0; i < 10; i++)
	{
		arraySurfacePoints[i] = new vec4[10];
	}
	for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			u = 3 + (double)a / (10 - 1);
			w = 3 + (double)b / (10 - 1);
			double m = 0;
			for (int i = 0; i <= 3; i++)
			{
				for (int j = 0; j <= 3; j++)
				{
					m += N(i, 3, u) * N(j, 3, w);
					valueX = surfaceControl[i][j].x * N(i, 3, u) * N(j, 3, w);
					valueY = surfaceControl[i][j].y * N(i, 3, u) * N(j, 3, w);
					valueZ = surfaceControl[i][j].z * N(i, 3, u) * N(j, 3, w);
					arraySurfacePoints[a][b] += vec4(valueX, valueY, valueZ, 0);
				}
			}
			arraySurfacePoints[a][b].w = 1;
			surfacePoints[numPoints] = arraySurfacePoints[a][b];
			numPoints++;
		}
	}
	normalizeMesh();
}

void normalizeMesh()
{
    float cx = 0, cy = 0, cz = 0; // centroid
    float minX = 1e10, maxX = -minX, minY = minX, maxY = maxX, 
        minZ = minX, maxZ = maxX;
    
    for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			cx += arraySurfacePoints[a][b].x;
			cy += arraySurfacePoints[a][b].y;
			cz += arraySurfacePoints[a][b].z;
		}
	}
    cx /= 100;
    cy /= 100;
    cz /= 100;
    
	for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			arraySurfacePoints[a][b].x -= cx;
			arraySurfacePoints[a][b].y -= cy;
			arraySurfacePoints[a][b].z -= cz;
		}
	}
    
	for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			minX = min(arraySurfacePoints[a][b].x, minX);   
			minY = min(arraySurfacePoints[a][b].y, minY);
			minZ = min(arraySurfacePoints[a][b].z, minZ);
			maxX = max(arraySurfacePoints[a][b].x, maxX);
			maxY = max(arraySurfacePoints[a][b].y, maxY);
			maxZ = max(arraySurfacePoints[a][b].z, maxZ);
		}
	}
    float sizeX = maxX - minX;
    float sizeY = maxY - minY;
    float sizeZ = maxZ - minZ;
    float maxSize = max(sizeX, sizeY);
    maxSize = max(maxSize, sizeZ);
    
    float scale = 1.5f / maxSize;
	for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			arraySurfacePoints[a][b].x *= scale;
			arraySurfacePoints[a][b].y *= scale;
			arraySurfacePoints[a][b].z *= scale;
		}
	}
	normalizeSurface();
}

void normalizeSurface()
{
	double v1x, v1y, v1z, v2x, v2y, v2z, ox, oy, oz, q, nx, ny, nz;

	//Normal Surface Points
	normalSurfacePoints = new vec3*[10];
	for (int i = 0; i < 10; i++)
	{
		normalSurfacePoints[i] = new vec3[10];
	}

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			v1x = arraySurfacePoints[i][j + 1].x - arraySurfacePoints[i][j].x;
			v1y = arraySurfacePoints[i][j + 1].y - arraySurfacePoints[i][j].y;
			v1z = arraySurfacePoints[i][j + 1].z - arraySurfacePoints[i][j].z;

			v2x = arraySurfacePoints[i + 1][j].x - arraySurfacePoints[i][j].x;
			v2y = arraySurfacePoints[i + 1][j].y - arraySurfacePoints[i][j].y;
			v2z = arraySurfacePoints[i + 1][j].z - arraySurfacePoints[i][j].z;

			ox = (v1y * v2z) - (v2y * v1z);
			oy = (v1z * v2x) - (v2z * v1x);
			oz = (v1x * v2y) - (v2x * v1y);

			q = sqrt((pow(ox,2)) + (pow(oy, 2)) + (pow(oz, 2)));

			nx = ox / q;
			ny = oy / q;
			nz = oz / q;
			
			normalSurfacePoints[i][j] = vec3(nx, ny, nz);
			normalSurfacePoints[i][j + 1] = vec3(nx, ny, nz);
			normalSurfacePoints[i + 1][j + 1] = vec3(nx, ny, nz);
			normalSurfacePoints[i + 1][j] = vec3(nx, ny, nz);
		}
	}

	int normPoints = 0;
	normalSurface = new vec3[100];
	for (int a = 0; a < 10; a++)
	{
		for (int b = 0; b < 10; b++)
		{
			normalSurface[normPoints] = normalSurfacePoints[a][b];
			normPoints++;
		}
	}
}

void idleRotateZ()
{
	if (leftButton == 1)
	{
		//Control Points
		for (int i = 0; i < numControl; i++)
		{
			float x = cos(angle)*surfaceControlPoints[i].x - sin(angle)*surfaceControlPoints[i].y;
			float y = sin(angle)*surfaceControlPoints[i].x + cos(angle)*surfaceControlPoints[i].y;
			surfaceControlPoints[i].x = x;
			surfaceControlPoints[i].y = y;
		}

		//Control Lines
		for (int i = 0; i < 3 * numControl; i++)
		{
			float x = cos(angle)*surfaceControlLines[i].x - sin(angle)*surfaceControlLines[i].y;
			float y = sin(angle)*surfaceControlLines[i].x + cos(angle)*surfaceControlLines[i].y;
			surfaceControlLines[i].x = x;
			surfaceControlLines[i].y = y;
		}

		//Control Lines
		for (int i = 0; i < 100; i++)
		{
			float x = cos(angle)*surfacePoints[i].x - sin(angle)*surfacePoints[i].y;
			float y = sin(angle)*surfacePoints[i].x + cos(angle)*surfacePoints[i].y;
			surfacePoints[i].x = x;
			surfacePoints[i].y = y;
		}

		//Surface QUADS
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				float x = cos(angle)*arraySurfacePoints[i][j].x - sin(angle)*arraySurfacePoints[i][j].y;
				float y = sin(angle)*arraySurfacePoints[i][j].x + cos(angle)*arraySurfacePoints[i][j].y;
				arraySurfacePoints[i][j].x = x;
				arraySurfacePoints[i][j].y = y;
			}
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfSurfaceControlPoints, surfaceControlPoints);
		glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints, sizeOfSurfaceControlLines, surfaceControlLines);
		glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints + sizeOfSurfaceControlLines, sizeOfNormalSurface, normalSurface);
	}
	if (rightButton == 1)
	{
		//Control Points
		for (int i = 0; i < numControl; i++)
		{
			float y = cos(angle)*surfaceControlPoints[i].y - sin(angle)*surfaceControlPoints[i].z;
			float z = sin(angle)*surfaceControlPoints[i].y + cos(angle)*surfaceControlPoints[i].z;
			surfaceControlPoints[i].z = z;
			surfaceControlPoints[i].y = y;
		}

		//Control Lines
		for (int i = 0; i < 3 * numControl; i++)
		{
			float y = cos(angle)*surfaceControlLines[i].y - sin(angle)*surfaceControlLines[i].z;
			float z = sin(angle)*surfaceControlLines[i].y + cos(angle)*surfaceControlLines[i].z;
			surfaceControlLines[i].z = z;
			surfaceControlLines[i].y = y;
		}

		//Control Lines
		for (int i = 0; i < 100; i++)
		{
			float y = cos(angle)*surfacePoints[i].y - sin(angle)*surfacePoints[i].z;
			float z = sin(angle)*surfacePoints[i].y + cos(angle)*surfacePoints[i].z;
			surfacePoints[i].z = z;
			surfacePoints[i].y = y;
		}
		
		//Surface QUADS
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				float y = cos(angle)*arraySurfacePoints[i][j].y - sin(angle)*arraySurfacePoints[i][j].z;
				float z = sin(angle)*arraySurfacePoints[i][j].y + cos(angle)*arraySurfacePoints[i][j].z;
				arraySurfacePoints[i][j].z = z;
				arraySurfacePoints[i][j].y = y;
			}
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfSurfaceControlPoints, surfaceControlPoints);
		glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints, sizeOfSurfaceControlLines, surfaceControlLines);
		glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints + sizeOfSurfaceControlLines, sizeOfNormalSurface, normalSurface);
	}
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftButton = 0;
		glutIdleFunc(NULL);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		leftButton = 1;
		glutIdleFunc(idleRotateZ);
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		rightButton = 0;
		glutIdleFunc(NULL);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rightButton = 1;
		glutIdleFunc(idleRotateZ);
	}
	
	glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
	
}

void keyboard(unsigned char key, int width, int height)
{
	switch( key ) 
	{
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Surface QUADS
	glBegin(GL_QUADS);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			glVertex3d(arraySurfacePoints[i][j].x, arraySurfacePoints[i][j].y, arraySurfacePoints[i][j].z);
			glVertex3d(arraySurfacePoints[i][j + 1].x, arraySurfacePoints[i][j + 1].y, arraySurfacePoints[i][j + 1].z);
			glVertex3d(arraySurfacePoints[i + 1][j + 1].x, arraySurfacePoints[i + 1][j + 1].y, arraySurfacePoints[i + 1][j + 1].z);
			glVertex3d(arraySurfacePoints[i + 1][j].x, arraySurfacePoints[i + 1][j].y, arraySurfacePoints[i + 1][j].z);
		}
	}
	glEnd();
	//Control Points
	glDrawArrays(GL_POINTS, 0, numControl);
	//Control Box
	glDrawArrays(GL_LINES, numControl, 3 * numControl);
    glFlush();
}

void init()
{   
    // Vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer );
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
    // Empty buffer
	glBufferData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints + sizeOfSurfaceControlLines + sizeOfNormalSurface, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfSurfaceControlPoints, surfaceControlPoints);
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints, sizeOfSurfaceControlLines, surfaceControlLines);
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfSurfaceControlPoints + sizeOfSurfaceControlLines, sizeOfNormalSurface, normalSurface);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader    
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeOfSurfaceControlPoints + sizeOfSurfaceControlLines));

	// Initialize shader lighting parameters
    vec4 light_position( 0.0, 0.0, -1.0, 0.0 );
    vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    vec4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    vec4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    vec4 ambient_product = light_ambient * material_ambient;
    vec4 diffuse_product = light_diffuse * material_diffuse;
    vec4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product );
	
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		  1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );

    glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
    glClearColor(0, 0, 0, 0);
	glPointSize(5);
	glLineWidth(2.5);
}

int main(int argc, char **argv)
{
	string filename = "SurfaceControlPoints.txt";
	readFile(filename);

	createSurface();

	sizeOfSurfaceControlPoints = numControl * sizeof(vec4);
	sizeOfSurfaceControlLines = (3 * numControl) * sizeof(vec4);
	sizeOfNormalSurface = 100 * sizeof(vec3);

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Geometric Modeling");

	glewExperimental = GL_TRUE;
	glewInit();
	init();	

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(NULL);

    glutMainLoop();
	return 0;
}