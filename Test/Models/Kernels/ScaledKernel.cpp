#define BOOST_TEST_MODULE Kernels_ScaledKernel
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <shark/Models/Kernels/ScaledKernel.h>
#include <shark/Models/Kernels/LinearKernel.h>
#include "KernelDerivativeTestHelper.h"
#include <shark/Core/Random.h>

using namespace shark;

BOOST_AUTO_TEST_SUITE (Models_Kernels_ScaledKernel)

BOOST_AUTO_TEST_CASE( ScaledKernel_Test ){
	double result=-0.2;
	
	DenseLinearKernel base;
	ScaledKernel<> kernel(&base,0.1);

	// test points
	RealVector x1(2);
	x1(0)=1;
	x1(1)=0;
	RealVector x2(2);
	x2(0)=-2;
	x2(1)=0;
	//evaluate single point
	double test = kernel(x1,x2);
	BOOST_CHECK_SMALL(result-test,1.e-15);
	
	//evaluate a batch of points
	RealMatrix batch1(10,2);
	RealMatrix batch2(20,2);
	for(std::size_t i = 0; i != 10;++i){
		for(std::size_t j = 0; j != 2; ++j)
			batch1(i,j)=random::uni(random::globalRng(),-1,1);
	}
	for(std::size_t i = 0; i != 20;++i){
		for(std::size_t j = 0; j != 2; ++j)
			batch2(i,j)=random::uni(random::globalRng(),-1,1);
	}
	testEval(kernel,batch1,batch2);

	//test first derivative
	testKernelDerivative(kernel,2,1.e-7,1.e-5);
	testKernelInputDerivative(kernel,2,1.e-7,1.e-5);

}

BOOST_AUTO_TEST_SUITE_END()
