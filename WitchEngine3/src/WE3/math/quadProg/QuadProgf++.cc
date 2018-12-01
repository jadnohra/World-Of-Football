/* 
 File QuadProgf++.cc
 
see QuadProg++.cc, modified by Jad Nohra to use float instead of float.
 
*/

#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <sstream>
#include "QuadProg++.hh"





// Utility functions for updating some data needed by the solution method 
void compute_d(BigVector<float>& d, BigMatrix<float>& J, BigVector<float>& np, int n);
void update_z(BigVector<float>& z, BigMatrix<float>& J, BigVector<float>& d, int n, int iq);
void update_r(BigMatrix<float>&  R, BigVector<float>& r, BigVector<float>& d, int n, int iq);
bool add_constraint(BigMatrix<float>&  R, BigMatrix<float>&  J, BigVector<float>& d, int n, int& iq, float& rnorm);
void delete_constraint(BigMatrix<float>&  R, BigMatrix<float>&  J, BigVectorI& A, BigVector<float>& u, int n, int p, int& iq, int l);

// Utility functions for computing the Cholesky decomposition and solving
// linear systems
void cholesky_decomposition(BigMatrix<float>& A, int n);
void cholesky_solve(BigMatrix<float>& L, BigVector<float>& x, BigVector<float>& b, int n);
void forward_elimination(BigMatrix<float>& L, BigVector<float>& y, BigVector<float>& b, int n);
void backward_elimination(BigMatrix<float>& U, BigVector<float>& x, BigVector<float>& y, int n);

// Utility functions for computing the scalar product and the euclidean 
// distance between two numbers
float scalar_product(BigVector<float>& x, BigVector<float>& y, int n);
float distance(float a, float b);

// Utility functions for printing vectors and matrices
void print_matrix(char* name, BigMatrix<float>& A, int n);
void print_rmatrix(char* name, BigMatrix<float>& A, int n, int m);
void print_vector(char* name, BigVector<float>& v, int n);
void print_ivector(char* name, BigVectorI& v, int n);

// The Solving function, implementing the Goldfarb-Idnani method

float solve_quadprogf(BigMatrix<float>& G, BigVector<float>& g0, int n, 
                      BigMatrix<float>& CE, BigVector<float>& ce0, int p, 
                      BigMatrix<float>& CI, BigVector<float>& ci0, int m,
                      BigVector<float>& x)
{
	int i, j, k, l; /* indices */
	int ip, me, mi; 
	//float R[n][MATRIX_DIM], s[m + p], z[n], r[m + p], d[n], J[n][MATRIX_DIM], np[n], u[m + p];
	
	BigMatrix<float> R(n, MATRIX_DIM);
	BigVector<float> s(m + p);
	BigVector<float> z(n);
	BigVector<float> r(m + p);
	BigVector<float> d(n);
	BigMatrix<float> J(n, MATRIX_DIM);
	BigVector<float> np(n);
	BigVector<float> u(m + p);

	//float x_old[n], u_old[m + p];
	BigVector<float> x_old(n);
	BigVector<float> u_old(m + p);

	float f_value, psi, c1, c2, sum, ss, R_norm;
	const float inf = std::numeric_limits<float>::infinity();
  float t, t1, t2; /* t is the step lenght, which is the minimum of the partial step length t1 
    * and the full step length t2 */
	
	//int A[m + p], A_old[m + p], iai[m + p], q;
	BigVector<int> A(m + p);
	BigVector<int> A_old(m + p);
	BigVector<int> iai(m + p);
	int q;



	int iq, iter = 0;
	//bool iaexcl[m + p];
	BigVector<bool> iaexcl(m + p);
	
	me = p; /* number of equality constraints */
	mi = m; /* number of inequality constraints */
	q = 0;  /* size of the active set A (containing the indices of the active constraints) */
#ifdef TRACE_SOLVER
  print_matrix("G", G, n);
	print_vector("g0", g0, n);
  print_rmatrix("CE", CE, n, p);
	print_vector("ce0", ce0, p);
	print_rmatrix("CI", CI,  n, m);
	print_vector("ci0", ci0, m);
#endif  
  
	/*
	 * Preprocessing phase
	 */
	
	/* compute the trace of the original matrix G */
	c1 = 0.0f;
	for (i = 0; i < n; i++)
	{
		c1 += G[i][i];
	}
	/* decompose the matrix G in the form L^T L */
	cholesky_decomposition(G, n);
#ifdef TRACE_SOLVER
  print_matrix("G", G, n);
#endif
	
	/* initialize the matrix R */
	for (i = 0; i < n; i++)
	{
		d[i] = 0.0f;
		for (j = 0; j < n; j++)
			R[i][j] = 0.0f;
	}
	R_norm = 1.0f; /* this variable will hold the norm of the matrix R */
  
	/* compute the inverse of the factorized matrix G^-1, this is the initial value for H */
	c2 = 0.0f;
  for (i = 0; i < n; i++) 
	{
		d[i] = 1.0f;
		forward_elimination(G, z, d, n);
		for (j = 0; j < n; j++)
			J[i][j] = z[j];
		c2 += z[i];
    d[i] = 0.0f;
	}
#ifdef TRACE_SOLVER
 print_matrix("J", J, n);
#endif
  
	/* c1 * c2 is an estimate for cond(G) */
  
	/* 
   * Find the unconstrained minimizer of the quadratic form 0.5f * x G x + g0 x 
   * this is a feasible point in the dual space
	 * x = G^-1 * g0
   */
  cholesky_solve(G, x, g0, n);
  for (i = 0; i < n; i++)
		x[i] = -x[i];
	/* and compute the current solution value */ 
	f_value = 0.5f * scalar_product(g0, x, n);
#ifdef TRACE_SOLVER
  std::cerr << "Unconstrained solution: " << f_value << std::endl;
  print_vector("x", x, n);
#endif
  
	/* Add equality constraints to the working set A */
  iq = 0;
	for (i = 0; i < me; i++)
	{
		for (j = 0; j < n; j++)
			np[j] = CE[j][i];
    compute_d(d, J, np, n);
		update_z(z, J, d, n, iq);
		update_r(R, r, d, n, iq);
#ifdef TRACE_SOLVER
		print_matrix("R", R, iq);
		print_vector("z", z, n);
		print_vector("r", r, iq);
		print_vector("d", d, n);
#endif
    
    /* compute full step length t2: i.e., the minimum step in primal space s.t. the contraint 
      becomes feasible */
		t2 = 0.0f;
		if (fabs(scalar_product(z, z, n)) > std::numeric_limits<float>::epsilon()) // i.e. z != 0
			t2 = (-scalar_product(np, x, n) - ce0[i]) / scalar_product(z, np, n);
    
    /* set x = x + t2 * z */
		for (k = 0; k < n; k++)
			x[k] += t2 * z[k];
			
    /* set u = u+ */
		u[iq] = t2;
		for (k = 0; k < iq; k++)
			u[k] -= t2 * r[k];
    
		/* compute the new solution value */
		f_value += 0.5f * (t2 * t2) * scalar_product(z, np, n);
		A[i] = -i - 1;
    
		if (!add_constraint(R, J, d, n, iq, R_norm))
		{
			// FIXME: it should raise an error
			// Equality constraints are linearly dependent
			return f_value;
		}
	}
  
	/* set iai = K \ A */
	for (i = 0; i < mi; i++)
		iai[i] = i;
  
l1:	iter++;
#ifdef TRACE_SOLVER
  print_vector("x", x, n);
#endif
  /* step 1: choose a violated constraint */
	for (i = me; i < iq; i++)
	{
	  ip = A[i];
		iai[ip] = -1;
	}
	
	/* compute s[x] = ci^T * x + ci0 for all elements of K \ A */
	ss = 0.0f;
	psi = 0.0f; /* this value will contain the sum of all infeasibilities */
	ip = 0; /* ip will be the index of the chosen violated constraint */
	for (i = 0; i < mi; i++)
	{
		iaexcl[i] = true;
		sum = 0.0f;
		for (j = 0; j < n; j++)
			sum += CI[j][i] * x[j];
		sum += ci0[i];
		s[i] = sum;
		psi += std::min(0.0f, sum);
	}
#ifdef TRACE_SOLVER
  print_vector("s", s, mi);
#endif

    
	if (fabs(psi) <= mi * std::numeric_limits<float>::epsilon() * c1 * c2* 100.0)
	{
    /* numerically there are not infeasibilities anymore */
    q = iq;
		return f_value;
  }
    
  /* save old values for u and A */
	for (i = 0; i < iq; i++)
	{
		u_old[i] = u[i];
		A_old[i] = A[i];
	}
   /* and for x */
	for (i = 0; i < n; i++)
		x_old[i] = x[i];
    
l2: /* Step 2: check for feasibility and determine a new S-pair */
	for (i = 0; i < mi; i++)
	{
		if (s[i] < ss && iai[i] != -1 && iaexcl[i])
		{
			ss = s[i];
			ip = i;
		}
	}
  if (ss >= 0.0f)
  {
    q = iq;
    return f_value;
  }
    
  /* set np = n[ip] */
  for (i = 0; i < n; i++)
    np[i] = CI[i][ip];
  /* set u = [u 0]^T */
  u[iq] = 0.0f;
  /* add ip to the active set A */
  A[iq] = ip;

#ifdef TRACE_SOLVER
	std::cerr << "Trying with constraint " << ip << std::endl;
	print_vector("np", np, n);
#endif
    
l2a:/* Step 2a: determine step direction */
  /* compute z = H np: the step direction in the primal space (through J, see the paper) */
  compute_d(d, J, np, n);
  update_z(z, J, d, n, iq);
  /* compute N* np (if q > 0): the negative of the step direction in the dual space */
  update_r(R, r, d, n, iq);
#ifdef TRACE_SOLVER
  std::cerr << "Step direction z" << std::endl;
		print_vector("z", z, n);
		print_vector("r", r, iq + 1);
    print_vector("u", u, iq + 1);
    print_vector("d", d, n);
    print_ivector("A", A, iq + 1);
#endif
    
  /* Step 2b: compute step length */
	l = 0;
  /* Compute t1: partial step length (maximum step in dual space without violating dual feasibility */
	t1 = inf; /* +inf */
  /* find the index l s.t. it reaches the minimum of u+[x] / r */
	for (k = me; k < iq; k++)
	{
		if (r[k] > 0.0f)
		{
			if (u[k] / r[k] < t1)
			{
				t1 = u[k] / r[k];
				l = A[k];
       }
     }
  }
  /* Compute t2: full step length (minimum step in primal space such that the constraint ip becomes feasible */
	if (fabs(scalar_product(z, z, n))  > std::numeric_limits<float>::epsilon()) // i.e. z != 0
	  t2 = -s[ip] / scalar_product(z, np, n);
  else
    t2 = inf; /* +inf */
  
  /* the step is chosen as the minimum of t1 and t2 */
  t = std::min(t1, t2);
#ifdef TRACE_SOLVER
  std::cerr << "Step sizes: " << t << " (t1 = " << t1 << ", t2 = " << t2 << ") ";
#endif
  
  /* Step 2c: determine new S-pair and take step: */
  
  /* case (i): no step in primal or dual space */
  if (t >= inf)
  {
    /* QPP is infeasible */
    // FIXME: unbounded to raise
    q = iq;
     return inf;
  }
  /* case (ii): step in dual space */
  if (t2 >= inf)
  {
    /* set u = u +  t * [-r 1] and drop constraint l from the active set A */
    for (k = 0; k < iq; k++)
      u[k] -= t * r[k];
    u[iq] += t;
    iai[l] = l;
    delete_constraint(R, J, A, u, n, p, iq, l);
#ifdef TRACE_SOLVER
    std::cerr << " in dual space: " 
      << f_value << std::endl;
    print_vector("x", x, n);
    print_vector("z", z, n);
		print_ivector("A", A, iq + 1);
#endif
    goto l2a;
  }
  
  /* case (iii): step in primal and dual space */
  
  /* set x = x + t * z */
  for (k = 0; k < n; k++)
    x[k] += t * z[k];
  /* update the solution value */
  f_value += t * scalar_product(z, np, n) * (0.5f * t + u[iq]);
  /* u = u + t * [-r 1] */
  for (k = 0; k < iq; k++)
		u[k] -= t * r[k];
  u[iq] += t;
#ifdef TRACE_SOLVER
  std::cerr << " in both spaces: " 
    << f_value << std::endl;
	print_vector("x", x, n);
	print_vector("u", u, iq + 1);
	print_vector("r", r, iq + 1);
	print_ivector("A", A, iq + 1);
#endif
  
  if (t == t2)
  {
#ifdef TRACE_SOLVER
    std::cerr << "Full step has taken " << t << std::endl;
    print_vector("x", x, n);
#endif
    /* full step has taken */
    /* add constraint ip to the active set*/
		if (!add_constraint(R, J, d, n, iq, R_norm))
		{
			iaexcl[ip] = false;
			delete_constraint(R, J, A, u, n, p, iq, ip);
#ifdef TRACE_SOLVER
      print_matrix("R", R, n);
      print_ivector("A", A, iq);
#endif
			for (i = 0; i < m; i++)
				iai[i] = i;
			for (i = 0; i < iq; i++)
			{
				A[i] = A_old[i];
				iai[A[i]] = -1;
				u[i] = u_old[i];
			}
			for (i = 0; i < n; i++)
				x[i] = x_old[i];
      goto l2; /* go to step 2 */
		}    
    else
      iai[ip] = -1;
#ifdef TRACE_SOLVER
    print_matrix("R", R, n);
    print_ivector("A", A, iq);
#endif
    goto l1;
  }
  
  /* a patial step has taken */
#ifdef TRACE_SOLVER
  std::cerr << "Partial step has taken " << t << std::endl;
  print_vector("x", x, n);
#endif
  /* drop constraint l */
	iai[l] = l;
	delete_constraint(R, J, A, u, n, p, iq, l);
#ifdef TRACE_SOLVER
  print_matrix("R", R, n);
  print_ivector("A", A, iq);
#endif
  
  /* update s[ip] = CI * x + ci0 */
	sum = 0.0f;
	for (k = 0; k < n; k++)
		sum += CI[k][ip] * x[k];
	s[ip] = sum + ci0[ip];

#ifdef TRACE_SOLVER
  print_vector("s", s, mi);
#endif
  goto l2a;
}

void compute_d(BigVector<float>& d, BigMatrix<float>& J, BigVector<float>& np, int n)
{
  register int i, j;
  register float sum;
  
  /* compute d = H^T * np */
	for (i = 0; i < n; i++)
	{
		sum = 0.0f;
		for (j = 0; j < n; j++)
			sum += J[j][i] * np[j];
		d[i] = sum;
	}
}

void update_z(BigVector<float>& z, BigMatrix<float>& J, BigVector<float>& d, int n, int iq)
{
	register int i, j;
	
	/* setting of z = H * d */
  for (i = 0; i < n; i++)
	{
		z[i] = 0.0f;
		for (j = iq; j < n; j++)
			z[i] += J[i][j] * d[j];
	}
}

void update_r(BigMatrix<float>& R, BigVector<float>& r, BigVector<float>& d, int n, int iq) 
{
	register int i, j;
	register float sum;
  
  /* setting of r = R^-1 d */
	for (i = iq - 1; i >= 0; i--)
	{
		sum = 0.0f;
		for (j = i + 1; j < iq; j++)
			sum += R[i][j] * r[j];
		r[i] = (d[i] - sum) / R[i][i];
	}
}

bool add_constraint(BigMatrix<float>& R, BigMatrix<float>& J, BigVector<float>& d, int n, int& iq, float& R_norm)
{
#ifdef TRACE_SOLVER
  std::cerr << "Add constraint " << iq << '/';
#endif
	int i, j, k;
	float cc, ss, h, t1, t2, xny;
	
  /* we have to find the Givens rotation which will reduce the element
		d[j] to zero.
		if it is already zero we don't have to do anything, except of
		decreasing j */  
	for (j = n - 1; j >= iq + 1; j--)
	{
    /* The Givens rotation is done with the matrix (cc cs, cs -cc).
			 If cc is one, then element (j) of d is zero compared with element
			 (j - 1). Hence we don't have to do anything. 
			 If cc is zero, then we just have to switch column (j) and column (j - 1) 
			 of J. Since we only switch columns in J, we have to be careful how we
			 update d depending on the sign of gs.
			 Otherwise we have to apply the Givens rotation to these columns.
			 The i - 1 element of d has to be updated to h. */
		cc = d[j - 1];
		ss = d[j];
		h = distance(cc, ss);
		if (h == 0.0f)
			continue;
		d[j] = 0.0f;
		ss = ss / h;
		cc = cc / h;
		if (cc < 0.0f)
		{
			cc = -cc;
			ss = -ss;
			d[j - 1] = -h;
		}
		else
			d[j - 1] = h;
		xny = ss / (1.0f + cc);
		for (k = 0; k < n; k++)
		{
			t1 = J[k][j - 1];
			t2 = J[k][j];
			J[k][j - 1] = t1 * cc + t2 * ss;
			J[k][j] = xny * (t1 + J[k][j - 1]) - t2;
		}
	}
  /* update the number of constraints added*/
	iq++;
  /* To update R we have to put the iq components of the d vector
    into column iq - 1 of R
    */
	for (i = 0; i < iq; i++)
		R[i][iq - 1] = d[i];
#ifdef TRACE_SOLVER
  std::cerr << iq << std::endl;
#endif
  
	if (fabs(d[iq - 1]) <= std::numeric_limits<float>::epsilon() * R_norm)
		// problem degenerate
		return false;
	R_norm = std::max<float>(R_norm, fabs(d[iq - 1]));
	return true;
}

void delete_constraint(BigMatrix<float>& R, BigMatrix<float>& J, BigVectorI& A, BigVector<float>& u, int n, int p, int& iq, int l)
{
#ifdef TRACE_SOLVER
  std::cerr << "Delete constraint " << l << ' ' << iq;
#endif
	int i, j, k, qq;
	float cc, ss, h, xny, t1, t2;
  
	/* Find the index qq for active constraint l to be removed */
  for (i = p; i < iq; i++)
		if (A[i] == l)
		{
			qq = i;
			break;
		}
      
  /* remove the constraint from the active set and the duals */
  for (i = qq; i < iq - 1; i++)
  {
    A[i] = A[i + 1];
    u[i] = u[i + 1];
    for (j = 0; j < n; j++)
      R[j][i] = R[j][i + 1];
  }
      
  A[iq - 1] = A[iq];
  u[iq - 1] = u[iq];
  A[iq] = 0; 
  u[iq] = 0.0f;
  for (j = 0; j < iq; j++)
    R[j][iq - 1] = 0.0f;
  /* constraint has been fully removed */
  iq--;
#ifdef TRACE_SOLVER
  std::cerr << '/' << iq << std::endl;
#endif 
  
  if (iq == 0)
    return;
  
  for (j = qq; j < iq; j++)
  {
    cc = R[j][j];
    ss = R[j + 1][j];
    h = distance(cc, ss);
    if (h == 0.0f)
      continue;
    cc = cc / h;
    ss = ss / h;
    R[j + 1][j] = 0.0f;
    if (cc < 0.0f)
    {
      R[j][j] = -h;
      cc = -cc;
      ss = -ss;
    }
    else
      R[j][j] = h;
    
    xny = ss / (1.0f + cc);
    for (k = j + 1; k < iq; k++)
    {
      t1 = R[j][k];
      t2 = R[j + 1][k];
      R[j][k] = t1 * cc + t2 * ss;
      R[j + 1][k] = xny * (t1 + R[j][k]) - t2;
    }
    for (k = 0; k < n; k++)
    {
      t1 = J[k][j];
      t2 = J[k][j + 1];
      J[k][j] = t1 * cc + t2 * ss;
      J[k][j + 1] = xny * (J[k][j] + t1) - t2;
    }
  }
}

float distance(float a, float b)
{
	register float a1, b1, t;
	a1 = fabs(a);
	b1 = fabs(b);
	if (a1 > b1) 
	{
		t = (b1 / a1);
		return a1 * sqrt(1.0f + t * t);
	}
	else
		if (b1 > a1)
		{
			t = (a1 / b1);
			return b1 * sqrt(1.0f + t * t);
		}
	return a1 * sqrt(2.0f);
}


float scalar_product(BigVector<float>& x, BigVector<float>& y, int n)
{
	register int i;
	register float sum;
	
	sum = 0.0f;
	for (i = 0; i < n; i++)
		sum += x[i] * y[i];
	return sum;			
}

void cholesky_decomposition(BigMatrix<float>& A, int n) 
{
  register int i, j, k;
  register float sum;
	
  for (i = 0; i < n; i++)
  {
    for (j = i; j < n; j++)
    {
      sum = A[i][j];
      for (k = i - 1; k >= 0; k--)
        sum -= A[i][k]*A[j][k];
      if (i == j) 
      {
        if (sum <= 0.0f)
				{
          // raise error
					print_matrix("A", A, n);
          throw std::runtime_error("The matrix passed to the Cholesky A = L L^T decomposition is not positive definite");
				}
        A[i][i] = sqrt(sum);
      }
      else
        A[j][i] = sum / A[i][i];
    }
    for (k = i + 1; k < n; k++)
      A[i][k] = A[k][i];
  } 
}

void cholesky_solve(BigMatrix<float>& L, BigVector<float>& x, BigVector<float>& b, int n)
{
  //float y[n];
	BigVector<float> y(n);
	
	/* Solve L * y = b */
	forward_elimination(L, y, b, n);
  /* Solve L^T * x = y */
	backward_elimination(L, x, y, n);
}

void forward_elimination(BigMatrix<float>& L, BigVector<float>& y, BigVector<float>& b, int n)
{
	register int i, j;
	
	y[0] = b[0] / L[0][0];
	for (i = 1; i < n; i++)
	{
		y[i] = b[i];
		for (j = 0; j < i; j++)
			y[i] -= L[i][j] * y[j];
		y[i] = y[i] / L[i][i];
	}
}

void backward_elimination(BigMatrix<float>& U, BigVector<float>& x, BigVector<float>& y, int n)
{
	register int i, j;
	
	x[n - 1] = y[n - 1] / U[n - 1][n - 1];
	for (i = n - 2; i >= 0; i--)
	{
		x[i] = y[i];
		for (j = i + 1; j < n; j++)
			x[i] -= U[i][j] * x[j];
		x[i] = x[i] / U[i][i];
	}
}

void print_matrix(char* name, BigMatrix<float>& A, int n)
{
	std::ostringstream s;
	std::string t;
	
	s << name << ": " << std::endl;
  for (int i = 0; i < n; i++)
  {
		s << " ";
    for (int j = 0; j < n; j++)
      s << A[i][j] << ", ";
    s << std::endl;
  }
	t = s.str();
	t = t.substr(0, t.size() - 3); // To remove the trailing space, comma and newline
  std::cerr << t << std::endl;
}

void print_rmatrix(char* name, BigMatrix<float>& A, int n, int m)
{
	std::ostringstream s;
	std::string t;
	
  s << name << ": " << std::endl;
  for (int i = 0; i < n; i++)
  {
		s << " ";
    for (int j = 0; j < m; j++)
      s << A[i][j] << ", ";
    s << std::endl;
  }
	t = s.str();
	t = t.substr(0, t.size() - 3); // To remove the trailing space, comma and newline
	
  std::cerr << t << std::endl;
}

void print_vector(char* name, BigVector<float>& v, int n)
{
	std::ostringstream s;
	std::string t;
	
	s << name << ": " << std::endl << " ";
  for (int i = 0; i < n; i++)
	{
		s << v[i] << ", ";
  }
	t = s.str();
	t = t.substr(0, t.size() - 2); // To remove the trailing space and comma
	
  std::cerr << t << std::endl;
}

