#include <shark/ObjectiveFunctions/Loss/HuberLoss.h>
#include <shark/Core/Random.h>
#include "TestLoss.h"

#define BOOST_TEST_MODULE OBJECTIVEFUNCTIONS_HUBERLOSS
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace shark;
using namespace std;

BOOST_AUTO_TEST_SUITE (ObjectiveFunctions_HuberLoss)

BOOST_AUTO_TEST_CASE( HUBERLOSS_TEST ) {
	unsigned int maxTests = 10000;
	for (unsigned int test = 0; test != maxTests; ++test) {
		HuberLoss loss(2.0);

		RealMatrix testPoint(2,3);
		testPoint(0,0) = random::uni(random::globalRng(), -1,1);
		testPoint(1,0) = random::uni(random::globalRng(), -1,1);
		testPoint(0,1) = random::uni(random::globalRng(), -1,1);
		testPoint(1,1) = random::uni(random::globalRng(), -1,1);
		testPoint(0,2) = random::uni(random::globalRng(), -1,1);
		testPoint(1,2) = random::uni(random::globalRng(), -1,1);
		
		RealMatrix testLabel(2,3);
		testLabel(0,0) = random::uni(random::globalRng(), -1,1);
		testLabel(1,0) = random::uni(random::globalRng(), -1,1);
		testLabel(0,1) = random::uni(random::globalRng(), -1,1);
		testLabel(1,1) = random::uni(random::globalRng(), -1,1);
		testLabel(0,2) = random::uni(random::globalRng(), -1,1);
		testLabel(1,2) = random::uni(random::globalRng(), -1,1);
		
		//test evalDerivative (first)
		RealMatrix derivative;
		double valueEval = loss.eval(testLabel, testPoint);
		double valueEvalDerivative = loss.evalDerivative(testLabel, testPoint, derivative);
		BOOST_CHECK_SMALL(valueEval - valueEvalDerivative, 1.e-13);
		BOOST_REQUIRE_EQUAL(derivative.size1(), 2);
		BOOST_REQUIRE_EQUAL(derivative.size2(), 3);
		
		for(std::size_t i = 0; i != 2; ++i){
			RealVector estimatedDerivative = estimateDerivative(loss, RealMatrix(rows(testPoint,i,i+1)), rows(testLabel,i,i+1));
			BOOST_CHECK_SMALL(norm_2(row(derivative,i) - estimatedDerivative), 1.e-5);
		}
	}
}
BOOST_AUTO_TEST_SUITE_END()
