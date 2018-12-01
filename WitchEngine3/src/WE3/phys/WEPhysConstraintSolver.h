#ifndef _WEPhysConstraintSolver_h
#define _WEPhysConstraintSolver_h

#include "WEPhysConstraint.h"

/*

    void run(Group & group,value_type const &h)
    {
      unsigned int n = group.getNumberOfBodies();
      if(!n)
        return;
      m_tmp.resize(6*n);
      m_tmp.clear();
      group.extractPosition(m_s);
      group.extractVelocity(m_u);
      group.extractInverseMassMatrix(m_invM,m_invI);
      if(h)
      {
        //--- Compute tmp = h*inv(M)f_ext
        group.extractExternalForces(m_f_ext);
        CompressedMassMatrix<Types >::mul(m_invM,m_invI,m_f_ext,m_tmp);
        m_tmp *=h;
      }
      //--- Make sure that body tags are set proberly
      int tag=0;
      for(typename Group::body_iterator body = group.bodyBegin();body!=group.bodyEnd();++body)
      {
        body->m_tag = tag++;
      }
      //--- Determine how many constraint variables we have
      typename Group::contact_iterator begin = group.contactBegin();
      typename Group::contact_iterator end = group.contactEnd();
      typename Group::contact_iterator contact;
      //--- Determine the number of constraint variables
      unsigned int m=0;
      for(contact=begin;contact!=end;++contact)
      {
        unsigned int eta = contact->m_material->getNumberOfFrictionDirections();
        value_type mu = contact->m_material->getFriction();
        if(!mu)
          eta = 0;
        m +=  1 + eta;
      }
      //--- If there is any constraints then solve for them
      if(m)
      {
        //--- First we allocate memory
        m_jb.resize(2*m);
        m_J.resize(12*m);
        m_b.resize(m);
        m_error.resize(m);
        m_tmp2.resize(6*n);
        m_Fc.resize(6*n);
        m_lambda.resize(m);
        m_lo.resize(m);
        m_hi.resize(m);
        m_dep.resize(m);
        m_factor.resize(m);
        //--- Temporary variables used for computations
        vector3 rAXn,rBXn;
        vector3 T1,T2;
        vector3 rAXT1,rBXT1;
        vector3 rAXT2,rBXT2;
        vector3 u,v_a,w_a,v_b,w_b,un,ut;
        vector3 abs_n;
        //--- Iterators so we can fill out vectors with data
        typename int_vector_type::iterator jb  = m_jb.begin();
        typename vector_type::iterator j       = m_J.begin();
        typename vector_type::iterator err     = m_error.begin();
        typename vector_type::iterator lo      = m_lo.begin();
        typename vector_type::iterator hi      = m_hi.begin();
        typename int_vector_type::iterator dep = m_dep.begin();
        typename vector_type::iterator fac     = m_factor.begin();
        unsigned int idx = 0;//--- Global idx of first variable in a constraint
        for(contact=begin;contact!=end;++contact)
        {
          rAXn = (contact->m_rA % contact->m_n);
          rBXn = (contact->m_rB % contact->m_n);
          *j++ = -contact->m_n(0);   *j++ = -contact->m_n(1);  *j++ = -contact->m_n(2);
          *j++ = -rAXn(0);           *j++ = -rAXn(1);          *j++ = -rAXn(2);
          *j++ = contact->m_n(0);    *j++ = contact->m_n(1);   *j++ = contact->m_n(2);
          *j++ = rBXn(0);            *j++ = rBXn(1);           *j++ = rBXn(2);
          *jb++ = contact->get_body_A()->m_tag;
          *jb++ = contact->get_body_B()->m_tag;
          contact->get_body_A()->getVelocity(v_a);
          contact->get_body_B()->getVelocity(v_b);
          contact->get_body_A()->getSpin(w_a);
          contact->get_body_B()->getSpin(w_b);
          u = computeRelativeContactVelocity(v_a,w_a,contact->m_rA,v_b,w_b,contact->m_rB);
          value_type u_n_before = contact->m_n * u;
          value_type u_n_after = - contact->m_material->getNormalRestitution() * u_n_before;
          *err++ = std::max(0.,u_n_after);
          *lo++ = 0;
          *hi++ = Math::highest<value_type>();
          *dep++ = -1;
          *fac++ = 0;
          //--- See if we have any friction on this variable
          unsigned int eta = contact->m_material->getNumberOfFrictionDirections();
          value_type mu = contact->m_material->getFriction();
          if(!mu)
            eta = 0;
          if(eta)
          {
            //--- A little neet trick, if friction cone is badly approximated
            //--- (low eta), there might not even be a friction direction along
            //--- the relative sliding direction. To remedy this we try to pick orientate
            //--- tangent vectors such that the first vector lies in the direction
            //--- of relative sliding.
            //---
            //--- Intuitively this will help reduce the error in the friction approximation.
            un = contact->m_n * u_n_before;  //--- normal relative velocity
            ut = u - un;    //--- tangential relative velocity
            if(ut.isZero())
            {
              //--- Find a vector orthogonal to the contact normal
              orthonormal_vectors(T1,T2,contact->m_n);
            }
            else
            {
              //--- Compute two othogonal vectors spanning the contact plane
              T1 = unit( ut );
              T2 = unit( contact->m_n % T1);
            }
            contact->m_t1 = T1;//--- only for visual debug
            contact->m_t2 = T2;//--- only for visual debug
            rAXT1 = contact->m_rA % T1;
            rBXT1 = contact->m_rB % T1;
            *j++ = -T1(0);     *j++ = -T1(1);     *j++ = -T1(2);
            *j++ = -rAXT1(0);  *j++ = -rAXT1(1);  *j++ = -rAXT1(2);
            *j++ = T1(0);      *j++ = T1(1);      *j++ = T1(2);
            *j++ = rBXT1(0);   *j++ = rBXT1(1);   *j++ = rBXT1(2);
            *jb++ = contact->get_body_A()->m_tag;  *jb++ = contact->get_body_B()->m_tag;
            *err++ = 0; *lo++ = -mu; *hi++ = mu; *dep++ = idx; *fac++ = mu;
            if(eta>1)
            {
              rAXT2 = (contact->m_rA % T2);
              rBXT2 = (contact->m_rB % T2);
              *j++ = -T2(0);     *j++ = -T2(1);      *j++ = -T2(2);
              *j++ = -rAXT2(0);  *j++ = -rAXT2(1);   *j++ = -rAXT2(2);
              *j++ = T2(0);      *j++ = T2(1);       *j++ = T2(2);
              *j++ = rBXT2(0);   *j++ = rBXT2(1);    *j++ = rBXT2(2);
              *jb++ = contact->get_body_A()->m_tag;  *jb++ = contact->get_body_B()->m_tag;
              *err++ = 0; *lo++ = -mu; *hi++ = mu; *dep++ = idx; *fac++ = mu;
            }
          }
          idx += 1+ eta;
        }
        //--- Compute Right Hand Side:  b = J(u + h*inv(M)f_ext) - error
        //--- Compute b = J(u+tmp)
        m_tmp2 = m_tmp;
        m_tmp2 += m_u;     //--- tmp2 = u + h*inv(M)f_ext
        CompressedJacobian< Types >::mul(m,m_J,m_jb,m_tmp2,m_b); //--- b = J*tmp2
        m_b -= m_error;
        //--- Solve LCP problem
        assert(m_iterations>0);
        m_solver.setMaxIterations(m_iterations);
        m_solver.run( m, m_invM, m_invI, m_J, m_jb, m_lambda, m_Fc, m_lo, m_hi, m_b, m_dep, m_factor );
#ifdef RETRO_CACHE_LAGRANGE_MULTIPLIERS
      group.cacheSolution(m_lambda);
#endif
        //--- Compute Velocity Update
        //---  u = u + Minv (Jtrans * lambda + h*f_ext);
        m_u += m_Fc;//--- Add constraint forces
      }
      if(h)
      {
        m_u += m_tmp;  //--- Add external forces
        group.positionUpdate(m_s,m_u,h,m_s);
        group.restorePosition(m_s);
      }
      group.restoreVelocity(m_u);
    };

  };// End of class RetroDynamicsStepper

*/


 /**
    * Run Iterative LCP Solver.
    * In the following m denotes the total number of constraints (ie. rows in
    * the jacobian matrix), and n denotes the number of bodies.
    *
    *  given A = J inv(M) transpose(J), solve
    *
    *    A lambda + b >= 0 compl.  lo <= lambda <= hi
    *
    * Please make sure to notice on what side of the inequality the b-vector resides. If
    * you have modelled you problem as A lambda >= b, remember to flip the sign
    * of the b-vector.
    *
    *
    * @param invM         Vector of size n, holding inverted mass values of bodies.
    * @param invI         Vector of size  n*9, Inverse inertia tensor of i'th body
    *                     is:  Ixx = invI[i*12],Ixy = invI[i*12+1],...,Izz = invI[i*12+11].
    * @param J            Vector of size n*12, each row describes a constraint between two bodies.
    * @param jb           Vector of size m*2, entry jb[k*2] holds the index for the first
    *                     body in the k'th row of J, jb[k*2+1] holds the index of the second
    *                     body in the k'th row of J.
    * @param lambda       Vector of size m, these are the lagarange multilpiers, the solution
    *                     we seek. Solver can be warmstarted by passing the last solution returned
    *                     by solver as input.
    * @param fc           Vector of size n*6, entries fc[i*6],fc[i*6+1],fc[i*6+2]  holds to total
    *                     linear constraint force that should be applied to the i'th body, entries:
    *                     fc[i*6+3],fc[i*6+4],fc[i*6+5] holds to total torque.
    * @param lo           Same as upper bounds except that this is lower bounds.
    * @param hi           Vector of size m, holds the upper limits, except for those constraints
    *                     that have linear dependent bounds, in which case the enty holds to factor.
    * @param b            Vector of size m.
    * @param dep          Vector of size m, entry dep[m] holds the index of the constraint, which the
    *                     bounds of m may be linear dependent on. If value is -1 then there is no dependency.
    * @param factor       Vector of size m, entry factor[m] holds the scalaing factor for the m-constraint,
    *                     ie.  lo[m] = -hi[m] = factor[m]*lambda[dep[m]].
    */



namespace WE {

	class PhysConstraintSolver {

	public:



	};
}

#endif