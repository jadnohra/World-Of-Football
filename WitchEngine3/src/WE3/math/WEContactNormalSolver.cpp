#include "WEContactNormalSolver.h"
#include "quadProg/QuadProg++.hh"

#include <limits>

namespace WE {

const ContactNormalSolver::Type ContactNormalSolver::kEps = (Type) 0.1;
const bool ContactNormalSolver::kAddHelperConstraints = true;
const ContactNormalSolver::Type ContactNormalSolver::kZero = (Type) 0;
const ContactNormalSolver::Type ContactNormalSolver::kOne = (Type) 1;

ContactNormalSolver::ContactNormalSolver() {

	reserveSize(8);	

	n = 3;

	G.create(n, n);
	g0.create(n);
	x.create(n);
	
	//Identity matrix
	for (int i = 0; i < n; ++i) {

		g0[i] = kZero;

		for (int j = 0; j < n; ++j) {

			G[i][j] = i == j ? kOne : kZero;
		}
	}

	//no equality constraints
	m = 0;
}

void ContactNormalSolver::reset() {

	mContacts.count = 0;
}

void ContactNormalSolver::reserveSize(Index size) {

	mContacts.reserve(size);
}

int ContactNormalSolver::initHelperConstraints(bool settestdim[3], Type testdim[3], int padddim[3], int p) {

	for (int i = 0; i < 3; ++i) {

		for (int j = 0; j < mContacts.count; ++j) {

			Type val = mContacts.el[j].normal.el[i];

			if (val != kZero) {

				if (!settestdim[i]) {

					settestdim[i] = true;
					testdim[i] = val;

				} else {

					if (testdim[i] > kZero) {

						if (val > kZero) {

							if (fabs(val) > fabs(testdim[i])) {

								testdim[i] = val;
							}

						} else {

							testdim[i] = kZero;
						}

					} else if (testdim[i] < kZero) {

						if (val < kZero) {

							if (fabs(val) > fabs(testdim[i])) {

								testdim[i] = val;
							}

						} else {

							testdim[i] = kZero;
						}

					} else {
					}

				}
			}
		}
	}
	
	int padd = 0;
	

	for (int i = 0; i < 3; ++i) {

		if (settestdim[i] && testdim[i] != kZero) {

			padddim[padd] = i;
			++padd;
		}
	}

	return padd;
}

void ContactNormalSolver::addHelperConstraints(bool settestdim[3], Type testdim[3], int padddim[3], int p, int padd) {

	for (int i = 0; i < padd; ++i) {

		CI[0][p + i] = kZero;
		CI[1][p + i] = kZero;
		CI[2][p + i] = kZero;

		Type val = testdim[padddim[i]];

		if (val > 0) {

			CI[padddim[i]][p + i] = kOne;
			ci0[p + i] = -val;

		} else {

			CI[padddim[i]][p + i] = -kOne;
			ci0[p + i] = val;
		}
	}
}

const Vector3* ContactNormalSolver::solve() {

	//inequality constraints
	Index p = mContacts.count;

	if (p == 0)
		return NULL;

	bool settestdim[3] = { false, false, false };
	Type testdim[3];
	int padddim[3];

	int padd = 0;

	if (kAddHelperConstraints)
		padd = initHelperConstraints(settestdim, testdim, padddim, p);

	if (CI.cols < p + padd) {

		CI.create(3, p + padd);
		ci0.create(p + padd);
	}

	for (Index i = 0; i < p; ++i) {

		const Vector3& normal = mContacts.el[i].normal;

		CI[0][i] = normal.el[0];
		CI[1][i] = normal.el[1];
		CI[2][i] = normal.el[2];

		ci0.el(i) = -kEps;
	}

	
	if (kAddHelperConstraints)
		addHelperConstraints(settestdim, testdim, padddim, p, padd);

	Type f = solve_quadprog<Type>(G, g0, n, CE, ce0, m, CI, ci0, p + padd, x);

	if (f == std::numeric_limits<Type>::infinity())
		return NULL;

	mSolution.set(x.el(0), x.el(1), x.el(2));

	if (mSolution.isZero())
		return NULL;

	return &mSolution;
}

}