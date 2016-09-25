#include <stdio.h>
#include <GL/glut.h>    
#include<mmsystem.h>
float r = 0.0;//speed
GLUquadricObj *quadratic;//PLANETS' ARRAY
GLuint texture[10];//10 images
//Images' size and bit infos
struct Image
{
	unsigned long sizeX;
	unsigned long sizeY;
	GLubyte *data;
};

static unsigned int getint(FILE *fp)
{
	int c, c1, c2, c3;

	// get 4 bytes
	c = getc(fp);
	c1 = getc(fp);
	c2 = getc(fp);
	c3 = getc(fp);

	return ((unsigned int)c) +
		(((unsigned int)c1) << 8) +
		(((unsigned int)c2) << 16) +
		(((unsigned int)c3) << 24);
}

static unsigned short getshort(FILE* fp)
{
	int c, c1;
	//get 2 bytes
	c = getc(fp);
	c1 = getc(fp);

	return ((unsigned int)c) + (((unsigned int)c1) << 8);
}
//OPEN FILE
int ImageLoad(char *filename, Image *image)
{
	FILE *file;
	unsigned long size;               
	size_t i;					
	unsigned short int planes;          
	unsigned short int bpp;          
	char temp;                          
	if ((file = fopen(filename, "rb")) == NULL)
	{
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	fseek(file, 18, SEEK_CUR);

	image->sizeX = getint(file);
	image->sizeY = getint(file);
	size = image->sizeX * image->sizeY * 3;
	planes = getshort(file);
	if (planes != 1)
	{
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	bpp = getshort(file);
	if (bpp != 24)
	{
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data. 
	image->data = new GLubyte[size];
	if (image->data == NULL)
	{
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}

	if ((i = fread(image->data, size, 1, file)) != 1)
	{
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}

	// reverse all of the colors. (bgr -> rgb)
	for (i = 0; i<size; i += 3)
	{
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}

	// we're done.
	return 1;
}

void LoadGLTextures() {
	Image *image[10];
	bool status;
	for (int i = 0; i < 10; i++){
		image[i] = new Image();
		if (image[i] == NULL)
		{
			printf("Error allocating space for image");
			status = false;
		}
	}
	//bmp files
	if ((ImageLoad("Data/sun.bmp", image[0]) && ImageLoad("Data/mercury.bmp", image[1]) && ImageLoad("Data/venus.bmp", image[2]) && ImageLoad("Data/earth.bmp", image[3]) && ImageLoad("Data/mars.bmp", image[4])
		&& ImageLoad("Data/jupiter.bmp", image[5]) && ImageLoad("Data/saturn.bmp", image[6]) && ImageLoad("Data/uranus.bmp", image[7]) && ImageLoad("Data/neptune.bmp", image[8]) && ImageLoad("Data/pluto.bmp", image[9])))
	{
		status = true;

	}

	glGenTextures(10, &texture[0]);
	for (int loop = 0; loop<10; loop++)
	{
		glBindTexture(GL_TEXTURE_2D, texture[loop]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, image[loop]->sizeX, image[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[loop]->data);
	}};

void InitGL()
{
	LoadGLTextures();						
	glEnable(GL_TEXTURE_2D);				
	glClearColor(0.0f, 0.0f, 0.05f, 0.1f);//background color
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);				
	quadratic = gluNewQuadric();							
	gluQuadricNormals(quadratic, GL_SMOOTH);	//smooth objects		
	gluQuadricTexture(quadratic, GL_TRUE);
	}

void ReSizeGLScene(int Width, int Height)
{
	if (Height == 0)				
		Height = 1;

	glViewport(0, 0, Width, Height);		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, (GLfloat)Width / (GLfloat)Height, 1.0,500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									

}
void spin(void) {
	r += 0.3;
	glutPostRedisplay();
}
void DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(30.0, 30.0, 10.0, 0.0, 0.0, 5.0, 5, 5, 5);	//camera
	glTranslatef(0.0f, 1.0f,5.0f);
	glEnable(GL_TEXTURE_2D);//Enable texture

	for (int b = 0; b < 10; b++){
		glBindTexture(GL_TEXTURE_2D, texture[b]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glPushMatrix();

		if (b == 0){//SUN
			gluSphere(quadratic, 2.0, 32, 32);
		}
		if (b == 1){//MERCURY

			glRotatef(r*2, 0, 0, 1);
			glTranslatef(0.0f, 5.0f, 0.0f);
			glRotatef(r, 0, 0, 1);
			gluSphere(quadratic, 0.5, 16, 16);
			
		}
		if (b == 2){//VENUS
			glRotatef(r*1.5, 0, 0, 1);
			glTranslatef(0.0f, 8.0f, 0.0f);
			glRotatef(r*1.7, 0, 0, 1);
			gluSphere(quadratic, 1, 16, 16);
		}
		if (b == 3){//EARTH
			glRotatef(r , 0, 0, 1);
			glTranslatef(0.0f, 11.0f, 0.0f);
			glRotatef(r*1.2, 0, 0, 1);
			gluSphere(quadratic, 1, 16, 16);
		}
		if (b == 4){//MARS
			glRotatef(r *0.70, 0, 0, 1);
			glTranslatef(0.0f, 14.0f, 0.0f);
			glRotatef(r, 0, 0, 1);
			gluSphere(quadratic, 0.50, 16, 16);
		}
		if (b == 5){//JUPITER
			glRotatef(r *0.8, 0, 0, 1);
			glTranslatef(0.0f, 17.0f, 0.0f);
			glRotatef(r, 0, 0, 1);
			gluSphere(quadratic, 1.5, 32, 32);
		}
		if (b == 6){//SATURN
			glRotatef(r, 0, 0, 1);
			glTranslatef(0.0f, 20.0f, 0.0f);
			glRotatef(r*1.2, 0, 0, 1);
			glutSolidTorus(0.25, 1.5, 16, 32);
			gluSphere(quadratic, 1.0, 16, 16);
		}
		
		if (b == 7){//URANUS
		glRotatef(r *0.75, 0, 0, 1);
		glTranslatef(0.0f,23.0f, 0.0f);
		glRotatef(r, 0, 0, 1);
		gluSphere(quadratic, 1, 32, 32);
		}
		if (b == 8){//NEPTUNE
		glRotatef(r *0.8, 0, 0, 1);
		glTranslatef(0.0f, 25.0f, 0.0f);
		glRotatef(r, 0, 0, 1);
		gluSphere(quadratic, 1, 32, 32);
		}
		/*if (b == 9){//PLUTO---> gezegenlikten atmýþlar:)))
		glRotatef(r, 0, 0, 1);
		glTranslatef(0.0f, 21.0f, 0.0f);
		glRotatef(r, 0, 0, 1);
		gluSphere(quadratic,0.25, 16, 16);
		}*/
		
		glPopMatrix();

	}
	glutSwapBuffers();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1366, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("SOLAR SYSTEM");
	InitGL();
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(ReSizeGLScene);
	glutIdleFunc(spin);
	PlaySound(TEXT("Data/music.wav"), NULL, SND_FILENAME | SND_ASYNC);//MUSIC FILE(ADAMLAR-KOCA YAÞLI ÞÝÞKO DÜNYA)
	glutMainLoop();
}

