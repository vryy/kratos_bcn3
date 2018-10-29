//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Bodhinanda Chandra
//
//  References:      This class is adapted from applications/SolidMechanicsApplication/custom_utilities/solid_mechanics_math_utilities.hpp


#if !defined(KRATOS_PARTICLE_MECHANICS_MATH_UTILITIES_H_INCLUDED)
#define      KRATOS_PARTICLE_MECHANICS_MATH_UTILITIES_H_INCLUDED


#ifdef FIND_MAX
#undef FIND_MAX
#endif

#define FIND_MAX(a, b) ((a)>(b)?(a):(b))

// System includes
#include <cmath>

// External includes

// Project includes
#include "utilities/math_utils.h"
#include "geometries/point.h"


namespace Kratos
{
template<class TDataType>
class ParticleMechanicsMathUtilities
{
public:
    /**
     * @name type definitions
     * @{
     */
    typedef Matrix MatrixType;

    typedef Vector VectorType;

    typedef unsigned int IndexType;

    typedef unsigned int SizeType;

    typedef MathUtils<TDataType> MathUtilsType;

    typedef DenseVector<Vector> Second_Order_Tensor;

    typedef DenseVector<Second_Order_Tensor> Third_Order_Tensor;

    typedef DenseVector<DenseVector<Matrix> > Fourth_Order_Tensor;

    typedef DenseMatrix<Second_Order_Tensor> Matrix_Second_Tensor;


    /**
     * @}
     */
    /**
     * calculates the QR Factorization of given square matrix A=QR.
     * The Factorization is performed using the householder algorithm
     * @param A the given square matrix the factorization is to be calculated.
     * @param Q the result matrix Q
     * @param R the result matrix R
     */
    static inline void QRFactorization(const MatrixType& A, MatrixType& Q, MatrixType& R)
    {

        if(A.size1()!= A.size2())
            KRATOS_ERROR<<" GIVEN MATRIX IS NOT A SQUARE MATRIX: QRFactorization calculation"<<std::endl;

        //QR Factorization with Householder-Algo
        unsigned int dim= A.size1();

        Vector y(dim);

        Vector w(dim);

        R.resize(dim,dim,false);

        noalias(R)=ZeroMatrix(dim,dim);

        Q.resize(dim,dim,false);

        noalias(Q)=ZeroMatrix(dim,dim);

        Matrix Help(A.size1(),A.size2());
	    noalias(Help) = A;

        Matrix unity(dim,dim);
	    noalias(unity) = ZeroMatrix(dim,dim);

        for(unsigned int j=0; j<dim; j++)
            unity(j,j)=1.0;

        std::vector<Matrix> HelpQ(dim-1);

        std::vector<Matrix> HelpR(dim-1);

        for(unsigned int i=0; i< dim-1; i++)
        {
            HelpQ[i].resize(dim,dim,false);
            HelpR[i].resize(dim,dim,false);
            noalias(HelpQ[i])= unity;
            noalias(HelpR[i])= ZeroMatrix(dim,dim);
        }

        for(unsigned int iteration=0; iteration< dim-1; iteration++)
        {
            //Vector y
            for(unsigned int i=iteration; i<dim; i++)
                y(i)= Help(i,iteration);


            //Helpvalue l
            double normy=0.0;

            for(unsigned int i=iteration; i<dim; i++)
                normy += y(i)*y(i);

            normy= std::sqrt(normy);

            double l= std::sqrt((normy*(normy+std::abs(y(iteration))))/2);

            double k=0.0;

            if(y[iteration] !=0)
                k= - y(iteration)/std::abs(y(iteration))*normy;
            else
                k= -normy;

            for(unsigned int i=iteration; i<dim; i++)
            {
                double e=0;

                if(i==iteration)
                    e=1;

                w(i)= 1/(2*l)*(y(i)-k*e);
            }

            for(unsigned int i=iteration; i<dim; i++)
                for(unsigned int j=iteration; j<dim; j++)
                    HelpQ[iteration](i,j)= unity(i,j)- 2*w(i)*w(j);


            for(unsigned int i=iteration; i<dim; i++)
                for(unsigned int j=iteration; j<dim; j++)
                    for(unsigned int k=iteration; k<dim; k++)
                        HelpR[iteration](i,j)+= HelpQ[iteration](i,k)*Help(k,j);

            Help= HelpR[iteration];

        }

        //Assembling R
        for(unsigned int k=0; k<dim-1; k++)
        {
            for(unsigned int i=k; i<dim; i++)
                for(unsigned int j=k; j<dim; j++)
                    R(i,j) =HelpR[k](i,j);

        }


        for(unsigned int k=1; k<dim-1; k++)
        {
            for(unsigned int i=0; i<dim; i++)
                for(unsigned int j=0; j<dim; j++)
                    for(unsigned int l=0; l<dim; l++)
                        Q(i,j)+= HelpQ[(k-1)](i,l)*HelpQ[k](l,j);
            noalias(HelpQ[k])=Q;
        }
        if(dim-1==1)
            noalias(Q)=HelpQ[0];

    }


    /**
     * @}
      */
    /**
     * calculates Eigenvalues of given square matrix A.
     * The QR Algorithm with shifts is used
     * @param A the given square matrix the eigenvalues are to be calculated.
     * @param tolerance convergence criteria
     * @param zero number treated as zero
     * @return Vector of eigenvalues
     * WARNING only valid for 2*2 and 3*3 Matrices yet
     */

    static inline Vector EigenValues(const Matrix& A, double tolerance = 1e-9, double zero = 1e-9)
    {
        unsigned int dim= A.size1();

        Matrix Convergence(2,dim);
	    noalias(Convergence) = ZeroMatrix(2,dim);

        double delta = 0.0;

        double abs   = 0.0;

        Vector Result(dim);
	    noalias(Result) = ZeroVector(dim);

        Matrix HelpA(dim,dim);
	    noalias(HelpA) = ZeroMatrix(dim, dim);

        Matrix HelpQ(dim,dim);
	    noalias(HelpQ) = ZeroMatrix(dim, dim);

        Matrix HelpR(dim,dim);
	    noalias(HelpR) = ZeroMatrix(dim, dim);

        HelpA=A;

        bool is_converged=false;
        unsigned int max_iters = 10;
        unsigned int iter = 0;

        while(is_converged == false && iter<max_iters )
        {
            double shift= HelpA((dim-1),(dim-1));

            for(unsigned int i=0; i<dim; i++)
            {
                HelpA(i,i) = HelpA(i,i)- shift;
            }

            ParticleMechanicsMathUtilities<double>::QRFactorization(HelpA, HelpQ, HelpR);

            HelpA= ZeroMatrix(dim, dim);

            for(unsigned int i=0; i<dim; i++)
            {
                HelpA(i,i) += shift;
                for(unsigned int j=0; j< dim; j++)
                {
                    for(unsigned int k=0; k< dim; k++)
                    {
                        HelpA(i,j) += HelpR(i,k)*HelpQ(k,j);
                    }
                }
            }

            delta= 0.0;

            abs = 0.0;

            for(unsigned int i=0; i<dim; i++)
            {
                Convergence(0,i)=Convergence(1,i);
                Convergence(1,i)=HelpA(i,i);
                delta+= (Convergence(1,i)-Convergence(0,i))*(Convergence(1,i)-Convergence(0,i));
                abs+=(Convergence(1,i))*(Convergence(1,i));
            }

            delta= std::sqrt(delta);

            abs=std::sqrt(abs);

            if(abs < zero)
                abs=1.0;

            if(delta < zero || (delta/abs) < tolerance)
                is_converged=true;

            iter++;
        }


        for(unsigned int i=0; i<dim; i++)
        {
            Result(i)= HelpA(i,i);

            if(std::abs(Result(i)) <zero)
                Result(i)=0.0;
        }

        return Result;
    }


    /**
     * @}
     */
    /**
     * calculates the Eigenvalues using a direct method.
     * @param A the given square matrix the eigenvalues are to be calculated.
     * WARNING only valid symmetric 3*3 Matrices
     */
    static inline Vector EigenValuesDirectMethod(const Matrix& A)
    {
        // Given a real symmetric 3x3 matrix A, compute the eigenvalues
        unsigned int dim= A.size1();
        Vector Result(dim);
	    noalias(Result) = ZeroVector(dim);

        const double p1 = A(0,1)*A(0,1) + A(0,2)*A(0,2) + A(1,2)*A(1,2);
        if (p1 == 0)
        {//A is diagonal.
            Result[0] = A(0,0);
            Result[1] = A(1,1);
            Result[2] = A(2,2);
            return Result;
        }

        const double q = (A(0,0) + A(1,1) + A(2,2)) / 3.0;
        const double p2 = (A(0,0) - q) * (A(0,0) - q) + (A(1,1) - q) * (A(1,1) - q) + (A(2,2) - q) * (A(2,2) - q) + 2.0 * p1;
        const double p = std::sqrt(p2 / 6.0);

        Matrix B(3,3);
        const double inv_p = 1.0/p;

        // B = (1 / p) * (A - q * I)  where  I is the identity matrix
        B(0,0) = inv_p * (A(0,0) - q);
        B(1,1) = inv_p * (A(1,1) - q);
        B(2,2) = inv_p * (A(2,2) - q);
        B(0,1) = inv_p * A(0,1);
        B(1,0) = inv_p * A(1,0);
        B(0,2) = inv_p * A(0,2);
        B(2,0) = inv_p * A(2,0);
        B(1,2) = inv_p * A(1,2);
        B(2,1) = inv_p * A(2,1);

        //r = det(B) / 2
        double r = 0.5 * ( B(0,0)*B(1,1)*B(2,2) + B(0,1)*B(1,2)*B(2,0) + B(1,0)*B(2,1)*B(0,2) - B(2,0)*B(1,1)*B(0,2) - B(1,0)*B(0,1)*B(2,2) - B(0,0)*B(2,1)*B(1,2) );

        // In exact arithmetic for a symmetric matrix  -1 <= r <= 1
        // but computation error can leave it slightly outside this range.
        double phi = 0.0;
        if (r <= -1) { phi = Globals::Pi / 3.0; }
        else if (r >= 1) { phi = 0.0; }
        else { phi = acos(r) / 3.0;}

        // the eigenvalues satisfy eig3 <= eig2 <= eig1
        Result[0] = q + 2.0 * p * cos(phi);
        Result[2] = q + 2.0 * p * cos(phi + (2.0 * Globals::Pi / 3.0));
        Result[1] = 3.0 * q - Result[0] - Result[2];     //% since trace(A) = eig1 + eig2 + eig3

        return Result;
    }


    /**
     * @}
     */
    /**
     * calculates the Eigenvectors and Eigenvalues of given symmetric matrix A.
     * The eigenvectors and eigenvalues are calculated using the iterative
     * Gauss-Seidel-method
     * @param A the given symmetric matrix where the eigenvectors have to be calculated.
     * @param vectors where the eigenvectors will be stored
     * @param lambda where the eigenvalues will be stored
     * @param zero_tolerance the largest value considered to be zero
     * @param max_iterations allowed
     */
    static inline void EigenVectors(const MatrixType& A,
				    MatrixType& vectors,
				    VectorType& lambda,
				    double zero_tolerance =1e-9,
				    unsigned int max_iterations = 10)
    {
        if(A.size1()!=3 || A.size2()!=3)
            KRATOS_ERROR<<" GIVEN MATRIX IS NOT 3x3: Eigenvectors calculation"<<std::endl;

        Matrix Help= A;

        for(unsigned int i=0; i<3; i++)
            for(unsigned int j=0; j<3; j++)
                Help(i,j)= Help(i,j);


        vectors.resize(Help.size1(),Help.size2(),false);

        lambda.resize(Help.size1(),false);

        Matrix HelpDummy(Help.size1(),Help.size2());

        bool is_converged = false;

        Matrix unity(Help.size1(),Help.size2());
	    noalias(unity) = ZeroMatrix(Help.size1(),Help.size2());

        for(unsigned int i=0; i< Help.size1(); i++)
            unity(i,i)= 1.0;

        Matrix V= unity;

        Matrix VDummy(Help.size1(),Help.size2());

        Matrix Rotation(Help.size1(),Help.size2());


        for(unsigned int iterations=0; iterations<max_iterations; iterations++)
        {

            is_converged= true;

            double a= 0.0;

            unsigned int index1= 0;

            unsigned int index2= 1;

            for(unsigned int i=0; i< Help.size1(); i++)
            {
                for(unsigned int j=(i+1); j< Help.size2(); j++)
                {
                    if((std::abs(Help(i,j)) > a ) && (std::abs(Help(i,j)) > zero_tolerance))
                    {
                        a= std::abs(Help(i,j));

                        index1= i;
                        index2= j;

                        is_converged= false;
                    }
                }
            }

            if(is_converged)
                break;

            // Calculation of Rotationangle

            double gamma= (Help(index2,index2)-Help(index1,index1))/(2*Help(index1,index2));

            double u=1.0;

            if(std::abs(gamma) > zero_tolerance && std::abs(gamma)< (1/zero_tolerance))
            {
                u= gamma/std::abs(gamma)*1.0/(std::abs(gamma)+std::sqrt(1.0+gamma*gamma));
            }
            else
            {
                if  (std::abs(gamma)>= (1.0/zero_tolerance))
                    u= 0.5/gamma;
            }

            double c= 1.0/(std::sqrt(1.0+u*u));

            double s= c*u;

            double teta= s/(1.0+c);

            // Rotation of the Matrix
            HelpDummy= Help;

            HelpDummy(index2,index2)= Help(index2,index2)+u*Help(index1,index2);
            HelpDummy(index1,index1)= Help(index1,index1)-u*Help(index1,index2);
            HelpDummy(index1,index2)= 0.0;
            HelpDummy(index2,index1)= 0.0;

            for(unsigned int i=0; i<Help.size1(); i++)
            {
                if((i!= index1) && (i!= index2))
                {
                    HelpDummy(index2,i)=Help(index2,i)+s*(Help(index1,i)- teta*Help(index2,i));
                    HelpDummy(i,index2)=Help(index2,i)+s*(Help(index1,i)- teta*Help(index2,i));

                    HelpDummy(index1,i)=Help(index1,i)-s*(Help(index2,i)+ teta*Help(index1,i));
                    HelpDummy(i,index1)=Help(index1,i)-s*(Help(index2,i)+ teta*Help(index1,i));
                }
            }


            Help= HelpDummy;

            //Calculation of the eigenvectors V
            Rotation =unity;
            Rotation(index2,index1)=-s;
            Rotation(index1,index2)=s;
            Rotation(index1,index1)=c;
            Rotation(index2,index2)=c;

            noalias(VDummy) = ZeroMatrix(Help.size1(), Help.size2());

            for(unsigned int i=0; i< Help.size1(); i++)
            {
                for(unsigned int j=0; j< Help.size1(); j++)
                {
                    for(unsigned int k=0; k< Help.size1(); k++)
                    {
                        VDummy(i,j) += V(i,k)*Rotation(k,j);
                    }
                }
            }
            V= VDummy;

        }

        if(!(is_converged))
        {
            KRATOS_INFO("ParticleMechanicsMathUtilities")<<"########################################################"<<std::endl;
            KRATOS_INFO("ParticleMechanicsMathUtilities")<<"Max_Iterations exceed in Jacobi-Seidel-Iteration (eigenvectors)"<<std::endl;
            KRATOS_INFO("ParticleMechanicsMathUtilities")<<"########################################################"<<std::endl;
        }

        for(unsigned int i=0; i< Help.size1(); i++)
        {
            for(unsigned int j=0; j< Help.size1(); j++)
            {
                vectors(i,j)= V(j,i);
            }
        }

        for(unsigned int i=0; i<Help.size1(); i++)
            lambda(i)= Help(i,i);

    }


    /**
     * @}
      */
    /**
     * calculates hypothenusa of a triangle
     * @param x first length
     * @param y second length
     */
    static double hypot2(const double x, const double y) {
      return std::sqrt(x*x+y*y);
    }


    /**
     * creates identity matrix.
     * Given matrix will be overwritten
     * @param given matrix to be overwritten by identity matrix
     */
    static inline MatrixType IdentityMatrix( SizeType size )
    {
        MatrixType A(size,size);
        noalias(A) = ZeroMatrix( size, size );
        for( unsigned int i=0; i<size ; i++ )
        {
            A(i,i) = 1.0;
        }
        return A;
    }//IdentityMatrix


    /**
     * Adds two matrices. first argument is overwritten by sum of both
     * Matrices are assumed to be of same dimension (no check on boundaries is made!)
     * @param A first matrix argument (overwritten by solution)
     * @param B second matrix argument
     */
    static inline void Add( MatrixType& A, const MatrixType& B )
    {
        if(A.size1()!= B.size1() || A.size2()!= B.size2()  )
            KRATOS_ERROR<<" GIVEN MATRICES ARE NOT THE SAME SIZE: Add calculation"<<std::endl;

        for( unsigned int i=0; i<A.size1(); i++ )
        {
            for( unsigned int j=0; j<A.size2(); j++ )
            {
                A(i,j) += B(i,j);
            }
        }
    }


    /**
     * multiplies two matrices. Performs operation \f$ C = A B \f$
     * @param A matrix A
     * @param B matrix B
     * @return matrix \f$ C = A B \f$
     */
    static inline MatrixType Mult( const MatrixType& A,
                                   const MatrixType& B)
    {
        if(A.size2()!= B.size1())
            KRATOS_ERROR<<" GIVEN MATRIX NUMBER OF COLUMN AND ROW ARE NOT THE SAME SIZE: Mult calculation"<<std::endl;

        MatrixType C(A.size1(),B.size2());
        for( unsigned int i=0; i<A.size1(); i++ )
        {
            for( unsigned int j=0; j<B.size2(); j++ )
            {
                for( unsigned int k=0; k<A.size2(); k++ )
                {
                    C(i,j) += B(k,j)*A(i,k);
                }
            }
        }
        return C;
    }//Mult


    /**
     * multiplies a matrix by a scalar
     */
    static inline void Mult( MatrixType& M, const TDataType a )
    {
        for( unsigned int i=0; i<M.size1(); i++ )
        {
            for( unsigned int j=0; j<M.size2(); j++ )
            {
                M(i,j) = M(i,j)*a;
            }
        }
    }


    /**
     * multiplies a vector by a scalar
     */
    static inline void Mult( VectorType& v, const TDataType a )
    {
        for( unsigned int i=0; i<v.size(); i++ )
        {
            v(i) = v(i)*a;
        }
    }//Mult


    /**
     * transposes matrix A. Matrix A is not overwritten!
     * @param A the given Matrix
     * @return the transposed matrix \f$ A^T \f$
     */
    static inline MatrixType Transpose( MatrixType& A )
    {
        MatrixType AT(A.size2(),A.size1());
	    noalias(AT) = ZeroMatrix(A.size2(),A.size1());
        for( unsigned int i=0; i<A.size1(); i++ )
        {
            for( unsigned int j=0; j<A.size2(); j++ )
            {
                AT(j,i) = A(i,j);
            }
        }
        return AT;
    }//Transpose


    /**
     * normalises a vector. Vector is scaled by \f$ V_{norm} = \frac{V}{|V|} \f$
     */
    static inline void Normalize( VectorType& v )
    {
        Mult( v, 1.0/(MathUtilsType::Norm( v )) );
    }


    /**
    * Builds the Inverse of Matrix input
    * @param input the given Matrix
    * @param inverse inverse of the given Matrix
    */
    static int InvertMatrix( const MatrixType& input, MatrixType& inverse )
    {
    #ifdef KRATOS_USE_AMATRIX   // This macro definition is for the migration period and to be removed afterward please do not use it
        Matrix A(input);
        AMatrix::LUFactorization<MatrixType, DenseVector<std::size_t> > lu_factorization(A);
        int singular = lu_factorization.determinant();
        inverse = lu_factorization.inverse();
        return singular;
    #else
        typedef permutation_matrix<std::size_t> pmatrix;
        Matrix A(input);
        pmatrix pm(A.size1());
        int singular = lu_factorize(A,pm);
        inverse.assign( IdentityMatrix(A.size1()));
        lu_substitute(A, pm, inverse);
        return singular;
    #endif // ifdef KRATOS_USE_AMATRIX
    }


    /**
    * Builds the norm of a gibven second order tensor
    * @param Tensor the given second order tensor
    * @return the norm of the given tensor
    */
    static double NormTensor(Matrix& Tensor)
    {
        double result=0.0;
        for(unsigned int i=0; i< Tensor.size1(); i++)
            for(unsigned int j=0; j< Tensor.size2(); j++)
                result+= Tensor(i,j)*Tensor(i,j);

        result= std::sqrt(result);

        return result;
    }


    /**
     * converts a strain vector into a matrix. Strains are assumed to be stored
     * in the following way:
     * \f$ [ e11, e22, e33, 2*e12, 2*e23, 2*e13 ] \f$ for 3D case and
     * \f$ [ e11, e22, 2*e12 ] \f$ fir 2D case.
     * Hence the deviatoric components of the strain vector are divided by 2
     * while they are stored into the matrix
     * @param Strains the given strain vector
     * @return the corresponding strain tensor in matrix form
     */
    static inline MatrixType StrainVectorToTensor( const VectorType& Strains )
    {
        KRATOS_TRY

        Matrix StrainTensor;
        //KRATOS_WATCH( Strains )
        if (Strains.size()==3)
        {
            StrainTensor.resize(2,2, false);
            //KRATOS_WATCH( StrainTensor )
            StrainTensor(0,0) = Strains[0];
            StrainTensor(0,1) = 0.5*Strains[2];
            StrainTensor(1,0) = 0.5*Strains[2];
            StrainTensor(1,1) = Strains[1];
        }
        else if (Strains.size()==6)
        {
            StrainTensor.resize(3,3, false);
            StrainTensor(0,0) = Strains[0];
            StrainTensor(0,1) = 0.5*Strains[3];
            StrainTensor(0,2) = 0.5*Strains[5];
            StrainTensor(1,0) = 0.5*Strains[3];
            StrainTensor(1,1) = Strains[1];
            StrainTensor(1,2) = 0.5*Strains[4];
            StrainTensor(2,0) = 0.5*Strains[5];
            StrainTensor(2,1) = 0.5*Strains[4];
            StrainTensor(2,2) = Strains[2];
        }

        //KRATOS_WATCH( StrainTensor )
        return StrainTensor;
        KRATOS_CATCH( "" )
    }


    static inline Vector TensorToStrainVector( const Matrix& Tensor )
    {
        KRATOS_TRY
        Vector StrainVector;


        if (Tensor.size1()==2)
        {
            StrainVector.resize(3);
            noalias(StrainVector) = ZeroVector(3);
            StrainVector(0) = Tensor(0,0);
            StrainVector(1) = Tensor(1,1);
            StrainVector(2) = 2.00*Tensor(0,1);
        }
        else if (Tensor.size1()==3)
        {
            StrainVector.resize(6);
            noalias(StrainVector) = ZeroVector(6);
            StrainVector(0) = Tensor(0,0);
            StrainVector(1) = Tensor(1,1);
            StrainVector(2) = Tensor(2,2);
            StrainVector(3) = 2.00*Tensor(0,1);
            StrainVector(4) = 2.00*Tensor(1,2);
            StrainVector(5) = 2.00*Tensor(0,2);
        }


        return StrainVector;
        KRATOS_CATCH( "" )
    }


    /**
    * Transforms a given 6*1 Vector to a corresponing symmetric Tensor of second order (3*3)
    * @param Vector the given vector
    * @param Tensor the symmetric second order tensor
    */
    static inline void VectorToTensor(const Vector& Stress,Matrix& Tensor)
    {
        if(Stress.size()==6)
        {
           Tensor.resize(3,3,false);
            Tensor(0,0)= Stress(0);
            Tensor(0,1)= Stress(3);
            Tensor(0,2)= Stress(5);
            Tensor(1,0)= Stress(3);
            Tensor(1,1)= Stress(1);
            Tensor(1,2)= Stress(4);
            Tensor(2,0)= Stress(5);
            Tensor(2,1)= Stress(4);
            Tensor(2,2)= Stress(2);
        }
        if(Stress.size()==3)
        {
            Tensor.resize(2,2,false);
            Tensor(0,0)= Stress(0);
            Tensor(0,1)= Stress(2);
            Tensor(1,0)= Stress(2);
            Tensor(1,1)= Stress(1);
        }

    }


    /**
    * Transforms a given symmetric Tensor of second order (3*3) to a corresponing 6*1 Vector
    * @param Tensor the given symmetric second order tensor
    * @param Vector the vector
    */
    static void TensorToVector( const Matrix& Tensor, Vector& Vector)
    {
        //if(Vector.size()!= 6)
        unsigned int dim = Tensor.size1();
        if (dim==3)
        {
            Vector.resize(6,false);
            Vector(0)= Tensor(0,0);
            Vector(1)= Tensor(1,1);
            Vector(2)= Tensor(2,2);
            Vector(3)= Tensor(0,1);
            Vector(4)= Tensor(1,2);
            Vector(5)= Tensor(2,0);
        }
        else if(dim==2)
        {
            Vector.resize(3,false);
            Vector(0)= Tensor(0,0);
            Vector(1)= Tensor(1,1);
            Vector(2)= Tensor(0,1);
        }

    }


    static inline void TensorToMatrix(Fourth_Order_Tensor& Tensor,Matrix& Matrix)
    {
        if (Tensor[0].size()== 3)
        {
            if(Matrix.size1()!=6 || Matrix.size2()!=6)
                Matrix.resize(6,6,false);
            Matrix(0,0) = Tensor[0][0](0,0);
            Matrix(0,1) = Tensor[0][0](1,1);
            Matrix(0,2) = Tensor[0][0](2,2);
            Matrix(0,3) = Tensor[0][0](0,1);
            Matrix(0,4) = Tensor[0][0](0,2);
            Matrix(0,5) = Tensor[0][0](1,2);

            Matrix(1,0) = Tensor[1][1](0,0);
            Matrix(1,1) = Tensor[1][1](1,1);
            Matrix(1,2) = Tensor[1][1](2,2);
            Matrix(1,3) = Tensor[1][1](0,1);
            Matrix(1,4) = Tensor[1][1](0,2);
            Matrix(1,5) = Tensor[1][1](1,2);

            Matrix(2,0) = Tensor[2][2](0,0);
            Matrix(2,1) = Tensor[2][2](1,1);
            Matrix(2,2) = Tensor[2][2](2,2);
            Matrix(2,3) = Tensor[2][2](0,1);
            Matrix(2,4) = Tensor[2][2](0,2);
            Matrix(2,5) = Tensor[2][2](1,2);

            Matrix(3,0) = Tensor[0][1](0,0);
            Matrix(3,1) = Tensor[0][1](1,1);
            Matrix(3,2) = Tensor[0][1](2,2);
            Matrix(3,3) = Tensor[0][1](0,1);
            Matrix(3,4) = Tensor[0][1](0,2);
            Matrix(3,5) = Tensor[0][1](1,2);

            Matrix(4,0) = Tensor[0][2](0,0);
            Matrix(4,1) = Tensor[0][2](1,1);
            Matrix(4,2) = Tensor[0][2](2,2);
            Matrix(4,3) = Tensor[0][2](0,1);
            Matrix(4,4) = Tensor[0][2](0,2);
            Matrix(4,5) = Tensor[0][2](1,2);

            Matrix(5,0) = Tensor[1][2](0,0);
            Matrix(5,1) = Tensor[1][2](1,1);
            Matrix(5,2) = Tensor[1][2](2,2);
            Matrix(5,3) = Tensor[1][2](0,1);
            Matrix(5,4) = Tensor[1][2](0,2);
            Matrix(5,5) = Tensor[1][2](1,2);
        }
        else
        {
            if(Matrix.size1()!=3 || Matrix.size2()!=3)
                Matrix.resize(3,3,false);
            Matrix(0,0) = Tensor[0][0](0,0);
            Matrix(0,1) = Tensor[0][0](1,1);
            Matrix(0,2) = Tensor[0][0](0,1);
            Matrix(1,0) = Tensor[1][1](0,0);
            Matrix(1,1) = Tensor[1][1](1,1);
            Matrix(1,2) = Tensor[1][1](0,1);
            Matrix(2,0) = Tensor[0][1](0,0);
            Matrix(2,1) = Tensor[0][1](1,1);
            Matrix(2,2) = Tensor[0][1](0,1);

        }

    }


    /**
    * Transforms a given 6*6 Matrix to a corresponing 4th order tensor
    * @param Tensor the given Matrix
    * @param Vector the Tensor
    */
    static void MatrixToTensor(MatrixType& A,std::vector<std::vector<Matrix> >& Tensor)
    {
        int help1 = 0;
        int help2 = 0;
        double coeff = 1.0;

        Tensor.resize(3);

        for(unsigned int i=0; i<3; i++)
        {
            Tensor[i].resize(3);
            for(unsigned int j=0; j<3; j++)
            {
                Tensor[i][j].resize(3,3,false);
                noalias(Tensor[i][j])= ZeroMatrix(3,3);
                for(unsigned int k=0; k<3; k++)
                    for(unsigned int l=0; l<3; l++)
                    {
                        if(i==j) help1= i;
                        else
                        {
                            if((i==0 && j==1) || (i==1 && j==0)) help1= 3;
                            if((i==1 && j==2) || (i==2 && j==1)) help1= 4;
                            if((i==2 && j==0) || (i==0 && j==2)) help1= 5;
                        }
                        if(k==l)
                        {
                            help2= k;
                            coeff=1.0;
                        }
                        else
                        {
                            coeff=0.5;
                            if((k==0 && l==1) || (k==1 && l==0)) help2= 3;
                            if((k==1 && l==2) || (k==2 && l==1)) help2= 4;
                            if((k==2 && l==0) || (k==0 && l==2)) help2= 5;
                        }

                        Tensor[i][j](k,l)= A(help1,help2)*coeff;
                    }
            }
        }


    }


    /**
    * Transforms a given 6*6 Matrix to a corresponing 4th order tensor
    * @param Tensor the given Matrix
    * @param Vector the Tensor
    */
    static void MatrixToTensor(MatrixType& A,array_1d<double, 81>& Tensor)
    {
        int help1 = 0;
        int help2 = 0;
        double coeff = 1.0;
        std::fill(Tensor.begin(), Tensor.end(), 0.0);
        for(unsigned int i=0; i<3; i++)
        {
            for(unsigned int j=0; j<3; j++)
            {
                for(unsigned int k=0; k<3; k++)
                    for(unsigned int l=0; l<3; l++)
                    {
                        if(i==j) help1= i;
                        else
                        {
                            if((i==0 && j==1) || (i==1 && j==0)) help1= 3;
                            if((i==1 && j==2) || (i==2 && j==1)) help1= 4;
                            if((i==2 && j==0) || (i==0 && j==2)) help1= 5;
                        }
                        if(k==l)
                        {
                            help2= k;
                            coeff=1.0;
                        }
                        else
                        {
                            coeff=0.5;
                            if((k==0 && l==1) || (k==1 && l==0)) help2= 3;
                            if((k==1 && l==2) || (k==2 && l==1)) help2= 4;
                            if((k==2 && l==0) || (k==0 && l==2)) help2= 5;
                        }

                        Tensor[i*27+j*9+k*3+l]= A(help1,help2)*coeff;
                    }
            }
        }


    }


    /**
    * Transforms a given 4th order tensor to a corresponing 6*6 Matrix
    * @param Tensor the given Tensor
    * @param Vector the Matrix
    */
    static void TensorToMatrix(std::vector<std::vector<Matrix> >& Tensor,Matrix& Matrix)
    {
        int help1 = 0;
        int help2 = 0;
        int help3 = 0;
        int help4 = 0;
        double coeff = 1.0;

        if(Matrix.size1()!=6 || Matrix.size2()!=6)
            Matrix.resize(6,6,false);

        for(unsigned int i=0; i<6; i++)
            for(unsigned int j=0; j<6; j++)
            {
                if(i<3)
                {
                    help1= i;
                    help2= i;
                }
                else
                {
                    if(i==3)
                    {
                        help1= 0;
                        help2= 1;
                    }
                    if(i==4)
                    {
                        help1= 1;
                        help2= 2;
                    }
                    if(i==5)
                    {
                        help1= 2;
                        help2= 0;
                    }
                }

                if(j<3)
                {
                    help3= j;
                    help4= j;
                    coeff= 1.0;
                }
                else
                {
                    if(j==3)
                    {
                        help3= 0;
                        help4= 1;
                    }
                    if(j==4)
                    {
                        help3= 1;
                        help4= 2;
                    }
                    if(j==5)
                    {
                        help3= 2;
                        help4= 0;
                    }
                    coeff= 2.0;
                }

                Matrix(i,j)= Tensor[help1][help2](help3,help4)*coeff;
            }

    }


    /**
     * Transforms a given 4th order tensor to a corresponing 6*6 Matrix
     * @param Tensor the given Tensor
     * @param Vector the Matrix
     */
    static void TensorToMatrix( const array_1d<double, 81>& Tensor, Matrix& Matrix )
    {
        if(Matrix.size1()!=6 || Matrix.size2()!=6)
            Matrix.resize(6,6,false);

        Matrix(0,0) = Tensor[0];
        Matrix(0,1) = Tensor[4];
        Matrix(0,2) = Tensor[8];
        Matrix(0,3) = 2.0*Tensor[1];
        Matrix(0,4) = 2.0*Tensor[5];
        Matrix(0,5) = 2.0*Tensor[6];

        Matrix(1,0) = Tensor[36];
        Matrix(1,1) = Tensor[40];
        Matrix(1,2) = Tensor[44];
        Matrix(1,3) = 2.0*Tensor[37];
        Matrix(1,4) = 0.0*Tensor[41];
        Matrix(1,5) = 0.0*Tensor[42];

        Matrix(2,0) = Tensor[72];
        Matrix(2,1) = Tensor[76];
        Matrix(2,2) = Tensor[80];
        Matrix(2,3) = 2.0*Tensor[73];
        Matrix(2,4) = 2.0*Tensor[77];
        Matrix(2,5) = 2.0*Tensor[78];

        Matrix(3,0) = Tensor[9];
        Matrix(3,1) = Tensor[13];
        Matrix(3,2) = Tensor[18];
        Matrix(3,3) = 2.0*Tensor[10];
        Matrix(3,4) = 2.0*Tensor[14];
        Matrix(3,5) = 2.0*Tensor[15];

        Matrix(4,0) = Tensor[45];
        Matrix(4,1) = Tensor[49];
        Matrix(4,2) = Tensor[53];
        Matrix(4,3) = 2.0*Tensor[46];
        Matrix(4,4) = 0.0*Tensor[50];
        Matrix(4,5) = 0.0*Tensor[51];

        Matrix(5,0) = Tensor[54];
        Matrix(5,1) = Tensor[58];
        Matrix(5,2) = Tensor[62];
        Matrix(5,3) = 2.0*Tensor[55];
        Matrix(5,4) = 2.0*Tensor[59];
        Matrix(5,5) = 2.0*Tensor[60];

    }


private:
};// class ParticleMechanicsMathUtilities
}
#endif /* KRATOS_PARTICLE_MECHANICS_MATH_UTILITIES_H_INCLUDED defined */
