#ifndef FWSPRTETGEN_H
#define FWSPRTETGEN_H
#include <Base/Affine.h>
#include <Graphics/SprGRMesh.h>
#include <Foundation/UTDllLoader.h>


namespace Spr{;

static UTDllLoader dllLoader("sprTetgen.dll", ".;$(SPRINGHEAD2)\\bin\\win32");	//	�O���[�o���ϐ��Ń��[�_�[�����D

#define DLLFUNC_CTYPE	__cdecl						//	�Ăяo���K��
#define DLLFUNC_RTYPE	bool						//	�Ԃ�l�̌^ void�̏ꍇ�͒�`���Ă͂Ȃ�Ȃ��D
#define DLLFUNC_NAME	sprTetgen					//	�֐���
#define DLLFUNC_STR		"sprTetgen"					//	�֐���
#define DLLFUNC_ARGDEF	(int& nVtxsOut, Spr::Vec3d*& vtxsOut, int& nTetsOut, int*& tetsOut, int nVtxsIn, Spr::Vec3d* vtxsIn, int nFacesIn, Spr::GRMeshFace* facesIn, char* option)		//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(nVtxsOut, vtxsOut, nTetsOut, tetsOut, nVtxsIn, vtxsIn, nFacesIn, facesIn, option)						//	�֐��Ăяo���̈���
#include <Foundation/UTDllLoaderImpl.h>

#define DLLFUNC_CTYPE	__cdecl						//	�Ăяo���K��
#define DLLFUNC_RTYPE	void						//	�Ԃ�l�̌^ void�̏ꍇ�͒�`���Ă͂Ȃ�Ȃ��D
#define DLLFUNC_NAME	sprTetgenFree				//	�֐���
#define DLLFUNC_STR		"sprTetgenFree"				//	�֐���
#define DLLFUNC_ARGDEF	(Vec3d* vtxOut, int* tetsOut)	//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(vtxOut, tetsOut)				//	�֐��Ăяo���̈���
#include <Foundation/UTDllLoaderImpl.h>


};

#endif
