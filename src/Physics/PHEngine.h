/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHEngine_H
#define PHEngine_H

#include <Foundation/Object.h>
#include <SprPhysics.h>

namespace Spr {;


///	�V�[���̐U�����i�r�w�C�r�A�j���Ǘ�����G���W��
/// �r�w�C�r�A�̕��ނƗD�揇��
///	100�̔{���͕��ނ�\���A�ȉ��̈ʂ͂��̒��ł̎��s�������K�肷��
enum PHBehaviorPriority{
	//���������W���[��
	SGBP_INITIALIZER			= 100,
		SGBP_SOLIDINITIALIZER,
		//SGBP_CLEARFORCE,
	//�͐������W���[��
	SGBP_FORCEGENERATOR			= 200,
		SGBP_GRAVITYENGINE,
		SGBP_COLLISIONENGINE,
		SGBP_PENALTYENGINE,
		SGBP_CREATURE,
		//SGBP_WATERCONTACTENGINE,
	//�ϕ��̑Ώ�
	SGBP_DYNAMICALSYSTEM		= 300,
		SGBP_JOINTENGINE,
		SGBP_CONSTRAINTENGINE,
		SGBP_SOLIDCONTAINER,
		//SGBP_WATERENGINE,
	//
	SGBP_MOTION					= 400,
		//SGBP_CHANGEOBJECT,
		//SGBP_CONSTRAINT,
	//���̃G���W���̃��X�i
	SGBP_LISTENER				= 500,
		//SGBP_CONTACTENGINE,
		//SGBP_STICKENGINE,
		//SGBP_WARPENGINE,
};

class PHScene;
///	����G���W���̊�{�N���X�D
class PHEngine:public InheritSceneObject<PHEngineIf, SceneObject>{
	OBJECT_DEF(PHEngine);
public:
	///	���s���������߂�v���C�I���e�B�l�D�������قǑ���
	virtual int GetPriority() const { return SGBP_FORCEGENERATOR; }
	///	���Ԃ� dt �i�߂�
	virtual void Step(){}
};
inline bool operator < (const PHEngine& e1, const PHEngine& e2){
	return e1.GetPriority() < e2.GetPriority();
}
template<class intf, class base>
struct InheritPHEngine: public InheritSceneObject<intf,base>{
	void Clear(){ return base::Clear(); }
	int  GetPriority(){ return base::GetPriority(); }
	void Step(){ return base::Step(); }
};


///	PHEngine�̔z��
class PHEngines: std::vector< UTRef<PHEngine> >{
public:
	iterator initializer;
	iterator forceGenerator;
	iterator dynamicalSystem;
	iterator motion;
	iterator listener;
protected:
	friend class PHScene;
	PHScene* scene;

	iterator begin2() { return base::begin(); }
	iterator end2() { return base::end(); }
public:
	typedef std::vector< UTRef<PHEngine> > base;
	typedef base::iterator iterator;
	typedef base::const_iterator const_iterator;

	PHEngines();
	virtual ~PHEngines(){}
	
	const_iterator begin() const { return base::begin(); }
	const_iterator end() const { return base::end(); }
	size_t size(){ return base::size(); }
	size_t size() const { return base::size(); }
	UTRef<PHEngine>& operator [] (int id){ return base::begin()[id]; }
	const UTRef<PHEngine>& operator [] (int id) const { return base::begin()[id]; }
	///@name �V�~�����[�V������i�߂郁�\�b�h
	//@{	
	///	�͂̃N���A�D
	virtual void ClearForce();
	///	�͂𐶐�(�v�Z)����
	virtual void GenerateForce();
	///	ODE(�ϕ�����)�D�́����x�C���x���ʒu �̌v�Z���s���D
	virtual void Integrate();
	//@}
	void Add(PHEngine* e);
	bool Del(PHEngine* e);
	void Sort();
	///	�V�[�����N���A����Ƃ��̏���
	virtual void Clear();
	///	�w�肵���^���p�������G���W���������� (�ŏ��Ɍ����������̂�Ԃ�)
	PHEngine* FindInherit(const UTTypeInfo& tinfo, UTString name) const;
	PHEngine* FindInherit(const UTTypeInfo& tinfo) const;
	///	�w�肵���^�����̔h���N���X�̃G���W���������� (�ŏ��Ɍ����������̂�Ԃ�)
	template <class T> bool Find(T*& t) const{
		t = (T*)FindInherit(*T::GetTypeInfoStatic());
		return t != NULL;
	}
	template <class T> bool Find(UTRef<T>& t) const{
		t = UTRef<T>((T*)FindInherit(*T::GetTypeInfoStatic()));
		return t != NULL;
	}
	///	�w�肵���^�����̔h���N���X�̃G���W���Ŗ��O����v������̂������� (�ŏ��Ɍ����������̂�Ԃ�)
	template <class T> bool Find(T*& t, UTString name) const{
		t = (T*)FindInherit(*T::GetTypeInfoStatic(), name);
		return t != NULL;
	}
	template <class T> bool Find(UTRef<T>& t, UTString name) const{
		t = UTRef<T>((T*)FindInherit(*T::GetTypeInfoStatic(), name));
		return t != NULL;
	}
};


}
#endif
