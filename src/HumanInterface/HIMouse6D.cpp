/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "HumanInterface.h"
#pragma hdrstop
#include "HIMouse6D.h"

namespace Spr {;

bool HIMouse6D::Init(HISdkIf* sdk, const void* desc){
	HIVirtualDeviceIf* vd = sdk->RentVirtualDevice("KeyMouse");
	keyMouse = DCAST(DVKeyMouse, vd);
	if (!keyMouse) return false;
	keyMouse->SetHandler(this);

/*	oldX = oldY = newX = newY = 0;
	btnState = NONE;

	// �f�t�H���g�̐ݒ�
	scaleTransPointer = 0.01f;
	scaleRotPointer = 0.008f;
	scaleTransAxis = 0.03f;
	scaleRotAxis = 0.008f;
	mass = 0.25f;
	pos = Vec3f();
	ori = Quaternionf();
	axis.Pos() = Vec3f(0, 0, -10);
    axis.LookAt(Vec3f(), Vec3f(0,1,0));
	axis = axis.inv();
	axisInit = axis;
	// �}�E�X���A�N�e�B�u���ǂ����̌��o���@���悭�킩��Ȃ��̂łƂ肠����true�ɂ��Ă���
	bGood = true;	
*/
//-> �֌��ɂ��ύX (2005/1/05)
	SetViewSize(640, 480);
//<-

	return true;
}

void HIMouse6D::SetScale(float tp, float rp, float ta, float ra){
	scaleTransPointer = tp;
	scaleRotPointer = rp;
	scaleTransAxis = ta;
	scaleRotAxis = ra;
}

//-> �֌��ɂ��ύX (2005/1/05)
void HIMouse6D::SetViewSize(float inWidth, float inHeight){
	//	VirtualTrackBall�ɂ�View�̑傫���̃p�����[�^��
	//	�K�{�Ȃ��߁A������Z�b�g���邽�߂̃����o�֐�

	viewCenterX = inWidth / 2;
	viewCenterY = inHeight / 2;

	//	View�̒Z������VirtualTrackBall�̔��a�Ƃ���
	float	d;
	if (viewCenterX < viewCenterY)
		d = viewCenterX;
	else
		d = viewCenterY;
	trackBallRadius = d * 1.0f;
}

Vec3f HIMouse6D::GetTrackBallRotation(float inX, float inY){
	//	�N���b�N���ꂽ�|�C���g��VirtualTrackBall������
	//	�_�Ƀ}�b�s���O���邽�߂̃����o�֐�

	Vec3f	v;
	float	d;

	//	TrackBall�̒��S�����_�ƂȂ�悤�ɂ��A�Ȃ�����
	//	���a��1�ƂȂ�悤�ɍ��W�𐳋K��
	v.x = 1.0f * (inX - viewCenterX) / trackBallRadius;
	v.y = -1.0f * (inY - viewCenterY) / trackBallRadius;
	v.z = 0.0f;

	//	x, y�����܂����̂�z�̌���
	d = v.x * v.x + v.y * v.y;
	if (d < 1.0)
	{
		v.z = (float)sqrt(1.0 - d);
	}
	else
	{
		d = sqrt(d);
		v.x = v.x / d;
		v.y = v.y / d;
		v.z = 0;
	}

	return v;
}
//<-

void HIMouse6D::Update(float dt){

	// force,torque�Ɋ�Â���pos,ori���X�V
	// �����Ƃ�����ƌv�Z�����ق���������������Ȃ�
	pos += force * dt * dt / mass /2;
	torque	= ori.Inv() * torque;
	Vec3f drot;
	drot = torque * dt * dt / mass;
	float norm = drot.norm();
	if(norm > 1.0e-10){
		ori = Quaternionf::Rot(norm, drot) * ori;
		ori.unitize();
	}

	if(btnState != NONE){
		// �O�X�e�b�v�̃}�E�X�̈ʒu�Ƃ̍�
		float dx, dy;
		dx = float(newX - oldX);
		dy = float(newY - oldY);

		// �h���b�O����ɂ���ē����������̊
		Affinef afAxis = axis.inv();		

		if (btnState == (DVKeyMouseHandler::SHIFT+ DVKeyMouseHandler::LBUTTON) ){

//-> �֌��ɂ��ύX (2005/1/05)

			//	Virtual Track Ball�̎���
			//		�����ϊ��̉e���𖳎������i�܂蕽�s���e������j
			//		�����ɂ��񂿂���Virtual Track Ball�ł��邪�A
			//		�����Ɠ����Ă���悤�Ɋ�����̂ł悵�Ƃ���B
			//		�i�����炭���s���e�����肵�����Ƃɂ��덷��
			//		�����ł��邭�炢�ł��낤...�j
			Vec3f	oldRot = GetTrackBallRotation((float)oldX, (float)oldY);
			Vec3f	newRot = GetTrackBallRotation((float)newX, (float)newY);
			Vec3f	rotAxis, rotDirection;
			float	rotVelocity;

			rotDirection = newRot - oldRot;
			rotVelocity = rotDirection.norm();

			if (rotVelocity > 0.001)
			{
				rotAxis = cross(newRot, oldRot);
				rotAxis = afAxis.Rot() * rotAxis;
				rotAxis.unitize();

				afAxis = Affinef::Rot(rotVelocity * 1.0f, rotAxis) * afAxis;
				axis = afAxis.inv();
			}

			//-> �I���W�i��
			// Shift + ���h���b�O�̂Ƃ��͊���W����x��,y���𒆐S�ɃJ��������]
			//afAxis = Affinef::Rot(dx*scaleRotAxis, afAxis.Ey()) * Affinef::Rot(dy*scaleRotAxis, afAxis.Ex()) * afAxis;
			//axis = afAxis.inv();
			//<- �����܂Ł@�i�I���W�i���j
//<-
		}
		else if (btnState == (DVKeyMouseHandler::SHIFT + DVKeyMouseHandler::MBUTTON) ){
			// Shift + ���h���b�O�̂Ƃ��͊���W����x-z���ʂŕ��s�ړ�
			afAxis.Pos() += (-afAxis.Ex() * dx - afAxis.Ez() * dy) * scaleTransAxis;	
			axis = afAxis.inv();
		}
		else if (btnState == (DVKeyMouseHandler::SHIFT + DVKeyMouseHandler::LBUTTON + DVKeyMouseHandler::RBUTTON) ) {
			// Shift + ���E�h���b�O�̂Ƃ��͊���W����x-z���ʂŕ��s�ړ�
			afAxis.Pos() += (-afAxis.Ex() * dx - afAxis.Ez() * dy) * scaleTransAxis;
			axis = afAxis.inv();
		}
		else if (btnState == (DVKeyMouseHandler::SHIFT + DVKeyMouseHandler::RBUTTON + DVKeyMouseHandler::MBUTTON) ) {
			// Shift + �E���h���b�O�̂Ƃ��͊���W����y-z���ʂŕ��s�ړ�
			afAxis.Pos() += (-afAxis.Ey() * dx - afAxis.Ez() * dy) * scaleTransAxis;
			axis = afAxis.inv();
		}
		else if (btnState == (DVKeyMouseHandler::SHIFT + DVKeyMouseHandler::RBUTTON) ){
			// Shift + �E�h���b�O�̂Ƃ��͊���W����x-y���ʂŕ��s�ړ�
			afAxis.Pos() += (-afAxis.Ex() * dx + afAxis.Ey() * dy) * scaleTransAxis;	
			axis = afAxis.inv();
		}
		else if (btnState == DVKeyMouseHandler::LBUTTON) {
			// ���h���b�O�̂Ƃ��͊���W����x��,y���𒆐S�Ƀ|�C���^����]
			ori = Quaternionf::Rot(dx*scaleRotPointer, -afAxis.Ey()) * Quaternionf::Rot(dy*scaleRotPointer, -afAxis.Ex()) * ori;
		}
		else if (btnState == DVKeyMouseHandler::MBUTTON) {
			// ���h���b�O�̂Ƃ��͊���W����x-z���ʂŃ|�C���^�𕽍s�ړ�
			Vec3f up(0,1,0);
			Vec3f ex = afAxis.Ex();
			ex = (ex - (ex*up)*ex).unit();
			Vec3f ez = afAxis.Ez();
			ez = (ez - (ez*up)*ez).unit();
			pos += (ex * dx + ez * dy) * scaleTransPointer;
		}
		else if (btnState == DVKeyMouseHandler::LBUTTON + DVKeyMouseHandler::RBUTTON) {
			// ���E�h���b�O�̂Ƃ��͊���W����x-z���ʂŃ|�C���^�𕽍s�ړ�
			Vec3f up(0,1,0);
			Vec3f ex = afAxis.Ex();
			ex = (ex - (ex*up)*ex).unit();
			Vec3f ez = afAxis.Ez();
			ez = (ez - (ez*up)*ez).unit();
			pos += (ex * dx + ez * dy) * scaleTransPointer;
		}
		else if (btnState == DVKeyMouseHandler::RBUTTON + DVKeyMouseHandler::MBUTTON) {
			// �E���h���b�O�̂Ƃ��͊���W����y-z���ʂŃ|�C���^�𕽍s�ړ�
			Vec3f up(0,1,0);
			Vec3f ez = afAxis.Ez();
			ez = (ez - (ez*up)*ez).unit();
			pos += (up * dx + ez * dy) * scaleTransPointer;
		}
		else if (btnState == DVKeyMouseHandler::RBUTTON) {
			Vec3f up(0,1,0);
			Vec3f ex = afAxis.Ex();
			ex = (ex - (ex*up)*ex).unit();
			// �E�h���b�O�̂Ƃ��͊���W����x-y���ʂŃ|�C���^�𕽍s�ړ�
			pos += (ex * dx - up * dy) * scaleTransPointer;
		}

		oldX = newX;
		oldY = newY;

		//DSTR << "(x, y, z) = (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
	}
	HIPose::Update(dt);
}
#ifdef _WIN32
}	//	namespace Spr;
#define WIN32_LEAN_AND_MEAN        // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#define WINVER 0x0500            //    �o�[�W������` Windows2000�ȏ�
#define _WIN32_WINNT WINVER
#include <Windows.h>
namespace Spr{;

bool HIMouse6D::PreviewMessage(void* m){
	MSG* msg = (MSG*)m;
	switch (msg->message){
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:{
		int x = (short)LOWORD(msg->lParam);  // horizontal position of cursor 
		int y = (short)HIWORD(msg->lParam);  // vertical position of cursor 
		OnButtonDown(x, y);
		SetCapture(msg->hwnd);
		}break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		OnButtonUp();
		break;
	case WM_MOUSEMOVE:{
		unsigned fwKeys = msg->wParam;  // key flags 
		int x = (short)LOWORD(msg->lParam);	// horizontal position of cursor 
		int y = (short)HIWORD(msg->lParam);	// vertical position of cursor 
		OnButtonMove(fwKeys, x, y);
		//DSTR << "mouse:" << x << " " << y << std::endl;
		}break;
	case WM_MOUSEWHEEL:{
		unsigned fwKeys = LOWORD(msg->wParam);    // key flags
		int zDelta = (short) HIWORD(msg->wParam);    // wheel rotation
		int xPos = (short) LOWORD(msg->lParam);    // horizontal position of pointer
		int yPos = (short) HIWORD(msg->lParam);    // vertical position of pointer
		OnWheel(fwKeys, zDelta);
		}break;
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:{
		unsigned fwKeys = msg->wParam;        // key flags 
		int x = (short)LOWORD(msg->lParam);  // horizontal position of cursor 
		int y = (short)HIWORD(msg->lParam);  // vertical position of cursor 
		OnDblClick(fwKeys, x, y);
		}break;
	case WM_KEYDOWN:{
		int nVirtKey = (int) msg->wParam;
		long lKeyData = msg->lParam;
		return OnKeyDown(nVirtKey);
		}break;
	default:
		return false;
	}
	return true;
}
bool HIMouse6D::OnKeyDown(unsigned nChar){
	//	���_�ړ��֌W
	
	Affinef afBody = GetAxis().inv();
	float df = 0.2f ; //�ړ��ω���
	// �J�����̕��s�ړ�
	if ( nChar == 'W' ) afBody.Pos() = afBody.Pos() + afBody.Ex() * df;
	else if ( nChar == 'Q' ) afBody.Pos() = afBody.Pos() - afBody.Ex() * df;
	else if ( nChar == 'A' ) afBody.Pos() = afBody.Pos() + afBody.Ey() * df;
	else if ( nChar == 'Z' ) afBody.Pos() = afBody.Pos() - afBody.Ey() * df;
	else if ( nChar == VK_DOWN ) afBody.Pos() = afBody.Pos() + afBody.Ez() * df;
	else if ( nChar == VK_UP ) afBody.Pos() = afBody.Pos() - afBody.Ez() * df;
	// �J�����̉�]
	else if ( nChar == VK_LEFT ) afBody = Affinef::Rot(Radf(5), 'y') * afBody;
	else if ( nChar == VK_RIGHT ) afBody = Affinef::Rot(Radf(-5), 'y') * afBody;
	else if ( nChar == 'S' ) afBody = Affinef::Rot(Radf(5),afBody.Ex()) * afBody;
	else if ( nChar == 'X' ) afBody = Affinef::Rot(Radf(-5),afBody.Ex()) * afBody;
	else if ( nChar == 'C' ){	// �|�C���^�ƃJ���������Z�b�g
		afBody = axisInit.inv();
		afBody.Ex().unitize();
		afBody.Ey().unitize();
		afBody.Ez().unitize();
		pos = Vec3f();
		ori = Quaternionf();
	}
	else return false;	
	SetAxis(afBody.inv());
	return true;
}
#endif

void HIMouse6D::OnButtonDown(int x, int y){
	oldX = newX = x;
	oldY = newY = y;
}	

void HIMouse6D::OnButtonUp(){ 
	btnState = NONE;
	oldX = newX;
	oldY = newY;
}

void HIMouse6D::OnButtonMove(UINT state, int x, int y){ 
	// ������Ă���{�^���̏�Ԃ�ݒ�
	btnState = (ButtonState)state; 

	newX = x;
	newY = y;
}

void HIMouse6D::OnDblClick(unsigned fwKeys, int x, int y){
	if (fwKeys & DVKeyMouseHandler::LBUTTON) OnLButtonDClick();
	if (fwKeys & DVKeyMouseHandler::RBUTTON) OnRButtonDClick();
}

/// �|�C���^�̎p����������
void HIMouse6D::OnLButtonDClick(){
	ori = Quaternionf();
}

/// �|�C���^�̈ʒu��������
void HIMouse6D::OnRButtonDClick(){
	pos = Vec3f();
}

/// ����W���̈ʒu�E�p����������
void HIMouse6D::OnShiftLButtonDClick(){
	axis = axisInit;
}

/// �z�C�[�����܂킵���Ƃ��̏���(�|�C���^or����W����Z�������ɐ����ړ�)
void HIMouse6D::OnWheel(UINT state, short dz){
	btnState = (ButtonState)state;	

	Affinef afAxis = axis.inv();		
	if (btnState == DVKeyMouseHandler::SHIFT){
		afAxis.Pos() += afAxis.Ez() * dz * scaleTransAxis;	
		axis = afAxis.inv();
	} else if (btnState == NONE){
		pos += -afAxis.Ez() * dz * scaleTransPointer;
	}
}

}	//	namespace Spr

