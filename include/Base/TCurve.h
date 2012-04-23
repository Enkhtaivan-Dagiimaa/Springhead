/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef TCURVE_H
#define TCURVE_H

#include <algorithm>
#include <tuple>
#include <vector>

namespace Spr{;

struct Interpolate{
	enum{
		Constant,		//< �萔���				��Ԏn�_�̒l����Ԓ��ŕێ��D�����l��0
		LinearDiff,		//< ���`���(����)		��Ԏn�_�ƏI�_�̈ʒu����`�ɕ��
		LinearInt,		//< ���`���(�ϕ�)		��Ԏn�_�̈ʒu�Ƒ��x�Ő��`�ɕ��
		Quadratic,		//< 2�����				��Ԏn�_�̒l������l�C��ԏI�_�̒l�����ƂɌW��������
		Cubic,			//< 3�����				��Ԏn�_�ƏI�_�̒l�Ɣ����l�����ƂɌW��������
		SlerpDiff,		//< ���ʐ��`���(����)	��Ԏn�_�ƏI�_�̌�������`���
		SlerpInt,		//< ���ʐ��`���(�ϕ�)	��Ԏn�_�̌����Ɗp���x�Ő��`���
		Squad,			//< ����3�����
	};
};

/**
	��Ԋ֐�
 **/
template<class P, class V, class T>
inline P InterpolatePos(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return p0;
		
	T h = t1 - t0;
	const T eps = 1.0e-12;
	if(h < eps)
		return p0;
		
	if(type == Interpolate::LinearInt)
		return p0 + (t - t0) * v0;

	T s = (t - t0)/h;
	if(type == Interpolate::LinearDiff)
		return (1.0 - s)*p0 + s*p1;
		
	T s2 = s*s;
	if(type == Interpolate::Quadratic)
		return (1.0 - s2) * p0 + s2 * p1 + ((s - s2) * h) * v0;

	T s3 = s2*s;
	if(type == Interpolate::Cubic)
		return (1.0 - 3.0*s2 + 2.0*s3) * p0 + ((s - 2.0*s2 + s3)*h) * v0 + (3.0*s2 - 2.0*s3) * p1 + ((-s2 + s3)*h) * v1;
		
	return P();
}

template<class P, class V, class T>
inline V InterpolateVel(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return  V();

	if(type == Interpolate::LinearInt)
		return v0;
				
	T h = t1 - t0;
	const T eps = 1.0e-12;
	if(h < eps)
		return V();

	if(type == Interpolate::LinearDiff)
		return (p1 - p0)/h;

	T s = (t - t0)/h;

	if(type == Interpolate::Quadratic)
		return (2*s/h) * (p1 - p0) + (1 - 2*s) * v0;

	T s2 = s*s;
	if(type == Interpolate::Cubic)
		return (6.0*(s - s2)/h) * (p1 - p0) + (1.0 - 4*s + 3*s2) * v0 + (-2*s + 3*s2) * v1;

	return V();
}

template<class P, class V, class T>
inline V InterpolateAcc(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return V();
	if(type == Interpolate::LinearInt)
		return V();
	if(type == Interpolate::LinearDiff)
		return V();
				
	T h = t1 - t0;
	T h2 = h*h;
	const T eps = 1.0e-12;
	if(h < eps)
		return V();

	T s = (t - t0)/h;

	if(type == Interpolate::Quadratic)
		return (2/h2) * (p1 - p0) - (2/h) * v0;

	T s2 = s*s;
	if(type == Interpolate::Cubic)
		return (6.0*(1 - 2*s)/h2) * (p1 - p0) + ((-4 + 6*s)/h) * v0 + ((-2 + 6*s)/h) * v1;

	return V();
}

template<class P, class V, class T>
inline P InterpolateOri(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return p0;
		
	T h = t1 - t0;
	const T eps = 1.0e-12;
	if(h < eps)
		return p0;
		
	// �p���x�̃m�����̎������߁C���܂���(�m����*�o�ߎ���)��]����
	if(type == Interpolate::SlerpInt){
		V w = v0;
		T wnorm = w.norm();
		if(wnorm < eps)
			return p0;
		V axis = w/wnorm;
		return P::Rot(wnorm * (t - t0), axis) * p0;	//< �p���x���O���[�o�����W�Ȃ̂ō����炩����̂ɒ���
	}

	T s = (t - t0)/h;
	// ����quaternion�����߁C���̎��܂��ɒ葬��]
	if(type == Interpolate::SlerpDiff){
		P qrel = p0.Conjugated() * p1;
		V axis = qrel.Axis();
		T angle = qrel.Theta();
		return p0 * P::Rot(s * angle, axis);			//< ������͉E���炩����
	}
		
	// SQUAD�͖�����

	return P();
}

template<class P, class V, class T>
inline V InterpolateAngvel(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return V();

	if(type == Interpolate::SlerpInt)
		return v0;
				
	T h = t1 - t0;
	const T eps = 1.0e-12;
	if(h < eps)
		return V();

	if(type == Interpolate::SlerpDiff){
		// ����quaternion����p���x�����߂�
		P qrel = p0.Conjugated() * p1;
		V w = (qrel.Theta() / h) * qrel.Axis();
		// �O���[�o�����W�ɕϊ�
		return p0 * w;
	}

	// SQUAD�͖�����
		
	return V();
}

template<class P, class V, class T>
inline V InterpolateAngacc(T t, T t0, P p0, V v0, T t1, P p1, V v1, int type){
	if(type == Interpolate::Constant)
		return V();
	if(type == Interpolate::SlerpInt)
		return V();
	if(type == Interpolate::SlerpDiff)
		return V();
	
	// SQUAD�͖�����	
	return V();
}

/**
	�Ȑ��N���X
 **/

template<class P, class V, class T>
class TCurve{
public:
	typedef P	pos_t;
	typedef V	vel_t;
	typedef T	real_t;

protected:
	struct Point{
		int		id;		//
		real_t	t;
		pos_t	pos;
		vel_t	vel;

		Point(int _id, real_t _t):id(_id), t(_t){}
	};

	int	type;

	// �_�̔z��D���t�̏����ɕ��ԁD
	std::vector<Point>	points;

	struct CompByTime{
		bool operator()(const Point& lhs, const Point& rhs){
			return lhs.t < rhs.t;
		}
	};
	struct CompByID{
		bool operator()(const Point& lhs, const Point& rhs){
			return lhs.id < rhs.id;
		}
	};
	struct CheckID{
		int id;
		bool operator()(const Point& p){
			return p.id == id;
		}
		CheckID(int _id):id(_id){}
	};

	// t���Ƀ\�[�g
	void SortPoints(){
		std::sort(points.begin(), points.end(), CompByTime());
	}
	
public:
	void SetType(int t){ type = t; }
	int  GetType()const{ return type; }
	
	/** @brief �_��ǉ�����
		@param	t	����
		@return	ID	�ǉ����ꂽ�_��ID
		�_�̏����͓_�̒ǉ��E�폜�⎞���̕ύX�ɂ��ω����邪�CID�͕s�ρD
	 **/
	int AddPoint(real_t t){
		int id;
		if(points.empty())
			 id = 0;
		else id = std::max_element(points.begin(), points.end(), CompByID())->id + 1;
		points.push_back(Point(id, t));
		SortPoints();
		return id;
	}

	/// ID����C���f�b�N�X���擾
	int	IndexFromID(int id){
		vector<Point>::iterator it = find_if(points.begin(), points.end(), CheckID(id));
		if(it == points.end())
			return -1;
		return it - points.begin();
	}

	void RemovePoint(int idx){
		if(idx < points.size())
		points.erase(points.begin() + idx);
	}

	size_t	NPoints()const{ return points.size(); }

	real_t GetTime(int idx)const{
		return points[idx].t;
	}
	void SetTime(int idx, real_t t){
		points[idx].t = t;
		SortPoints();
	}

	pos_t GetPos(int idx)const{
		return points[idx].pos;
	}
	void SetPos(int idx, pos_t p){
		points[idx].pos = p;
	}

	vel_t GetVel(int idx)const{
		return points[idx].vel;
	}
	void SetVel(int idx, vel_t v){
		points[idx].vel = v;
	}

	std::pair<int, int>	GetSegment(real_t t)const{
		if(points.size() < 2)
			return std::make_pair(0, 0);
		int idx = 0;
		while(idx < (int)points.size()-1 && points[idx+1].t < t)
			idx++;
		if(idx == points.size()-1)
			return std::make_pair(idx, idx);
		return std::make_pair(idx, idx+1);
	}

	int	GetNearest(real_t t)const{
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		return (t - points[i0].t < points[i1].t - t) ? i0 : i1;
	}

};

/**
	R^n��̋Ȑ�
 **/
template<class V, class T>
class TCurveEuclid : public TCurve<V, V, T>{
public:
	pos_t	CalcPos(real_t t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolatePos(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	vel_t	CalcVel(real_t t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolateVel(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	vel_t	CalcAcc(real_t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolateAcc(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	TCurveEuclid(){
		type = Interpolate::LinearDiff;
	}
};

/**
	Quaternion�̕��
	- �p���x�̓O���[�o�����W�Ŏw�肷��
 **/
template<class T>
class TCurveQuat : public TCurve<TQuaternion<T>, TVec3<T>, T>{
public:
	pos_t	CalcPos(real_t t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolateOri(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	vel_t	CalcVel(real_t t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolateAngvel(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	vel_t	CalcAcc(real_t){
		int i0, i1;
		std::tie(i0, i1) = GetSegment(t);
		Point& p0 = points[i0];
		Point& p1 = points[i1];
		return InterpolateAngacc(t, p0.t, p0.pos, p0.vel, p1.t, p1.pos, p1.vel, type);
	}

	TCurveQuat(){
		type = SLERP_DIFF;
	}
};

typedef TCurveEuclid<float, float>		Curvef;
typedef TCurveEuclid<double, double>	Curved;
typedef TCurveEuclid<Vec2f, float>		Curve2f;
typedef TCurveEuclid<Vec2d, double>		Curve2d;
typedef TCurveEuclid<Vec3f, float>		Curve3f;
typedef TCurveEuclid<Vec3d, double>		Curve3d;
typedef TCurveEuclid<Vec4f, float>		Curve4f;
typedef TCurveEuclid<Vec4d, double>		Curve4d;
typedef TCurveQuat<float>				QuatCurvef;
typedef TCurveQuat<double>				QuatCurved;

}

#endif
