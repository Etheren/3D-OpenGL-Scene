#include "windows.h"
#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"
#include "CGModel\CGModel.h"
#include "Importers\CGImporters.h"

using namespace std;
using namespace CoreStructures;

#define FIXED_FUNCTION false

float theta = 0.0f;

float playerPositionX = 0;		//Coordinates for player to move
float playerPositionY = 0;
float playerPositionZ = 0;

int mouse_x, mouse_y;
bool mDown = false;

GLuint myShaderProgram;

GUMatrix4 myModelMatrix;

GUPivotCamera *mainCamera = NULL;

float lightX0 = 55;			// XYZ coords for lights
float lightY0 = 50;
float lightZ0 = 30;
float lightX1 = -12;
float lightY1 = 43;
float lightZ1 = 33;
float lightX2 = -70;
float lightY2 = 20;
float lightZ2 = -0;

int skyMove = 0;			// SkyMove used to track how far the sky moves along

float candleMove = 0;				//candleMove is used to track how far the candle has moved
float candleLightTranslate = 0.075;	//This value changes to either positive or negative, depending on what way the candle is supposed to be moving
bool candleLeft = true;				//This is true when it's moving left,
bool candleRight = false;			//And this is true when it's moving right. The other boolean is then made false

float forwardVec = 0;
float sideVec = 0;
float upVec = 0;

GLuint gLPosition; //first light position
GLuint gDiffuse; //first light diffuse
GLuint gAmbient; //first light ambient
GLuint gAttenuation; //first light attenuation

GLuint gMatAmbient;	//Material Properties
GLuint gMatDiffuse;
GLuint gMatSpecular;

GLuint gEyePos;		//eye Position

GLuint gLPosition1; //second light position
GLuint gDiffuse1; //second light diffuse
GLuint gAmbient1; //second light ambient
GLuint gAttenuation1; //second light attenuation

GLuint gMatSpecular1;

GLuint gLPosition2; //third  light position
GLuint gDiffuse2; //third light diffuse
GLuint gAmbient2; //third light ambient
GLuint gAttenuation2; //third light attenuation

GLuint gMatSpecular2;


//Shader Matrix setup
GLuint gModelMatrix;
GLuint gViewMatrix;
GLuint gProjectionMatrix;
GLuint gNormalMatrix;

GLuint floorTexture = 0;	//Textures
GLuint wallTexture = 0;
GLuint roofTexture = 0;
GLuint woodTexture = 0;
GLuint metalTexture = 0;
GLuint skyTexture = 0;
GLuint doorTexture = 0;
GLuint shelfTexture = 0;
GLuint gold = 0;
GLuint wax = 0;

CGModel *walls = NULL;	//Models
CGModel *roof = NULL;
CGModel *floors = NULL;
CGModel *door = NULL;
CGModel *ladder = NULL;
CGModel *barrel0 = NULL;
CGModel *barrel1 = NULL;
CGModel *barrel2 = NULL;
CGModel *sky = NULL;
CGModel *chest0 = NULL;
CGModel *chest1 = NULL;
CGModel *shelf = NULL;
CGModel *candle0 = NULL;
CGModel *candle1 = NULL;

GUPivotCamera *camera = NULL; //Camera

GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 }; //Texture map will provide ambient and diffuse.
GLfloat mat_shininess[] = { 150 };				 //Shiny surface
GLfloat mat_emission[] = { 0.0, 0.0, 0.0, 0.0 }; //Not emissive
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; //White highlight


GLfloat light_position[] = { -33.0, 73.0, 50.0, 1.0 }; //Light placed at the hole
GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1.0 }; //White reflection
GLfloat light_diffuse[] = { 0.2, 0.2, 0.2, 1.0 }; //White light 
GLfloat	attenuation[] = { 1.0, 1.0, 1.05 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; //White material specular light
GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; //White highlight

GLfloat light_position1[] = { 0, 0, 0, 0.0 }; //Point light (w=1.0)
GLfloat light_ambient1[] = { 0.1, 0.05, 0.0, 1.0 }; //Orange-like tint
GLfloat light_diffuse1[] = { 16.0, 8.0, 0.0, 1.0 }; //Orange-like  light 
GLfloat	attenuation1[] = { 1.0, 0.5, 0.05 };
GLfloat mat_specular1[] = { 1.0, 0.5, 0.0, 1.0 }; //Orange-like material specular light
GLfloat mat_specular_shader1[] = { mat_specular1[0], mat_specular1[1], mat_specular1[2], mat_shininess[0] }; //Orange-like highlight

//Thirds light
GLfloat light_position2[] = { 0, -4, -12, 0.0 }; //Point light (w=1.0)
GLfloat light_ambient2[] = { 0.1, 0.05, 0.0, 1.0 }; //Orange-like tint
GLfloat light_diffuse2[] = { 16.0, 8.0, 2.0, 1.0 }; //Orange-like tint 
GLfloat	attenuation2[] = { 1.0, 0.5, 0.05 };
GLfloat mat_specular2[] = { 0.0, 0.0, 1.0, 1.0 }; //Orange-like material specular light
GLfloat mat_specular_shader2[] = { mat_specular2[0], mat_specular2[1], mat_specular2[2], mat_shininess[0] }; //Orange-like highlight


//Function Prototypes
void init(int argc, char* argv[]);
void report_version(void);
void update(void);
void display(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void checkKey();

//Material declarations
void waxMaterial();
void brickMaterial();
void woodMaterial();
void metalMaterial();

int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	//Define main glut loop
	glutMainLoop();

	// Shut down COM
	shutdownCOM();

	return 0;
}

void init(int argc, char* argv[]) {

	//Initialise COM so we can use Windows Imaging Component
	initCOM();

	//Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	//Set screen to 800 x 800
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("3D scene - 14040859");

	//Register callback functions
	glutDisplayFunc(display);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseMove);


	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	}
	else {

		cout << "GLEW could not be initialised\n";
		throw;
	}

	//query OpenGL state
	report_version();


	//Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Setup main 3D camera
	mainCamera = new GUPivotCamera(0.5f, 0.0f, 0.0f, 75.0f, 1.0f, 0.1f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glLineWidth(4.0f);

	//Shader setup
	myShaderProgram = setupShaders(string("Shaders\\diffuse_vertex_shader.txt"), string("Shaders\\diffuse_fragment_shader.txt"));

	//Get unifom locations in the shader
	gLPosition = glGetUniformLocation(myShaderProgram, "lposition");
	gAmbient = glGetUniformLocation(myShaderProgram, "lambient");
	gDiffuse = glGetUniformLocation(myShaderProgram, "ldiffuse");
	gAttenuation = glGetUniformLocation(myShaderProgram, "lattenuation");

	gLPosition1 = glGetUniformLocation(myShaderProgram, "lposition1");
	gAmbient1 = glGetUniformLocation(myShaderProgram, "lambient1");
	gDiffuse1 = glGetUniformLocation(myShaderProgram, "ldiffuse1");
	gAttenuation1 = glGetUniformLocation(myShaderProgram, "lattenuation1");

	gLPosition2 = glGetUniformLocation(myShaderProgram, "lposition2");
	gAmbient2 = glGetUniformLocation(myShaderProgram, "lambient2");
	gDiffuse2 = glGetUniformLocation(myShaderProgram, "ldiffuse2");
	gAttenuation2 = glGetUniformLocation(myShaderProgram, "lattenuation2");

	gEyePos = glGetUniformLocation(myShaderProgram, "eyePos");

	//Material uniform locations in the shader
	gMatAmbient = glGetUniformLocation(myShaderProgram, "matAmbient");
	gMatDiffuse = glGetUniformLocation(myShaderProgram, "matDiffuse");

	gMatSpecular = glGetUniformLocation(myShaderProgram, "matSpecular");
	gMatSpecular1 = glGetUniformLocation(myShaderProgram, "matSpecular1");
	gMatSpecular2 = glGetUniformLocation(myShaderProgram, "matSpecular2");

	gModelMatrix = glGetUniformLocation(myShaderProgram, "g_ModelMatrix");
	gViewMatrix = glGetUniformLocation(myShaderProgram, "g_ViewMatrix");
	gProjectionMatrix = glGetUniformLocation(myShaderProgram, "g_ProjectionMatrix");
	gNormalMatrix = glGetUniformLocation(myShaderProgram, "g_NormalMatrix");

	//Textures being loaded into the program
	floorTexture = fiLoadTexture("Resources\\Textures\\FloorText.jpg");
	wallTexture = fiLoadTexture("Resources\\Textures\\WallText.jpg");
	roofTexture = fiLoadTexture("Resources\\Textures\\FloorText.jpg");
	skyTexture = fiLoadTexture("Resources\\Textures\\Sky.jpg");
	woodTexture = fiLoadTexture("Resources\\Textures\\BarrelWood.jpg");
	metalTexture = fiLoadTexture("Resources\\Textures\\Metal.jpg");
	doorTexture = fiLoadTexture("Resources\\Textures\\roughWood.jpg");
	shelfTexture = fiLoadTexture("Resources\\Textures\\Wood.jpg");
	gold = fiLoadTexture("Resources\\Textures\\Gold.png");
	wax = fiLoadTexture("Resources\\Textures\\wax.jpg");

	//Enable Vertex Arrays
	//Tell OpenGL to expect vertex position information from an array
	glEnableClientState(GL_VERTEX_ARRAY);
	//Tell OpenGL to expect vertex colour information from an array
	glEnableClientState(GL_COLOR_ARRAY);
	//Tell OpenGL to expect vertex colour information from an array
	glEnableClientState(GL_NORMAL_ARRAY);
	//Tell OpenGL to expect texture coordinate information from an array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	//Models used in the Scene
	walls = new CGModel();
	import3DS(L"Resources\\Models\\WallsNew.3DS", walls);	//The Walls
	walls->setTextureForModel(wallTexture);
	walls->translate(GUVector4(0, -15, 0));

	roof = new CGModel();
	import3DS(L"Resources\\Models\\SimpleRoof.3DS", roof);	//The Roof
	roof->setTextureForModel(roofTexture);
	roof->translate(GUVector4(0, -13, 0));

	sky = new CGModel();
	import3DS(L"Resources\\Models\\SkyBox.3DS", sky);		//The "Sky"
	sky->setTextureForModel(skyTexture);
	sky->translate(GUVector4(0, -13, 10));

	floors = new CGModel();
	import3DS(L"Resources\\Models\\SimpleFloor.3DS", floors);	//The Floor
	floors->setTextureForModel(floorTexture);
	floors->translate(GUVector4(0, -14, 0));

	door = new CGModel();
	import3DS(L"Resources\\Models\\Door.3DS", door);		//The Door
	door->setTextureForModel(doorTexture);
	door->translate(GUVector4(0, -15, 0));

	ladder = new CGModel();
	import3DS(L"Resources\\Models\\Ladder.3DS", ladder);	//The Ladder
	ladder->setTextureForModel(metalTexture);
	ladder->translate(GUVector4(0, -10, 0));

	shelf = new CGModel();
	import3DS(L"Resources\\Models\\SimpleShelf.3DS", shelf); //The Shelf
	shelf->setTextureForModel(shelfTexture);
	shelf->translate(GUVector4(0, -14, 0));
	
	barrel0 = new CGModel();
	import3DS(L"Resources\\Models\\Barrel.3DS", barrel0);	//The first Barrel
	barrel0->setTextureForMesh(barrel0->getMeshAtIndex(0), woodTexture);
	barrel0->setTextureForMesh(barrel0->getMeshAtIndex(1), metalTexture);
	barrel0->translate(GUVector4(-75, -27, 0));

	barrel1 = new CGModel();
	import3DS(L"Resources\\Models\\Barrel.3DS", barrel1);	//The second Barrel
	barrel1->setTextureForMesh(barrel1->getMeshAtIndex(0), woodTexture);
	barrel1->setTextureForMesh(barrel1->getMeshAtIndex(1), metalTexture);
	barrel1->rotate(GUQuaternion(0, 147, 0));
	barrel1->translate(GUVector4(-60, -27, 10));

	barrel2 = new CGModel();
	import3DS(L"Resources\\Models\\Barrel.3DS", barrel2);	//The third Barrel
	barrel2->setTextureForMesh(barrel2->getMeshAtIndex(0), woodTexture);
	barrel2->setTextureForMesh(barrel2->getMeshAtIndex(1), metalTexture);
	barrel2->translate(GUVector4(-60, -27, -10));

	chest0 = new CGModel();
	import3DS(L"Resources\\Models\\Chest.3DS", chest0);		//The first Chest
	chest0->setTextureForMesh(chest0->getMeshAtIndex(0), woodTexture);
	chest0->setTextureForMesh(chest0->getMeshAtIndex(1), metalTexture);
	chest0->setTextureForMesh(chest0->getMeshAtIndex(2), metalTexture);
	chest0->translate(GUVector4(0 ,-14 , -8));

	chest1 = new CGModel();
	import3DS(L"Resources\\Models\\Chest.3DS", chest1);		//The second Chest
	chest1->setTextureForMesh(chest1->getMeshAtIndex(0), woodTexture);
	chest1->setTextureForMesh(chest1->getMeshAtIndex(1), metalTexture);
	chest1->setTextureForMesh(chest1->getMeshAtIndex(2), metalTexture);
	chest1->translate(GUVector4(28, -14, -8));

	candle0 = new CGModel();
	import3DS(L"Resources\\Models\\Candle.3DS", candle0);	//The first Candle
	candle0->setTextureForMesh(candle0->getMeshAtIndex(0), gold);
	candle0->setTextureForMesh(candle0->getMeshAtIndex(1), wax);
	candle0->setTextureForMesh(candle0->getMeshAtIndex(2), wax);
	candle0->setTextureForMesh(candle0->getMeshAtIndex(3), gold);
	candle0->translate(GUVector4(0, -14, 0));

	candle1 = new CGModel();
	import3DS(L"Resources\\Models\\Candle.3DS", candle1);	//The second Candle
	candle1->setTextureForMesh(candle1->getMeshAtIndex(0), gold);
	candle1->setTextureForMesh(candle1->getMeshAtIndex(1), wax);
	candle1->setTextureForMesh(candle1->getMeshAtIndex(2), wax);
	candle1->setTextureForMesh(candle1->getMeshAtIndex(3), gold);
	candle1->translate(GUVector4(-61, -41.5, -38));

	myModelMatrix = GUMatrix4::identity();
}

void report_version(void) { //Reports the current version of OpenGL

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get camera view and projection transforms
	GUMatrix4 viewMatrix = mainCamera->viewTransform();
	GUMatrix4 projMatrix = mainCamera->projectionTransform();

	if (FIXED_FUNCTION)
	{
		glUseProgram(0);

		// set fixed-function projection transform
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((GLfloat*)&projMatrix);

		// set fixed-function model-view transform
		glMatrixMode(GL_MODELVIEW);
		GUMatrix4 modelViewMatrix = viewMatrix * myModelMatrix;
		glLoadMatrixf((GLfloat*)&modelViewMatrix);

		glEnable(GL_TEXTURE_2D);

		// Turn on light
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

		// Set Material properties for the objects in the scene
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	}
	else
	{
		// Get Camera Position in World Coordinates
		GUVector4 eyePos = mainCamera->cameraLocation();

		// Get Normal Matrix (transform Normals to World Coordinates)
		GUMatrix4 normalMatrix = myModelMatrix.inverseTranspose();

		//Moves the lights within the scene whenever the character moves
		GLfloat light_position[] = { lightX0 += sideVec , lightY0 += upVec, lightZ0 += forwardVec, 1.0 };
		GLfloat light_position1[] = { (lightX1 += sideVec) += candleLightTranslate , lightY1 += upVec, lightZ1 += forwardVec, 1.0 }; //Allows the light to move alongside with the moving candle
		GLfloat light_position2[] = { lightX2 += sideVec , lightY2 += upVec, lightZ2 += forwardVec, 1.0 };

		sky->translate(GUVector4(-1, 0, 0));		//Moves the sky above the Hold by 1 per tick
		skyMove = skyMove - 1;						//Lowers the skyMove value by 1 every tick
		if (skyMove <= -80)							//If the skyMove value is equal or less to -80
		{
			skyMove = 0;							//Set the value back to 0
			sky->translate(GUVector4(80, 0, 0));	//Then move the sky back to it's original position
		}	
	
		if (candleMove > 70)						//Check to see if candleMove is greater than 70, if it is..
		{
			candleLeft = false;						//set candleLeft to false
			candleRight = true;						//set candleRight to true
			candleLightTranslate = -0.075;			//set the translate to be -0.075
		}
		if (candleMove < 0)							//Check to see if candleMove is less than 0, if it is..
		{
			candleRight = false;					//set candleRight to be false
			candleLeft = true;						//set candleLeft to be true
			candleLightTranslate = 0.075;			//set the translate to be 0.075
		}
		if (candleLeft == true)						//if candleLeft is true...
		{
			candle0->translate(GUVector4(candleLightTranslate, 0, 0));	//Translate the first candle by the value under candleLight Translate
			candleMove = candleMove + 0.2;			//increase candleMove by 0.2
		}
		if (candleRight == true)					//if candleRight is true...
		{
			candle0->translate(GUVector4(candleLightTranslate, 0, 0));	//Translate the first candle by the value under candleLight Translate
			candleMove = candleMove - 0.2;			//decrease candleMove by 0.2
		}

		// Render using finished shader
		glUseProgram(myShaderProgram);
		glUniform4fv(gDiffuse, 1, (GLfloat*)&light_diffuse);
		glUniform4fv(gAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(gLPosition, 1, (GLfloat*)&light_position);
		glUniform4fv(gEyePos, 1, (GLfloat*)&eyePos);

		glUniform3fv(gAttenuation, 1, (GLfloat*)&attenuation);
		glUniformMatrix4fv(gModelMatrix, 1, false, (GLfloat*)&myModelMatrix);
		glUniformMatrix4fv(gViewMatrix, 1, false, (GLfloat*)&viewMatrix);
		glUniformMatrix4fv(gProjectionMatrix, 1, false, (GLfloat*)&projMatrix);
		glUniformMatrix4fv(gNormalMatrix, 1, false, (GLfloat*)&normalMatrix);

		//Material Properties
		glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);

		//render the second light with shaders
		glUniform4fv(gDiffuse1, 1, (GLfloat*)&light_diffuse1);
		glUniform4fv(gLPosition1, 1, (GLfloat*)&light_position1);
		glUniform4fv(gAmbient1, 1, (GLfloat*)&light_ambient1);
		glUniform3fv(gAttenuation1, 1, (GLfloat*)&attenuation1);
		glUniform4fv(gMatSpecular1, 1, (GLfloat*)&mat_specular_shader1);

		//render the third light with shaders
		glUniform4fv(gDiffuse2, 1, (GLfloat*)&light_diffuse2);
		glUniform4fv(gLPosition2, 1, (GLfloat*)&light_position2);
		glUniform4fv(gAmbient2, 1, (GLfloat*)&light_ambient2);
		glUniform3fv(gAttenuation2, 1, (GLfloat*)&attenuation2);
		glUniform4fv(gMatSpecular2, 1, (GLfloat*)&mat_specular_shader2);

		brickMaterial();	//brickMaterial provides very minimal specular lighting to the object
		if (roof)
			roof->renderTexturedModel();
		if (floors)
			floors->renderTexturedModel();
		if (walls)
			walls->renderTexturedModel();

		woodMaterial();		//woodMaterial makes the object not very reflective, however applies the lighting to it correctly
		if (barrel0)
			barrel0->renderTexturedModel();
		if (barrel1)
			barrel1->renderTexturedModel();
		if (barrel2)
			barrel2->renderTexturedModel();
		if (chest0)
			chest0->renderTexturedModel();
		if (chest1)
			chest1->renderTexturedModel();
		if (door)
			door->renderTexturedModel();
		if (shelf)
			shelf->renderTexturedModel();
		
		waxMaterial();		//Makes the material very shiny (mainly for the bases of the candles)
		if (candle0)
			candle0->renderTexturedModel();
		if (candle1)
			candle1->renderTexturedModel();

		metalMaterial();	//metalMaterial makes the object very reflective, but not bright
		if (ladder)
			ladder->renderTexturedModel();
		if (sky)
			sky->renderTexturedModel();

		checkKey();			//Check to see what key is being pressed

		myModelMatrix = GUMatrix4::translationMatrix(playerPositionX += sideVec, playerPositionY += upVec, playerPositionZ += forwardVec );

		glutSwapBuffers();
		glutPostRedisplay(); //Redraw is required for animation

	}
}

void mouseButtonDown(int button_id, int state, int x, int y) {		//this allows the user to look around with the mouse

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	int dx = x - mouse_x;
	int dy = y - mouse_y;

	theta += (float)dy * (3.142f * 0.01f);

	//rotate camera around the origin
	if (mainCamera)
		mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);

	mouse_x = x;
	mouse_y = y;

	glutPostRedisplay();
}

//This is a very unreflective material
void brickMaterial() {
	GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 }; //Texture map will provide ambient and diffuse.
	GLfloat mat_shininess[] = { 250 };				 //Very Shiny surface

	GLfloat mat_specular[] = { 0.00, 0.0, 0.0, 1.0 }; //Very weak white reflection
	GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; //Very weak white reflection
	GLfloat mat_specular1[] = { 0.01, 0.005, 0.0, 1.0 }; //Very weak orange-like highlight
	GLfloat mat_specular_shader1[] = { mat_specular1[0], mat_specular1[1], mat_specular1[2], mat_shininess[0] }; //Very weak orange-like highlight
	GLfloat mat_specular2[] = { 0.01, 0.005, 0.00, 1.0 }; //Very weak orange-like highlight
	GLfloat mat_specular_shader2[] = { mat_specular2[0], mat_specular2[1], mat_specular2[2], mat_shininess[0] }; //Very weak orange-like highlight

	//Apply the uniform with the new coordinates for the lighting effects
	glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);
	glUniform4fv(gMatSpecular1, 1, (GLfloat*)&mat_specular_shader1);
	glUniform4fv(gMatSpecular2, 1, (GLfloat*)&mat_specular_shader2);
}

//This is a Shiny like material, mostly used for the base of the candles, despite it's name
void waxMaterial() {
	GLfloat mat_amb_diff[] = { 2.0, 2.0, 2.0, 2.0 }; //Texture map will provide more ambient and diffuse.
	GLfloat mat_shininess[] = { 100 };				 //Shiny surface

	GLfloat mat_specular[] = { 2.0, 2.0, 2.0, 1.0 }; //White highlight
	GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; //White highlight
	GLfloat mat_specular1[] = { 2.0, 1.0, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader1[] = { mat_specular1[0], mat_specular1[1], mat_specular1[2], mat_shininess[0] }; //Orange-like highlight
	GLfloat mat_specular2[] = { 2.0, 1.0, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader2[] = { mat_specular2[0], mat_specular2[1], mat_specular2[2], mat_shininess[0] }; //Orange-like highlight

	//Apply tthe uniform with the new coordinates for the lighting effects
	glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);
	glUniform4fv(gMatSpecular1, 1, (GLfloat*)&mat_specular_shader1);
	glUniform4fv(gMatSpecular2, 1, (GLfloat*)&mat_specular_shader2);
}

//This is a matte-like surface, with a slight reflection
void woodMaterial() {
	GLfloat mat_amb_diff[] = { 1, 1, 1, 1 }; // Texture map will provide ambient and diffuse.
	GLfloat mat_shininess[] = { 250 };		 //Very Shiny surface

	GLfloat mat_specular[] = { 0.2, 0.20, 0.2, 1.0 }; //White highlight
	GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; //White highlight
	GLfloat mat_specular1[] = { 0.2, 0.1, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader1[] = { mat_specular1[0], mat_specular1[1], mat_specular1[2], mat_shininess[0] }; //Orange-like highlight
	GLfloat mat_specular2[] = { 0.2, 0.1, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader2[] = { mat_specular2[0], mat_specular2[1], mat_specular2[2], mat_shininess[0] }; //Orange-like highlight

	//Apply the uniform with the new coordinates for the lighting effects
	glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);
	glUniform4fv(gMatSpecular1, 1, (GLfloat*)&mat_specular_shader1);
	glUniform4fv(gMatSpecular2, 1, (GLfloat*)&mat_specular_shader2);
}

// This is a shiny surface, with a a slight reflection
void metalMaterial() {
	GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 }; //Texture map will provide ambient and diffuse.
	GLfloat mat_shininess[] = { 100 };				 //Shiny surface

	GLfloat mat_specular[] = { 0.8, 0.8, 0.8, 1.0 }; //White highlight
	GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; //White highlight
	GLfloat mat_specular1[] = { 0.8, 0.4, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader1[] = { mat_specular1[0], mat_specular1[1], mat_specular1[2], mat_shininess[0] }; //Orange-like highlight
	GLfloat mat_specular2[] = { 0.8, 0.4, 0.0, 1.0 }; //Orange-like highlight
	GLfloat mat_specular_shader2[] = { mat_specular2[0], mat_specular2[1], mat_specular2[2], mat_shininess[0] }; //Orange-like highlight

	//Apply tthe uniform with the new coordinates for the lighting effects
	glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
	glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);
	glUniform4fv(gMatSpecular1, 1, (GLfloat*)&mat_specular_shader1);
	glUniform4fv(gMatSpecular2, 1, (GLfloat*)&mat_specular_shader2);
}

void checkKey() {								//Allows the user to move around the scene with WASD to move forward, sideways and backwards, R and F to move up/down (control scheme totally taken from Elite: Dangerous)
	if (GetAsyncKeyState('W') & 0x8000) {
		forwardVec = 0.6;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		forwardVec = -0.6;
	}
	else {
		forwardVec = 0;
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		sideVec = 0.6;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		sideVec = -0.6;
	}
	else {
		sideVec = 0;
	}
	if (GetAsyncKeyState('R') & 0x8000){
		upVec = -0.6;
	}
	else if (GetAsyncKeyState('F') & 0x8000){
		upVec = 0.6;
	}
	else{
		upVec = 0.0;
	}
}