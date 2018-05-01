//
//  Mat4-SSE.inl
//  Cornell University Game Library (CUGL)
//
//  This module provides vectorized support for matrix multiplication on SSE
//  (e.g. Windows) platforms.  Profiling tests show that it is really not that
//  much slower than non-vectorized computation because of the small size of
//  the matrix.
//
//  Because math objects are intended to be on the stack, we do not provide
//  any shared pointer support in this class.
//
//  This module is based on an original file from GamePlay3D: http://gameplay3d.org.
//  It has been modified to support the CUGL framework.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 6/12/16

/**
 * Adds a scalar to each component of mat and stores the result in dst.
 *
 * @param mat       The matrix to add to.
 * @param scalar    The scalar value to add.
 * @param dst       A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::add(const Mat4& mat, float scalar, Mat4* dst) {
    __m128 s = _mm_set1_ps(scalar);
    dst->col[0] = _mm_add_ps(mat.col[0], s);
    dst->col[1] = _mm_add_ps(mat.col[1], s);
    dst->col[2] = _mm_add_ps(mat.col[2], s);
    dst->col[3] = _mm_add_ps(mat.col[3], s);
    return dst;
}

/**
 * Adds the specified matrices and stores the result in dst.
 *
 * @param m1    The first matrix.
 * @param m2    The second matrix.
 * @param dst   The destination matrix to add to.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::add(const Mat4& m1, const Mat4& m2, Mat4* dst) {
    dst->col[0] = _mm_add_ps(m1.col[0], m2.col[0]);
    dst->col[1] = _mm_add_ps(m1.col[1], m2.col[1]);
    dst->col[2] = _mm_add_ps(m1.col[2], m2.col[2]);
    dst->col[3] = _mm_add_ps(m1.col[3], m2.col[3]);
    return dst;
}

/**
 * Subtracts a scalar from each component of mat and stores the result in dst.
 *
 * @param mat       The matrix to subtract from.
 * @param scalar    The scalar value to subtract.
 * @param dst       A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::subtract(const Mat4& mat, float scalar, Mat4* dst) {
    __m128 s = _mm_set1_ps(scalar);
    dst->col[0] = _mm_sub_ps(mat.col[0], s);
    dst->col[1] = _mm_sub_ps(mat.col[1], s);
    dst->col[2] = _mm_sub_ps(mat.col[2], s);
    dst->col[3] = _mm_sub_ps(mat.col[3], s);
    return dst;
}

/**
 * Subtracts the matrix m2 from m1 and stores the result in dst.
 *
 * @param m1    The first matrix.
 * @param m2    The second matrix.
 * @param dst   A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::subtract(const Mat4& m1, const Mat4& m2, Mat4* dst) {
    dst->col[0] = _mm_sub_ps(m1.col[0], m2.col[0]);
    dst->col[1] = _mm_sub_ps(m1.col[1], m2.col[1]);
    dst->col[2] = _mm_sub_ps(m1.col[2], m2.col[2]);
    dst->col[3] = _mm_sub_ps(m1.col[3], m2.col[3]);
    return dst;
}

/**
 * Multiplies the specified matrix by a scalar and stores the result in dst.
 *
 * @param mat       The matrix.
 * @param scalar    The scalar value.
 * @param dst       A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::multiply(const Mat4& mat, float scalar, Mat4* dst) {
    __m128 s = _mm_set1_ps(scalar);
    dst->col[0] = _mm_mul_ps(mat.col[0], s);
    dst->col[1] = _mm_mul_ps(mat.col[1], s);
    dst->col[2] = _mm_mul_ps(mat.col[2], s);
    dst->col[3] = _mm_mul_ps(mat.col[3], s);
    return dst;
}

/**
 * Multiplies m1 by the matrix m2 and stores the result in dst.
 *
 * The matrix m2 is on the right.  This means that it corresponds to
 * an subsequent transform, when looking at a sequence of transforms.
 *
 * @param m1    The first matrix to multiply.
 * @param m2    The second matrix to multiply.
 * @param dst   A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::multiply(const Mat4& m1, const Mat4& m2, Mat4* dst) {
    __m128 dst0, dst1, dst2, dst3;
    {
		__m128 col = m1.col[0];
        __m128 e0 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 e1 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 e2 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 e3 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(3, 3, 3, 3));
        
        __m128 v0 = _mm_mul_ps(m2.col[0], e0);
        __m128 v1 = _mm_mul_ps(m2.col[1], e1);
        __m128 v2 = _mm_mul_ps(m2.col[2], e2);
        __m128 v3 = _mm_mul_ps(m2.col[3], e3);
        
        __m128 a0 = _mm_add_ps(v0, v1);
        __m128 a1 = _mm_add_ps(v2, v3);
        __m128 a2 = _mm_add_ps(a0, a1);
        
        dst0 = a2;
    }
    
    {
		__m128 col = m1.col[1];
		__m128 e0 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 v0 = _mm_mul_ps(m2.col[0], e0);
        __m128 v1 = _mm_mul_ps(m2.col[1], e1);
        __m128 v2 = _mm_mul_ps(m2.col[2], e2);
        __m128 v3 = _mm_mul_ps(m2.col[3], e3);
        
        __m128 a0 = _mm_add_ps(v0, v1);
        __m128 a1 = _mm_add_ps(v2, v3);
        __m128 a2 = _mm_add_ps(a0, a1);
        
        dst1 = a2;
    }
    
    {
		__m128 col = m1.col[2];
		__m128 e0 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 v0 = _mm_mul_ps(m2.col[0], e0);
        __m128 v1 = _mm_mul_ps(m2.col[1], e1);
        __m128 v2 = _mm_mul_ps(m2.col[2], e2);
        __m128 v3 = _mm_mul_ps(m2.col[3], e3);
        
        __m128 a0 = _mm_add_ps(v0, v1);
        __m128 a1 = _mm_add_ps(v2, v3);
        __m128 a2 = _mm_add_ps(a0, a1);
        
        dst2 = a2;
    }
    
    {
		__m128 col = m1.col[3];
		__m128 e0 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(col, col, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 v0 = _mm_mul_ps(m2.col[0], e0);
        __m128 v1 = _mm_mul_ps(m2.col[1], e1);
        __m128 v2 = _mm_mul_ps(m2.col[2], e2);
        __m128 v3 = _mm_mul_ps(m2.col[3], e3);
        
        __m128 a0 = _mm_add_ps(v0, v1);
        __m128 a1 = _mm_add_ps(v2, v3);
        __m128 a2 = _mm_add_ps(a0, a1);
        
        dst3 = a2;
    }
    dst->col[0] = dst0;
    dst->col[1] = dst1;
    dst->col[2] = dst2;
    dst->col[3] = dst3;
    return dst;
}

/**
 * Negates m1 and stores the result in dst.
 *
 * @param m1    The matrix to negate.
 * @param dst   A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::negate(const Mat4& mat, Mat4* dst) {
    __m128 z = _mm_setzero_ps();
    dst->col[0] = _mm_sub_ps(z, mat.col[0]);
    dst->col[1] = _mm_sub_ps(z, mat.col[1]);
    dst->col[2] = _mm_sub_ps(z, mat.col[2]);
    dst->col[3] = _mm_sub_ps(z, mat.col[3]);
    return dst;
}

/**
 * Transposes m1 and stores the result in dst.
 *
 * Transposing a matrix swaps columns and rows. This allows to transform
 * a vector by multiplying it on the left. If the matrix is orthonormal,
 * this is also the inverse.
 *
 * @param m1    The matrix to negate.
 * @param dst   A matrix to store the result in.
 *
 * @return A reference to dst for chaining
 */
Mat4* Mat4::transpose(const Mat4& m1, Mat4* dst) {
    __m128 tmp0 = _mm_shuffle_ps(m1.col[0], m1.col[1], 0x44);
    __m128 tmp2 = _mm_shuffle_ps(m1.col[0], m1.col[1], 0xEE);
    __m128 tmp1 = _mm_shuffle_ps(m1.col[2], m1.col[3], 0x44);
    __m128 tmp3 = _mm_shuffle_ps(m1.col[2], m1.col[3], 0xEE);
    
    dst->col[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
    dst->col[1] = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
    dst->col[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
    dst->col[3] = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    return dst;
}

/**
 * Transforms the vector by the given matrix, and stores the result in dst.
 *
 * The vector is treated as is.  Hence whether or not translation is applied
 * depends on the value of w.
 *
 * @param mat   The transform matrix.
 * @param vec   The vector to transform.
 * @param dst   A vector to store the transformed point in.
 *
 * @return A reference to dst for chaining
 */
Vec4* Mat4::transform(const Mat4& mat, const Vec4& vec, Vec4* dst) {
    __m128 col1 = _mm_shuffle_ps(vec.v, vec.v, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 col2 = _mm_shuffle_ps(vec.v, vec.v, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 col3 = _mm_shuffle_ps(vec.v, vec.v, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 col4 = _mm_shuffle_ps(vec.v, vec.v, _MM_SHUFFLE(3, 3, 3, 3));
    
    dst->v = _mm_add_ps(
                        _mm_add_ps(_mm_mul_ps(mat.col[0], col1), _mm_mul_ps(mat.col[1], col2)),
                        _mm_add_ps(_mm_mul_ps(mat.col[2], col3), _mm_mul_ps(mat.col[3], col4))
                        );
    return dst;
}