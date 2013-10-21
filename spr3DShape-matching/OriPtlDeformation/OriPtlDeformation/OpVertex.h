//#ifndef OPVERTEX_H							// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
//#define OPVERTEX_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWApp.h>						// Framework�N���X���錾����Ă���w�b�_�t�@�C��

#include <set>
#include <iostream>
#include <sstream>
#include <GL/glut.h> 
#include <Windows.h> 


#define ESC 27	
using namespace std;
using namespace Spr;

class Vertex{
public:
	bool deleted;
	Vec3d pos;		//position
	Vec3d vel;		//velocity
	Vec3d force;
	Vec3d normal;
	double area;	//	���_�̒S���ʐ�
	void AddForce(Vec3d f);
	Vertex():deleted(false){}
};

class OpVertex: public Vertex
{
public:
	//CDQHPlane<QhullVertex>* horizon;
	Vec3f orientation;
	///  ���_�̈ʒu
	Vec3f GetPos() const{ return pos;}
	//@}
	///	�ʒu�̐ݒ�
	void SetPos(Vec3d p){ pos = p; }
};