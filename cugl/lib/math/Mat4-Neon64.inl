//
//  Mat4-Neon64.inl
//  Cornell University Game Library (CUGL)
//
//  This module provides vectorized support for matrix multiplication on Neon
//  64 (e.g. Android) platforms.  Profiling tests show that it is really not
//  that much slower than non-vectorized computation because of the small size
//  of the matrix.
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
    asm volatile(
                 "ld4  {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]    	\n\t" // M[m0-m7] M[m8-m15]
                 "ld1r {v4.4s}, [%2]				                \n\t" //ssss
                 
                 "fadd v8.4s, v0.4s, v4.4s			\n\t" // DST->M[m0-m3] = M[m0-m3] + s
                 "fadd v9.4s, v1.4s, v4.4s			\n\t" // DST->M[m4-m7] = M[m4-m7] + s
                 "fadd v10.4s, v2.4s, v4.4s			\n\t" // DST->M[m8-m11] = M[m8-m11] + s
                 "fadd v11.4s, v3.4s, v4.4s			\n\t" // DST->M[m12-m15] = M[m12-m15] + s
                 
                 "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0] 	\n\t"    // Result in V9
                 :
                 : "r"(dst->m), "r"(mat.m), "r"(&scalar)
                 : "v0", "v1", "v2", "v3", "v4", "v8", "v9", "v10", "v11", "memory"
                 );
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
    asm volatile(
                 "ld4     {v0.4s, v1.4s, v2.4s, v3.4s},     [%1] 	\n\t" // M1[m0-m7] M1[m8-m15]
                 "ld4     {v8.4s, v9.4s, v10.4s, v11.4s},   [%2] 	\n\t" // M2[m0-m7] M2[m8-m15]
    
                 "fadd   v12.4s, v0.4s, v8.4s          \n\t" // DST->M[m0-m3] = M1[m0-m3] + M2[m0-m3]
                 "fadd   v13.4s, v1.4s, v9.4s          \n\t" // DST->M[m4-m7] = M1[m4-m7] + M2[m4-m7]
                 "fadd   v14.4s, v2.4s, v10.4s         \n\t" // DST->M[m8-m11] = M1[m8-m11] + M2[m8-m11]
                 "fadd   v15.4s, v3.4s, v11.4s         \n\t" // DST->M[m12-m15] = M1[m12-m15] + M2[m12-m15]
    
                 "st4    {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]    \n\t" // DST->M[m0-m7] DST->M[m8-m15]
                 :
                 : "r"(dst->m), "r"(m1.m), "r"(m2.m)
                 : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
                 );
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
    asm volatile(
                 "ld4  {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]    	\n\t" // M[m0-m7] M[m8-m15]
                 "ld1r {v4.4s}, [%2]				                \n\t" //ssss
                 
                 "fsub v8.4s, v0.4s, v4.4s			\n\t" // DST->M[m0-m3] = M[m0-m3] - s
                 "fsub v9.4s, v1.4s, v4.4s			\n\t" // DST->M[m4-m7] = M[m4-m7] - s
                 "fsub v10.4s, v2.4s, v4.4s			\n\t" // DST->M[m8-m11] = M[m8-m11] - s
                 "fsub v11.4s, v3.4s, v4.4s			\n\t" // DST->M[m12-m15] = M[m12-m15] - s
                 
                 "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0] 	\n\t"    // Result in V9
                 :
                 : "r"(dst->m), "r"(mat.m), "r"(&scalar)
                 : "v0", "v1", "v2", "v3", "v4", "v8", "v9", "v10", "v11", "memory"
                 );
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
    asm volatile(
                 "ld4     {v0.4s, v1.4s, v2.4s, v3.4s},     [%1]  \n\t" // M1[m0-m7] M1[m8-m15]
                 "ld4     {v8.4s, v9.4s, v10.4s, v11.4s},   [%2]  \n\t" // M2[m0-m7] M2[m8-m15]
                 
                 "fsub   v12.4s, v0.4s, v8.4s         \n\t" // DST->M[m0-m3] = M1[m0-m3] - M2[m0-m3]
                 "fsub   v13.4s, v1.4s, v9.4s         \n\t" // DST->M[m4-m7] = M1[m4-m7] - M2[m4-m7]
                 "fsub   v14.4s, v2.4s, v10.4s        \n\t" // DST->M[m8-m11] = M1[m8-m11] - M2[m8-m11]
                 "fsub   v15.4s, v3.4s, v11.4s        \n\t" // DST->M[m12-m15] = M1[m12-m15] - M2[m12-m15]
                 
                 "st4    {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]   \n\t" // DST->M[m0-m7] DST->M[m8-m15]
                 :
                 : "r"(dst->m), "r"(m1.m), "r"(m2.m)
                 : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
                 );
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
    asm volatile(
                 "ld1     {v0.s}[0],         [%2]            \n\t" //s
                 "ld4     {v4.4s, v5.4s, v6.4s, v7.4s}, [%1]       \n\t" //M[m0-m7] M[m8-m15]
                 
                 "fmul     v8.4s, v4.4s, v0.s[0]               \n\t" // DST->M[m0-m3] = M[m0-m3] * s
                 "fmul     v9.4s, v5.4s, v0.s[0]               \n\t" // DST->M[m4-m7] = M[m4-m7] * s
                 "fmul     v10.4s, v6.4s, v0.s[0]              \n\t" // DST->M[m8-m11] = M[m8-m11] * s
                 "fmul     v11.4s, v7.4s, v0.s[0]              \n\t" // DST->M[m12-m15] = M[m12-m15] * s
                 
                 "st4     {v8.4s, v9.4s, v10.4s, v11.4s},           [%0]     \n\t" // DST->M[m0-m7] DST->M[m8-m15]
                 :
                 : "r"(dst->m), "r"(mat.m), "r"(&scalar)
                 : "v0", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "memory"
                 );
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
    asm volatile(
                 "ld1     {v8.4s, v9.4s, v10.4s, v11.4s}, [%1] \n\t"       // M1[m0-m7] M1[m8-m15] M2[m0-m7]  M2[m8-m15]
                 "ld4     {v0.4s, v1.4s, v2.4s, v3.4s},  [%2]   \n\t"       // M2[m0-m15]
                 
                 
                 "fmul    v12.4s, v8.4s, v0.s[0]     \n\t"         // DST->M[m0-m3] = M1[m0-m3] * M2[m0]
                 "fmul    v13.4s, v8.4s, v0.s[1]     \n\t"         // DST->M[m4-m7] = M1[m4-m7] * M2[m4]
                 "fmul    v14.4s, v8.4s, v0.s[2]     \n\t"         // DST->M[m8-m11] = M1[m8-m11] * M2[m8]
                 "fmul    v15.4s, v8.4s, v0.s[3]     \n\t"         // DST->M[m12-m15] = M1[m12-m15] * M2[m12]
                 
                 "fmla    v12.4s, v9.4s, v1.s[0]     \n\t"         // DST->M[m0-m3] += M1[m0-m3] * M2[m1]
                 "fmla    v13.4s, v9.4s, v1.s[1]     \n\t"         // DST->M[m4-m7] += M1[m4-m7] * M2[m5]
                 "fmla    v14.4s, v9.4s, v1.s[2]     \n\t"         // DST->M[m8-m11] += M1[m8-m11] * M2[m9]
                 "fmla    v15.4s, v9.4s, v1.s[3]     \n\t"         // DST->M[m12-m15] += M1[m12-m15] * M2[m13]
                 
                 "fmla    v12.4s, v10.4s, v2.s[0]    \n\t"         // DST->M[m0-m3] += M1[m0-m3] * M2[m2]
                 "fmla    v13.4s, v10.4s, v2.s[1]    \n\t"         // DST->M[m4-m7] += M1[m4-m7] * M2[m6]
                 "fmla    v14.4s, v10.4s, v2.s[2]    \n\t"         // DST->M[m8-m11] += M1[m8-m11] * M2[m10]
                 "fmla    v15.4s, v10.4s, v2.s[3]    \n\t"         // DST->M[m12-m15] += M1[m12-m15] * M2[m14]
                 
                 "fmla    v12.4s, v11.4s, v3.s[0]    \n\t"         // DST->M[m0-m3] += M1[m0-m3] * M2[m3]
                 "fmla    v13.4s, v11.4s, v3.s[1]    \n\t"         // DST->M[m4-m7] += M1[m4-m7] * M2[m7]
                 "fmla    v14.4s, v11.4s, v3.s[2]    \n\t"         // DST->M[m8-m11] += M1[m8-m11] * M2[m11]
                 "fmla    v15.4s, v11.4s, v3.s[3]    \n\t"         // DST->M[m12-m15] += M1[m12-m15] * M2[m15]
                 
                 "st1    {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]  \n\t"       // DST->M[m0-m7]// DST->M[m8-m15]
                 
                 : // output
                 : "r"(dst->), "r"(m1.m), "r"(m2.m) // input - note *value* of pointer doesn't change.
                 : "memory", "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
                 );
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
    asm volatile(
                 "ld4     {v0.4s, v1.4s, v2.4s, v3.4s},  [%1]     \n\t" // load m0-m7 load m8-m15
                 
                 "fneg     v4.4s, v0.4s             \n\t" // negate m0-m3
                 "fneg     v5.4s, v1.4s             \n\t" // negate m4-m7
                 "fneg     v6.4s, v2.4s             \n\t" // negate m8-m15
                 "fneg     v7.4s, v3.4s             \n\t" // negate m8-m15
                 
                 "st4     {v4.4s, v5.4s, v6.4s, v7.4s},  [%0]     \n\t" // store m0-m7 store m8-m15
                 :
                 : "r"(dst->m), "r"(m1.m)
                 : "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "memory"
                 );
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
    asm volatile(
                 "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]    \n\t" // DST->M[m0, m4, m8, m12] = M[m0-m3]
                 //DST->M[m1, m5, m9, m12] = M[m4-m7]
                 "st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [%0]    \n\t"
                 :
                 : "r"(dst->m), "r"(m1.m)
                 : "v0", "v1", "v2", "v3", "memory"
                 );
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
    asm volatile(
                 "ld1    {v0.s}[0],        [%1]    \n\t"    // V[x]
                 "ld1    {v0.s}[1],        [%2]    \n\t"    // V[y]
                 "ld1    {v0.s}[2],        [%3]    \n\t"    // V[z]
                 "ld1    {v0.s}[3],        [%4]    \n\t"    // V[w]
                 "ld1    {v9.4s, v10.4s, v11.4s, v12.4s}, [%5]   \n\t"    // M[m0-m7] M[m8-m15]
                 
                 
                 "fmul v13.4s, v9.4s, v0.s[0]           \n\t"      // DST->V = M[m0-m3] * V[x]
                 "fmla v13.4s, v10.4s, v0.s[1]           \n\t"    // DST->V += M[m4-m7] * V[y]
                 "fmla v13.4s, v11.4s, v0.s[2]           \n\t"    // DST->V += M[m8-m11] * V[z]
                 "fmla v13.4s, v12.4s, v0.s[3]           \n\t"    // DST->V += M[m12-m15] * V[w]
                 
                 //"st1 {v13.4s}, [%0]               \n\t"    // DST->V[x, y] // DST->V[z]
                 "st1 {v13.2s}, [%0], 8               \n\t"
                 "st1 {v13.s}[2], [%0]                \n\t"
                 :
                 : "r"((float*)dst), "r"(&(vec.x)), "r"(&(vec.y)), "r"(&(vec.z)), "r"(&(vec.w)), "r"(mat.m)
                 : "v0", "v9", "v10","v11", "v12", "v13", "memory"
                 );
    return dst;
}