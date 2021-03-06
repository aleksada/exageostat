/**
 *
 * Copyright (c) 2017-2018  King Abdullah University of Science and Technology
 * All rights reserved.
 *
 * ExaGeoStat is a software package provided by KAUST
 **/
/**
 *
 * @file core_dcmg.c
 *
 * Generate covariance matrix of a set of locations in 2D using Matern kernel.
 *
 * @version 1.0.0
 *
 * @author Sameh Abdulah
 * @date 2018-11-11
 *
 **/
#include "../include/exageostatcore.h"

// This function converts decimal degrees to radians
static double deg2rad(double deg) {
  return (deg * PI / 180);
}
//  This function converts radians to decimal degrees
static double rad2deg(double rad) {
  return (rad * 180 / PI);
}
/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @param lat1d Latitude of the first point in degrees
 * @param lon1d Longitude of the first point in degrees
 * @param lat2d Latitude of the second point in degrees
 * @param lon2d Longitude of the second point in degrees
 * @return The distance between the two points in kilometers
 */
static double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(lat1d);
  lon1r = deg2rad(lon1d);
  lat2r = deg2rad(lat2d);
  lon2r = deg2rad(lon2d);
  u = sin((lat2r - lat1r)/2);
  v = sin((lon2r - lon1r)/2);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

static double calculateDistance(double x1, double y1, double x2, double y2, int distance_metric) {

	if(distance_metric == 1)
		return distanceEarth(x1, y1, x2, y2);
        return  sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

/***************************************************************************//**
 *
 *  core_dcmg - Generate covariance matrix A in dense format between two sets of locations (l1, l2) (Matern Kernel).
 *  The routine makes only one pass through the tile A.
 *  One tile operation.	
 *******************************************************************************
 *
 * @param[out] A
 *           The m-by-n matrix on which to compute the covariance matrix.
 *
 * @param[in] m
 *          The number of rows in the tile A. 
 *
 * @param[in] n
 *          The number of cols in the tile A. 
 *
 * @param[in] m0
 *          Global row index of the tile A.
 *
 * @param[in] n0
 *          Global col index of the tile A.
 *
 * @param[in] l1
 *          Location struct of the first input.
 *
 * @param[in] l2
 *          Location struct of the second input.
 *
 * @param[in] localtheta
 *          Parameter vector that is used to generate the output covariance matrix.
 *
 * @param[in] distance_metric
 *          Distance metric "euclidean Distance (ED) ->0" or "Great Circle Distance (GCD) ->1"
 *
 *******************************************************************************
 *
 *
 ******************************************************************************/
void core_dcmg (double *A, int m, int n, int m0, int n0, location  *l1, location *l2, double *localtheta, int distance_metric) {
    
	int i, j;
	double l1x, l1y, l2x, l2y;
	double expr = 0.0;
	double con;
	double sigma_square = localtheta[0];// * localtheta[0]; 

	con = pow(2,(localtheta[2]-1)) * tgamma(localtheta[2]);
	con = 1.0/con;
	con = sigma_square * con;

	for (j = 0; j < n; j++) {
		l1x = l1->x[j+n0];		
		l1y = l1->y[j+n0];	
		for (i = 0; i < m; i++) {
			l2x = l2->x[i+m0];
			l2y = l2->y[i+m0];
                        expr = calculateDistance(l1x, l1y, l2x, l2y, distance_metric)/localtheta[1];
			if(expr == 0)
                                A[i + j * m] = sigma_square; /* + 1e-4*/
                        else
                                A[i + j * m] = con*pow(expr, localtheta[2])*gsl_sf_bessel_Knu(localtheta[2],expr); // Matern Function

		}
	}

}




