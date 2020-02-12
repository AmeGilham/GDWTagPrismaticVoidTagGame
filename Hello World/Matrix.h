#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Vector.h"
#include <iostream>

class mat4;

class mat3
{
public:
	//Empty constructor
	mat3() { }
	//Sets row1, row2, and row3
	mat3(vec3 _row1, vec3 _row2, vec3 _row3);
	//Creates a 3x3 matrix using a 4x4 matrix
	//*useful for grabing rotation matrix from a homogenous transformation matrix
	mat3(mat4 m);

	void Add(mat3 m);
	void Subtract(mat3 m);

	void Print();
	mat3 Transpose();
	float Determinant();
	mat3 Inverse();

	static mat3 Transpose(mat3 R);

	//Rows are made up of vectors
	//*defaults to zero
	vec3 row1 = vec3();
	vec3 row2 = vec3();
	vec3 row3 = vec3();
	//For use so that you can index the matrix using [] operator
	vec3* hold[3] = { &row1, &row2, &row3 };

	//Negates the matrix
	mat3 operator-();
	//For use so that you can index the matrix using [] operator
	vec3 operator[](int i);
	//Multiplies the matrix with a 3D vector
	vec3 operator*(vec3 vec);
	mat3 operator+(mat3 m);
	mat3 operator-(mat3 m);
	mat3 operator*(float f);
	mat3 operator*(mat3 m);
};

class mat4
{
public:
	mat4() { }
	//Sets row1, row2, row3, and row4
	mat4(vec4 _row1, vec4 _row2, vec4 _row3, vec4 _row4);
	//creates a 4x4 matrix using a 3x3 rotation matrix and a 3D translation vector
	mat4(mat3 rot, vec3 trans);

	void Add(mat4 m);
	void Subtract(mat4 m);

	void Print();
	mat4 Transpose();
	float Determinant();

	//Gets the fast inverse of a homogenous transformation matrix
	static mat4 FastInverse(mat4 mat);
	//Gets the translation data from a homogenous transformation matrix
	static vec3 Translation(mat4 mat);

	//Rows are made up of vectors
	vec4 row1 = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 row2 = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 row3 = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 row4 = vec4(0.f, 0.f, 0.f, 0.f);
	//For use so that you can index the matrix using [] operator
	vec4* hold[4] = { &row1, &row2, &row3, &row4 };

	//For use so that you can index the matrix using [] operator
	vec4 operator[](int i);
	mat4 operator+(mat4 m);
	mat4 operator-(mat4 m);
	mat4 operator*(float f);
	mat4 operator*(mat4 m);
	vec4 operator*(vec4 v);
};


class mat2
{
public:
	//Empty constructor
	mat2() { }
	//sets row1 and row2
	mat2(vec2 _row1, vec2 _row2);

	//Adds to the current mat2
	void Add(mat2 m);
	//Subtracts the passed parameter from THIS matrix
	void Subtract(mat2 m);

	//Prints out the formatted matrix in console
	void Print();

	//Returns transposed version of our matrix
	mat2 Transpose();
	//Returns the determinant of our matrix
	float Determinant();
	//Returns the inverse of our matrix
	mat2 Inverse();

	//Rows are made up of vectors
	vec2 row1 = vec2();
	vec2 row2 = vec2();
	//For use so that you can index the matrix using [] operator
	vec2* hold[2] = { &row1, &row2 };

	//For adding two mat2s together
	mat2 operator+(mat2 m);
	//For subtracting two mat2s together
	mat2 operator-(mat2 m);
	//For multiplying mat2 by float
	mat2 operator*(float f);
	//For multiplying two mat2s
	mat2 operator*(mat2 m);
	//For multiplying mat2 by vec2
	vec2 operator*(vec2 v);
	//for use so that you can index the matrix using [] operator
	vec2 operator[](int i);
};

#endif // !__MATRIX_H__
