#ifndef UT_CLAPACK_H
#define UT_CLAPACK_H

//#include <Springhead.h>

#define BOOST_NUMERIC_BINDINGS_USE_CLAPACK
#pragma warning(push)
#pragma warning(disable:4267)
//#include <boost/numeric/ublas/fwd.hpp>
#include <boost/numeric/bindings/lapack/driver/sygv.hpp>
#include <boost/numeric/bindings/lapack/driver/sygvx.hpp>
#include <boost/numeric/bindings/lapack/driver/gesv.hpp>
#include <boost/numeric/bindings/lapack/driver/gels.hpp>
#include <boost/numeric/bindings/lapack/driver/gelsd.hpp>
#include <boost/numeric/bindings/lapack/driver/gesdd.hpp>
#include <boost/numeric/bindings/noop.hpp>
#include <boost/numeric/bindings/ublas/banded.hpp>
#include <boost/numeric/bindings/ublas/matrix.hpp>
#include <boost/numeric/bindings/ublas/matrix_proxy.hpp>
#include <boost/numeric/bindings/ublas/symmetric.hpp>
#include <boost/numeric/bindings/ublas/vector.hpp>
#include <boost/numeric/bindings/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#pragma warning(pop)

#ifdef TRACE		// Trace
  #if (_MSC_VER == 1500)    // Visual Studio 2008
    #ifdef _WIN64 
      # pragma comment(lib, "LIBF2C10x64.lib")
      # pragma comment(lib, "BLAS10x64.lib")
      # pragma comment(lib, "CLAPACK10x64.lib")
    #else
      # pragma comment(lib, "LIBF2C10.lib")
      # pragma comment(lib, "BLAS10.lib")
      # pragma comment(lib, "CLAPACK10.lib")
    #endif
  #else			    // after Visual Studio 2010
    #ifdef _WIN64 
      # pragma comment(lib, "LIBF2C10Tx64.lib")
      # pragma comment(lib, "BLAS10Tx64.lib")
      # pragma comment(lib, "CLAPACK10Tx64.lib")
    #else
      # pragma comment(lib, "LIBF2C10T.lib")
      # pragma comment(lib, "BLAS10T.lib")
      # pragma comment(lib, "CLAPACK10T.lib")
    #endif
  #endif
#else /* TRACE */
  #ifdef _DEBUG
    #ifdef _DLL		// Debug (former DebugDll)
      #ifdef _WIN64 
        # pragma comment(lib, "LIBF2C10MDx64.lib")
        # pragma comment(lib, "BLAS10MDx64.lib")
        # pragma comment(lib, "CLAPACK10MDx64.lib")
      #else
        # pragma comment(lib, "LIBF2C10MD.lib")
        # pragma comment(lib, "BLAS10MD.lib")
        # pragma comment(lib, "CLAPACK10MD.lib")
      #endif
    #else		// (former Debug)
      #ifdef _WIN64 
        # pragma comment(lib, "LIBF2C10Dx64.lib")
        # pragma comment(lib, "BLAS10Dx64.lib")
        # pragma comment(lib, "CLAPACK10Dx64.lib")
      #else
        # pragma comment(lib, "LIBF2C10D.lib")
        # pragma comment(lib, "BLAS10D.lib")
        # pragma comment(lib, "CLAPACK10D.lib")
      #endif
    #endif
  #else /* _DEBUG */
    #ifdef _DLL		// Release (former ReleaseDll)
      #ifdef _WIN64 
        # pragma comment(lib, "LIBF2C10Mx64.lib")
        # pragma comment(lib, "BLAS10Mx64.lib")
        # pragma comment(lib, "CLAPACK10Mx64.lib")
      #else
        # pragma comment(lib, "LIBF2C10M.lib")
        # pragma comment(lib, "BLAS10M.lib")
        # pragma comment(lib, "CLAPACK10M.lib")
      #endif
    #else		// (former Release)
      #ifdef _WIN64 
        # pragma comment(lib, "LIBF2C10x64.lib")
        # pragma comment(lib, "BLAS10x64.lib")
        # pragma comment(lib, "CLAPACK10x64.lib")
      #else
        # pragma comment(lib, "LIBF2C10.lib")
        # pragma comment(lib, "BLAS10.lib")
        # pragma comment(lib, "CLAPACK10.lib")
      #endif
    #endif
  #endif /* _DEBUG */
#endif /* TRACE */

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
	int size = (int)a.height();
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

// c.f.) http://d.hatena.ne.jp/blono/20081118/1227001319
inline int least_squares(const ublas::matrix<double>& A, const ublas::vector<double>& b, ublas::vector<double>& x, ublas::vector<double>& S, double rcond=0.01) {
    BOOST_UBLAS_CHECK(A.size1() == b.size(), ublas::external_logic());

    ublas::matrix<double> B(b.size(), 1), X;
    ublas::column(B, 0).assign(b);

    ublas::matrix<double, ublas::column_major> CA(A), CX((std::max)(A.size1(), A.size2()), B.size2());
    ublas::project(CX, ublas::range(0, B.size1()), ublas::range(0, B.size2())).assign(B);

    lapack::optimal_workspace work;
	int info;
	fortran_int_t rank;

	S.resize((std::min)(A.size1(), A.size2()));

	info = (int) lapack::gelsd(CA, CX, S, rcond, rank, work);
    BOOST_UBLAS_CHECK(info == 0, ublas::internal_logic());

    X = ublas::project(CX, ublas::range(0, A.size2()), ublas::range(0, B.size2()));

    x = ublas::column(X, 0);

	return rank;
}

// c.f.) http://d.hatena.ne.jp/blono/20080922/1222049807
inline void svd(const ublas::matrix<double>& A, ublas::matrix<double>& U, ublas::diagonal_matrix<double>& D, ublas::matrix<double>& VT) {
	ublas::vector<double> s((std::min)(A.size1(), A.size2()));
    ublas::matrix<double, ublas::column_major> CA(A), CU(A.size1(), A.size1()), CVT(A.size2(), A.size2());
    int info;

    info = (int) lapack::gesdd('A', CA, s, CU, CVT);
    BOOST_UBLAS_CHECK(info == 0, ublas::internal_logic());

    ublas::matrix<double> CCU(CU), CCVT(CVT);
    ublas::diagonal_matrix<double> CD(A.size1(), A.size2());

    for (std::size_t i = 0; i < s.size(); ++i) {
        CD(i, i) = s[i];
    }

	#if BOOST_UBLAS_TYPE_CHECK
		BOOST_UBLAS_CHECK(
			ublas::detail::expression_type_check(ublas::prod(ublas::matrix<double>(ublas::prod(CCU, CD)), CCVT), A),
			ublas::internal_logic()
		);
	#endif

    U.assign_temporary(CCU);
    D.assign_temporary(CD);
    VT.assign_temporary(CCVT);
}


// c.f.) http://d.hatena.ne.jp/blono/20080913/1221328687
template <class M, class MI>
void invert(const M& m, MI& mi) {
    BOOST_UBLAS_CHECK(m.size1() == m.size2(), ublas::external_logic());

    ublas::matrix<double>       lhs(m);
    ublas::matrix<double>       rhs(ublas::identity_matrix<double>(m.size1()));
    ublas::permutation_matrix<> pm(m.size1());

    ublas::lu_factorize(lhs, pm);
    ublas::lu_substitute(lhs, pm, rhs);

    BOOST_UBLAS_CHECK(rhs.size1() == m.size1(), ublas::internal_logic());
    BOOST_UBLAS_CHECK(rhs.size2() == m.size2(), ublas::internal_logic());

	#if BOOST_UBLAS_TYPE_CHECK
		BOOST_UBLAS_CHECK(
			ublas::detail::expression_type_check(
				ublas::prod(m, rhs),
				ublas::identity_matrix<typename M::value_type>(m.size1())
			),
			ublas::internal_logic()
		);
	#endif

    mi.resize(rhs.size1(), rhs.size2(), false);
    mi.assign(rhs);
    // mi.assign_temporary(rhs);
}

}

#endif
