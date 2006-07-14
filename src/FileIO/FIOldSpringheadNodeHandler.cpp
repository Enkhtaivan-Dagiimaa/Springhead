/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <FileIO/FINodeHandler.h>
#include <FileIO/FIOldSpringheadNode.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <Graphics/GRRender.h>

namespace Spr{;
using namespace SprOldSpringehead;

class FINodeHandlerXFrame: public FINodeHandlerImp<Frame>{
public:	
	FINodeHandlerXFrame():FINodeHandlerImp<Desc>("Frame"){}
	void Load(Desc& d, FILoadContext* fc){
		fc->PushCreateNode(GRFrameIf::GetIfInfoStatic(), &GRFrameDesc());
	}
	void Loaded(Desc& d, FILoadContext* fc){
		fc->objects.Pop();
	}
};
class FINodeHandlerXFrameTransformMatrix: public FINodeHandlerImp<FrameTransformMatrix>{
public:	
	FINodeHandlerXFrameTransformMatrix():FINodeHandlerImp<Desc>("FrameTransformMatrix"){}
	void Load(Desc& d, FILoadContext* fc){
		GRFrame* fr = DCAST(GRFrame, fc->objects.Top());
		if (fr){
			fr->transform = d.matrix;
		}else{
			fc->ErrorMessage(NULL, "FrameTransformMatrix must be inside of Frame node.");
		}
	}
};

class FINodeHandlerXMesh: public FINodeHandlerImp<Mesh>{
public:
	FINodeHandlerXMesh():FINodeHandlerImp<Desc>("Mesh"){}
	void Load(Desc& d, FILoadContext* fc){
		fc->PushCreateNode(GRMeshIf::GetIfInfoStatic(), &GRMeshDesc());	
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			mesh->positions = d.vertices;	// ���_���W
			for (int f=0; f < d.nFaces; ++f){		
				if ((d.faces[f].nFaceVertexIndices == 3) || (d.faces[f].nFaceVertexIndices == 4)) {
					mesh->faces.push_back( d.faces[f].faceVertexIndices[0] );
					mesh->faces.push_back( d.faces[f].faceVertexIndices[1] );
					mesh->faces.push_back( d.faces[f].faceVertexIndices[2] );
					mesh->elementIndex.push_back(f);
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[0] );
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[1] );
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[2] );

					if (d.faces[f].nFaceVertexIndices == 4){
						// faces�ɂ́A�ʂ��l�p�`�Ȃ�O�p�`�ɕ��������C���f�b�N�X��push
						mesh->faces.push_back( d.faces[f].faceVertexIndices[0] );
						mesh->faces.push_back( d.faces[f].faceVertexIndices[2] );
						mesh->faces.push_back( d.faces[f].faceVertexIndices[3] );
						mesh->elementIndex.push_back(f);
						// originalFaces �ɂ́A4���_�ڂ̃C���f�b�N�X��push
						mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[3] );
					}
				}else{
					fc->ErrorMessage(NULL, "Number of faces for mesh = 3 or 4.");
				}
			}
		}else{
			fc->ErrorMessage(NULL, "cannot create Mesh node.");
		}
	}
	void Loaded(Desc& d, FILoadContext* fc){
		fc->objects.Pop();
	}
};

class FINodeHandlerXMeshMaterialList: public FINodeHandlerImp<MeshMaterialList>{
public:
	FINodeHandlerXMeshMaterialList():FINodeHandlerImp<Desc>("MeshMaterialList"){}
	void Load(Desc& d, FILoadContext* fc){
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			mesh->materialList = d.faceIndexes;		// �}�e���A���ԍ��̃��X�g
		}else{
			fc->ErrorMessage(NULL, "MeshMaterialList must be inside of Mesh node.");
		}
	}
};

class FINodeHandlerXMaterial: public FINodeHandlerImp<Material>{
public:
	FINodeHandlerXMaterial():FINodeHandlerImp<Desc>("Material"){}
#if 0
	void Load(Desc& d, FILoadContext* fc){
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			GRMaterialDesc mat;
			mat.ambient = mat.diffuse = d.face;
			mat.specular = Vec4f(mat.specular.x, mat.specular.y, mat.specular.z, 1.0);
			mat.emissive = Vec4f(mat.emissive.x, mat.emissive.y, mat.emissive.z, 1.0);
			mat.power = d.power;
			mesh->material.push_back(mat);
		}else{
			fc->ErrorMessage(NULL, "Material appered outside of Mesh. ");
		}
	}
#else
	// TextureFilename���}�e���A���Ɗ֘A�t���邽�߁AMaterial���I�u�W�F�N�g�X�^�b�N�� Push .
	void Load(Desc& d, FILoadContext* fc){
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			fc->PushCreateNode(GRMaterialIf::GetIfInfoStatic(), &GRMaterialDesc());	
			GRMaterial* mat = DCAST(GRMaterial, fc->objects.Top());
			if(mat){
				GRMaterialDesc md;
				md.ambient	= md.diffuse = d.face;
				md.specular = Vec4f(d.specular.x, d.specular.y, d.specular.z, 1.0);
				md.emissive = Vec4f(d.emissive.x, d.emissive.y, d.emissive.z, 1.0);
				md.power = d.power;
				mesh->material.push_back(md);
			}else{
				fc->ErrorMessage(NULL, "cannot create Material node.");
			}
		}else{
			fc->ErrorMessage(NULL, "Material must be inside of Mesh node.  ");
		}
	}
	void Loaded(Desc& d, FILoadContext* fc){
		fc->objects.Pop();
	}
#endif
};

class FINodeHandlerXMeshNormals: public FINodeHandlerImp<MeshNormals>{
public:
	FINodeHandlerXMeshNormals():FINodeHandlerImp<Desc>("MeshNormals"){}
	void Load(Desc& d, FILoadContext* fc){
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			mesh->normals = d.normals;														// �@���x�N�g��
			for (int f=0; f < d.nFaceNormals; ++f){
				for (int i=0; i < d.faceNormals[f].nFaceVertexIndices; ++i){
					mesh->faceNormals.push_back( d.faceNormals[f].faceVertexIndices[i] );	// �@���C���f�b�N�X
				}
			}
		}else{
			fc->ErrorMessage(NULL, "MeshNormals must be inside of Mesh node.");
		}
	}
};

class FINodeHandlerXTextureFilename: public FINodeHandlerImp<TextureFilename>{
public:
	FINodeHandlerXTextureFilename():FINodeHandlerImp<Desc>("TextureFilename"){}
	void Load(Desc& d, FILoadContext* fc){
		GRMaterial* mat = DCAST(GRMaterial, fc->objects.Top());
		if (mat){
			mat->texture = d.filename;
		}else{
			fc->ErrorMessage(NULL, "TextureFilename must be inside of Material node.");
		}
	}
};

class FINodeHandlerXMeshTextureCoords: public FINodeHandlerImp<MeshTextureCoords>{
public:
	FINodeHandlerXMeshTextureCoords():FINodeHandlerImp<Desc>("MeshTextureCoords"){}
	void Load(Desc& d, FILoadContext* fc){
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		if (mesh){
			fc->ErrorMessage(NULL, "MeshNormals must be inside of Mesh node OKOKOK !!");
			mesh->texCoords = d.textureCoords;
		}else{
			fc->ErrorMessage(NULL, "MeshNormals must be inside of Mesh node NGNG ( _ _)");
		}
	}
};


void RegisterOldSpringheadNodeHandlers(){
REGISTER_NODE_HANDLER(FINodeHandlerXFrame);
REGISTER_NODE_HANDLER(FINodeHandlerXFrameTransformMatrix);
REGISTER_NODE_HANDLER(FINodeHandlerXMesh);
REGISTER_NODE_HANDLER(FINodeHandlerXMeshMaterialList);
REGISTER_NODE_HANDLER(FINodeHandlerXMaterial);
REGISTER_NODE_HANDLER(FINodeHandlerXMeshNormals);
REGISTER_NODE_HANDLER(FINodeHandlerXTextureFilename);
REGISTER_NODE_HANDLER(FINodeHandlerXMeshTextureCoords);
}
}
