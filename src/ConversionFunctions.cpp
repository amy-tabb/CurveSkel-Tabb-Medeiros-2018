#include "ConversionFunctions.hpp"
#include "DirectoryFunctions.hpp"
#include "Includes.hpp"


bool sort_ascending(pair<int, int_type_t> a, pair<int, int_type_t> b){
	return a < b;
}

bool sort_descending(pair<int, int_type_t> a, pair<int, int_type_t> b){
	return a > b;
}


void ReadIndividualImages(string write_directory){

	vector<double> pA(3, 0);
	vector<double> pB(3, 1000);
	ifstream in;
	vector<vector<double> > BB;
	string filename;


	string image_directory = write_directory + "/rawimage";

	vector<string> filenames;
	ReadDirectory(image_directory, filenames);

	ReconstructionStructure R_model;
	cv::Mat image_raw;


	for (int_type_t i = 0; i < filenames.size(); i++){
		filename = write_directory + "/rawimage/" + filenames[i];
		//OpenCV 3.0 version
		//image_raw = cv::imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

		// OpenCV 4.0 version
		image_raw = cv::imread(filename.c_str(), cv::IMREAD_GRAYSCALE);

		if (i == 0){
			// x = number of slices, y = rows, z = cols
			pA[0] = 0;
			pA[1] = 0;
			pA[2] = 0;
			pB[0] = filenames.size();
			pB[1] = image_raw.rows;
			pB[2] = image_raw.cols;


			BB.push_back(pA);
			BB.push_back(pB);

			// intervoxel distance is 1
			R_model.CreateGridMinimal(BB, 1);
		}

		for (int_type_t j = 0; j < R_model.number_voxels_per_dim[1]*R_model.number_voxels_per_dim[2]; j++){
			if (image_raw.data[j] == 0){
				R_model.configuration_grid[i*R_model.number_voxels_per_dim[1]*R_model.number_voxels_per_dim[2] + j] = true;
			}	else {
				R_model.configuration_grid[i*R_model.number_voxels_per_dim[1]*R_model.number_voxels_per_dim[2] + j] = false;
			}

		}


	}


	// for troubleshooting
	R_model.GenerateAndWriteSurfaceInPlyFormat(write_directory, 0, "converted_model", NULL, false);


	// write the model in our format
	std::ofstream out;

	string write_file = write_directory + "0.txt";
	out.open(write_file.c_str());

	for (int_type_t i = 0; i < R_model.number_voxels_grid; i++){
		if (R_model.configuration_grid[i] == false){
			out << i << " ";
		}
	}
	out << "-1" << endl;

	out.close();

	write_file = write_directory + "BB.txt";
	out.open(write_file.c_str());
	out << "1" << endl;
	out << "0 0 0" << endl;
	out << R_model.number_voxels_per_dim[0] << " " << R_model.number_voxels_per_dim[1] << " " << R_model.number_voxels_per_dim[2] << endl;

	out.close();

}

void WriteIndividualImages(string write_directory){

	vector<double> pA(3, 0);
	vector<double> pB(3, 1000);
	ifstream in;
	vector<vector<double> > BB;
	string filename;


	string image_directory = write_directory + "/processedimage";


	ReconstructionStructure R_model;
	string BBfile = write_directory + "BB.txt";
	string object_file = write_directory + "result.txt";
	double division;


	in.open(BBfile.c_str());

	if (!in){
		cout << "You forgot the BB files or the path is wrong" << endl << BBfile << endl;;
		exit(1);
	}

	in >> division;
	in >> pA[0] >> pA[1] >> pA[2];
	in >> pB[0] >> pB[1] >> pB[2];

	BB.push_back(pA);
	BB.push_back(pB);

	mkdir (image_directory.c_str(), S_IRWXU);



	ifstream oin;
	oin.open(object_file.c_str());


	if (oin.fail()){
		oin.close();

		cout << "file " << object_file << " returns error " << endl;
		exit(1);

	}	else {
		R_model.CreateGridMinimal(BB, division);

		string val;

		oin >> val;

		int_type_t index_i;
		while (val != "-1"){

			index_i = FromString<int_type_t>(val);
			R_model.configuration_grid[index_i] = false;

			oin >> val;
		}
	}



	cv::Mat image_raw(R_model.number_voxels_per_dim[1], R_model.number_voxels_per_dim[2], CV_8UC1, cv::Scalar(0));

	for (int_type_t i = 0; i < R_model.number_voxels_per_dim[0]; i++){

		string number = ToString<int_type_t>(i);
		while (number.size() < 4){
			number = "0" + number;
		}

		filename = image_directory + "/"  + number + ".tif";


		for (int_type_t j = 0; j < R_model.number_voxels_per_dim[1]*R_model.number_voxels_per_dim[2]; j++){
			if (R_model.configuration_grid[i*R_model.number_voxels_per_dim[1]*R_model.number_voxels_per_dim[2] + j] == true){
				image_raw.data[j] = 0;
			}	else {
				image_raw.data[j] = 255;
			}
		}

		cv::imwrite(filename.c_str(), image_raw);

	}
}

void ReadXYZ(string write_directory)
{
	EnsureDirHasTrailingBackslash(write_directory);

	string in_file = write_directory + "xyz.txt"; /// insist on this naming convention.

	string BBfile = write_directory + "BB.txt";
	string supplied_object_file = write_directory + "0.txt";

	ifstream in_test;
	ofstream out_test;

	in_test.open(in_file.c_str());

	if (!in_test){
		cout << "The path for the input file is wrong" << endl << in_file << endl;;
		exit(1);
	}
	in_test.close();


	out_test.open(BBfile.c_str());

	if (!out_test){
		cout << "The path for the output directory is wrong" << endl << write_directory << endl;;
		exit(1);
	}
	out_test.close();

	vector<vector<int64_t> > BB;
	vector<int64_t> temp(3, 0);
	vector<int64_t> current_vec(3, 0);
	uint64_t index;

	BB.push_back(temp);  BB.push_back(temp);
	int64_t n;

	// need to read the file twice -- once to compute the BB, second time to compute the indices of the voxels and to write.

	in_test.open(in_file.c_str());
	in_test >> n;

	if (n < 0){
		cout << "Number of items in the file cannot be < 0.  Error and quitting." << endl;
		exit(1);
	}

	for (int64_t i = 0; i < n; i++){
		for (int j = 0; j < 3; j++){
			in_test >> current_vec[j];
		}


		if (i == 0){
			for (int j = 0; j < 3; j++){
				BB[0][j] = current_vec[j];
				BB[1][j] = current_vec[j];
			}
		}	else {
			for (int j = 0; j < 3; j++){
				current_vec[j] < BB[0][j] ?  BB[0][j]= current_vec[j] : 0;
				current_vec[j] > BB[1][j] ? BB[1][j] = current_vec[j] : 0;
			}

		}
	}

	/// BB is the typical strict limits on the top .... add one to make sure everything works and add some padding
	for (int j = 0; j < 3; j++){
		//BB[0][j]--;
		BB[1][j]++;
	}


	in_test.close();

	/// can write the BB (Bounding box) file now
	out_test.open(BBfile.c_str());
	out_test << "1" << endl; /// this is how far away the voxels are from each other.  Obviously, one can change this.
	for (int64_t j = 0; j < 3; j++){
		out_test << BB[0][j] << " ";
	}
	out_test << endl;
	for (int j = 0; j < 3; j++){
		out_test << BB[1][j] << " ";
	}
	out_test << endl;

	out_test.close();


	////// Create the Conversion Class ////////////////
	ConversionClass CC(BB);


	out_test.open(supplied_object_file.c_str());


	in_test.open(in_file.c_str());
	in_test >> n;


	for (int64_t i = 0; i < n; i++){
		for (int j = 0; j < 3; j++){
			in_test >> current_vec[j];
		}

		index = CC.ReturnIndexFromXYZIndices(current_vec);
		out_test << index << " ";

	}
	out_test << "-1 " << endl;

	in_test.close();

	out_test.close();
}



void EnsureDirHasTrailingBackslash(string& write_directory){
	int n_letters = write_directory.size();
	bool eval =  (write_directory[n_letters - 1] == '/');
	if (eval == false){
		write_directory = write_directory + "/";
	}

}

ConversionClass::ConversionClass(vector<vector<int64_t> >& BB_xyz){

	/// assume that the inter-voxel distance is 1 when going from xyz to sparse representation.
	number_voxels_per_dim.resize(3, 0);
	inter_voxel_distance = 1;


	BB.push_back(vector<double>(3, 0));
	BB.push_back(vector<double>(3, 0));

	//copy + convert
	for (int i = 0; i < 3; i++){
		BB[0][i] = double(BB_xyz[0][i]);
		BB[1][i] = double(BB_xyz[1][i]);
	}

	BB_int = BB_xyz;

	for (int i = 0; i < 3; i++){
		number_voxels_per_dim[i] = floor((BB[1][i] - BB[0][i])/inter_voxel_distance);
		cout << "Number voxels per dimension " << number_voxels_per_dim[i] << endl;
	}

	// configuration grid is only for the voxels.
	number_voxels_grid = number_voxels_per_dim[0]*number_voxels_per_dim[1]*number_voxels_per_dim[2];
}

ConversionClass::ConversionClass(vector<vector<double> >& boundingbox, double d){
	number_voxels_per_dim.resize(3, 0);

	inter_voxel_distance = d;

	BB = boundingbox;


	for (int i = 0; i < 3; i++){
		number_voxels_per_dim[i] = floor((BB[1][i] - BB[0][i])/inter_voxel_distance);
		cout << "Number voxels per dimension " << number_voxels_per_dim[i] << endl;
	}

	// configuration grid is only for the voxels.
	number_voxels_grid = number_voxels_per_dim[0]*number_voxels_per_dim[1]*number_voxels_per_dim[2];

}

void ConversionClass::ReturnXYZIndicesFromIndex(uint64_t voxel_index, double& xd, double& yd, double& zd){

	/// first, get indices wrt BB
	int64_t x, y, z;

	uint64_t temp_index = voxel_index;
	x = temp_index/(number_voxels_per_dim[1]*number_voxels_per_dim[2]);

	temp_index -= x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]);
	y = temp_index/(number_voxels_per_dim[2]);

	temp_index -= y*(number_voxels_per_dim[2]);

	z = temp_index;

	if (x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z != voxel_index){
		cout << "ERROR on vox computation! " << endl << endl;
		exit(1);
	}

	/// then interpret as doubles outside of the BB.
	xd = BB[0][0] + inter_voxel_distance*double(x);
	yd = BB[0][1] + inter_voxel_distance*double(y);
	zd = BB[0][2] + inter_voxel_distance*double(z);


}


uint64_t ConversionClass::ReturnIndexFromXYZIndices(uint64_t x, uint64_t y, uint64_t z){
	if (x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z >= number_voxels_grid){
		cout << "ERROR ON size " <<x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z << ", n voxesl " << number_voxels_grid << endl;
		cout << "x , y, z " << x << ", " << y << ",  " << z << endl;
		exit(1);
	}

	return x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z;

}

uint64_t ConversionClass::ReturnIndexFromXYZIndices(vector<uint64_t>& v){

	return ReturnIndexFromXYZIndices(v[0], v[1], v[2]);
}


uint64_t ConversionClass::ReturnIndexFromXYZIndices(vector<int64_t>& v){

	/// need to compute these indices as uints first ....
	vector<uint64_t> vu(3, 0);

	for (int i = 0; i < 3; i++){
		vu[i] = v[i] - BB_int[0][i];
	}

	return ReturnIndexFromXYZIndices(vu);
}



