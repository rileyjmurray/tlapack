// Copyright (c) 2017-2021, University of Tennessee. All rights reserved.
// Copyright (c) 2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef SLATE_BLAS_SYRK_HH
#define SLATE_BLAS_SYRK_HH

#include "blas/utils.hpp"
#include "blas/syrk.hpp"

namespace blas {

/**
 * Symmetric rank-k update:
 * \[
 *     C = \alpha A A^T + \beta C,
 * \]
 * or
 * \[
 *     C = \alpha A^T A + \beta C,
 * \]
 * where alpha and beta are scalars, C is an n-by-n symmetric matrix,
 * and A is an n-by-k or k-by-n matrix.
 *
 * Generic implementation for arbitrary data types.
 *
 * @param[in] layout
 *     Matrix storage, Layout::ColMajor or Layout::RowMajor.
 *
 * @param[in] uplo
 *     What part of the matrix C is referenced,
 *     the opposite triangle being assumed from symmetry:
 *     - Uplo::Lower: only the lower triangular part of C is referenced.
 *     - Uplo::Upper: only the upper triangular part of C is referenced.
 *
 * @param[in] trans
 *     The operation to be performed:
 *     - Op::NoTrans: $C = \alpha A A^T + \beta C$.
 *     - Op::Trans:   $C = \alpha A^T A + \beta C$.
 *     - In the real    case, Op::ConjTrans is interpreted as Op::Trans.
 *       In the complex case, Op::ConjTrans is illegal (see @ref herk instead).
 *
 * @param[in] n
 *     Number of rows and columns of the matrix C. n >= 0.
 *
 * @param[in] k
 *     - If trans = NoTrans: number of columns of the matrix A. k >= 0.
 *     - Otherwise:          number of rows    of the matrix A. k >= 0.
 *
 * @param[in] alpha
 *     Scalar alpha. If alpha is zero, A is not accessed.
 *
 * @param[in] A
 *     - If trans = NoTrans:
 *       the n-by-k matrix A, stored in an lda-by-k array [RowMajor: n-by-lda].
 *     - Otherwise:
 *       the k-by-n matrix A, stored in an lda-by-n array [RowMajor: k-by-lda].
 *
 * @param[in] lda
 *     Leading dimension of A.
 *     - If trans = NoTrans: lda >= max(1, n) [RowMajor: lda >= max(1, k)],
 *     - Otherwise:              lda >= max(1, k) [RowMajor: lda >= max(1, n)].
 *
 * @param[in] beta
 *     Scalar beta. If beta is zero, C need not be set on input.
 *
 * @param[in] C
 *     The n-by-n symmetric matrix C,
 *     stored in an lda-by-n array [RowMajor: n-by-lda].
 *
 * @param[in] ldc
 *     Leading dimension of C. ldc >= max(1, n).
 *
 * @ingroup syrk
 */
template< typename TA, typename TC >
void syrk(
    blas::Layout layout,
    blas::Uplo uplo,
    blas::Op trans,
    blas::idx_t n, blas::idx_t k,
    scalar_type<TA, TC> alpha,
    TA const *A, blas::idx_t lda,
    scalar_type<TA, TC> beta,
    TC       *C, blas::idx_t ldc )
{
    typedef blas::scalar_type<TA, TC> scalar_t;
    using blas::internal::colmajor_matrix;

    // constants
    const scalar_t zero( 0 );
    const scalar_t one( 1 );

    // check arguments
    blas_error_if( layout != Layout::ColMajor &&
                   layout != Layout::RowMajor );
    blas_error_if( uplo != Uplo::Lower &&
                   uplo != Uplo::Upper &&
                   uplo != Uplo::General );
    blas_error_if( trans != Op::NoTrans &&
                   trans != Op::Trans &&
                   trans != Op::ConjTrans );
    blas_error_if( is_complex<TA>::value && trans == Op::ConjTrans );
    blas_error_if( n < 0 );
    blas_error_if( k < 0 );
    blas_error_if( lda < (
        (layout == Layout::RowMajor)
            ? ((trans == Op::NoTrans) ? k : n)
            : ((trans == Op::NoTrans) ? n : k)
        )
    );
    blas_error_if( ldc < n );

    // quick return
    if (n == 0)
        return;

    // This algorithm only works with Op::NoTrans or Op::Trans
    if(trans == Op::ConjTrans) trans = Op::Trans;

    // adapt if row major
    if (layout == Layout::RowMajor) {       
        if (uplo == Uplo::Lower)
            uplo = Uplo::Upper;
        else if (uplo == Uplo::Upper)
            uplo = Uplo::Lower;
        trans = (trans == Op::NoTrans)
            ? Op::Trans
            : Op::NoTrans;
    }

    // Matrix views
    const auto _A = (trans == Op::NoTrans)
                  ? colmajor_matrix<TA>( (TA*)A, n, k, lda )
                  : colmajor_matrix<TA>( (TA*)A, k, n, lda );
    auto _C = colmajor_matrix<TC>( C, n, n, ldc );

    // alpha == zero
    if (alpha == zero) {
        if (beta == zero) {
            if (uplo != Uplo::Upper) {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = 0; i <= j; ++i)
                        _C(i,j) = zero;
                }
            }
            else if (uplo != Uplo::Lower) {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = j; i < n; ++i)
                        _C(i,j) = zero;
                }
            }
            else {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = 0; i < n; ++i)
                        _C(i,j) = zero;
                }
            }
        }
        else if (beta != one) {
            if (uplo != Uplo::Upper) {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = 0; i <= j; ++i)
                        _C(i,j) *= beta;
                }
            }
            else if (uplo != Uplo::Lower) {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = j; i < n; ++i)
                        _C(i,j) *= beta;
                }
            }
            else {
                for(idx_t j = 0; j < n; ++j) {
                    for(idx_t i = 0; i < n; ++i)
                        _C(i,j) *= beta;
                }
            }
        }
        return;
    }
    
    syrk( uplo, trans, alpha, _A, beta, _C );
}

}  // namespace blas

#endif        //  #ifndef SLATE_BLAS_SYMM_HH
