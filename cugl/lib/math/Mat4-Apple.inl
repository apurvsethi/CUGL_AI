//
//  Mat4-Apple.inl
//  Cornell University Game Library (CUGL)
//
//  This module provides vectorized support for matrix multiplication on Apple
//  platforms.  Profiling tests show that it is really not that much slower than
//  non-vectorized computation because of the small size of the matrix.
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
    vSzaxpy(16,scalar,ONE.col,mat.col,dst->col);
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
    vSgeadd(4,4,m1.col,'N',m2.col,'N',dst->col);
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
    vSzaxpy(16,-scalar,ONE.col,mat.col,dst->col);
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
    vSgesub(4,4,m1.col,'N',m2.col,'N',dst->col);
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
    vSyax(16,scalar,mat.col,dst->col);
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
    vSgemul(4,4,4,m1.col,'N',m2.col,'N',dst->col);
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
Mat4* Mat4::negate(const Mat4& m1, Mat4* dst) {
    vSyax(16,-1.0f,m1.col,dst->col);
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
    vSgetmo(4,4,m1.col,dst->col);
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
    Vec4 tmp;
    vSgemtx(4,4,1.0f,&(mat.col[0]),&(vec.v),&(tmp.v));
    dst->v = tmp.v;
    return dst;
}