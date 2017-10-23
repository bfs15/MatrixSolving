#ifndef GAUSSEL_H
#define GAUSSEL_H
/**
@file GaussEl.h
*/

#include "Double.h"
#include "Matrix.h"

namespace std {

/**
 @brief For the matrix LU finds its LU decomposition overwriting it
 Has partial pivoting, stores final indexes in P
 @param LU Matrix to be decomposed Output: lower triangle of this matrix will store L 1 diagonal implicit, upper triangle stores U
 @param P Permutation vector resulting of the pivoting
 */
void GaussEl(const Matrix& A, Matrix& LU, vector<long>& P) {
	// copy A to LU
	set(LU, A);
	// initializing permutation vector
	for(long i = 0; i < A.size; i++){
		P.at(i) = i;
	}

	// for each pivot
	for(long p = 0; p < A.size; p++){
		/* partial pivoting */
		long maxRow = p;
		for(long i = p+1; i < A.size; i++){
			// for each value below the p pivot
			if(abs(LU.at(i,p)) > abs(LU.at(maxRow,p))) maxRow = i;
		} // finds max value
		// pivots rows of U
		swap_rows(LU, maxRow, p);
		swap(P.at(p), P.at(maxRow));

		if(close_zero(LU.at(p,p))){
			fprintf(stderr, "Found a pivot == 0, system is not solvable with partial pivoting");
			exit(EXIT_FAILURE);
		}
		// LU.at(p,p) = 1; implicit
		
		// Calc pivot multipliers
		//for(long i = p+1; i < LU.size; i++){	// going from pivot+1 to end
		for(long i = LU.size-1; i >= p+1; i--){	// going from end to pivot+1 Optm: If no pivoting ocurred: 1 less cache miss
			LU.at(i, p) = LU.at(i, p)/LU.at(p, p);
		}
		
		for(long i = p+1; i < A.size; i++){   // going from pivot+1 to end
		//for(long i = A.size-1; i >= p+1; i--){	// going from end to pivot+1 Optm: If no pivoting ocurred: 1 less cache miss
			// subtract pivot row U.at(p, _) from current row LU.at(i, _)
			for(long k = p+1; k < A.size; k++){
				LU.at(i, k) = LU.at(i, k) - LU.at(p, k) * LU.at(i, p);
				// mulitply pivot line value to multiplier
			}
		}
	}
}


}
#endif