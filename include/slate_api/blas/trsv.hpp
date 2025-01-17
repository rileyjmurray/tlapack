// Copyright (c) 2017-2021, University of Tennessee. All rights reserved.
// Copyright (c) 2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef SLATE_BLAS_TRSV_HH
#define SLATE_BLAS_TRSV_HH

#include "blas/utils.hpp"
#include "blas/trsv.hpp"

namespace blas {

/**
 * Solve the triangular matrix-vector equation
 * \[
 *     op(A) x = b,
 * \]
 * where $op(A)$ is one of
 *     $op(A) = A$,
 *     $op(A) = A^T$, or
 *     $op(A) = A^H$,
 * x and b are vectors,
 * and A is an n-by-n, unit or non-unit, upper or lower triangular matrix.
 *
 * No test for singularity or near-singularity is included in this
 * routine. Such tests must be performed before calling this routine.
 * @see LAPACK's latrs for a more numerically robust implementation.
 *
 * Generic implementation for arbitrary data types.
 *
 * @param[in] layout
 *     Matrix storage, Layout::ColMajor or Layout::RowMajor.
 *
 * @param[in] uplo
 *     What part of the matrix A is referenced,
 *     the opposite triangle being assumed to be zero.
 *     - Uplo::Lower: A is lower triangular.
 *     - Uplo::Upper: A is upper triangular.
 *
 * @param[in] trans
 *     The equation to be solved:
 *     - Op::NoTrans:   $A   x = b$,
 *     - Op::Trans:     $A^T x = b$,
 *     - Op::ConjTrans: $A^H x = b$.
 *
 * @param[in] diag
 *     Whether A has a unit or non-unit diagonal:
 *     - Diag::Unit:    A is assumed to be unit triangular.
 *                      The diagonal elements of A are not referenced.
 *     - Diag::NonUnit: A is not assumed to be unit triangular.
 *
 * @param[in] n
 *     Number of rows and columns of the matrix A. n >= 0.
 *
 * @param[in] A
 *     The n-by-n matrix A, stored in an lda-by-n array [RowMajor: n-by-lda].
 *
 * @param[in] lda
 *     Leading dimension of A. lda >= max(1, n).
 *
 * @param[in, out] x
 *     The n-element vector x, in an array of length (n-1)*abs(incx) + 1.
 *
 * @param[in] incx
 *     Stride between elements of x. incx must not be zero.
 *     If incx < 0, uses elements of x in reverse order: x(n-1), ..., x(0).
 *
 * @ingroup trsv
 */
template< typename TA, typename TX >
void trsv(
    blas::Layout layout,
    blas::Uplo uplo,
    blas::Op trans,
    blas::Diag diag,
    blas::idx_t n,
    TA const *A, blas::idx_t lda,
    TX       *x, blas::int_t incx )
{
    using blas::internal::colmajor_matrix;
    using blas::internal::vector;

    // check arguments
    blas_error_if( layout != Layout::ColMajor &&
                   layout != Layout::RowMajor );
    blas_error_if( uplo != Uplo::Lower &&
                   uplo != Uplo::Upper );
    blas_error_if( trans != Op::NoTrans &&
                   trans != Op::Trans &&
                   trans != Op::ConjTrans );
    blas_error_if( diag != Diag::NonUnit &&
                   diag != Diag::Unit );
    blas_error_if( n < 0 );
    blas_error_if( lda < n );
    blas_error_if( incx == 0 );

    // quick return
    if (n == 0)
        return;

    // for row major, swap lower <=> upper and
    // A => A^T; A^T => A; A^H => A & conj
    if (layout == Layout::RowMajor) {
        uplo = (uplo == Uplo::Lower ? Uplo::Upper : Uplo::Lower);
        trans = (trans == Op::NoTrans)
              ? Op::Trans
              : ((trans == Op::Trans) ? Op::NoTrans : Op::Conj);
    }
        
    // Matrix views
    const auto _A = colmajor_matrix<TA>( (TA*)A, n, n, lda );
    auto _x = vector<TX>( &x[(incx > 0 ? 0 : (-n + 1)*incx)], n, incx );

    trsv( uplo, trans, diag, _A, _x );
}

}  // namespace blas

#endif        //  #ifndef SLATE_BLAS_TRSV_HH
