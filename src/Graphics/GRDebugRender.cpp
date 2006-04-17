#include "Graphics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <GL/glut.h>

namespace Spr {;
//----------------------------------------------------------------------------
//	GRDebugRender
OBJECTIMP(GRDebugRender, GRRender);
IF_IMP(GRDebugRender, GRRender);
///	Viewport�Ǝˉe�s���ݒ�
void GRDebugRender::Reshape(Vec2f screen){
	glViewport(0, 0, static_cast<GLsizei>(screen.x), static_cast<GLsizei>(screen.y));
	Affinef afProj = Affinef::ProjectionGL(Vec3f(camera.center.x, camera.center.y, camera.size.y/2.0), // ����p90�x
											Vec2f(camera.size.x, camera.size.x*screen.y/screen.x), 
											camera.front, camera.back);
	SetProjectionMatrix(afProj);
}
/// ���̂������_�����O����
void GRDebugRender::DrawSolid(PHSolidIf* so){
	for (int i=0; i<so->NShape(); ++i) {	
		CDShapeIf **shapes = so->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for (size_t f=0; f<mesh->NFace(); ++f) {	
			CDFaceIf* face = mesh->GetFace(f);
			this->DrawFace(face, base);
		}
	}
}
/// �ʂ������_�����O����
void GRDebugRender::DrawFace(CDFaceIf* face, Vec3f * base){
	int numIndices = face->NIndex();			// (=3 :�O�p�`�Ȃ̂�3���_)
	Vec3f *vtx = new Vec3f[numIndices];
#if 1
	// DrawDirect��
	for (int v=0; v<numIndices; ++v)
		vtx[v] = base[face->GetIndices()[v]].data;
	Vec3f normal, edge0, edge1;
	edge0 = vtx[1] - vtx[0];
	edge1 = vtx[2] - vtx[0];
	normal = edge0^edge1;
	normal.unitize();
	glNormal3fv(normal);
	DrawDirect(TRIANGLES, vtx, vtx + numIndices);
#else
	// DrawIndexed��
	size_t *vtxIndex = new size_t[numIndices];
	for (int v=0; v<numIndices; ++v) {
		vtx[v] = base[face->GetIndices()[v]].data;
		vtxIndex[v] = v;
	}
	DrawIndexed(TRIANGLES, vtxIndex, vtxIndex + numIndices, vtx);
	
	delete[] vtxIndex;
#endif
	delete[] vtx;
}

}	//	Spr

