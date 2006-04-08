#ifndef TQUATERNION_H
#define TQUATERNION_H

#include <Base/TinyVec.h>
#include <Base/TinyMat.h>
#include <Base/Affine.h>

namespace Spr{;

/**	\addtogroup gpLinearAlgebra �x�N�g���E�s��E���W�ϊ�	*/
//@{

/**	Quaternion/4�����D3x3�s��̑���ɉ�]��\�����߂Ɏg����D
	4�ϐ��ōςށD��Ԃ�����E�ϕ������₷���D<br>
	TVec3�Ƃ̊|���Z�́C��]�ϊ��ŃI�[�o�[���[�h����Ă���D

	
	���̂������ꂽ���C�I�C���[�p�ւ̕ϊ��D
	heading = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy2 - 2*qz2)
	attitude = asin(2*qx*qy + 2*qz*qw) 
	bank = atan2(2*qx*qw-2*qy*qz , 1 - 2*qx2 - 2*qz2)

	except when qx*qy + qz*qw = 0.5 (north pole)
	which gives:
	heading = 2 * atan2(x,w)
	bank = 0
	and when qx*qy + qz*qw = -0.5 (south pole)
	which gives:
	heading = -2 * atan2(x,w)
	bank = 0 
*/
template<class ET>
class TQuaternion:public PTM::TVectorBase<DIMENC(4), TVecDesc<TQuaternion<ET>, ET> >{
public:
	typedef TVecDesc<TQuaternion<ET>, ET> desc;
	typedef PTM::TVectorBase<DIMENC(4), desc> base_type;
	///	�p������Ȃ���{�I�ȃ����o�̒�`.	@see ::DEF_TVECTOR_BASIC_MEMBER
	DEF_TVECTOR_BASIC_MEMBER(TQuaternion);
	union{
		ET data[4];
		struct{
			ET w,x,y,z;
		};
	};
	///	�v�f�̃A�N�Z�X
	ET& item_impl(size_t i){ return data[i]; }
	///	�v�f�̃A�N�Z�X
	const ET& item_impl(size_t i) const { return data[i]; }
	///	�X�g���C�h
	size_t stride_impl() const { return 1; }

	///	3�����̕����x�N�g��
	typedef PTM::TSubVector<3, desc> vector_type;
	///@name �ϐ��A�N�Z�X
	//@{
	///	w����
	const element_type& W() const { return w; }
	///	x����
	const element_type& X() const { return x; }
	///	y����
	const element_type& Y() const { return y; }
	///	z����
	const element_type& Z() const { return z; }
	///
	const vector_type& V() const {return sub_vector(vector_type());}

	///	z����
	element_type& W(){ return w; }
	///	x����
	element_type& X(){ return x; }
	///	y����
	element_type& Y(){ return y; }
	///	z����
	element_type& Z(){ return z; }
	/// 
	vector_type& V() {return sub_vector(1,vector_type());}
	/// ��]�x�N�g���D0..PI�͈̔͂ŉ�]�x�N�g����Ԃ��D
	TVec3<ET> RotationHalf() {
		TQuaternion<ET> tmp;
		if (tmp.W() < 0) tmp = -*this;
		else tmp = *this;
		TVec3<ET> r;
		if (tmp.W() > 1) tmp.W() = 1;
		ET theta = (ET)( acos(tmp.W()) * 2 );
		r = tmp.sub_vector(1, vector_type());
		ET len = r.norm();
		if (len > 1e-20){
			r = r/len;
		}
		r *= theta;
		return r;
	}
	///	��]�x�N�g��2�D 0..2PI�͈̔͂ŉ�]�x�N�g����Ԃ��D	angle ����֐����ύX
	TVec3<ET> Rotation() {
		//	W() = cos(theta/2) �Ȃ̂�
		TVec3<ET> r;
		if (W() < -1) W() = -1;
		if (W() > 1) W() = 1;
		ET theta = (ET)( acos(W()) * 2 );
		r = sub_vector(1, vector_type());
		ET len = r.norm();
		if (len > 1e-20){
			r = r/len;
		}
		r *= theta;
		return r;
	}

	/// ��]��
	TVec3<ET> Axis(){
		TVec3<ET> r;
		r = sub_vector(1, vector_type());
		ET len = r.norm();
		if (len > 1e-20){
			r = r/len;
		}
		return r;
	}

	/// ��]�p�x (angle�Ɋ֐�����ύX����\��)
	ET Theta(){
		return (ET)( acos(W()) * 2 );
	}
	//@}

	///@name �������E�\�z
	//@{
	///	�R���X�g���N�^
	TQuaternion(element_type wi, element_type xi, element_type yi, element_type zi){ W() = wi; X() = xi; Y() = yi; Z() = zi;}
	template <class B>
	void InitDirect(element_type a, const PTM::TVectorBase<DIMENC(3), B> v){
		W() = a; V() = v;
	}
	template <class B>
	void InitDirect(element_type a, const PTM::TVectorBase<DIMENC(4), B> v){
		W() = v[0]; X() = v[1]; Y() = v[2]; Z() = v[3];
	}
	static TQuaternion<ET> Rot(element_type angle, const TVec3<element_type>& axis){
		TQuaternion<ET> quat;
		PTM::init_quaternion(quat, angle, axis);
		return quat;
	}
	static TQuaternion<ET> Rot(element_type angle, char axis){
		TQuaternion<ET> quat;
		PTM::init_quaternion(quat, angle, axis);
		return quat;
	}
	static TQuaternion<ET> Rot(const TVec3<element_type>& rot){
		TQuaternion<ET> quat;
		PTM::init_quaternion(quat, rot);
		return quat;
	}

	//@}
	///����
	void Conjugate() { V() = -V(); }
	///	
	TQuaternion Conjugated() const { TQuaternion rv(*this); rv.Conjugate(); return rv;}
	///�t
	TQuaternion Inv() const { return Conjugated() / this->square(); }

	///��]�s��ϊ�
	template<class AM> void FromMatrix(const AM& m)
	{
		ET tr = m[0][0] + m[1][1] + m[2][2] + 1;
		if (tr > 1e-6f){
			ET s = ET( 0.5/sqrt(tr) );
			W() = ET( 0.25 / s );
			X() = ET( (m[2][1] - m[1][2]) * s );
			Y() = ET( (m[0][2] - m[2][0]) * s );
			Z() = ET( (m[1][0] - m[0][1]) * s );
		}else if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) { 
			ET s = ET(sqrt( 1.0 + m[0][0] - m[1][1] - m[2][2] ) * 2);
			X() = ET(0.25) * s;
			Y() = (m[0][1] + m[1][0]) / s; 
			Z() = (m[0][2] + m[2][0]) / s; 
			W() = (m[1][2] - m[2][1]) / s;
		} else if (m[1][1] > m[2][2]) {
			ET s = ET( sqrt(1.0 + m[1][1] - m[0][0] - m[2][2] ) * 2);
			X() = (m[0][1] + m[1][0] ) / s;
			Y() = ET(0.25) * s;
			Z() = (m[1][2] + m[2][1] ) / s; 
			W() = (m[0][2] - m[2][0] ) / s;
		} else { 
			ET s = ET( sqrt( 1.0 + m[2][2] - m[0][0] - m[1][1] ) * 2); 
			X() = (m[0][2] + m[2][0] ) / s;
			Y() = (m[1][2] + m[2][1] ) / s;
			Z() = ET(0.25) * s;
			W() = (m[0][1] - m[1][0] ) / s;
		}
		this->unitize();
	}
	template<class AM> void ToMatrix(AM& mat) const
	{
		typedef TYPENAME AM::element_type AMET;
		mat[0][0] = AMET( 1.0 - 2.0 * (Y()*Y() + Z()*Z()) );
		mat[1][0] = AMET( 2.0 * (X()*Y() + Z()*W()) );
		mat[2][0] = AMET( 2.0 * (X()*Z() - Y()*W()) );
		mat[0][1] = AMET( 2.0 * (Y()*X() - Z()*W()) );
		mat[1][1] = AMET( 1.0 - 2.0 * (X()*X() + Z()*Z()) );
		mat[2][1] = AMET( 2.0 * (Y()*Z() + X()*W()) );
		mat[0][2] = AMET( 2.0 * (X()*Z() + Y()*W()) );
		mat[1][2] = AMET( 2.0 * (Y()*Z() - X()*W()) );
		mat[2][2] = AMET( 1.0 - 2.0 * (X()*X() + Y()*Y()) );
	}
	///	�I�C���[�p�֕ϊ�
	template <class VET> void ToEular(TVec3<VET>& v){
		ET poleCheck = X()*Y() + Z()*W();
		VET& heading = v[0];
		VET& attitude = v[1];
		VET& bank = v[2];
		if (poleCheck == 0.5){				//	north pole
			heading = 2 * atan2(X(),W());
			bank = 0;
		}else if(poleCheck == -0.5){		//	south pole
			heading = -2 * atan2(X(),W());
			bank = 0;
		}else{
			heading = atan2(2*Y()*W()-2*X()*Z() , 1 - 2*Y()*Y() - 2*Z()*Z());
			attitude = asin(2*X()*Y() + 2*Z()*W());
			bank = atan2(2*X()*W()-2*Y()*Z() , 1 - 2*X()*X() - 2*Z()*Z());
		}
	}
	///
	template <class VET> void FromEular(const TVec3<VET>& v){
		VET& heading = v[0];
		VET& attitude = v[1];
		VET& bank = v[2];

		ET c1 = cos(heading / 2);
		ET c2 = cos(attitude / 2);
		ET c3 = cos(bank / 2);
		ET s1 = sin(heading / 2);
		ET s2 = sin(attitude / 2);
		ET s3 = sin(bank / 2);
		
		W() = c1*c2*c3 - s1*s2*s3;
		X() = s1*s2*c3 + c1*c2*s3;
		Y() = s1*c2*c3 + c1*s2*s3;
		Z() = c1*s2*c3 - s1*c2*s3;
	}
	///lhs����]����rhs�Ɉ�v������N�E�H�[�^�j�I��
	void RotationArc(const TVec3<ET>& lhs, const TVec3<ET>& rhs)
	{
		TVec3<ET> v0, v1, c;
		ET d, s;
		v0 = lhs.unit();
		v1 = rhs.unit();
		c = PTM::cross(v0, v1);
		d = PTM::dot(v0, v1);
		s = sqrt((1.0 + d) * 2.0);
		W() = s / 2.0;
		V() = c / s;
	}

	///�I�C���[�p�Ŏw��
	void Euler(ET yaw, ET pitch, ET roll) {
		ET cosYaw	= cos(yaw / 2);
		ET sinYaw	= sin(yaw / 2);
		ET cosPitch	= cos(pitch / 2);
		ET sinPitch	= sin(pitch / 2);
		ET cosRoll	= cos(roll / 2);
		ET sinRoll	= sin(roll / 2);
		W() = cosRoll * cosPitch * cosYaw - sinRoll * sinPitch * sinYaw;
		X() = cosRoll * sinPitch * sinYaw + sinRoll * cosPitch * cosYaw;
		Y() = cosRoll * cosPitch * sinYaw + sinRoll * sinPitch * cosYaw;
		Z() = cosRoll * sinPitch * cosYaw - sinRoll * cosPitch * sinYaw;
	}

	/** @brief �p���x����quaternion�̎��Ԕ������v�Z
		@param w �p���x

		����quaternion�Ɗp���xw����Cquaternion�̎��Ԕ������v�Z����D
		������w�͉�]���i���[���h�j���W�n���猩����]��i���[�J���j���W�n�̊p���x��
		��]�����W�n�ŕ\�킵�����́D
	 */
	TQuaternion<ET> Derivative(const TVec3<ET>& w){
		return 0.5 * TQuaternion<ET>(0.0, w.X(), w.Y(), w.Z()) * *this;
	}

	/** @brief �N�E�H�[�^�j�I���̎��Ԕ�������p���x���v�Z
		@param qd quaternion�̎��Ԕ���
		@return �p���x

		����quaternion�Ƃ��̎��Ԕ���qd����p���xw���v�Z����D
		������w�͉�]���i���[���h�j���W�n���猩����]��i���[�J���j���W�n�̊p���x��
		��]�����W�n�ŕ\�킵�����́D
	 */
	TVec3<ET> AngularVelocity(const TQuaternion<ET>& qd){
		return 2.0 * (qd * Conjugated()).V();
	}

protected:
	///	�R���X�g���N�^
	void set_default(){ W() = 1; X() = 0; Y() = 0; Z() = 0;}
};

///	TQuaternion ���m�̊|���Z�D��]�ϊ��Ƃ��ẮC�����ɂȂ�D
template <class A, class B>
TQuaternion<A> operator*(const TQuaternion<A>& q1, const TQuaternion<B>& q2){
    TQuaternion<A> rv;
	rv.W() = q1.W() * q2.W() - q1.X() * q2.X() - q1.Y() * q2.Y() - q1.Z() * q2.Z();
	rv.X() = q1.W() * q2.X() + q1.X() * q2.W() + q1.Y() * q2.Z() - q1.Z() * q2.Y();
	rv.Y() = q1.W() * q2.Y() + q1.Y() * q2.W() + q1.Z() * q2.X() - q1.X() * q2.Z();
	rv.Z() = q1.W() * q2.Z() + q1.Z() * q2.W() + q1.X() * q2.Y() - q1.Y() * q2.X();
	return rv;
}

///	TQuaternion�Ńx�N�g������]�D TQuaternion * vector * TQuaternion.conjugated() �Ɠ����D
template <class ET, class BD>
inline TYPENAME BD::ret_type operator*(const TQuaternion<ET>& q, const PTM::TVectorBase<DIMENC(3), BD>& v){
	TQuaternion<ET> qv(1, ET(v[0]), ET(v[1]), ET(v[2]));
	TYPENAME BD::ret_type r = (q * qv * q.Conjugated()).sub_vector(PTM::TSubVectorDim<1,3>());
	//TYPENAME BD::ret_type tmp = q.V() % v;
	//TYPENAME BD::ret_type r = q.W()*q.W()*v + 2*q.W()*tmp + (q.V()*v)*q.V() + q.V()%tmp;
	return r;
}
///	TQuaternion �̓��ρD
template <class T1, class T2>
inline T1 dot(const TQuaternion<T1>& q1, const TQuaternion<T2>& q2) {
	return q1.X() * q2.X() + q1.Y() * q2.Y() + q1.Z() * q2.Z() + q1.W() * q2.W();
}
template <class T1, class T2>
TQuaternion<T1> interpolate(T1 t, const TQuaternion<T1>& q1, const TQuaternion<T2>& q2){
	TQuaternion<T1> ret;
	TQuaternion<T1> q3 ;
    float      dot;

    dot = q1.X() * q2.X() + q1.Y() * q2.Y() + q1.Z() * q2.Z() + q1.W() * q2.W();
    
    // dot < 0.0f�̎��A���]����
    if (dot < 0.0f){
        dot = -dot;
        q3 = TQuaternion<T1>(-q2.W(), -q2.X(), -q2.Y(), -q2.Z());
    }
    else q3 = q2;
    
    if (dot >  1.0f) dot = 1.0f;
	if (dot < -1.0f) dot = -1.0f;
	
	float angle = acos(dot);
	float sina, sinat, sinaomt;
	
	sina    = sin(angle          );
	sinat   = sin(angle *      t );
	sinaomt = sin(angle * (1 - t));
	
	if (sina){
		ret.X() = (q1.X() * sinaomt + q3.X() * sinat) / sina;
		ret.Y() = (q1.Y() * sinaomt + q3.Y() * sinat) / sina;
		ret.Z() = (q1.Z() * sinaomt + q3.Z() * sinat) / sina;
		ret.W() = (q1.W() * sinaomt + q3.W() * sinat) / sina;
	}else{
		ret = q1;
	}
	return ret;
}

///	float��TQuaternion.
typedef TQuaternion<float> Quaternionf;
///	double��TQuaternion.
typedef TQuaternion<double> Quaterniond;


/**	�p���D�ʒu�ƌ�����\���N���X�D
	TQuaternion �� TVec3 �Ō����ƈʒu��\���D
	TAffine�̑���Ɏg����D
*/
template<class ET>
class TPose:public PTM::TVectorBase<DIMENC(7), TVecDesc<TPose<ET>, ET> >{
public:
	typedef TVecDesc<TPose<ET>, ET> desc;
	typedef PTM::TVectorBase<DIMENC(4), desc> base_type;
	///	�p������Ȃ���{�I�ȃ����o�̒�`.	@see ::DEF_TVECTOR_BASIC_MEMBER
	DEF_TVECTOR_BASIC_MEMBER(TPose);
	union{
		ET data[7];
//		struct{
//			TQuaternion<ET> Ori();
//			TVec3<ET> Pos();
//		};
		struct{
			ET w,x,y,z;
			ET px, py, pz;
		};
	};

	///	�v�f�̃A�N�Z�X
	ET& item_impl(size_t i){ return data[i]; }
	///	�v�f�̃A�N�Z�X
	const ET& item_impl(size_t i) const { return data[i]; }
	///	�X�g���C�h
	size_t stride_impl() const { return 1; }

	///	�v�f�̃A�N�Z�X
	TVec3<ET>& Pos() { return *(TVec3<ET>*)(data+4); }
	const TVec3<ET>& Pos() const { return *(TVec3<ET>*)(data+4); }
	TQuaternion<ET>& Ori() { return *(TQuaternion<ET>*)(data); }
	const TQuaternion<ET>& Ori() const { return *(TQuaternion<ET>*)(data); }
	ET& W(){return w;}
	const ET& W() const{return w;}
	ET& X(){return x;}
	const ET& X() const{return x;}
	ET& Y(){return y;}
	const ET& Y() const{return y;}
	ET& Z(){return z;}
	const ET& Z() const{return z;}
	ET& Px(){return px;}
	const ET& Px() const{return px;}
	ET& Py(){return py;}
	const ET& Py() const{return py;}
	ET& Pz(){return pz;}
	const ET& Pz() const{return pz;}

	/**@name	�v�f�ւ̃A�N�Z�X	*/
	//@{
	/// 
	ET& PosX() {return px;}
	const ET& PosX() const {return px;}
	/// 
	ET& PosY() {return py;}
	const ET& PosY() const {return py;}
	/// 
	ET& PosZ() {return pz;}
	const ET& PosZ() const {return pz;}
	/// 
	ET& OriX() {return x;}
	const ET& OriX() const {return x;}
	/// 
	ET& OriY() {return y;}
	const ET& OriY() const {return y;}
	/// 
	ET& OriZ() {return z;}
	const ET& OriZ() const {return z;}
	/// 
	ET& OriW() {return w;}
	const ET& OriW() const {return w;}
	
	TPose<ET> Inv() const { 
		TPose<ET> rv;
		rv.Ori() = Ori().Inv();
		rv.Pos() = -(rv.Ori()*Pos());
		return rv;
	}

	/**@name	�������ƍ\�z	*/
	///	�P�ʍs��
	static TPose<ET> Unit(){
		TPose<ET> y;
		//PTM::init_unitize(y);
		y.Pos() = TVec3<ET>();
		y.Ori() = TQuaternion<ET>();
		return y;
	}

	/// �^����ꂽ�ʒu�ł�Pose�����������ĕԂ�
	static TPose<ET> Unit(TVec3<ET> &v){
		TPose<ET> y;
		//PTM::init_unitize(y);
		y.Pos() = v;
		y.Ori() = TQuaternion<ET>();
		return y;
	}

	/// �^����ꂽ��]�ł�Pose�����������ĕԂ�
	static TPose<ET> Unit(TQuaternion<ET> &q){
		TPose<ET> y;
		//PTM::init_unitize(y);
		y.Pos() = TVec3<ET>();
		y.Ori() = q;
		return y;
	}

	///	���s�ړ�
	static TPose<ET> Trn(element_type px, element_type py, element_type pz){
		return Trn(TVec3<ET>(px, py, pz));
	}
	static TPose<ET> Trn(TVec3<ET> &v){
		TPose<ET> y;
		y.Pos() = v;
		y.Ori() = TQuaternion<ET>();
		return y;
	}

	/// ��]
	static TPose<ET> Rot(ET wi, ET xi, ET yi, ET zi){ 
		TPose<ET> y;
		y.Pos() = TVec3<ET>();
		y.Ori() = TQuaternion<ET>(wi, xi, yi, zi);
		return y;
	}

	static TPose<ET> Rot(ET angle, const TVec3<element_type>& axis){ 
		TPose<ET> y;
		y.Pos() = TVec3<ET>();
		y.Ori() = TQuaternion<ET>::Rot(angle, axis);
		return y;
	}

	static TPose<ET> Rot(ET angle, char axis){
		TPose<ET> y;
		y.Pos() = TVec3<ET>();
		y.Ori() = TQuaternion<ET>::Rot(angle, axis);
		return y;
	}

	static TPose<ET> Rot(const TVec3<ET>& rot){
		TPose<ET> y;
		y.Pos() = TVec3<ET>();
		y.Ori() = TQuaternion<ET>::Rot(rot);
		return y;
	}

	/// Affine�ϊ��̍s�񂩂�ǂݏo���܂��D
	template <class AT>
	void FromAffine(const TAffine<AT>& f){
		/// Affine�s��̈ʒu�x�N�g����pose�̈ʒu���R�s�[����
		Pos() = f.Trn();
		/// Ori����ł���s���Affine�s��̑Ή�����ꏊ�ɏ㏑��
		Ori().FromMatrix(f.Rot());
	}

	/// Affine�ϊ��̍s��ɕϊ����Ԃ�
	template <class B>
	void ToAffine(TAffine<B>& af) const {
		/// Affine�s��̈ʒu�x�N�g����pose�̈ʒu���R�s�[����
		af.Trn() = Pos();
		/// Ori����ł���s���Affine�s��̑Ή�����ꏊ�ɏ㏑��
		Ori().ToMatrix(af.Rot());
	}

	operator TAffine<ET>(){ 
		TAffine<ET> rv;
		ToAffine(rv);
		return rv;
	}

protected:
	///	�R���X�g���N�^
	void set_default(){ 
		W() = 1; X() = 0; Y() = 0; Z() = 0;
		Pos().clear();
	}
};
template <class EP, class EV>
TVec3<EV> operator * (const TPose<EP>& p, const TVec3<EV>& v){
	return p.Ori()*v + p.Pos();
}

template <class EA, class EB>
TPose<EA> operator * (const TPose<EA>& a, const TPose<EB>& b){
	TPose<EA> rv;
	rv.Pos() = a.Pos() + a.Ori() * b.Pos();
	rv.Ori() = a.Ori() * b.Ori();
	return rv;
}

///	float��TPose.
typedef TPose<float> Posef;
///	double��TPose.
typedef TPose<double> Posed;

//@}

}
#endif
