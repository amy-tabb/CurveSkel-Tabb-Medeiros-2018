#ifndef TabbMedeiros_HPP_
#define TabbMedeiros_HPP_

#include "Includes.hpp"

#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <math.h>
#include <cstdlib>
#include <stdio.h>

#include <list>
#include <ctime>
#include <time.h>

#include <fstream>
#include <sstream>


void TabbMedeiros(DISTANCE dist_type, string supplied_write_directory, double threshold, bool read_all_connected_components, bool record_xyz_rep);


#endif /* TabbMedeiros_HPP_ */
