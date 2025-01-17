// Copyright (c) 2017-2021, University of Tennessee. All rights reserved.
// Copyright (c) 2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef BLAS_COPY_HH
#define BLAS_COPY_HH

#include "blas/utils.hpp"

namespace blas {

/**
 * Copy vector, $y = x$.
 *
 * Generic implementation for arbitrary data types.
 *
 * @param[in] n
 *     Number of elements in x and y. n >= 0.
 *
 * @param[in] x
 *     The n-element vector x, in an array of length (n-1)*abs(incx) + 1.
 *
 * @param[in] incx
 *     Stride between elements of x. incx must not be zero.
 *     If incx < 0, uses elements of x in reverse order: x(n-1), ..., x(0).
 *
 * @param[out] y
 *     The n-element vector y, in an array of length (n-1)*abs(incy) + 1.
 *
 * @param[in] incy
 *     Stride between elements of y. incy must not be zero.
 *     If incy < 0, uses elements of y in reverse order: y(n-1), ..., y(0).
 *
 * @ingroup copy
 */
template< class vectorX_t, class vectorY_t >
void copy( const vectorX_t& x, vectorY_t& y )
{
    using idx_t = size_type< vectorY_t >;

    // constants
    const idx_t n = size(y);

    // check arguments
    blas_error_if( size(x) < n );

    for (idx_t i = 0; i < n; ++i)
        y[i] = x[i];
}

}  // namespace blas

#endif        //  #ifndef BLAS_COPY_HH
