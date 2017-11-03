#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "varray.hpp"

namespace gm
{
using namespace std;

#define PAD(X) (div_down((X),L1_LINE_DN)*(L1_LINE_DN*(L1_LINE_DN-1))/2)
// Optm: test switching, the below doesnt work probably
//#define PAD(X) ((size_t)floor((X)/(double)L1_LINE_DN)*(L1_LINE_DN*(L1_LINE_DN-1))/2)

#define PADDING true

// Non member access functions

template<template<class> class Cont, class Elem>
Elem& at(Cont<Elem>& C, size_t i, size_t j){
	return C.at(i,j);
}
template<template<class> class Cont, class Elem>
const Elem& at(Cont<Elem> const& M, size_t i, size_t j){
	return M.at(i,j);
}

/**
 * @brief Stores values of matrix in a vector, Row Major Order
 */
template<class Elem>
class Matrix
{
public:
	varray<Elem> varr;
	size_t mSize;
	size_t mSizeVec; // n of vec<element>s
	
	size_t mSizeMem;
	size_t mSizeVecMem;
	size_t mPad;

	size_t mEndVec;

	/** @return Number of elements in register*/
	size_t regEN() { return varr.regEN(); }

	void memAlloc(size_t mSize){
		varr.alloc(mSize*mSize);
	}
	void alloc(size_t size){
		mSize = size;
		mSizeVec = mSize/regEN();
		mSizeMem = mSize;
		if(PADDING){
			mSizeMem += mod(-mSize,(ptrdiff_t)L1_LINE_DN);
			// make sure m_size is odd multiple of cache line
			//if(((mSizeMem/L1_LINE_DN) % 2) == 0)
			if(mSizeMem > L1LINE_N-1 && isPowerOfTwo(mSizeMem))
				mSizeMem = mSizeMem + L1_LINE_DN;
		}
		mPad = mSizeMem - mSize;
		mSizeVecMem = mSizeMem/regEN();
		mEndVec = Lower_Multiple(mSize, regEN());
		memAlloc(mSizeMem);
	}
	/**
	 * @param size of matrix, total number of lines
	 */
	Matrix(size_t size){
		alloc(size);
	}
	
	Matrix(){}

	/** @brief size of the varray */
	size_t size() const { return mSize; }
	/** @brief size of the vectorized varray */
	size_t sizeVec() const { return mSizeVec; }
	/** @brief size of the vectorized varray */
	size_t sizeMem() const { return mSizeMem; }
	/** @brief vectorization end index */
	size_t vecEnd(){ return mEndVec; }
	/** @brief vectorization end index */
	size_t pad(){ return mPad; }

	size_t indVecMem(size_t i, size_t j) const {
		assert(i < mSizeMem && j < mSizeVecMem);
		return i*mSizeVecMem + j;
	}
	vec<Elem>& atv(size_t i, size_t j) {
		return varr.atv(indVecMem(i,j));
	}
	const vec<Elem>& atv(size_t i, size_t j) const {
		return varr.atv(indVecMem(i,j));
	}
	size_t indMem(size_t i, size_t j) const {
		assert(i < mSizeMem && j < mSizeMem);
		return i*mSizeMem + j;
	}
	Elem& at(size_t i, size_t j){
		return varr.at(indMem(i,j));
	}
	const Elem& at(size_t i, size_t j) const {
		return varr.at(indMem(i,j));
	}
};

/**
 * @brief Stores values of matrix in a vector, Column Major Order
 */
template<class Elem>
class MatrixColMajor : public Matrix<Elem>
{
public:
	using Matrix<Elem>::Matrix;
	using Matrix<Elem>::varr;
	using Matrix<Elem>::mSizeMem;
	using Matrix<Elem>::mSizeVecMem;

	size_t indVecMem(size_t i, size_t j) const {
		assert(i < mSizeVecMem && j < mSizeMem);
		return j*mSizeVecMem + i;
	}
	vec<Elem>& atv(size_t i, size_t j) {
		return varr.atv(indVecMem(i,j));
	}
	const vec<Elem>& atv(size_t i, size_t j) const {
		return varr.atv(indVecMem(i,j));
	}
	size_t indMem(size_t i, size_t j) const {
		assert(i < mSizeMem && j < mSizeMem);
		return j*mSizeMem + i;
	}
	Elem& at(size_t i, size_t j){
		return varr.at(indMem(i,j));
	}
	const Elem& at(size_t i, size_t j) const {
		return varr.at(indMem(i,j));
	}
	/**/
};

template<class Mat>
void swap_rows(Mat& M, size_t row0, size_t row1){
	if(row0 == row1)
		return;
	for(size_t j = 0; j < M.size(); j++){
		swap(M.at(row0, j), M.at(row1, j));
	}
}
/**
 * @brief M += B
 * @param sign -1 with you want to add -b
 */
template<class Mat>
void add(Mat& M, Mat& B, double sign = 1){
	for(size_t i=0; i < M.size(); i++){
		for(size_t j=0; j < M.size(); j++){
			M.at(i,j) += sign*B.at(i,j);
		}
	}
}
/**
 * @brief copy matrix A to yourself
 */
template<class Mat, class elem>
void set(Mat& M, const Matrix<elem>& A){
	for(size_t i=0; i < M.size(); i++){
		for(size_t j=0; j < M.size(); j++){
			M.at(i,j) = A.at(i,j);
		}
	}
}
/**
 * @brief sets all matrix to parameter
 */
template<class Mat>
void set(Mat& M, double x){
	for(size_t i=0; i < M.size(); i++){
		for(size_t j=0; j < M.size(); j++){
			M.at(i,j) = x;
		}
	}
}

template<class Mat>
void print(Mat& M){
	for(size_t i = 0; i < M.size(); i++){
		for(size_t j = 0; j < M.size(); j++){
			cout << M.at(i, j) <<" ";
		}
		cout << endl;
	}
}
/**
 * @brief sets I to identity
 */
template<class Mat>
void identity(Mat& I){
	for(size_t i = 0; i < I.size(); i++){
		for(size_t j = 0; j < I.size(); j++){
			I.at(i, j) = 0;
		}
		I.at(i, i) = 1;
	}
}
/**
 * @brief Assigns random matrix to M
 * @param M needs to have been allocated
 */
template<class Mat>
void randomMatrix(Mat& M){
	size_t i, j;
	double invRandMax = 1.0/(double)RAND_MAX;

	for(i = 0; i < M.size(); i++){
		for(j = 0; j < M.size(); j++){
			M.at(i,j) = (double)rand() * invRandMax;
		}
	}
}
/**
 * @brief prints matrix with size in the first line
 */
template<class Mat>
void printm(Mat& M){
	cout<<  M.size() <<"\n";
	print(M);
}


}
#endif