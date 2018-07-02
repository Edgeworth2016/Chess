/*
Kai Bernstein
kdbernst@syr.edu
Final Project
Visual Studio 2017
May 1, 2018
References:
Textures:
Knight: http://all-free-download.com/free-vector/download/knight-chess-piece-clip-art_23437.html
Wave: https://chrome.google.com/webstore/detail/%D0%B2%D0%BE%D0%BB%D0%BD%D0%B0/hhkjeaddnbmpjafmnmcjlhihmhmjjngm
Beach: http://www.australiangeographic.com.au/news/2016/12/australias-top-10-beaches-for-2017
Matrix: http://www.indiewire.com/2017/10/the-matrix-code-digital-rain-meaning-1201891684/
Metal: https://www.pinterest.com/pin/351843789612245080/

Interactions: 
Right click to bring up the menu, all keyboard controls (except for the arrow keys) are also menu options.
Left click on the pawns to move them.
Press 1-5 to change the material of the pieces.
1: Just color
2: Transparent
3: Emissive (only at night)
4: Metal (texture)
5: Matrix (texture)
Press 6-8 to change the background.
6: Just color
7: Beach background
8: Matrix background
Press t to add palm trees when on the beach.
Press f to add fog when on the beach.
Press b to blend the chessboard texture with the matrix texture.
Press d to make it daytime, n to make it nighttime, and u to make it dusk.
Press the left and right arrow keys to rotate the chessboard.
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
//#  include <GL/glext.h>
#endif

using namespace std;

#define PI 3.14159265

static float controlPoints[6][4][3] =
{
	{ {-40.0, -20.0, -100.0}, { -20, -10.0, -100.0 }, { 20, -20.0, -100.0 }, { 40.0, -40.0, -100.0 }},
	{ { -40.0, -10.0, -50.0 },{ -20, 0.0, -50.0 },{ 20, -20.0, -50.0 },{ 40.0, -40.0, -50.0 } },
	{ { -40.0, -20.0, 0.0 },{ -20, -20.0, 0.0 },{ 20, -20.0, 0.0 },{ 40.0, -20.0, 0.0 } },
	{ { -40.0, -20.0, 0.0 },{ -20, -20.0, 0.0 },{ 20, -20.0, 0.0 },{ 40.0, -20.0, 0.0 } },
	{ { -40.0, -10.0, -50.0 },{ -20, 0.0, -50.0 },{ 20, -20.0, -50.0 },{ 40.0, -40.0, -50.0 } },
	{ { -40.0, -20.0, -100.0 },{ -20, -10.0, -100.0 },{ 20, -20.0, -100.0 },{ 40.0, -40.0, -100.0 } },
};
static int layout[8][8] =
{ { 25,27,29,31,32,30,28,26},
{ 17,18,19,20,21,22,23,24},
{0,0,0,0,0,0,0,0},{ 0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0 },
{ 1, 2, 3, 4, 5, 6, 7, 8 },
{ 9,11,13,15,16,14,12,10 }
};
static unsigned int texture[6];
static unsigned char chessboard[64][64][3];
static int id = 0;
static bool isSelecting = 0;
static double width, height, width2, height2;
static int hits;
static unsigned int buffer[1024];
static unsigned int closestName = 0;
static float *vertices = NULL;
static int piece = 0;
static int row = 0, column = 0;
static int material = 1;
static int background = 6;
static bool isFog = 0;
static bool selection = 0;
static int piecetomove = -1, squaretomove = 0;
static bool motion = 0;
static float x = 0, z = 0;
static int ogsquare = 0;
static int ogx = 0, ogz = 0, stmx = 0, stmz = 0;
static bool bb = 0;
static int timeofday = 13;
static bool trees = 0;
static bool shadow = 0;
static double phi = 0;
static int maxLevel = 2;
class Source;
class Sequel
{
public:
	Sequel() { coords.clear(); v.clear(); }
	void drawTree();
	friend class Source;
private:
	vector<float> coords;
	vector<Source> v;
};

void Sequel::drawTree()
{
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 3; i++)
		glVertex2f(coords[2 * i], coords[2 * i + 1]);
	glEnd();
}

class Source
{
public:
	Source() { }
	Source(float coordsVal[4])
	{
		for (int i = 0; i < 4; i++) coords[i] = coordsVal[i];
	}
	void draw();
	Sequel sourceToSequelTree();
	void produceTree(int level);
	friend class Sequel;
private:
	float coords[4];
};

void Source::draw()
{
	glBegin(GL_LINES);
	for (int i = 0; i < 2; i++)
		glVertex2f(coords[2 * i], coords[2 * i + 1]);
	glEnd();
}

Sequel Source::sourceToSequelTree()
{
	float x0, y0, x1, y1, coordsVal[10], coordsVal1[4];
	int i, j;
	Source s;
	Sequel seq = *new Sequel();
	x0 = coords[0]; y0 = coords[1]; x1 = coords[2]; y1 = coords[3];
	coordsVal[0] = x1 + (1/2) * cos((PI / 180.0)*60 / 2.0)*(x1 - x0) - (1 / 2) * sin((PI / 180.0)* 60 / 2.0)*(y1 - y0);
	coordsVal[1] = y1 + (1 / 2) * cos((PI / 180.0)* 60 / 2.0)*(y1 - y0) + (1 / 2) * sin((PI / 180.0)* 60 / 2.0)*(x1 - x0);
	coordsVal[2] = x1;
	coordsVal[3] = y1;
	coordsVal[4] = x1 + (1 / 2) * cos((PI / 180.0)* 60 / 2.0)*(x1 - x0) + (1 / 2) * sin((PI / 180.0)* 60 / 2.0)*(y1 - y0);
	coordsVal[5] = y1 + (1 / 2) * cos((PI / 180.0)* 60 / 2.0)*(y1 - y0) - (1 / 2) * sin((PI / 180.0)* 60 / 2.0)*(x1 - x0);
	for (i = 0; i < 6; i++) seq.coords.push_back(coordsVal[i]);
	for (i = 0; i < 2; i++)
	{
		coordsVal1[0] = coordsVal[2];
		coordsVal1[1] = coordsVal[3];
		for (j = 2; j < 4; j++)
		{
			coordsVal1[j] = coordsVal[4 * i + j - 2];
		}
		s = *new Source(coordsVal1);
		seq.v.push_back(s);
	}
	return seq;
}

void drawLeaf(float x, float y)
{
	if (!shadow)
		glColor3f(0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(x, y, 0.0);
	glRotatef(rand() % 360, 0.0, 0.0, 1.0);
	glRotatef(rand() % 360, 1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 2.0);
	glVertex2f(0.0, 4.0);
	glVertex2f(-1.0, 2.0);
	glEnd();
	glPopMatrix();
}

void Source::produceTree(int level)
{
	if (!shadow)
		glColor3f(0.3, 0.3, 0.0);
	glLineWidth(2 * (maxLevel - level));
	if (maxLevel == 0) this->draw();
	else if (maxLevel == 1) { this->draw(); this->sourceToSequelTree().drawTree(); }
	else if (level < maxLevel)
	{
		if (level == 0) this->draw();
		this->sourceToSequelTree().drawTree();
		for (int i = 0; i < 2; i++) this->sourceToSequelTree().v[i].produceTree(level + 1);
	}
	if (level == maxLevel - 1)
	{
		drawLeaf(this->sourceToSequelTree().coords[0], this->sourceToSequelTree().coords[1]);
		drawLeaf(this->sourceToSequelTree().coords[4], this->sourceToSequelTree().coords[5]);
		drawLeaf(this->sourceToSequelTree().coords[0], this->sourceToSequelTree().coords[1]);
		drawLeaf(this->sourceToSequelTree().coords[4], this->sourceToSequelTree().coords[5]);
	}
	glLineWidth(1.0);
}

int findPiece(int p)
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (layout[i][j] == p)
				return (i * 8) + j;
		}
	}
	return -1;
}

void checkMovement()
{
	ogsquare = findPiece(piecetomove);
	ogx = ogsquare % 8; ogz = ogsquare / 8;
	if (piecetomove > 0 && piecetomove <= 8) {
		if (layout[ogz - 1][ogx + 1] != 0 && ogx != 7)
			squaretomove = ogsquare - 7;
		else if (layout[ogz - 1][ogx - 1] != 0 && ogx != 0)
			squaretomove = ogsquare - 9;
		else
			squaretomove = ogsquare - 8;
		stmx = squaretomove % 8; stmz = squaretomove / 8;
		if (squaretomove == (ogsquare - 8) || squaretomove == (ogsquare - 16)) {
			if (layout[stmz][stmx] == 0) {
				layout[ogz][ogx] = 0;
				layout[stmz][stmx] = piecetomove;
			}
		}
		else if (squaretomove == (ogsquare - 7) || squaretomove == (ogsquare - 9)) {
			if (layout[stmz][stmx] > 16) {
				layout[ogz][ogx] = 0;
				layout[stmz][stmx] = piecetomove;
			}
		}
	}
	else if (piecetomove > 16 && piecetomove <= 24) {
		if (layout[ogz + 1][ogx + 1] != 0)
			squaretomove = ogsquare + 9;
		else if (layout[ogz + 1][ogx - 1] != 0)
			squaretomove = ogsquare + 7;
		else
			squaretomove = ogsquare + 8;
		//squaretomove = ogsquare + 8;
		stmx = squaretomove % 8; stmz = squaretomove / 8;
		if (squaretomove == (ogsquare + 8) || squaretomove == (ogsquare + 16)) {
			if (layout[stmz][stmx] == 0) {
				layout[ogz][ogx] = 0;
				layout[stmz][stmx] = piecetomove;
			}
		}
		else if (squaretomove == (ogsquare + 7) || squaretomove == (ogsquare + 9)) {
			if (layout[stmz][stmx] <= 16) {
				layout[ogz][ogx] = 0;
				layout[stmz][stmx] = piecetomove;
			}
		}
	}
	piecetomove = -1;
}

void pieceMenu(int id)
{
	material = id;
	glutPostRedisplay();
}

void mainMenu(int id)
{
	if (id == 0)
		exit(0);
	glutPostRedisplay();
}

void backgroundMenu(int id)
{
	background = id;
	glutPostRedisplay();
}

void extrasMenu(int id)
{
	switch (id)
	{
	case 11:
		isFog = !isFog;
		glutPostRedisplay();
		break;
	case 12:
		bb = !bb;
		glutPostRedisplay();
		break;
	case 16:
		trees = !trees;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void lightingMenu(int id)
{
	timeofday = id;
	glutPostRedisplay();
}

void makeMenu(void)
{
	int sub1, sub2, sub3, sub4;
	sub1 = glutCreateMenu(pieceMenu);
	glutAddMenuEntry("Standard", 1);
	glutAddMenuEntry("Glass", 2);
	glutAddMenuEntry("Glow", 3);
	glutAddMenuEntry("Metal", 4);
	glutAddMenuEntry("Matrix", 5);
	sub2 = glutCreateMenu(backgroundMenu);
	glutAddMenuEntry("Standard", 6);
	glutAddMenuEntry("Beach", 7);
	glutAddMenuEntry("Matrix", 8);
	sub3 = glutCreateMenu(extrasMenu);
	glutAddMenuEntry("Fog", 11);
	glutAddMenuEntry("Blended Board", 12);
	glutAddMenuEntry("Trees", 16);
	sub4 = glutCreateMenu(lightingMenu);
	glutAddMenuEntry("Day", 13);
	glutAddMenuEntry("Night", 14);
	glutAddMenuEntry("Dusk", 15);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("Piece Customization", sub1);
	glutAddSubMenu("Backgrounds", sub2);
	glutAddSubMenu("Extras", sub3);
	glutAddSubMenu("Time of Day", sub4);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void findClosestHit(int hits, unsigned int buffer[])
{
	unsigned int *ptr, minZ;

	minZ = 0xffffffff; // 2^32 - 1
	ptr = buffer;
	closestName = 0;
	for (int i = 0; i < hits; i++)
	{
		ptr++;
		if (*ptr < minZ)
		{
			minZ = *ptr;
			ptr += 2;
			closestName = *ptr;
			ptr++;
		}
		else ptr += 3;
	}
}

struct BitMapFile
{
	int sizeX;
	int sizeY;
	unsigned char *data;
};

BitMapFile *getBMPData(string filename)
{
	BitMapFile *bmp = new BitMapFile;
	unsigned int size, offset, headerSize;
	ifstream infile(filename.c_str(), ios::binary);
	infile.seekg(10);
	infile.read((char *)&offset, 4);
	infile.read((char *)&headerSize, 4);
	infile.seekg(18);
	infile.read((char *)&bmp->sizeX, 4);
	infile.read((char *)&bmp->sizeY, 4);
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];
	infile.seekg(offset);
	infile.read((char *)bmp->data, size);
	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

void loadExternalTextures()
{
	BitMapFile *image[6];
	image[0] = getBMPData("../kdbernstTextures/metal2.bmp");
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
	image[1] = getBMPData("../kdbernstTextures/matrix2.bmp");
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
	image[2] = getBMPData("../kdbernstTextures/beach2.bmp");
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);
	image[3] = getBMPData("../kdbernstTextures/wave.bmp");
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);
	image[4] = getBMPData("../kdbernstTextures/knightw.bmp");
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);
}

void loadProceduralTextures()
{
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, chessboard);
}


void createChessboard(void)
{
	int i, j;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			if ((((i / 8) % 2) && ((j / 8) % 2)) || (!((i / 8) % 2) && !((j / 8) % 2)))
			{
				if (isSelecting) glLoadName(0);
				chessboard[i][j][0] = 0x00;
				chessboard[i][j][1] = 0x00;
				chessboard[i][j][2] = 0x00;
			}
			else
			{
				if (isSelecting) glLoadName(0);
				chessboard[i][j][0] = 0xFF;
				chessboard[i][j][1] = 0xFF;
				chessboard[i][j][2] = 0xFF;
			}
		}
}

float xc(float t)
{
	if (piece == 15 || piece == 31) {
		if (t <= 1.0) return t;
		else if (t <= 1.3) return 1;
		else if (t <= 2) return (2.3 - t);
		else if (t <= 4) return .3;
		else if (t <= 4.7) return 0.3 - (4 - t);
		else if (t <= 5) return 1;
		else                  return 1 - (t - 5);
	}
	else if (piece == 16 || piece == 32) {
		if (t <= 1.0) return t;
		else if (t <= 1.3) return 1;
		else if (t <= 2) return (2.3 - t);
		else if (t <= 4) return .3;
		else if (t <= 4.7) return 0.3 - (4 - t);
		else if (t <= 5) return 1;
		else if (t <= 5.9) return 1 - (t - 5);
		else             return .1;
	}
	else if (piece <= 8 || (piece > 16 && piece <= 24)) {
		if (t <= 1.0) return t;
		else if (t <= 1.3) return 1;
		else if (t <= 2) return (2.3 - t);
		else if (t <= 2.5) return .3;
		else if (t <= 3.2) return 0.3 - (2.5 - t);
		else if (t <= 3.5) return 1;
		else                  return 1 - (t - 3.5);
	}
	else if (piece == 13 || piece == 14 || piece == 29 || piece == 30) {
		if (t <= 1.0) return t;
		else if (t <= 1.3) return 1;
		else if (t <= 2) return (2.3 - t);
		else if (t <= 4) return .3 + (t-2)/2.0;
		else if (t <= 5) return 1.3 - (t-4);
		else             return .3 - (t-5);
	}
	else if (piece == 9 || piece == 10 || piece == 25 || piece == 26) {
		if (t <= 1.0) return t;
		else if (t <= 1.3) return 1;
		else if (t <= 1.6) return (2.3 - t);
		else if (t <= 4) return .7;
		else if (t <= 4.3) return .7 + (t - 4);
		else if (t <= 4.6) return 1;
		else if (t <= 4.8) return 1 - (t - 4.6);
		else if (t <= 5) return .8;
		else             return .8 - (t - 5);
	}
}

float yc(float t)
{
	if (piece == 15 || piece == 31) {
		if (t <= 1.0) return 10;
		else if (t <= 1.3) return t + 9;
		else if (t <= 2) return 10.3 + 2 * (t - 1.3);
		else if (t <= 4) return t + 8.7;
		else if (t <= 4.7) return t + 8.7;
		else if (t <= 5) return t + 8.7;
		else                  return t + 8.7;
	}
	else if (piece == 16 || piece == 32) {
		if (t <= 1.0) return 10;
		else if (t <= 1.3) return t + 9;
		else if (t <= 2) return 10.3 + 2 * (t - 1.3);
		else if (t <= 4) return t + 8.7;
		else if (t <= 4.7) return t + 8.7;
		else if (t <= 5) return t + 8.7;
		else if (t <= 6) return t + 8.7;
		else             return t + 8.7;
	}
	else if (piece <= 8 || (piece > 16 && piece <= 24)) {
		if (t <= 1.0) return 10;
		else if (t <= 1.3) return t + 9;
		else if (t <= 2) return t + 9;
		else if (t <= 2.5) return t + 8.7;
		else if (t <= 3.2) return t + 8.7;
		else if (t <= 3.5) return t + 8.7;
		else                  return t + 8.7;
	}
	else if (piece == 13 || piece == 14 || piece == 29 || piece == 30) {
		if (t <= 1.0) return 10;
		else if (t <= 1.3) return t + 9;
		else if (t <= 2) return 10.3 + 2 * (t - 1.3);
		else if (t <= 4) return t + 8.7;
		else if (t <= 5) return t + 8.7;
		else             return t + 8.7;
	}
	else if (piece == 9 || piece == 10 || piece == 25 || piece == 26) {
		if (t <= 1.0) return 10;
		else if (t <= 1.3) return t + 9;
		else if (t <= 1.6) return t + 9;
		else if (t <= 4) return t + 9;
		else if (t <= 4.3) return 13;
		else if (t <= 4.6) return t + 8.7;
		else if (t <= 4.8) return 13.3;
		else if (t <= 5) return 13.3 - (t - 4.8);
		else             return 13.1;
	}
}

float f(int i, int j)
{
	if (piece == 15 || piece == 31)
		return (xc((float)i / 30 * 6) * cos((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 16 || piece == 32)
		return (xc((float)i / 30 * 6.4) * cos((-1 + 2 * (float)j / 30) * PI));
	else if (piece <= 8 || (piece > 16 && piece <= 24))
		return (xc((float)i / 30 * 4.5) * cos((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 13 || piece == 14 || piece == 29 || piece == 30)
		return (xc((float)i / 30 * 5.3) * cos((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 9 || piece == 10 || piece == 25 || piece == 26)
		return (xc((float)i / 30 * 5.8) * cos((-1 + 2 * (float)j / 30) * PI));
}

float g(int i, int j)
{
	if (piece == 15 || piece == 31)
		return (yc((float)i / 30 * 6));
	else if (piece == 16 || piece == 32)
		return (yc((float)i / 30 * 6.4));
	else if (piece <= 8 || (piece > 16 && piece <= 24))
		return (yc((float)i / 30 * 4.5));
	else if (piece == 13 || piece == 14 || piece == 29 || piece == 30)
		return (yc((float)i / 30 * 5.3));
	else if (piece == 9 || piece == 10 || piece == 25 || piece == 26)
		return (yc((float)i / 30 * 5.3));
}

float h(int i, int j)
{
	if (piece == 15 || piece == 31)
		return (xc((float)i / 30 * 6) * sin((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 16 || piece == 32)
		return (xc((float)i / 30 * 6.4) * sin((-1 + 2 * (float)j / 30) * PI));
	else if (piece <= 8 || (piece > 16 && piece <= 24))
		return (xc((float)i / 30 * 4.5) * sin((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 13 || piece == 14 || piece == 29 || piece == 30)
		return (xc((float)i / 30 * 5.3) * sin((-1 + 2 * (float)j / 30) * PI));
	else if (piece == 9 || piece == 10 || piece == 25 || piece == 26)
		return (xc((float)i / 30 * 5.8) * sin((-1 + 2 * (float)j / 30) * PI));
}

void fillVertexArray(void)
{
	int i, j, k;

	k = 0;
	for (j = 0; j <= 30; j++)
		for (i = 0; i <= 30; i++)
		{
			vertices[k++] = f(i, j);
			vertices[k++] = g(i, j);
			vertices[k++] = h(i, j);
		}
}

void projectionStuff()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void projectionStuff2()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-40, 39, -40, 39, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void changeView()
{
	gluLookAt(25*sin(phi), 15, -35 + 25*cos(phi), 0, 0, -35, 0, 1, 0);
}

void drawPiece(int k)
{
	float matEmission[] = { 0.0, 0.0, 1.0, 1.0 };
	float matEmission2[] = { 1.0, 0.0, 0.0, 1.0 };
	float matEmissionOff[] = { 0.0, 0.0, 0.0, 0.0 };
	piece = k;
	if (timeofday == 14 && piece <= 16 && material == 3) glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission2);
	else if (timeofday == 14 && piece > 16 && material == 3) glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
	if (material == 4 || material == 5)
		glColor4f(1, 1, 1, 1);
	else if (material == 2 && piece <= 8)
		glColor4f(1, 0, 0, 0.5);
	else if (piece <= 8)
		glColor4f(1, 0, 0, 1);
	else if (material == 2 && (piece > 8 && piece <= 16))
		glColor4f(0.5, 0, 0, 0.5);
	else if (piece > 8 && piece <= 16)
		glColor4f(0.5, 0, 0, 1);
	else if (material == 2 && (piece <= 24 && piece > 16))
		glColor4f(0, 0, 1, 0.5);
	else if (piece <= 24 && piece > 16)
		glColor4f(0, 0, 1, 1);
	else if (material == 2 && piece > 24)
		glColor4f(0, 0, 0.5, 0.5);
	else
		glColor4f(0, 0, 0.5, 1);
	if (isSelecting) glLoadName(piece + 64);
	fillVertexArray();
	glPushMatrix();
	int location = findPiece(piece);
	if (location != -1) {
		if (phi == PI || phi == -1 * PI)
			glTranslatef(0, 0, -6);
		else if (phi == PI / 2 || phi == (-3 * PI) / 2)
			glTranslatef(3, 0, -3);
		else if (phi == PI / -2 || phi == (3 * PI) / 2)
			glTranslatef(-3, 0, -3);
		glTranslatef(-11 + ((20.0 / 9)*(location % 8 + 1)) + ((20.0 / 9)*.5), -10, -21 - ((20.0 / 9) * (9 - location / 8)) + ((20.0 / 9)*.5));
		if (piece == 11 || piece == 12 || piece == 27 || piece == 28) {
			if (material == 5)
				glColor4f(0, .5, 0, 1);
			if (material == 4)
				glColor4f(.5, .5, .5, 1);
			if (phi == PI || phi == -1 * PI)
				glRotatef(180, 0, 1, 0);
			else if (phi == PI / 2 || phi == (-3 * PI) / 2)
				glRotatef(90, 0, 1, 0);
			else if (phi == PI / -2 || phi == (3 * PI) / 2)
				glRotatef(-90, 0, 1, 0);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture[5]);
			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1, 7, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(1, 7, 0);
			glTexCoord2f(1.0, 1.0); glVertex3f(1, 10, 0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1, 10, 0.0);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		else {
			glScalef(0.75, 0.75, 0.75);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			for (int j = 0; j < 30; j++)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for (int i = 0; i <= 30; i++)
				{
					glArrayElement((j + 1)*(30 + 1) + i);
					glArrayElement(j*(30 + 1) + i);
				}
				glEnd();
			}
		}
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmissionOff);
}

void drawPieces()
{
	if (material == 4) {
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
	}
	if (material == 5) {
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
	}
	for (int i = 1; i <= 10; i++)
		drawPiece(i);
	for (int i = 13; i <= 26; i++)
		drawPiece(i);
	for (int i = 29; i <= 32; i++)
		drawPiece(i);
	drawPiece(11);
	drawPiece(12);
	drawPiece(27);
	drawPiece(28);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void drawBackground()
{
	if (background == 7) {
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
		if (isSelecting) glLoadName(0);
		glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPoints[0][0]);
		glEnable(GL_MAP2_VERTEX_3);
		glColor4f(1, 1, 1, 1);
		glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		if (isSelecting) glLoadName(0);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0); glVertex3f(-80.0, -80, -70.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-80.0, 50, -70.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(80.0, 50, -70.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(80.0, -80, -70.0);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	else if (background == 8) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		if (isSelecting) glLoadName(0);
		glColor4f(1.0, 1.0, 1.0, 1);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0); glVertex3f(-80.0, -80, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-80.0, 50, -100.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(80.0, 50, -100.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(80.0, -80, 0.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}

void drawEverything()
{
	glColor4f(1.0, 1.0, 1.0, 1);
	if (background == 8) glDisable(GL_LIGHTING);
	float globAmb[] = { 1, 1, 1, 1.0 };
	float globAmb2[] = { 0.1, 0.1, 0.1, 1.0 };
	float globAmb3[] = { 0.5, 0.5, 0.5, 1.0 };
	if (timeofday == 13)
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
	else if (timeofday == 14)
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb2);
	else
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb3);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	float fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
	if (isFog && background == 7) glEnable(GL_FOG);
	else glDisable(GL_FOG);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_START, 0);
	glFogf(GL_FOG_END, 100);
	glFogf(GL_FOG_DENSITY, .025);
	makeMenu();
	createChessboard();
	loadProceduralTextures();
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices = new float[3 * (30 + 1)*(30 + 1)];
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	changeView();
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	if (isSelecting) glLoadName(0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, -5, -25.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(10.0, -5, -25.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(10.0, -5, -45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, -5, -45.0);
	glEnd();
	if (bb) {
		glEnable(GL_BLEND);
		glColor4f(1.0, 1.0, 1.0, .5);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		if (isSelecting) glLoadName(0);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, -5, -25.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(10.0, -5, -25.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(10.0, -5, -45.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, -5, -45.0);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawPieces();
	glDisable(GL_BLEND);
	drawBackground();
	if (trees && background == 7) {
		glPushMatrix();
		glTranslatef(15, 10, -50);
		glRotatef(180, 1, 0, 0);
		float coordsVal1[4] = { 0.0, 10.0, 0.0, -5.0 };
		Source src1 = *new Source(coordsVal1);
		src1.produceTree(0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(13, 2, -37.2);
		glRotatef(-90, 1, 0, 0);
		glScalef(1, .75, 0);
		shadow = 1;
		glColor3f(0, 0, 0);
		src1.produceTree(0);
		shadow = 0;
		glPopMatrix();
		glPushMatrix();
		glTranslatef(9.9, 5, -25);
		glRotatef(180, 1, 0, 0);
		float coordsVal2[4] = { 0.0, 10.0, 0.0, -5.0 };
		Source src2 = *new Source(coordsVal2);
		src2.produceTree(0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(10, 8, -69);
		glRotatef(180, 1, 0, 0);
		float coordsVal3[4] = { 0.0, 10.0, 0.0, -5.0 };
		Source src3 = *new Source(coordsVal3);
		src3.produceTree(0);
		glPopMatrix();
	}
}

void drawBoard() 
{
	int square = 1;
	glColor3f(1, 1, 1);
	for (int i = 0; i < 4; i++) {
		int posx = -40, posy = 39;
		for (int j = 0; j < 4; j++) {
			if (isSelecting) glLoadName(square + (2*j) + (2*i));
			glBegin(GL_POLYGON);
			glVertex3f(posx + (20 * j), posy - (20 * i), 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i), 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i) - 10, 0);
			glVertex3f(posx + (20 * j), posy - (20 * i) - 10, 0);
			glEnd();
			if (isSelecting) glLoadName(square + (2 * j) + (2 * i) + 9);
			glBegin(GL_POLYGON);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i) - 10, 0);
			glVertex3f(posx + (20 * j) + 20, posy - (20 * i) - 10, 0);
			glVertex3f(posx + (20 * j) + 20, posy - (20 * i) - 20, 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i) - 20, 0);
			glEnd();
		}
	}
	int square2 = 9;
	glColor3f(0, 0, 0);
	for (int i = 0; i < 4; i++) {
		int posx = -40, posy = 29;
		for (int j = 0; j < 4; j++) {
			if (isSelecting) glLoadName(square2 + (2 * j) + (2 * i));
			glBegin(GL_POLYGON);
			glVertex3f(posx + (20 * j), posy - (20 * i), 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i), 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i) - 10, 0);
			glVertex3f(posx + (20 * j), posy - (20 * i) - 10, 0);
			glEnd();
			if (isSelecting) glLoadName(square2 + (2 * j) + (2 * i) - 7);
			glBegin(GL_POLYGON);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i) + 10, 0);
			glVertex3f(posx + (20 * j) + 20, posy - (20 * i) + 10, 0);
			glVertex3f(posx + (20 * j) + 20, posy - (20 * i), 0);
			glVertex3f(posx + (20 * j) + 10, posy - (20 * i), 0);
			glEnd();
		}
	}
}

void drawScene(void)
{
	if (timeofday == 13)
		glClearColor(1.0, 0.0, 1.0, 0.0);
	else if (timeofday == 14)
		glClearColor(.1, 0.0, .1, 0.0);
	else
		glClearColor(.5, 0.0, .5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	projectionStuff();
	glLoadIdentity();
	isSelecting = 0;
	drawEverything();
	glutSwapBuffers();
}

void setup(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_LIGHTING);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(6, texture);
	loadExternalTextures();
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	width = w;
	height = h;
}

void pickFunction(int button, int state, int x, int y)
{
	int viewport[4];
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glGetIntegerv(GL_VIEWPORT, viewport);
		glSelectBuffer(1024, buffer);
		(void)glRenderMode(GL_SELECT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);

		gluPerspective(70, 1, 1, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glInitNames();
		glPushName(0);
		isSelecting = 1;
		drawEverything();

		hits = glRenderMode(GL_RENDER);
		findClosestHit(hits, buffer);

		if (closestName > 64) {
			selection = 1;
			piecetomove = closestName - 64;
			checkMovement();
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glutPostRedisplay();
	}
}

void mouseMotion(int x, int y)
{

}

void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case '1':
		material = 1;
		glutPostRedisplay();
		break;
	case '2':
		material = 2;
		glutPostRedisplay();
		break;
	case '3':
		material = 3;
		glutPostRedisplay();
		break;
	case '4':
		material = 4;
		glutPostRedisplay();
		break;
	case '5':
		material = 5;
		glutPostRedisplay();
		break;
	case '6':
		background = 6;
		glutPostRedisplay();
		break;
	case '7':
		background = 7;
		glutPostRedisplay();
		break;
	case '8':
		background = 8;
		glutPostRedisplay();
		break;
	case 'f':
		isFog = !isFog;
		glutPostRedisplay();
		break;
	case 'b':
		bb = !bb;
		glutPostRedisplay();
		break;
	case 't':
		trees = !trees;
		glutPostRedisplay();
		break;
	case 'd':
		timeofday = 13;
		glutPostRedisplay();
	case 'n':
		timeofday = 14;
		glutPostRedisplay();
	case 'u':
		timeofday = 15;
		glutPostRedisplay();
	default:
		break;
	}
}

void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT && background == 6)
		phi += PI/2;
	else if (key == GLUT_KEY_RIGHT && background == 6)
		phi -= PI/2;
	if (phi >= 2 * PI)
		phi -= 2 * PI;
	else if (phi <= -2 * PI)
		phi += 2 * PI;
	glutPostRedisplay();
}

void printInteraction(void)
{
	cout << "Interactions:" << endl
		<< "Right click to bring up the menu, all keyboard controls(except for the arrow keys) are also menu options." << endl
		<< "Left click on the pawns to move them." << endl
		<< "Press 1 - 5 to change the material of the pieces." << endl
		<< "1: Just color" << endl
		<< "2 : Transparent" << endl
		<< "3 : Emissive(only at night)" << endl
		<< "4 : Metal(texture)" << endl
		<< "5 : Matrix(texture)" << endl
		<< "Press 6 - 8 to change the background." << endl
		<< "6 : Just color" << endl
		<< "7 : Beach background" << endl
		<< "8 : Matrix background" << endl
		<< "Press t to add palm trees when on the beach." << endl
		<< "Press f to add fog when on the beach." << endl
		<< "Press b to blend the chessboard texture with the matrix texture." << endl
		<< "Press d to make it daytime, n to make it nighttime, and u to make it dusk." << endl
		<< "Press the left and right arrow keys to rotate the chessboard." << endl;
}

int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Chess Time");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutMouseFunc(pickFunction);
	glutMotionFunc(mouseMotion);
	setup();
	glutMainLoop();
	return 0;
}