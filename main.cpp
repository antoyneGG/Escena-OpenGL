//=============================================================================
// Sample Application: Lighting (Per Fragment Phong)
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "glm.h"
#include <FreeImage.h> //*** Para Textura: Incluir librería
#include <math.h>

//-----------------------------------------------------------------------------


class myWindow : public cwc::glutWindow
{
protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   cwc::glShader* shader1; //Para Textura: variable para abrir los shader de textura
   GLuint ProgramObject;
   clock_t time0,time1;
   float timer010;  // timer counting 0->1->0
   bool bUp;        // flag if counting up or down.
   GLMmodel* escena;
   GLMmodel* bunny;
   GLMmodel* robot;
   GLMmodel* bandit; //*** Para Textura: variable para objeto texturizado
   GLMmodel* soldier;
   GLMmodel* palma;
   GLMmodel* piramides;
   GLuint texid[2]; //*** Para Textura: variable que almacena el identificador de textura, se utiliza un array debido a que hay dos objetos que utilizan distintas texturas (soldado y bandido)
   float yRotate = 0.0f;
   bool keyQ = false, keyE = false, xLeft = false, xRight = false, zForward = false, zBackwards = false, sprint = false;
   float xAccel = 0.0f, zAccel = 0.0f, xPos = 0.0f, zPos = 0.0f, accelRange;


public:
	myWindow(){}

	//*** Para Textura: aqui adiciono un método que abre la textura en JPG
	void initialize_bandit_textures(void)
	{
		int w, h;
		GLubyte* data = 0;
		//data = glmReadPPM("soccer_ball_diffuse.ppm", &w, &h);
		//std::cout << "Read soccer_ball_diffuse.ppm, width = " << w << ", height = " << h << std::endl;

		//dib1 = loadImage("soccer_ball_diffuse.jpg"); //FreeImage

		glGenTextures(1, &texid[0]);
		glBindTexture(GL_TEXTURE_2D, texid[0]);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Loading JPG file
		FIBITMAP* bitmap = FreeImage_Load(
			FreeImage_GetFileType("./Mallas/BomerFree_BasicBandit_Cloth_AlbedoTransparency.png", 0),
			"./Mallas/BomerFree_BasicBandit_Cloth_AlbedoTransparency.png");  //*** Para Textura: esta es la ruta en donde se encuentra la textura

		FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
			0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

		FreeImage_Unload(pImage);
		//
		glEnable(GL_TEXTURE_2D);
	}

	void moveBandit()
	{
		// Maxima velocidad (aumenta si se corre con espacio presionado)
		accelRange = 2.0f + 3.0f * sprint;
		// Ajustar aceleracion en el eje X
		if (xLeft)
		{
			xAccel -= 0.1f;
		}
		else if (xRight)
		{
			xAccel += 0.1f;
		}
		else
		{
			if (xAccel > 0)
			{
				xAccel -= 0.05f;
			}
			else if (xAccel < 0)
			{
				xAccel += 0.05f;
			}
		}
		// Ajustar aceleracion en el eje Z
		if (zForward)
		{
			zAccel -= 0.1f;
		}
		else if (zBackwards)
		{
			zAccel += 0.1f;
		}
		else
		{
			if (zAccel > 0)
			{
				zAccel -= 0.05f;
			}
			else if (zAccel < 0)
			{
				zAccel += 0.05f;
			}
		}
		// Asegurar que la aceleracion no supere el limite
		if (xAccel > accelRange)
		{
			xAccel = accelRange;
		}
		else if (xAccel < -accelRange)
		{
			xAccel = -accelRange;
		}
		if (zAccel > accelRange)
		{
			zAccel = accelRange;
		}
		else if (zAccel < -accelRange)
		{
			zAccel = -accelRange;
		}
		// Check para corregir error punto flotante
		if (xAccel > -0.00001 && xAccel < 0.00001)
		{
			xAccel = 0;
		}
		if (zAccel > -0.00001 && zAccel < 0.00001)
		{
			zAccel = 0;
		}
		// Movimiento
		xPos += 0.01f * xAccel;
		zPos += 0.01f * zAccel;
		std::cout << xAccel << "\n";
		std::cout << sprint << "\n";
		glTranslatef(xPos, 0.0f, zPos);
	}

	void initialize_soldier_textures(void)
	{
		int w, h;
		GLubyte* data = 0;
		//data = glmReadPPM("soccer_ball_diffuse.ppm", &w, &h);
		//std::cout << "Read soccer_ball_diffuse.ppm, width = " << w << ", height = " << h << std::endl;

		//dib1 = loadImage("soccer_ball_diffuse.jpg"); //FreeImage

		glGenTextures(1, &texid[1]);
		glBindTexture(GL_TEXTURE_2D, texid[1]);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Loading JPG file
		FIBITMAP* bitmap = FreeImage_Load(
			FreeImage_GetFileType("./Mallas/Type02/Tex_0013_1.png", 0),
			"./Mallas/Type02/Tex_0013_1.png");  //*** Para Textura: esta es la ruta en donde se encuentra la textura

		FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
			0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

		FreeImage_Unload(pImage);
		//
		glEnable(GL_TEXTURE_2D);
	}


	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      //timer010 = 0.09; //for screenshot!
		//Tecla E para rotar la camara hacia la derecha
		//Tecla Q para rotar la camara hacia la izquierda

		if (keyE) {
			yRotate = yRotate + 1;
		}
		if (keyQ) {
			yRotate = yRotate - 1;
		}
		
      glPushMatrix();
	  glTranslatef(0.0f, 0.0f, -20.0f); // Z Original: -20.0f
	  glRotatef(yRotate, 0.0, 1.0f, 0.0f); //Rotacion de la camara

      if (shader) shader->begin();
		  
	  
		  glPushMatrix();
		  glTranslatef(0.0f, 4.0f, 2.0f);
		  glScalef(15.0f, 15.0f, 15.0f);
		  glmDraw(escena, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();
		  

		  glPushMatrix();
		  glTranslatef(0.0f, 1.0f, 0.0f);
		  glScalef(30.0f, 30.0f, 30.0f);
		  glmDraw(piramides, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();
	

		  glPushMatrix();
		  glTranslatef(2.5f, 0.0f, -4.0f);
		  glmDraw(robot, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();

		  glPushMatrix();
		  glTranslatef(4.5f, 0.0f, -7.0f);
		  glmDraw(robot, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();

		  glPushMatrix();
		  glTranslatef(-0.4f, 0.0f, -7.4f);
		  glRotatef(-90, 0.0f, 1.0f, 0.0f);
		  glmDraw(soldier, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();

		  glPushMatrix();
		  glTranslatef(0.4f, 0.0f, -7.4f);
		  glRotatef(90, 0.0f, 1.0f, 0.0f);
		  glmDraw(soldier, GLM_SMOOTH | GLM_MATERIAL);
		  glPopMatrix();

      if (shader) shader->end();

	  //*** Para Textura: llamado al shader para objetos texturizados
	  if (shader1) shader1->begin();

		//Bandido interactivo con funcion moveBandit(), WASD utilizado para su movimiento y ESPACIO para sprint
		  glPushMatrix();
		  glTranslatef(1.9f, 0.0f, 1.2f);
		  glBindTexture(GL_TEXTURE_2D, texid[0]);
		  moveBandit();
		  glmDraw(bandit, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		  glPopMatrix();

		  glPushMatrix();
		  glTranslatef(2.5f, 0.0f, 1.9f);
		  glRotatef(-135, 0.0f, 1.0f, 0.0f);
		  glBindTexture(GL_TEXTURE_2D, texid[0]);
		  glmDraw(bandit, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		  glPopMatrix();

		  glPushMatrix();
		  glTranslatef(1.0f, 0.0f, 1.9f);
		  glRotatef(135, 0.0f, 1.0f, 0.0f);
		  glBindTexture(GL_TEXTURE_2D, texid[0]);
		  glmDraw(bandit, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		  glPopMatrix();

		  
	  if (shader1) shader1->end();


      glutSwapBuffers();
      glPopMatrix();

      UpdateTimer();

		Repaint();
	}

	virtual void OnIdle() {}

	// When OnInit is called, a render context (in this case GLUT-Window) 
	// is already available!
	virtual void OnInit()
	{
		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);

		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		if (shader==0) 
         std::cout << "Error Loading, compiling or linking shader\n";
      else
      {
         ProgramObject = shader->GetProgramObject();
      }

	 //*** Para Textura: abre los shaders para texturas
		shader1 = SM.loadfromFile("vertexshaderT.txt", "fragmentshaderT.txt"); // load (and compile, link) from file
		if (shader1 == 0)
			std::cout << "Error Loading, compiling or linking shader\n";
		else
		{
			ProgramObject = shader1->GetProgramObject();
		}

      time0 = clock();
      timer010 = 0.0f;
      bUp = true;

	  //Abrir mallas
	  //Escena estatica del mundo virtual (incluye edificios, carreteras, arboles, semaforos, lamparas y automoviles)
	  escena = NULL;

	  if (!escena)
	  {
		  escena = glmReadOBJ("./Mallas/escena.obj");
		  if (!escena)
			  exit(0);

		  glmUnitize(escena);
		  glmFacetNormals(escena);
		  glmVertexNormals(escena, 90.0);
	  }
	 
	  //Robot realizado por nosotros en blender
	  robot = NULL;

	  if (!robot)
	  {
		  robot = glmReadOBJ("./Mallas/estatua.obj");
		  if (!robot)
			  exit(0);

		  glmUnitize(robot);
		  glmFacetNormals(robot);
		  glmVertexNormals(robot, 90.0);
	  }


	  //Bandidos rebeldes del mundo (uno de ellos es interactivo)
	  bandit = NULL;

	  if (!bandit)
	  {
		  bandit = glmReadOBJ("./Mallas/BasicBanditBlend.obj");
		  if (!bandit)
			  exit(0);

		  glmUnitize(bandit);
		  glmFacetNormals(bandit);
		  glmVertexNormals(bandit, 90.0);
	  }

	  //Soldados del gran hermano (estaticos)
	  soldier = NULL;

	  if (!soldier)
	  {
		  soldier = glmReadOBJ("./Mallas/Soldier.obj");
		  if (!soldier)
			  exit(0);

		  glmUnitize(soldier);
		  glmFacetNormals(soldier);
		  glmVertexNormals(soldier, 90.0);
	  }

	  //Piramides y sus robots guardianes
	  piramides = NULL;

	  if (!piramides)
	  {
		  piramides = glmReadOBJ("./Mallas/piramides.obj");
		  if (!piramides)
			  exit(0);

		  glmUnitize(piramides);
		  glmFacetNormals(piramides);
		  glmVertexNormals(piramides, 90.0);
	  }
 
	  //*** Para Textura: abrir archivo de textura
	  initialize_bandit_textures();
      DemoLight();

	}

	virtual void OnResize(int w, int h)
   {
      if(h == 0) h = 1;
	   float ratio = 1.0f * (float)w / (float)h;

      glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	
	   glViewport(0, 0, w, h);

      gluPerspective(45,ratio,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(0.0f,0.0f,4.0f, 
		          0.0,0.0,-1.0,
			       0.0f,1.0f,0.0f);
   }
	virtual void OnClose(void){}
	virtual void OnMouseDown(int button, int x, int y) {}    
	virtual void OnMouseUp(int button, int x, int y) {}
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){

	}

	virtual void OnKeyDown(int nKey, char cAscii)
	{       
		switch(cAscii)
		{
		case 27: // 0x1b = ESC
			this->Close(); // Close Window!
			break;
		case 'e':
			keyE = true;
			break;
		case 'q':
			keyQ = true;
			break;
		case 'a':
			xLeft = true;
			break;
		case 'd':
			xRight = true;
			break;
		case 'w':
			zForward = true;
			break;
		case 's':
			zBackwards = true;
			break;
		case ' ':
			sprint = true;
			break;
		}
	};

	virtual void OnKeyUp(int nKey, char cAscii)
	{
		switch (cAscii)
		{
		case 'e':
			keyE = false;
			break;
		case 'q':
			keyQ = false;
			break;
		case 'a':
			xLeft = false;
			break;
		case 'd':
			xRight = false;
			break;
		case 'w':
			zForward = false;
			break;
		case 's':
			zBackwards = false;
			break;
		case ' ':
			sprint = false;
			break;
		case 'n':
			shader->enable(); // shader
			break;
		case 'm':
			shader->disable(); // fixed function
			break;
		}
		
	}

   void UpdateTimer()
   {
      time1 = clock();
      float delta = static_cast<float>(static_cast<double>(time1-time0)/static_cast<double>(CLOCKS_PER_SEC));
      delta = delta / 4;
      if (delta > 0.00005f)
      {
         time0 = clock();
         if (bUp)
         {
            timer010 += delta;
            if (timer010>=1.0f) { timer010 = 1.0f; bUp = false;}
         }
         else
         {
            timer010 -= delta;
            if (timer010<=0.0f) { timer010 = 0.0f; bUp = true;}
         }
      }
   }

   void DemoLight(void)
   {
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
     
     // Light model parameters:
     // -------------------------------------------
     /*
     GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
     glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
     glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
     // -------------------------------------------
     // Spotlight Attenuation
     
     GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
     GLint spot_exponent = 30;
     GLint spot_cutoff = 180;
     
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
     glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
     glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
     GLfloat Kc = 1.0;
     GLfloat Kl = 0.0;
     GLfloat Kq = 0.0;
     
     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
     // ------------------------------------------- 
     // Lighting parameters:

     GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
     GLfloat light_Ka[]  = {1.0f, 0.5f, 0.5f, 1.0f};
     GLfloat light_Kd[]  = {1.0f, 0.1f, 0.1f, 1.0f};
     GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

     // -------------------------------------------
     // Material parameters:

     GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
     GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
     GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
     GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
     GLfloat material_Se = 20.0f;

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
	 */
   }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() {std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------

