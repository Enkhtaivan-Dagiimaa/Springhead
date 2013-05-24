#ifndef UT_CLAPACK_H
#define UT_CLAPACK_H

//#include <Springhead.h>

#define BOOST_NUMERIC_BINDINGS_USE_CLAPACK
//#include <boost/numeric/ublas/fwd.hpp>
#include <boost/numeric/bindings/lapack/driver/sygv.hpp>
#include <boost/numeric/bindings/lapack/driver/sygvx.hpp>
#include <boost/numeric/bindings/noop.hpp>
#include <boost/numeric/bindings/ublas/matrix.hpp>
#include <boost/numeric/bindings/ublas/matrix_proxy.hpp>
#include <boost/numeric/bindings/ublas/symmetric.hpp>
#include <boost/numeric/bindings/ublas/vector.hpp>
#include <boost/numeric/bindings/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

#ifdef _DEBUG
#ifdef _DLL
#ifdef _WIN64 
# pragma comment(lib, "LIBF2C10MDx64.lib")
# pragma comment(lib, "BLAS10MDx64.lib")
# pragma comment(lib, "CLAPACK10MDx64.lib")
#else
# pragma comment(lib, "LIBF2C10MD.lib")
# pragma comment(lib, "BLAS10MD.lib")
# pragma comment(lib, "CLAPACK10MD.lib")
#endif	// WIN64
#else	// _DLL
#ifdef _WIN64 
# pragma comment(lib, "LIBF2C10Dx64.lib")
# pragma comment(lib, "BLAS10Dx64.lib")
# pragma comment(lib, "CLAPACK10Dx64.lib")
#else
# pragma comment(lib, "LIBF2C10D.lib")
# pragma comment(lib, "BLAS10D.lib")
# pragma comment(lib, "CLAPACK10D.lib")
#endif	// WIN64
#endif	// _DLL
#else	// _DEBUG
#ifdef _DLL
#ifdef _WIN64 
# pragma comment(lib, "LIBF2C10Mx64.lib")
# pragma comment(lib, "BLAS10Mx64.lib")
# pragma comment(lib, "CLAPACK10Mx64.lib")
#else
# pragma comment(lib, "LIBF2C10M.lib")
# pragma comment(lib, "BLAS10M.lib")
# pragma comment(lib, "CLAPACK10M.lib")
#endif	// WIN64
#else	// _DLL
#ifdef _WIN64 
# pragma comment(lib, "LIBF2C10x64.lib")
# pragma comment(lib, "BLAS10x64.lib")
# pragma comment(lib, "CLAPACK10x64.lib")
#else
# pragma comment(lib, "LIBF2C10.lib")
# pragma comment(lib, "BLAS10.lib")
# pragma comment(lib, "CLAPACK10.lib")
#endif	// WIN64
#endif	// _DLL
#endif	// _DEBUG

/*
/ �����N����lib�t�@�C����_cdecl�ŌĂяo��
/ _fastcall�Ń����N�������ꍇ��pringhead2\src\boost\numeric\bindings\lapack\lapack.h
/ �̊֐������ׂ�_cdecl�Ăяo���ɂ��邱��
*/

#define ublas boost::numeric::ublas
#define bindings boost::numeric::bindings
#define lapack bindings::lapack

namespace Spr{;

/** @brief ��ʉ��ŗL�l��������Ax = lamda Bx (inv(B)Ax = lamda x)
	@param a �Ώ̍s��
	@param b �Ώ̍s��(a�Ɠ��T�C�Y)
	@param e �ŗL�l���܂Ƃ߂��x�N�g��(�K�v�ȕ������j
	@param v �ŗL�x�N�g�����܂Ƃ߂��x�N�g��(a�̃T�C�Y�Ae�̃T�C�Y)
	@return �v�Z�ɐ����������ǂ����B0:�����A����ȊO:���s
*/
template <class AD, class BD> 
int sprsygv(const PTM::MatrixImp<AD>& a, const PTM::MatrixImp<AD>& b, PTM::VectorImp<BD>& e, PTM::MatrixImp<AD>& v){
	e.clear();
	v.clear();
	int size = a.height();
	assert(a.width() == size);
	assert(b.height() == size || b.width() == size);
	assert(e.size() == size);
	assert(v.height() == size || b.width() == size);

	typedef TYPENAME AD::element_type element_type;
	typedef typename bindings::remove_imaginary<element_type>::type real_type ;
	typedef ublas::vector< real_type > vector_type;
	typedef ublas::matrix< element_type, ublas::column_major > matrix_type;

	vector_type evec(size);
	matrix_type la(size, size), lb(size, size);

	// �s��̉��O�p���g��
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j <= i; ++j) {
			la(i, j) = a.item(i,j);
			lb(i, j) = b.item(i,j);
		}
	}
	ublas::symmetric_adaptor< matrix_type, ublas::lower> la_uplo( la ) ;
	ublas::symmetric_adaptor< matrix_type, ublas::lower> lb_uplo( lb ) ;
	int info = lapack::sygv(1,'V',la_uplo, lb_uplo, evec);

	int esize = e.size();
	for(int i = 0; i < esize; i++)
		e.item(i) = evec[i];

	for(int i = 0; i < size; i++)
		for(int j = 0; j < esize; j++)
			v.item(i, j) = la(i, j);
	return info;
}

/** @brief ��ʉ��ŗL�l���i�I�𕪁j������Ax = lamda Bx (inv(B)Ax = lamda x)
	@param a �Ώ̍s��
	@param b �Ώ̍s��(a�Ɠ��T�C�Y)
	@param e �ŗL�l���܂Ƃ߂��x�N�g��(�I�����������j
	@param v �ŗL�x�N�g�����܂Ƃ߂��x�N�g��(a�̃T�C�Y�A�I����������)
	@param start �ŗL�l�̃C���f�b�N�X(�ŏ���0)
	@param interval start����̌ŗL�l�̐�(�ő�a�̃T�C�Y�j
	@return �v�Z�ɐ����������ǂ����B0:�����A����ȊO:���s
*/
template <class AD, class BD> 
int sprsygvx(const PTM::MatrixImp<AD>& a, const PTM::MatrixImp<AD>& b, PTM::VectorImp<BD>& e, PTM::MatrixImp<AD>& v, int start, int interval){
	e.clear();
	v.clear();
	int size = a.height();
	assert(a.width() == size);
	assert(b.height() == size || b.width() == size);
	assert(e.size() == interval);
	assert(v.height() == size || b.width() == interval);

	typedef TYPENAME AD::element_type element_type;
	typedef typename bindings::remove_imaginary<element_type>::type real_type ;
	typedef ublas::vector< real_type > vector_type;
	typedef ublas::matrix< element_type, ublas::column_major > matrix_type;
	matrix_type la(size, size), lb(size, size); 
	// �s��̉��O�p���g��
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j <= i; ++j) {
			la(i, j) = a.item(i,j);
			lb(i, j) = b.item(i,j);
		}
	}
	ublas::symmetric_adaptor< matrix_type, ublas::lower> la_uplo( la ) ; 
	ublas::symmetric_adaptor< matrix_type, ublas::lower> lb_uplo( lb ) ;

	real_type vl = DBL_MIN;				// ���߂����ŗL�l�̉����Ə��
	real_type vu = DBL_MAX;		
	int il = start + 1;					// ���߂����ŗL�l�̍ŏ��C���f�b�N�X�i�ŏ���1)
	int iu = start + interval;			// ���߂����ŗL�l�̍ő�C���f�b�N�X�i�ő��size)
	real_type abstol = 0.0;				// ���e�덷
	int m = 0;							// ���߂�ꂽ�ŗL�l�̐�
	vector_type w(size);				// �ŗL�l�x�N�g��
	matrix_type z(size, interval);		// �ŗL�x�N�g��
	ublas::vector< int > ifail(size);	// �ŗL�l�����߂��ۂ̎����Ɋւ�����
	int info = lapack::sygvx(1, 'V', 'I', la_uplo, lb_uplo, vl, vu, il, iu, abstol, m, w, z, ifail);

	for(int i = 0; i < interval; i++)
		e.item(i) = w[i];

	for(int i = 0; i < size; i++)
		for(int j = 0; j < interval; j++)
			v.item(i, j) = z(i, j);
	return info;
}

}

#endif
