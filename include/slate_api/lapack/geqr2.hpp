/// @file geqr2.hpp
/// @author Weslley S Pereira, University of Colorado Denver, USA
/// Adapted from @see https://github.com/langou/latl/blob/master/include/geqr2.h
//
// Copyright (c) 2013-2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __SLATE_GEQR2_HH__
#define __SLATE_GEQR2_HH__

#include "lapack/geqr2.hpp"

namespace lapack {

/** Computes a QR factorization of a matrix A.
 * 
 * @param[in] m The number of rows of the matrix A.
 * @param[in] n The number of columns of the matrix A.
 * @param[in,out] A m-by-n matrix.
 *      On exit, the elements on and above the diagonal of the array
 *      contain the min(m,n)-by-n upper trapezoidal matrix R
 *      (R is upper triangular if m >= n); the elements below the diagonal,
 *      with the array tau, represent the unitary matrix Q as a
 *      product of elementary reflectors.
 * @param[in] lda The leading dimension of A. lda >= max(1,m).
 * @param[out] tau Real vector of length min(m,n).
 *      The scalar factors of the elementary reflectors.
 *      The subarray tau[1:n-1] is used as the workspace.
 * 
 * @see geqr2( matrix_t& A, vector_t &tau, vector_t &work )
 * 
 * @ingroup geqrf
 */
template< typename TA, typename Ttau >
inline int geqr2(
    blas::idx_t m, blas::idx_t n,
    TA*   A, blas::idx_t lda,
    Ttau* tau )
{
    using blas::internal::colmajor_matrix;
    using blas::internal::vector;
    using work_t = scalar_type<TA,Ttau>;

    // check arguments
    lapack_error_if( m < 0, -1 );
    lapack_error_if( n < 0, -2 );
    lapack_error_if( lda < m, -4 );

    // quick return
    if (n <= 0) return 0;

    // Local parameters
    int info = 0;
    work_t* work;
    if( !is_same_v< TA, Ttau > || n-1 >= m ) {
        work = new work_t[ (n > 0) ? n-1 : 0 ];
    } else {
        work = tau + 1;
    }

    // Matrix views
    auto _A    = colmajor_matrix<TA>( A, m, n, lda );
    auto _tau  = vector<Ttau>  ( tau, std::min<blas::idx_t>( m, n ), 1 );
    auto _work = vector<work_t>( work, n-1, 1 );
    
    info = geqr2( _A, _tau, _work );

    if( !is_same_v< TA, Ttau > || n-1 >= m )
        delete[] work;
        
    return info;
}

} // lapack

#endif // __SLATE_GEQR2_HH__
