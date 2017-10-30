#ifndef SUBST_H
#define SUBST_H
/** @file */

#include "Matrix.hpp"

namespace gm {

#define max(x,y) ((x) > (y) ? x : y)

enum class Direction {
	Forwards,
	Backwards,
};

enum class Diagonal {
	Unit,
	Value,
};

enum class Permute {
	True,
	False,
};

template<class Elem>
Elem& at(varray<Elem>& arr, size_t i, size_t j){
	return arr.at(i);
}
template<class Elem>
const Elem& at(varray<Elem> const& arr, size_t i, size_t j){
	return arr.at(i);
}

#define ind(M,i,j) (direction == Direction::Forwards ? \
	M.at(i, j) : \
	M.at((size-1)-i, (size-1)-j))

template<Direction direction, class Mat>
void set(Mat& M, const Mat& A, vector<long> P){
	size_t size = M.size();
	for(size_t i=0; i < size; i++){
		for(size_t j=0; j < size; j++){
			ind(M, i, j) = ind(A, P.at(i), j);
		}
	}
}
/**
 * @brief copy matrix A to yourself
 */
template<Direction direction, class Mat>
void set(Mat& M, const Mat& A){
	size_t size = M.size();
	for(size_t i=0; i < size; i++){
		for(size_t j=0; j < size; j++){
			ind(M, i, j) = ind(A, i, j);
		}
	}
}

#undef ind

/**
 * @brief Subtitution method for linear sistems, forward or backward, uses P from pivoting on the LU decomposition
 * @param T Triangular coeficient Matrix
 * @param X Variables to be found
 * @param I Independant terms
 * @param forward true is forward subst, false is backward.
 * @param P from LU Decomposition
 * @param unit_diagonal true if diagonal is equal to 1
 * @param col Column of the matrix I to be used as B
 */
template<Direction direction, Diagonal diagonal, Permute permute, class TMatrix, class XMatrix, class IMatrix>
void subst(TMatrix& T, XMatrix& X, IMatrix& I, vector<long>& P, long col){
	size_t i, j;
	int step;
	size_t size = T.size();
	
	if(direction == Direction::Forwards){
		i = 0;
		step = +1;
	} else {
		i = size-1;
		step = -1;
	}

	for (; i >= 0 && i < size; i += step) {
		if(permute == Permute::True)
			{ at(X, i,col) = at(I, P.at(i),col); }
		else{ at(X, i,col) = at(I, i,col); }
		if(direction == Direction::Forwards)
			{ j = 0; }
		else{ j = size-1; }
		
		for (; j != i; j += step) {
			at(X, i,col) -= at(X, j,col) * T.at(i,j);
		}
		
		if(diagonal != Diagonal::Unit)
			{ at(X, i,col) /= T.at(i,i); }
	}
}
/**
 * @brief Subtitution method for linear sistems, forward or backward, uses P from pivoting on the LU decomposition
 * @param T Triangular coeficient Matrix
 * @param X Variables to be found
 * @param I Independant terms
 * @param forward true is forward subst, false is backward.
 * @param P from LU Decomposition
 * @param unit_diagonal true if diagonal is equal to 1
 * @param col Column of the matrix I to be used as B
 */
template<Direction direction, Diagonal diagonal, Permute permute, class TMatrix, class XMatrix, class IMatrix>
void substR(TMatrix& T, XMatrix& X, IMatrix& I, vector<long>& P, long col){
	long i, j;
	long bi, bj;
	int step;
	int bstep;
	long size = T.size();
	long endi, endj;
	
	if(direction == Direction::Forwards){
		bi = 0;
		step = +1;
		bstep = step * BL1;
	} else {
		bi = size-1;
		step = -1;
		bstep = step * BL1;
	}
	
	for (; bi >= 0 && bi < size ; bi += bstep) {
		if(direction == Direction::Forwards)
			{ bj = 0;		endi = min(bi + bstep, size); }
		else{ bj = size-1;	endi = max(bi + bstep, -1); }
		
		for(int i = bi; i != endi; i += step)
			if(permute == Permute::True)
				{ at(X, i,col) = at(I, P.at(i),col); }
			else{ at(X, i,col) = at(I, i,col); }
		
		for (; bj != bi; bj += bstep) {
			for (i = bi; i != endi; i += step) {
//				if(direction == SubstForwards)
//					{ endj = min(bj + bstep, i); }
//				else{ endj = max(bj + bstep, i); }
				endj = bj + bstep;
				for (j = bj; j != endj; j += step) {
					at(X, i,col) = at(X, i,col) - T.at(i,j) * at(X, j,col);
				}
			}
		}
		// Remainder diagonal
		for (i = bi; i != endi; i += step) {
			if(direction == Direction::Forwards)
				{ endj = min(bj + bstep, i); }
			else{ endj = max(bj + bstep, i); }
			
			for (j = bj; j != endj; j += step) {
				at(X, i,col) = at(X, i,col) - T.at(i,j) * at(X, j,col);
			}
			if(diagonal != Diagonal::Unit)
				{ at(X, i,col) /= T.at(i,i); }
		}
	}
}
/**
 * @brief Subtitution method for linear sistems, forward or backward, uses P from pivoting on the LU decomposition
 * @param T Triangular coeficient Matrix
 * @param X Variables to be found
 * @param I Independant terms
 * @param forward true is forward subst, false is backward.
 * @param P from LU Decomposition
 * @param unit_diagonal true if diagonal is equal to 1
 * @param col Column of the matrix I to be used as B
 */
template<Direction direction, Diagonal diagonal, Permute permute, class TMatrix, class XMatrix, class IMatrix>
void substUnroll(TMatrix& T, XMatrix& X, IMatrix& I, vector<long>& P, long col){
	long i, j;
	int step;
	int bstep;
	long size = T.size;
	
	if(direction == Direction::Forwards){
		i = 0;
		step = +1;
		bstep = step * BL1;
	} else {
		i = size-1;
		step = -1;
	}
	
	#define ROUND_DOWN(x, s) ((x) & ~((s)-1))
	for (; i >= 0 && i < size; i += step) {
		at(X, i,col) = at(I, i,col);
		for (j = 0; j < ROUND_DOWN(i,BL1); j += bstep) {
			for(int c = 0; c < bstep; ++c)
				at(X, i,col) -= at(X, j+c,col) * T.at(i,j+c);
		}
		// Remainder
		for(; j < i; ++j)
			{ at(X, i,col) -= at(X, j,col) * T.at(i,j); }
		
		if(diagonal != Diagonal::Unit)
			{ at(X, i,col) /= T.at(i,i); }
	}
}
/**
 * @brief Subtitution method for linear sistems, forward or backward
 * @param T Triangular coeficient Matrix
 * @param X Variables to be found
 * @param B Independant terms
 * @param forward true is forward subst, false is backward.
 * @param col Column of the matrix to be used as B
 */
template<Direction direction, class TMatrix>
void subst(TMatrix& T, MatrixColMajor<double>& X, vector<double>& B, long col) {
	long size = T.size;
	long bi; long bj;
	long i; long j;
	long iend; long jend;
	long step;

	// TODO test iterate blocks by col (bj) instead of as currently by row (bi)
	if(direction == Direction::Forwards){
		bj = 0;
		step = +1;
	} else {
		bj = size-1;
		step = -1;
	}
	long bstep = step * BL1;
	
	for(; bj >= 0 && bj < size; bj += bstep){
		if(direction == Direction::Forwards) {
			jend = bj + bstep > size ? size : bj + bstep;
		} else {
			jend = bj + bstep < 0 ? -1 : bj + bstep;
		}
		// go though diagonal block
		for(i = bj; i != jend; i += step){
			if((bj == 0 && direction == Direction::Forwards) || (bj == size-1 && direction == Direction::Backwards)){
				X.at(i,col) = B.at(i);
			}
			for(j = bj; j != jend && j != i; j += step){
				X.at(i,col) -= X.at(j,col) * T.at(i, j);
			}
			X.at(i,col) = X.at(i,col) / T.at(i, i);
		}
		for(bi = bj+bstep; bi >= 0 && bi < size; bi += bstep){
			if(direction == Direction::Forwards) {
				iend = bi + bstep > size ? size : bi + bstep;
			} else {
				iend = bi + bstep < 0 ? -1 : bi + bstep;
			}
			if((bj == 0 && direction == Direction::Forwards) || (bj == size-1 && direction == Direction::Backwards))
				for(i = bi; i != iend; i += step){
					X.at(i,col) = B.at(i);
				}
			// go though current block
			for(i = bi; i != iend; i += step){
				for(j = bj; j != jend && j != i; j += step){
					X.at(i,col) -= X.at(j,col) * T.at(i, j);
				}
			}
		}
		// remainder
//		for(i = size - mod(size, bstep); i < size; i += step){
//			for(j = bj; j < bj+bstep; j += step){
//				cout << "(" << i << "," << j << ")" << endl; 
//			}
//		}
	}
	// remainder
//	for(i = size - mod(size, bstep); i < size; i += step){
//		for(j = size - mod(size, bstep); j != i; j += step){
//			cout << "(" << i << "," << j << ")" << endl; 
//		}
//		cout << "divided by (" << i << "," << i << ") dia" << endl;
//	}
}


}
#endif