// Copyright (c) 2017-2021, University of Tennessee. All rights reserved.
// Copyright (c) 2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __TBLAS_TYPES_HH__
#define __TBLAS_TYPES_HH__

#include <complex>

namespace blas {

// -----------------------------------------------------------------------------
// Enumerations
enum class Layout { ColMajor = 'C', RowMajor = 'R' };
enum class Op     { NoTrans  = 'N', Trans    = 'T', ConjTrans = 'C', Conj };
enum class Uplo   { Upper    = 'U', Lower    = 'L', General   = 'G' };
enum class Diag   { NonUnit  = 'N', Unit     = 'U' };
enum class Side   { Left     = 'L', Right    = 'R' };

// -----------------------------------------------------------------------------
// Check for Infs and NaNs types

struct nocheck_t { };
struct checkInfNaN_t { };

// constants
constexpr nocheck_t nocheck = { };
constexpr checkInfNaN_t checkInfNaN = { };

// -----------------------------------------------------------------------------
// Strong numeric expressions

struct zero_t { };
constexpr zero_t zero = { };

// -----------------------------------------------------------------------------
// common_type_t is defined in C++14; here's a C++11 definition
#if __cplusplus >= 201402L
    using std::common_type_t;
    using std::decay_t;
#else
    template< typename... Ts >
    using common_type_t = typename std::common_type< Ts... >::type;
    template< typename... Ts >
    using decay_t = typename std::decay< Ts... >::type;
#endif

// -----------------------------------------------------------------------------
// enable_if_t is defined in C++14; here's a C++11 definition
#if __cplusplus >= 201402L
    using std::enable_if_t;
#else
    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;
#endif

// -----------------------------------------------------------------------------
// is_same_v is defined in C++17; here's a C++11 definition
#if __cplusplus >= 201703L
    using std::is_same_v;
#else
    template< class T, class U >
    constexpr bool is_same_v = std::is_same<T, U>::value;
#endif

//------------------------------------------------------------------------------
/// True if T is std::complex<T2> for some type T2.
template <typename T>
struct is_complex:
    std::integral_constant<bool, false>
{};

/// specialize for std::complex
template <typename T>
struct is_complex< std::complex<T> >:
    std::integral_constant<bool, true>
{};

// -----------------------------------------------------------------------------
// Based on C++14 common_type implementation from
// http://www.cplusplus.com/reference/type_traits/common_type/
// Adds promotion of complex types based on the common type of the associated
// real types. This fixes various cases:
//
// std::common_type_t< double, complex<float> > is complex<float>  (wrong)
//        scalar_type< double, complex<float> > is complex<double> (right)
//
// std::common_type_t< int, complex<long> > is not defined (compile error)
//        scalar_type< int, complex<long> > is complex<long> (right)

// for zero types
template< typename... Types >
struct scalar_type_traits;

/// define scalar_type<> type alias
template< typename... Types >
using scalar_type = typename scalar_type_traits< Types... >::type;

// for one type
template< typename T >
struct scalar_type_traits< T >
{
    using type = decay_t<T>;
};

// for two types
// relies on type of ?: operator being the common type of its two arguments
template< typename T1, typename T2 >
struct scalar_type_traits< T1, T2 >
{
    using type = decay_t< decltype( true ? std::declval<T1>() : std::declval<T2>() ) >;
};

// for either or both complex,
// find common type of associated real types, then add complex
template< typename T1, typename T2 >
struct scalar_type_traits< std::complex<T1>, T2 >
{
    using type = std::complex< common_type_t< T1, T2 > >;
};

template< typename T1, typename T2 >
struct scalar_type_traits< T1, std::complex<T2> >
{
    using type = std::complex< common_type_t< T1, T2 > >;
};

template< typename T1, typename T2 >
struct scalar_type_traits< std::complex<T1>, std::complex<T2> >
{
    using type = std::complex< common_type_t< T1, T2 > >;
};

// for three or more types
template< typename T1, typename T2, typename... Types >
struct scalar_type_traits< T1, T2, Types... >
{
    using type = scalar_type< scalar_type< T1, T2 >, Types... >;
};

// -----------------------------------------------------------------------------
// for any combination of types, determine associated real, scalar,
// and complex types.
//
// real_type< float >                               is float
// real_type< float, double, complex<float> >       is double
//
// scalar_type< float >                             is float
// scalar_type< float, complex<float> >             is complex<float>
// scalar_type< float, double, complex<float> >     is complex<double>
//
// complex_type< float >                            is complex<float>
// complex_type< float, double >                    is complex<double>
// complex_type< float, double, complex<float> >    is complex<double>

// for zero types
template< typename... Types >
struct real_type_traits;

/// define real_type<> type alias
template< typename... Types >
using real_type = typename real_type_traits< Types... >::real_t;

/// define complex_type<> type alias
template< typename... Types >
using complex_type = std::complex< real_type< Types... > >;

// for one type
template< typename T >
struct real_type_traits<T>
{
    using real_t = T;
};

// for one complex type, strip complex
template< typename T >
struct real_type_traits< std::complex<T> >
{
    using real_t = T;
};

// for two or more types
template< typename T1, typename... Types >
struct real_type_traits< T1, Types... >
{
    using real_t = scalar_type< real_type<T1>, real_type< Types... > >;
};

// -----------------------------------------------------------------------------
// Data traits

#ifndef TBLAS_ARRAY_TRAITS
#define TBLAS_ARRAY_TRAITS

    // Data type
    template< class T > struct type_trait {};
    template< class T >
    using type_t = typename type_trait< T >::type;
    // Size type
    template< class T > struct sizet_trait {};
    template< class T >
    using size_type = typename sizet_trait< T >::type;

#endif // TBLAS_ARRAY_TRAITS

} // namespace blas

#endif // __TBLAS_TYPES_HH__
