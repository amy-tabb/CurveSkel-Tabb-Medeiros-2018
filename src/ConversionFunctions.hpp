
#ifndef CONVERSIONFUNCTIONS_HPP_
#define CONVERSIONFUNCTIONS_HPP_


#include "SkelGraph.hpp"

void ReadIndividualImages(string write_directory);

void WriteIndividualImages(string write_directory);

void ReadXYZ(string write_directory);

void EnsureDirHasTrailingBackslash(string& write_directory);

class ConversionClass{
public:
	vector<uint64_t> number_voxels_per_dim;
	uint64_t number_voxels_grid;
	double inter_voxel_distance;
	vector< vector<double> > BB;
	vector<vector<int64_t> > BB_int;

	ConversionClass(vector<vector<int64_t> >& BB_xyz);

	ConversionClass(vector<vector<double> >& boundingbox, double d);

	uint64_t ReturnIndexFromXYZIndices(uint64_t x, uint64_t y, uint64_t z);

	uint64_t ReturnIndexFromXYZIndices(vector<uint64_t>& v);

	uint64_t ReturnIndexFromXYZIndices(vector<int64_t>& v);

	void ReturnXYZIndicesFromIndex(uint64_t voxel_index, double& xd, double& yd, double& zd);


};



#endif /* CONVERSIONFUNCTIONS_HPP_ */
