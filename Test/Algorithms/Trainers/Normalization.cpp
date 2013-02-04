#define BOOST_TEST_MODULE Trainers_Normalization
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h>
#include <shark/Algorithms/Trainers/NormalizeComponentsUnitInterval.h>
#include <shark/Algorithms/Trainers/NormalizeComponentsWhitening.h>
#include <shark/Statistics/Distributions/MultiVariateNormalDistribution.h>

using namespace shark;

BOOST_AUTO_TEST_CASE( NORMALIZE_TO_UNIT_VARIANCE )
{
	std::vector<RealVector> input(3);
	RealVector v(1);
	v(0) = 0.0; input[0] = v;
	v(0) = 1.0; input[1] = v;
	v(0) = 2.0; input[2] = v;
	UnlabeledData<RealVector> set(input);
	NormalizeComponentsUnitVariance<> normalizer(true);
	Normalizer<> map;
	normalizer.train(map, set);
	Data<RealVector> transformedSet = map(set);
	double error = fabs(-sqrt(1.5) - transformedSet(0)(0)) + fabs(transformedSet(1)(0)) + fabs(sqrt(1.5) - transformedSet(2)(0));
	BOOST_CHECK_SMALL(error, 1e-10);
}

BOOST_AUTO_TEST_CASE( NORMALIZE_TO_UNIT_INTERVAL )
{
	std::vector<RealVector> input(3);
	RealVector v(1);
	v(0) = 0.0; input[0] = v;
	v(0) = 1.0; input[1] = v;
	v(0) = 2.0; input[2] = v;
	UnlabeledData<RealVector> set(input);
	NormalizeComponentsUnitInterval<> normalizer;
	Normalizer<> map;
	normalizer.train(map, set);
	Data<RealVector> transformedSet = map(set);
	BOOST_CHECK_SMALL(transformedSet(0)(0),1.e-10);
	BOOST_CHECK_SMALL(0.5 - transformedSet(1)(0),1.e-10);
	BOOST_CHECK_SMALL(1.0 - transformedSet(2)(0),1.e-10);
}

BOOST_AUTO_TEST_CASE( NORMALIZE_WHITENING)
{

	RealMatrix mat(3,3);
	mat(0,0)=2;   mat(0,1)=0.1; mat(0,2)=0.3;
	mat(1,0)=0.1; mat(1,1)=5;   mat(1,2)=0.05;
	mat(2,0)=0.3; mat(2,1)=0.05;mat(2,2)=8;
	
	RealVector mean(3);
	mean(0)=1;
	mean(1)=-1;
	mean(2)=3;
	
	MultiVariateNormalDistribution dist(3);
	dist.setCovarianceMatrix(mat);
	
	
	std::vector<RealVector> input(1000,RealVector(3));
	for(std::size_t i = 0; i != 1000;++i)
		input[i]=dist().first+mean;

	UnlabeledData<RealVector> set(input);
	NormalizeComponentsWhitening<> normalizer(1.5);
	LinearModel<> map(3, 3);
	normalizer.train(map, set);
	Data<RealVector> transformedSet = map(set);
	
	RealMatrix covariance;
	meanvar(transformedSet, mean, covariance);
	std::cout<<covariance<<std::endl;
	for(std::size_t i = 0; i != 3;++i){
		BOOST_CHECK_SMALL(mean(i),1.e-10);
		for(std::size_t j = 0; j != 3;++j){
			if(j != i){
				BOOST_CHECK_SMALL(covariance(i,j),1.e-5);
			}
			else
				BOOST_CHECK_SMALL(covariance(i,j)-1.5,1.e-5);
		}
	}
}