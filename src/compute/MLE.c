/**
 *
 * Copyright (c) 2017, King Abdullah University of Science and Technology
 * All rights reserved.
 *
 * ExaGeoStat is a software package provided by KAUST
 **/
/**
 *
 * @file MLE.c
 *
 * ExaGeoStat main functions.
 *
 * @version 1.0.0
 *
 * @author Sameh Abdulah
 * @date 2018-11-11
 *
 **/
#include "../include/MLE.h"
//***************************************************************************************
/** ****************************************************************************
 *  Current software version.
 **/
const char *argp_program_version = "Version 0.1.0";

void MLE_zvg(MLE_data *data,  double * Nrand, double * initial_theta, int n, int ts, int log, int p_grid, int q_grid)
//! Generate Z observation vector using generated or given locations
/*!  -- using dense or approximate computation
 * Returns Z observation vector
 * @param[in] data: MLE_data struct with different MLE inputs.
 * @param[in] Nrand: A uniform random vector with size n that is used to generate Z .
 * @param[in] initial_theta: Theta vector with three parameter (Variance, Range, Smoothness)
 *                           that is used to to generate the Covariance Matrix.
 * @param[in] n: Problem size (number spatial locations).
 * @param[in] ts: tile size (MB) is used only in the case of HiCMA not MORSE.
 * @param[in] log: equals one if the user needs to generate log files for his problem.
 * @param[in] p_grid: p_grid in the case of distributed system.
 * @param[in] q_grid: q_grid in the case of distributed system.
 * */
{
	data->iter_count	= 0;
        char *computation       = data->computation;
	int async		= data->async;

	if ( strcmp (computation, "exact") == 0 && async == 0)
		MORSE_MLE_dzvg_Tile(data, Nrand, initial_theta, n, ts, log);
	else if ( strcmp (computation, "exact") == 0 && async == 1)
		MORSE_MLE_dzvg_Tile_Async(data, Nrand, initial_theta, n, ts, log);
	else if ( strcmp (computation, "diag_approx") == 0 && async == 0)
                MORSE_MLE_dzvg_Tile(data, Nrand, initial_theta, n, ts, log);
        else if ( strcmp (computation, "diag_approx") == 0 && async == 1)
                MORSE_MLE_dzvg_Tile_Async(data, Nrand, initial_theta, n, ts, log);	
	#if defined( EXAGEOSTAT_USE_HICMA )
        else if ( strcmp (computation, "lr_approx") == 0 && async == 0)
		HICMA_MLE_dzvg_Tile(data, Nrand, initial_theta, n, ts, log, p_grid, q_grid);
	else if ( strcmp (computation, "lr_approx") == 0 && async == 1)
                HICMA_MLE_dzvg_Tile_Async(data, Nrand, initial_theta, n, ts, log, p_grid, q_grid);	
	#endif
}


//void MLE_zvr(MLE_data *data, int n, char *format)
//! Read Z observation vector using a give file.
/*!  -- using dense or approximate computation
 * Returns Z observation vector
 * @param[in] data: MLE_data struct with different MLE inputs.
 * @param[in] n: Problem size (number spatial locations).
 * */
double MLE_alg(unsigned n, const double * theta, double * grad, void * data)//int based_sys, int async)
//! Maximum Likelihood Evaluation (MLE)
/*!  -- using exact or approximation computation
 * Returns the loglikelihhod value for the given theta.
 * @param[in] n: unsigned variable used by NLOPT package.
 * @param[in] theta: theta Vector with three parameter (Variance, Range, Smoothness)
 *                           that is used to to generate the Covariance Matrix.
 * @param[in] grad: double variable used by NLOPT package.
 * @param[in] data: MLE_data struct with different MLE inputs.
*/
{
	char *computation	= ((MLE_data*)data)->computation;
	int async		= ((MLE_data*)data)->async;
	// printf("%s - %s\n", computation, __func__);

	if (strcmp (computation, "exact") == 0 && async == 0)
		return MORSE_MLE_Tile(n, theta,  grad,  data);                
	else if (strcmp (computation, "exact") == 0 && async == 1)
        	return MORSE_MLE_Tile_Async(n, theta,  grad,  data);
        else if (strcmp (computation, "diag_approx") == 0 && async == 0)
                return MORSE_MLE_diag_Tile(n, theta,  grad,  data);
        else if (strcmp (computation, "diag_approx") == 0 && async == 1)
                return MORSE_MLE_diag_Tile_Async(n, theta,  grad,  data);
	#if defined(EXAGEOSTAT_USE_HICMA)
	else if (strcmp (computation, "lr_approx") == 0 && async == 0)
                return HICMA_MLE_Tile(n, theta,  grad,  data);
        else if (strcmp (computation, "lr_approx") == 0 && async == 1)
                return HICMA_MLE_Tile_Async(n, theta,  grad,  data);
	#endif
	else
		return 0;
}

void set_args_default(arguments *arg_values)
//! set default values for input
/*!  arguments
 * @param[in] arg_values: user arguments
 * */
{

	arg_values->test		= 0;                 
	arg_values->check		= 0;               
        arg_values->verbose		= 0;             
        arg_values->zvecs		= "1";             
        arg_values->computation 	= "exact";
        arg_values->async		= 0;               
        arg_values->kernel		= "";             
        arg_values->ikernel		= "";   
        arg_values->ncores		= "1";              
        arg_values->gpus		= "0";              
        arg_values->p			= "1";             
        arg_values->q			= "1";              
        arg_values->N			= "0";                   
        arg_values->lts			= "0";
        arg_values->dts        		= "0";                  
        arg_values->locs_file		= "";        
        arg_values->obs_dir		= "";         
        arg_values->actualZ_file 	="";
        arg_values->actualZloc_file 	= "";
        arg_values->predict		= "0";       
        arg_values->dm			= "ed";
        arg_values->diag_thick  	= "1";
	arg_values->log			= 0;
	arg_values->maxrank		= "0";
	arg_values->acc			= "0";
	arg_values->profile		= 0;	
	arg_values->opt_tol		= "5";
	arg_values->opt_max_iters	= "-1";
        arg_values->ooc		        = 0;
}


void check_args(arguments *arg_values)
//! check  values for input
/*!  arguments
 * @param[in] arg_values: user arguments
 * */
{
        if (arg_values->test == 0)
	{
		if (strcmp(arg_values->locs_file,"") == 0 || strcmp(arg_values->obs_dir, "") == 0) 
		{
			fprintf(stdout, "(ExaGeoStat Error MSG): Real running mode requires both locs_file and obs_dir path.... \nExaGeoStat terminated \n\n");
			exit(EXIT_FAILURE);
		}
	}	
	else
	{
	        if (strcmp(arg_values->locs_file,"") != 0 || strcmp(arg_values->obs_dir, "") != 0)
                        fprintf(stdout, "(ExaGeoStat Warning MSG): Test running mode does not require locs_file and obs_dir paths, any will be ignored-> continue running...\n");
	}
	
	//if(strcmp(arg_values->computation,"exact") != 0)
//	{
//		fprintf(stdout, "(ExaGeoStat Error MSG): approximation is not supported yet, please use only exact computation for now.... \nExaGeoStat terminated \n\n");
//		exit(EXIT_FAILURE);
//	}	
	if (atoi(arg_values->predict) == 0)
	{
		if (strcmp(arg_values->actualZ_file,"") != 0 || strcmp(arg_values->actualZloc_file,"") != 0)
                        fprintf(stdout, "(ExaGeoStat Warning MSG): Test running mode does not require actualZ_file and actualZloc_file paths, any will be ignored-> continue running...\n");
	}
	
}


void init(int *test, int *N,  int *ncores, int *gpus, int *p_grid, int *q_grid, int *zvecs, int *dts,int *lts, int *nZmiss, int *log,  double *initial_theta, double *starting_theta, double *target_theta, double *lb, double *ub, MLE_data *data, arguments *arguments)
//! initialize exageostat by setting several
/*! variables from argument
 * Returns MLE_data struct.
 * @param[in] arguments: command line arguments.
 * @param[out] test: if test=1 ->test running mode  if test=0->real running mode. 
 * @param[out] N: number of spatial locations (problem size).
 * @param[out] ncores: number of CPU computing units.
 * @param[out] gpus: number of GPU computing units.
 * @param[out] p_grid: p_grid in the case of distributed system.
 * @param[out] q_grid: q_grid in the case of distributed system.
 * @param[out] zvecs: the number of Z vectors that should be generated in the case of test running mode.
 * @param[out] dts: dense tile size.
 * @param[out] lts: TLR tile size.
 * @param[out] nZmiss: number of unknown observation to be predicted in the case of test running mode.
 * @param[out] log: determine if log files should be generated or not (log files stored on disk)
 * @param[out] initial_theta: initial_theta Vector with three parameter (Variance, Range, Smoothness)
			that is used to to generate the Covariance Matrix and initial Z vector.
 * @param[out] starting_theta: theta Vector with three parameter (Variance, Range, Smoothness)
			that is used to to generate the Covariance Matrix of the first MLE iteration.
 * @param[out] target_theta: target theta Vector with three parameter (Variance, Range, Smoothness) unknown theta parameter should be shown as '?'
 * @param[out] lb: optimization lower bounds vector ( lb_1, lb_2, lb_3).
 * @param[out] ub: optimization upper bounds vector ( ub_1, ub_2, ub_3).
 * @param[out] data: MLE_data struct with different MLE inputs.
 * @param[out] arguments: command line arguments.
 * */
{

        init_data_values(data);
        *test                   = arguments->test;
        *ncores                 = atoi(arguments->ncores);
        *gpus                   = atoi(arguments->gpus);
        *p_grid                 = atoi(arguments->p);
        *q_grid                 = atoi(arguments->q);
        *N                      = atoi( arguments->N);
        *zvecs                  = atoi(arguments->zvecs);
        *dts                    = atoi(arguments->dts);
        *lts                    = atoi(arguments->lts);
        *nZmiss                 = atoi(arguments->predict);
	*log                    = arguments->log;
        data->computation       = arguments->computation;// exact or approx
        data->async             = arguments->async; // 0-->tile  1-->tile_async
        data->locsFPath         = arguments->locs_file;
        data->obsFPath          = arguments->obs_dir;
        data->actualZFPath      = arguments->actualZ_file;
        data->actualZLocFPath   = arguments->actualZloc_file;
        data->dm                = arguments->dm;
        data->diag_thick        = atoi(arguments->diag_thick);
        data->log               = arguments->log;
        data->hicma_maxrank     = atoi(arguments->maxrank);
        data->hicma_acc         = atof(arguments->acc);
	data->check		= arguments->check;
	data->verbose		= arguments->verbose;
	data->opt_tol		= atoi(arguments->opt_tol);
	data->opt_max_iters	= atoi(arguments->opt_max_iters);
        data->ooc               = arguments->ooc;
        //data->l2		= data->l1;
	
        theta_parser2(lb, arguments->olb);
        theta_parser2(ub, arguments->oub);
        starting_theta[0] = lb[0];
        starting_theta[1] = lb[1];
        starting_theta[2] = lb[2];

        theta_parser(initial_theta, target_theta, starting_theta, arguments->ikernel, arguments->kernel, lb, ub, *test);

}


void exageostat_init(int *ncores, int *gpus, int *dts, int *lts)
//! initialize exageostat (initiate underlying library)
/* @param[in] ncores: number of used CPU cores.
 * @param[in] gpus: number of used GPU units.	
 * @param[in] ts: tile size.
*/
{
	MORSE_user_tag_size(31,26);
        MORSE_Init(*ncores, *gpus);
       // MORSE_Enable(MORSE_AUTOTUNING);
       //MORSE_Set(MORSE_TILE_SIZE, *dts);
       //MORSE_Set(HICMA_TILE_SIZE, *lts); //should be added to HiCMA
}

void exageostat_finalize()
//! finalize exageostat (initiate underlying library)
{
        MORSE_Finalize();
}


void prediction_init(MLE_data *data, int nZmiss, int nZobs, int ts, int p_grid, int q_grid, int mse_flag)
//! initialize exageostat prediction allocation (allocate memory).
/* @param[in] data: MLE_data struct with different MLE inputs.
 * @param[in] nZmiss: number of missing measurements.
 * @param[in] nZobs: number of observed measurements.
 * @param[in] p_grid: p_grid in the case of distributed system.
 * @param[in] q_grid: q_grid in the case of distributed system.
 * @param[in] mse_flag: flag to enable or disable Mean Square Error (MSE) computing.
*/
{
        if(strcmp(data->computation,"exact") == 0 || strcmp(data->computation,"diag_approx") == 0)
		MORSE_MLE_Predict_Allocate(data, nZmiss, nZobs, ts, p_grid, q_grid, mse_flag);
	#if defined(EXAGEOSTAT_USE_HICMA)
	 else if(strcmp(data->computation,"lr_approx") == 0)
                HICMA_MLE_Predict_Allocate(data, nZmiss, nZobs, ts, p_grid, q_grid, mse_flag);		
	#endif
}


void prediction_finalize(MLE_data *data)
//! Destory and free prediction memory
/*! allocations.
 * @param[in] data: MLE_data struct with different MLE inputs.
*/
{
	if(strcmp(data->computation,"exact") == 0 || strcmp(data->computation,"diag_approx") == 0)
	{
		        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZobs) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZactual) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZmiss) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descC12) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descC22) );
			

	}
	#if defined(EXAGEOSTAT_USE_HICMA)
	else if(strcmp(data->computation,"lr_approx") == 0)
	{
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZobs) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZactual) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZmiss) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC12D) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC12UV) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC12rk) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC22D) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC22UV) );
                        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descC22rk) );
	}
	#endif
}

 void MLE_Finalize(MLE_data *data)
//! Destory and free MLE memory
/*! allocations.
 * @param[in] data: MLE_data struct with different MLE inputs.
*/
{
	if(strcmp(data->computation,"exact") == 0 || strcmp(data->computation,"diag_approx") == 0)
	{
        	MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descC) );
	        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZ) );
        	MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descZcpy) );
	        MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descproduct) );
        	MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->descdet) );
	}
	#if defined(EXAGEOSTAT_USE_HICMA)
	else if(strcmp(data->computation,"lr_approx") == 0)
	{
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descCD) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descCUV) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descCrk) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descZ) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descZcpy) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descproduct) );
                MORSE_Desc_Destroy( (MORSE_desc_t **)&(data->hicma_descdet) );
	}
	#endif
	MORSE_Finalize();
}


void MLE_get_zobs(MLE_data *data, double *z, int n)
//! Convert measurements vector from Lapack 
/*! represenation to Chameleon representation.
 * @param[in] data: MLE_data struct with different MLE inputs.
 * @param[in] z: Lapack measurements vector.
 * @param[in] n: number of meaurements.
*/
{
	if(strcmp(data->computation,"exact") == 0 || strcmp(data->computation,"diag_approx") == 0)
		MORSE_Tile_to_Lapack( ((MLE_data*)data)->descZcpy, z, n);
	#if defined(EXAGEOSTAT_USE_HICMA)
        else if(strcmp(data->computation,"lr_approx") == 0)
		MORSE_Tile_to_Lapack( ((MLE_data*)data)->hicma_descZcpy, z, n);
	#endif
}
