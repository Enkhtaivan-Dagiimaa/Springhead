/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Graphics.h"
#include "GRBlendMesh.h"
#include <gl/glut.h>

namespace Spr{;
IF_OBJECT_IMP(GRBlendMesh, GRVisual);

GRBlendMesh::GRBlendMesh(const GRBlendMeshDesc& desc):GRBlendMeshDesc(desc){
	render = NULL;
}
GRBlendMesh::~GRBlendMesh(){
	for (unsigned int id=0; id<list.size(); ++id){
		if (list[id]) glDeleteLists(list[id], 1);
	}
}
void GRBlendMesh::CreateList(GRRenderIf* r){
	for (unsigned int id=0; id<list.size(); ++id){
		if (list[id])	render->ReleaseList(list[id]);
	}
	list.clear();
	render = r;
	unsigned int vtxsize = max(max(positions.size(), normals.size()), max(colors.size(), texCoords.size()));

	if (texCoords.size() && normals.size() && colors.size()){
		std::vector<GRVertexElement::VFT2fC4fN3fP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<originalFaces.size(); ++i){ vtx[originalFaces[i]].n = normals[faceNormals[i]]; }
		for (unsigned i=0; i<colors.size(); ++i){ vtx[i].c = colors[i];	}
		for (unsigned i=0; i<texCoords.size(); ++i){ vtx[i].t = texCoords[i]; }
		render->SetVertexFormat(GRVertexElement::vfT2fC4fN3fP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (texCoords.size() && normals.size()){
		std::vector<GRVertexElement::VFT2fN3fP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<originalFaces.size(); ++i){ vtx[originalFaces[i]].n = normals[faceNormals[i]]; }
		for (unsigned i=0; i<texCoords.size(); ++i){ vtx[i].t = texCoords[i]; }
		render->SetVertexFormat(GRVertexElement::vfT2fN3fP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (texCoords.size() && colors.size()){
		std::vector<GRVertexElement::VFT2fC4bP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<colors.size(); ++i){ 
			vtx[i].c = ((unsigned char)(colors[i].x*255) << 24) |
			 		   ((unsigned char)(colors[i].y*255) << 16) |
					   ((unsigned char)(colors[i].z*255) << 8) |
					   ((unsigned char)(colors[i].w*255));
		}
		for (unsigned i=0; i<texCoords.size(); ++i){ vtx[i].t = texCoords[i]; }
		render->SetVertexFormat(GRVertexElement::vfT2fC4bP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (normals.size() && colors.size()){
		std::vector<GRVertexElement::VFC4fN3fP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<originalFaces.size(); ++i){ vtx[originalFaces[i]].n = normals[faceNormals[i]]; }
		for (unsigned i=0; i<colors.size(); ++i){ vtx[i].c = colors[i];	}
		render->SetVertexFormat(GRVertexElement::vfC4fN3fP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (normals.size()){
		std::vector<GRVertexElement::VFN3fP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<originalFaces.size(); ++i){ vtx[originalFaces[i]].n = normals[faceNormals[i]]; }
		render->SetVertexFormat(GRVertexElement::vfN3fP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (texCoords.size()){
		std::vector<GRVertexElement::VFT2fP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<texCoords.size(); ++i){ vtx[i].t = texCoords[i]; }
		render->SetVertexFormat(GRVertexElement::vfT2fP3f);
		this->CreateListElement(&*vtx.begin());
	}else if (colors.size()){
		std::vector<GRVertexElement::VFC4bP3f> vtx;
		vtx.resize(vtxsize);
		for (unsigned i=0; i<positions.size(); ++i){ vtx[i].p = positions[i]; }
		for (unsigned i=0; i<colors.size(); ++i){ 
			vtx[i].c = ((unsigned char)(colors[i].x*255) << 24) |
			 		   ((unsigned char)(colors[i].y*255) << 16) |
					   ((unsigned char)(colors[i].z*255) << 8) |
					   ((unsigned char)(colors[i].w*255));
		}
		this->CreateListElement(&*vtx.begin());
	}else{
		render->SetVertexFormat(GRVertexElement::vfP3f);
		this->CreateListElement(&*positions.begin());
	}
}

/// �����}�e���A���C���f�b�N�X�������ꍇ�́A���ꖈ�ɂP�̃f�B�X�v���C���X�g�Ƃ��āA�o�^����D
void GRBlendMesh::CreateListElement(void *vtx){
	int base = 0;		// �f�B�X�v���C���X�g�̎��ʎq
	// X�t�@�C������� materialList �w�肪�Ȃ��ꍇ
	//if (!materialList.size()){		
	if (materialList.empty()){
		base = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), vtx, faces.size());
		list.push_back(base);		
	}
	// X�t�@�C������� materialList �w�肪����ꍇ�Amaterial�P�ʂ� Display List ��������
	else{							
		unsigned int first	= 0;				// elementIndex[]��ɂ�����A���E�� first frame
		unsigned int end	= first+1;			// elementIndex[]��ɂ�����A���E�� end frame
		
		// (�������)�O�p�`�̖ʂ̐��������[�v
		while (end < elementIndex.size()){		
			// X�t�@�C������̎w��ŁA��`�����ʂ̐����A�}�e���A���ݒ肳�ꂽ�ʂ̐������Ȃ����ǂ����̃`�F�b�N
			if (materialList.size() > elementIndex[end]){ 
				// �}�e���A���C���f�b�N�X���������ǂ�����r
				if (materialList[elementIndex[first]] == materialList[elementIndex[end]]){
					end++;							// 1���炵�āA�܂�first�Ɣ�r
					if (end >= elementIndex.size()){		// elementIndex�z��̈�ԍŌ�̗v�f���ǂ����̃`�F�b�N					
						// �K�p����}�e���A��ID���Amaterial�Ƃ��ėp�ӂ���Ă��邩�H
						if (materialList[elementIndex[first]] < (int)material.size()){
							base = render->CreateIndexedList(material[materialList[elementIndex[first]]], 	
																GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3);
						}else{
							base = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3);							
						}
						list.push_back(base);
						break;
					}
				}else{
					// �K�p����}�e���A��ID���Amaterial�Ƃ��ėp�ӂ���Ă��邩�H
					if (materialList[elementIndex[first]] < (int)material.size()) {		
						base = render->CreateIndexedList(material[materialList[elementIndex[first]]], 					
																GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3);
					}else{
						base = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3);
					}
					list.push_back(base);
					
					first = end;
					end = end+1;
						
					if (end >= elementIndex.size()){			// elementIndex�z��̈�ԍŌ�̗v�f���ǂ����̃`�F�b�N
						// �K�p����}�e���A��ID���Amaterial�Ƃ��ėp�ӂ���Ă��邩�H
						if (materialList[elementIndex[first]] < (int)material.size()) {		
							base = render->CreateIndexedList(material[materialList[elementIndex[first]]],  
																	GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3);
						}else{
							base = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, (end-first)*3); 
						}	
						list.push_back(base);
						break;
					}
				}

			}else{
				/** X�t�@�C���� MeshMaterialList ���w�肵���ꍇ�A
					�S�Ă̒�`�����ʂɑ΂��āA�����Ă��}�e���A����ݒ�����Ă��邪�A
					�ʂ̐����A�}�e���A���ݒ肳�ꂽ�ʂ̐������Ȃ��ꍇ�́A
					�Ō�Ƀ}�e���A���ݒ肳�ꂽ�ʂƓ����}�e���A�����A�}�e���A���ݒ肳��Ă��Ȃ��ʂɑ΂��ēK�p�����B*/
				// �K�p����}�e���A��ID���Amaterial�Ƃ��ėp�ӂ���Ă��邩�H
				if (materialList[elementIndex[first]] < (int)material.size()) {	
					base = render->CreateIndexedList(material[materialList[elementIndex[first]]], 
														GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, faces.size()-first*3);
				}else{
					base = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin()+first*3, vtx, faces.size()-first*3);				
				}
				list.push_back(base);
				break;
					
			}
		}	
	}
}
void GRBlendMesh::Render(GRRenderIf* r){
	if (r!=render || !list.size()) CreateList(r);
	for (unsigned int id=0; id<list.size(); ++id){
		render->DrawList(list[id]);
	}
}
void GRBlendMesh::Rendered(GRRenderIf* r){
}

}
