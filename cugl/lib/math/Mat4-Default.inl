//
//  Mat4-Default.inl
//  Cornell University Game Library (CUGL)
//
//  This module provides support for matrix multiplication on platforms that
//  do not provide vectorization.  Profiling tests show that it is really
//  not that much slower than vectorized computation because of the small
//  size of the matrix.
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
    float* m = dst->m;
    m[0]  = mat.m[0]  + scalar;
    m[1]  = mat.m[1]  + scalar;
    m[2]  = mat.m[2]  + scalar;
    m[3]  = mat.m[3]  + scalar;
    m[4]  = mat.m[4]  + scalar;
    m[5]  = mat.m[5]  + scalar;
    m[6]  = mat.m[6]  + scalar;
    m[7]  = mat.m[7]  + scalar;
    m[8]  = mat.m[8]  + scalar;
    m[9]  = mat.m[9]  + scalar;
    m[10] = mat.m[10] + scalar;
    m[11] = mat.m[11] + scalar;
    m[12] = mat.m[12] + scalar;
    m[13] = mat.m[13] + scalar;
    m[14] = mat.m[14] + scalar;
    m[15] = mat.m[15] + scalar;
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
    float* m = dst->m;
    m[0]  = m1.m[0]  + m2.m[0];
    m[1]  = m1.m[1]  + m2.m[1];
    m[2]  = m1.m[2]  + m2.m[2];
    m[3]  = m1.m[3]  + m2.m[3];
    m[4]  = m1.m[4]  + m2.m[4];
    m[5]  = m1.m[5]  + m2.m[5];
    m[6]  = m1.m[6]  + m2.m[6];
    m[7]  = m1.m[7]  + m2.m[7];
    m[8]  = m1.m[8]  + m2.m[8];
    m[9]  = m1.m[9]  + m2.m[9];
    m[10] = m1.m[10] + m2.m[10];
    m[11] = m1.m[11] + m2.m[11];
    m[12] = m1.m[12] + m2.m[12];
    m[13] = m1.m[13] + m2.m[13];
    m[14] = m1.m[14] + m2.m[14];
    m[15] = m1.m[15] + m2.m[15];
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
    float* m = dst->m;
    m[0]  = mat.m[0]  - scalar;
    m[1]  = mat.m[1]  - scalar;
    m[2]  = mat.m[2]  - scalar;
    m[3]  = mat.m[3]  - scalar;
    m[4]  = mat.m[4]  - scalar;
    m[5]  = mat.m[5]  - scalar;
    m[6]  = mat.m[6]  - scalar;
    m[7]  = mat.m[7]  - scalar;
    m[8]  = mat.m[8]  - scalar;
    m[9]  = mat.m[9]  - scalar;
    m[10] = mat.m[10] - scalar;
    m[11] = mat.m[11] - scalar;
    m[12] = mat.m[12] - scalar;
    m[13] = mat.m[13] - scalar;
    m[14] = mat.m[14] - scalar;
    m[15] = mat.m[15] - scalar;
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
    float* m = dst->m;
    m[0]  = m1.m[0]  - m2.m[0];
    m[1]  = m1.m[1]  - m2.m[1];
    m[2]  = m1.m[2]  - m2.m[2];
    m[3]  = m1.m[3]  - m2.m[3];
    m[4]  = m1.m[4]  - m2.m[4];
    m[5]  = m1.m[5]  - m2.m[5];
    m[6]  = m1.m[6]  - m2.m[6];
    m[7]  = m1.m[7]  - m2.m[7];
    m[8]  = m1.m[8]  - m2.m[8];
    m[9]  = m1.m[9]  - m2.m[9];
    m[10] = m1.m[10] - m2.m[10];
    m[11] = m1.m[11] - m2.m[11];
    m[12] = m1.m[12] - m2.m[12];
    m[13] = m1.m[13] - m2.m[13];
    m[14] = m1.m[14] - m2.m[14];
    m[15] = m1.m[15] - m2.m[15];
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
    float* m = dst->m;
    m[0]  = mat.m[0]  * scalar;
    m[1]  = mat.m[1]  * scalar;
    m[2]  = mat.m[2]  * scalar;
    m[3]  = mat.m[3]  * scalar;
    m[4]  = mat.m[4]  * scalar;
    m[5]  = mat.m[5]  * scalar;
    m[6]  = mat.m[6]  * scalar;
    m[7]  = mat.m[7]  * scalar;
    m[8]  = mat.m[8]  * scalar;
    m[9]  = mat.m[9]  * scalar;
    m[10] = mat.m[10] * scalar;
    m[11] = mat.m[11] * scalar;
    m[12] = mat.m[12] * scalar;
    m[13] = mat.m[13] * scalar;
    m[14] = mat.m[14] * scalar;
    m[15] = mat.m[15] * scalar;
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
    float product[16];
    
    product[0]  = m2.m[0] * m1.m[0]  + m2.m[4] * m1.m[1] + m2.m[8]   * m1.m[2]  + m2.m[12] * m1.m[3];
    product[1]  = m2.m[1] * m1.m[0]  + m2.m[5] * m1.m[1] + m2.m[9]   * m1.m[2]  + m2.m[13] * m1.m[3];
    product[2]  = m2.m[2] * m1.m[0]  + m2.m[6] * m1.m[1] + m2.m[10]  * m1.m[2]  + m2.m[14] * m1.m[3];
    product[3]  = m2.m[3] * m1.m[0]  + m2.m[7] * m1.m[1] + m2.m[11]  * m1.m[2]  + m2.m[15] * m1.m[3];
    
    product[4]  = m2.m[0] * m1.m[4]  + m2.m[4] * m1.m[5] + m2.m[8]   * m1.m[6]  + m2.m[12] * m1.m[7];
    product[5]  = m2.m[1] * m1.m[4]  + m2.m[5] * m1.m[5] + m2.m[9]   * m1.m[6]  + m2.m[13] * m1.m[7];
    product[6]  = m2.m[2] * m1.m[4]  + m2.m[6] * m1.m[5] + m2.m[10]  * m1.m[6]  + m2.m[14] * m1.m[7];
    product[7]  = m2.m[3] * m1.m[4]  + m2.m[7] * m1.m[5] + m2.m[11]  * m1.m[6]  + m2.m[15] * m1.m[7];
    
    product[8]  = m2.m[0] * m1.m[8]  + m2.m[4] * m1.m[9] + m2.m[8]   * m1.m[10] + m2.m[12] * m1.m[11];
    product[9]  = m2.m[1] * m1.m[8]  + m2.m[5] * m1.m[9] + m2.m[9]   * m1.m[10] + m2.m[13] * m1.m[11];
    product[10] = m2.m[2] * m1.m[8]  + m2.m[6] * m1.m[9] + m2.m[10]  * m1.m[10] + m2.m[14] * m1.m[11];
    product[11] = m2.m[3] * m1.m[8]  + m2.m[7] * m1.m[9] + m2.m[11]  * m1.m[10] + m2.m[15] * m1.m[11];
    
    product[12] = m2.m[0] * m1.m[12] + m2.m[4] * m1.m[13] + m2.m[8]  * m1.m[14] + m2.m[12] * m1.m[15];
    product[13] = m2.m[1] * m1.m[12] + m2.m[5] * m1.m[13] + m2.m[9]  * m1.m[14] + m2.m[13] * m1.m[15];
    product[14] = m2.m[2] * m1.m[12] + m2.m[6] * m1.m[13] + m2.m[10] * m1.m[14] + m2.m[14] * m1.m[15];
    product[15] = m2.m[3] * m1.m[12] + m2.m[7] * m1.m[13] + m2.m[11] * m1.m[14] + m2.m[15] * m1.m[15];
    
    std::memcpy(&(dst->m[0]), &(product[0]), MATRIX_SIZE);
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
    float* m = dst->m;
    m[0]  = -m1.m[0];
    m[1]  = -m1.m[1];
    m[2]  = -m1.m[2];
    m[3]  = -m1.m[3];
    m[4]  = -m1.m[4];
    m[5]  = -m1.m[5];
    m[6]  = -m1.m[6];
    m[7]  = -m1.m[7];
    m[8]  = -m1.m[8];
    m[9]  = -m1.m[9];
    m[10] = -m1.m[10];
    m[11] = -m1.m[11];
    m[12] = -m1.m[12];
    m[13] = -m1.m[13];
    m[14] = -m1.m[14];
    m[15] = -m1.m[15];
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
    float t[16] = {
        m1.m[0], m1.m[4], m1.m[8],  m1.m[12],
        m1.m[1], m1.m[5], m1.m[9],  m1.m[13],
        m1.m[2], m1.m[6], m1.m[10], m1.m[14],
        m1.m[3], m1.m[7], m1.m[11], m1.m[15]
    };
    std::memcpy(&(dst->m[0]), &(t[0]), MATRIX_SIZE);
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
    // Handle case where v == dst.
    float x = vec.x * mat.m[0] + vec.y * mat.m[4] + vec.z * mat.m[8]  + vec.w * mat.m[12];
    float y = vec.x * mat.m[1] + vec.y * mat.m[5] + vec.z * mat.m[9]  + vec.w * mat.m[13];
    float z = vec.x * mat.m[2] + vec.y * mat.m[6] + vec.z * mat.m[10] + vec.w * mat.m[14];
    float w = vec.x * mat.m[3] + vec.y * mat.m[7] + vec.z * mat.m[11] + vec.w * mat.m[15];
    
    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = w;
    return dst;
}
