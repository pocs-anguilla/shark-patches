#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
#include <shark/Core/Images/ReadImage.h>

#define BOOST_TEST_MODULE Core_ImageReorder
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <fstream>
using namespace shark;
using namespace std;

struct ImageFixture {
	ImageFixture()
	: shapeRGBA({32,26,4}), shapeRGB({32,26,3}), shapeGray({32,26,1})
	, dataRGBA(32*26*4,0), dataRGB(32*26*3,0), dataGray(32*26){
		//red and green channels are 0
		for(std::size_t i = 0; i != 32; ++i){
			for(std::size_t j = 0; j != 26; ++j){
				dataRGBA[(i*26+j)*4+2] = 1.0;//blue channel is maximal
				double alpha = 0.0;
				if( i >= 16)
					alpha += 2.0/3.0;
				if(j >= 13)
					alpha += 1.0/3.0;
				dataRGBA[(i*26+j)*4+3] = alpha;//alpha channel are 4 squares
			}
		}
		
		//the jpeg image looks similar, jus without alpha channel
		for(std::size_t i = 0; i != 32; ++i){
			for(std::size_t j = 0; j != 26; ++j){
				double RG = 1.0;
				if( i < 16 && j >= 13)
					RG = 0.84;
				if( i >= 16 && j < 13)
					RG = 0.61;
				if( i >= 16 && j >= 13)
					RG = 0.0;
		
				dataRGB[(i*26+j)*3+0] = RG;
				dataRGB[(i*26+j)*3+1] = RG;
				dataRGB[(i*26+j)*3+2] = 1;
			}
		}
		
		//pgm grayscale image
		for(std::size_t i = 0; i != 32; ++i){
			for(std::size_t j = 0; j != 26; ++j){
				dataGray[i*26+j] = 1.0;
				if( i < 16 && j >= 13)
					dataGray[i*26+j] = 0.847;
				if( i >= 16 && j < 13)
					dataGray[i*26+j] = 0.639;
				if( i >= 16 && j >= 13)
					dataGray[i*26+j] = 0.071;
			}
		}
	}

	Shape shapeRGBA;
	Shape shapeRGB;
	Shape shapeGray;
	RealVector dataRGBA;
	RealVector dataRGB;
	RealVector dataGray;
};


BOOST_FIXTURE_TEST_SUITE (Core_ReadImage_Tests, ImageFixture )

BOOST_AUTO_TEST_CASE( Core_Read_PNG){
	std::ifstream file( "Test/test_data/testImage.png", std::ios::binary );
	if(!file)
		std::cout<<"could not open file"<<std::endl;
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(fileSize);
	file.read((char*) &buffer[0], fileSize);

	
	std::pair<blas::vector<double>, Shape> result = image::readPNG<double>(buffer);
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGBA[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGBA[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGBA[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGBA.size());
	
	for(std::size_t i = 0; i != dataRGBA.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGBA[i], 1.e-4);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_JPEG){
	std::ifstream file( "Test/test_data/testImage.jpeg", std::ios::binary );
	if(!file)
		std::cout<<"could not open file"<<std::endl;
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(fileSize);
	file.read((char*) &buffer[0], fileSize);

	
	std::pair<blas::vector<double>, Shape> result = image::readJPEG<double>(buffer);
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGB[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGB[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGB[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGB.size());
	
	//jpeg is not loss-less so we have to give it some slack
	for(std::size_t i = 0; i != dataRGB.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGB[i], 0.05);
	}
}


BOOST_AUTO_TEST_CASE( Core_Read_PGM){
	std::ifstream file( "Test/test_data/testImage.pgm", std::ios::binary );
	if(!file)
		std::cout<<"could not open file"<<std::endl;
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(fileSize);
	file.read((char*) &buffer[0], fileSize);

	
	std::pair<blas::vector<double>, Shape> result = image::readPGM<double>(buffer);
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeGray[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeGray[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeGray[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataGray.size());
	
	//jpeg is not loss-less so we have to give it some slack
	for(std::size_t i = 0; i != dataGray.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataGray[i], 0.05);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_Image_PNG){
	std::ifstream file( "Test/test_data/testImage.png", std::ios::binary );
	if(!file)
		std::cout<<"could not open file"<<std::endl;
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(fileSize);
	file.read((char*) &buffer[0], fileSize);

	
	std::pair<blas::vector<double>, Shape> result = image::readImage<double>(buffer);
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGBA[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGBA[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGBA[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGBA.size());
	
	for(std::size_t i = 0; i != dataRGBA.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGBA[i], 1.e-4);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_Image_JPEG){
	std::ifstream file( "Test/test_data/testImage.jpeg", std::ios::binary );
	if(!file)
		std::cout<<"could not open file"<<std::endl;
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(fileSize);
	file.read((char*) &buffer[0], fileSize);

	
	std::pair<blas::vector<double>, Shape> result = image::readImage<double>(buffer);
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGB[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGB[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGB[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGB.size());
	
	//jpeg is not loss-less so we have to give it some slack
	for(std::size_t i = 0; i != dataRGB.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGB[i], 0.05);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_Image_From_File_PNG){
	std::pair<blas::vector<double>, Shape> result = image::readImageFromFile<double>("Test/test_data/testImage.png");
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGBA[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGBA[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGBA[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGBA.size());
	
	for(std::size_t i = 0; i != dataRGBA.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGBA[i], 1.e-4);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_Image_From_File_JPEG){
	std::pair<blas::vector<double>, Shape> result = image::readImageFromFile<double>("Test/test_data/testImage.jpeg");
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeRGB[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeRGB[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeRGB[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataRGB.size());
	
	//jpeg is not loss-less so we have to give it some slack
	for(std::size_t i = 0; i != dataRGB.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataRGB[i], 0.05);
	}
}

BOOST_AUTO_TEST_CASE( Core_Read_Image_From_File_PGM){
	std::pair<blas::vector<double>, Shape> result = image::readImageFromFile<double>("Test/test_data/testImage.pgm");
	
	BOOST_REQUIRE_EQUAL(result.second[0], shapeGray[0]);
	BOOST_REQUIRE_EQUAL(result.second[1], shapeGray[1]);
	BOOST_REQUIRE_EQUAL(result.second[2], shapeGray[2]);
	BOOST_REQUIRE_EQUAL(result.first.size(), dataGray.size());
	
	//jpeg is not loss-less so we have to give it some slack
	for(std::size_t i = 0; i != dataGray.size(); ++i){
		BOOST_CHECK_SMALL(result.first[i] - dataGray[i], 0.05);
	}
}
BOOST_AUTO_TEST_SUITE_END()
