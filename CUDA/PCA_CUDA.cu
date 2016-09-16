#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <cuda.h>
#include <map>

#define num_of_threads 128


using namespace std;

struct timespec start_time;
struct timespec compute_scatter;
struct timespec before_jacobi;
struct timespec after_jacobi;
struct timespec end_time;

void runTest( int argc, char** argv);


////////////////////////////////////////////////////////////////////////////////
// declaration, forward

//抓下來的求特徵值特徵向量
/**
* @brief 求???矩?的特征值及特征向量的雅克比法
* 利用雅格比(Jacobi)方法求???矩?的全部特征值及特征向量
* @param pMatrix                n*n matrix, used to store scatter matrix
* @param nDim                   dimension of the matrix
* @param pdblVects              n*n matrix, used to store eigenvectors, column major
* @param dbEps                  maximum error
* @param nJt                    maximum number of iterations
* @param pdbEigenValues         特征值??
* @return
*/
bool Jacobi(double *pMatrix, int nDim, double *pdblVects, double *pdbEigenValues, double dbEps, int nJt)
    {
    for(int i = 0; i < nDim; i ++)
        {
        pdblVects[i*nDim+i] = 1.0f;
        for(int j = 0; j < nDim; j ++)
            {
            if(i != j)
                pdblVects[i*nDim+j]=0.0f;
            }
        }

    int nCount = 0;     //number of iterations
    while(1)
        {
        //Find the biggest element from pMatrix (except diagonal elements)
        double dbMax = pMatrix[1];
        int nRow = 0;
        int nCol = 1;
        for (int i = 0; i < nDim; i ++)          //row
            {
            for (int j = 0; j < nDim; j ++)      //col
                {
                double d = fabs(pMatrix[i*nDim+j]);

                if((i!=j) && (d> dbMax))
                    {
                    dbMax = d;
                    nRow = i;
                    nCol = j;
                    }
                }
            }

        if(dbMax < dbEps)      //check the error
            break;

        if(nCount > nJt)       //number of iterations exceeds nJt
            break;

        nCount++;

        double dbApp = pMatrix[nRow*nDim+nRow];
        double dbApq = pMatrix[nRow*nDim+nCol];
        double dbAqq = pMatrix[nCol*nDim+nCol];

        //compute the rotation angle
        double dbAngle = 0.5*atan2(-2*dbApq,dbAqq-dbApp);
        double dbSinTheta = sin(dbAngle);
        double dbCosTheta = cos(dbAngle);
        double dbSin2Theta = sin(2*dbAngle);
        double dbCos2Theta = cos(2*dbAngle);

        pMatrix[nRow*nDim+nRow] = dbApp*dbCosTheta*dbCosTheta + dbAqq*dbSinTheta*dbSinTheta + 2*dbApq*dbCosTheta*dbSinTheta;
        pMatrix[nCol*nDim+nCol] = dbApp*dbSinTheta*dbSinTheta + dbAqq*dbCosTheta*dbCosTheta - 2*dbApq*dbCosTheta*dbSinTheta;
        pMatrix[nRow*nDim+nCol] = 0.5*(dbAqq-dbApp)*dbSin2Theta + dbApq*dbCos2Theta;
        pMatrix[nCol*nDim+nRow] = pMatrix[nRow*nDim+nCol];

        for(int i = 0; i < nDim; i ++)
            {
            if((i!=nCol) && (i!=nRow))
                {
                int u = i*nDim + nRow;  //p
                int w = i*nDim + nCol;  //q
                dbMax = pMatrix[u];
                pMatrix[u]= pMatrix[w]*dbSinTheta + dbMax*dbCosTheta;
                pMatrix[w]= pMatrix[w]*dbCosTheta - dbMax*dbSinTheta;
                }
            }

        for (int j = 0; j < nDim; j ++)
            {
            if((j!=nCol) && (j!=nRow))
                {
                int u = nRow*nDim + j;  //p
                int w = nCol*nDim + j;  //q
                dbMax = pMatrix[u];
                pMatrix[u]= pMatrix[w]*dbSinTheta + dbMax*dbCosTheta;
                pMatrix[w]= pMatrix[w]*dbCosTheta - dbMax*dbSinTheta;
                }
            }

        //compute eigenvector
        for(int i = 0; i < nDim; i ++)
            {
            int u = i*nDim + nRow;      //p
            int w = i*nDim + nCol;      //q
            dbMax = pdblVects[u];
            pdblVects[u] = pdblVects[w]*dbSinTheta + dbMax*dbCosTheta;
            pdblVects[w] = pdblVects[w]*dbCosTheta - dbMax*dbSinTheta;
            }

        }
    //cout << "HI I am here" << endl;
    //sort eigenvalues and rearrange eigenvectors
    //eigenvalue = elements of the diagonal of pMatrix
	map<double,int> mapEigen;
    for(int i = 0; i < nDim; i ++)
        {
        pdbEigenValues[i] = pMatrix[i*nDim+i];

        mapEigen.insert(make_pair( pdbEigenValues[i],i ) );
        }
    //cout << "1" << endl;
    double *pdbTmpVec = new double[nDim*nDim];
    //cout << "Allocate the fucking memory" << endl;
    std::map<double,int>::reverse_iterator iter = mapEigen.rbegin();
    for (int j = 0; iter != mapEigen.rend(),j < nDim; ++iter,++j)
        {
        for (int i = 0; i < nDim; i ++)
            {
            pdbTmpVec[i*nDim+j] = pdblVects[i*nDim + iter->second];
            }

        //sort eigenvalues
        pdbEigenValues[j] = iter->first;
        }
    //cout << "Set +-" << endl;
    //set +-
    for(int i = 0; i < nDim; i ++)
        {
        double dSumVec = 0;
        for(int j = 0; j < nDim; j ++)
            dSumVec += pdbTmpVec[j * nDim + i];
        if(dSumVec<0)
            {
            for(int j = 0;j < nDim; j ++)
                pdbTmpVec[j * nDim + i] *= -1;
            }
        }
    //cout << "Almost there" << endl;

    memcpy(pdblVects,pdbTmpVec,sizeof(double)*nDim*nDim);
    delete []pdbTmpVec;

    return 1;
    }




double gettime()
    {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec+t.tv_usec*1e-6;
    }

////////////////////////////////////////////////////////////////////////////////
// CUDA kernel
////////////////////////////////////////////////////////////////////////////////
__global__ static void kernel_compute_scatter_matrix_seq(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    int blkID = blockIdx.x;
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, row_element_index;
    double total;
    row_element_index = threadIdx.x * workload; // Index of first element

    i = 0;
    while(i < workload)
        {
        if(row_element_index >= dim)
            break;
        else
            {
            //printf("(%d, %d)\n", blkID, row_element_index);
            total = 0;
            j = 0;
            while(j < n)
                {
                total = total + (d_samples[j * dim + blkID] - d_mean[blkID]) * (d_samples[j * dim + row_element_index] - d_mean[row_element_index]);
                ++j;
                }
            d_scatter[blkID * dim + row_element_index] = total;
            }
        ++row_element_index;
        ++i;
        }
    }

__global__ static void kernel_compute_scatter_matrix_seq_memory_sharing(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    extern __shared__ double data[];
    int blkID = blockIdx.x;
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, row_element_index;
    double total;
    row_element_index = threadIdx.x * workload; // Index of first element
    i = threadIdx.x;
    while(i < n)
        {
        data[i] = d_samples[i * dim + blkID];
        i = i + num_of_threads;
        }

    __syncthreads();

    i = 0;
    while(i < workload)
        {
        if(row_element_index >= dim)
            break;
        else
            {
            //printf("(%d, %d)\n", blkID, row_element_index);
            total = 0;
            j = 0;
            while(j < n)
                {
                total = total + (data[j] - d_mean[blkID]) * (d_samples[j * dim + row_element_index] - d_mean[row_element_index]);
                ++j;
                }
            d_scatter[blkID * dim + row_element_index] = total;
            }
        ++row_element_index;
        ++i;
        }
    }

__global__ static void kernel_compute_scatter_matrix_interleve(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    int blkID = blockIdx.x;
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, row_element_index;
    double total;
    //row_element_index = threadIdx.x * workload;

    i = threadIdx.x;
    while(i < dim)
        {
        j = 0;
        total = 0;
        while(j < n)
            {
            total = total + (d_samples[j * dim + blkID] - d_mean[blkID]) * (d_samples[j * dim + i] - d_mean[i]);
            ++j;
            }
        d_scatter[blkID * dim + i] = total;
        i = i + num_of_threads;
        }
    }

__global__ static void kernel_compute_scatter_matrix_interleve_memory_sharing(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    extern __shared__ double data[];
    int blkID = blockIdx.x;
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, row_element_index;
    double total;
    //row_element_index = threadIdx.x * workload;
    i = threadIdx.x;
    while(i < n)
        {
        data[i] = d_samples[i * dim + blkID];
        i = i + num_of_threads;
        }

    __syncthreads();

    i = threadIdx.x;
    while(i < dim)
        {
        j = 0;
        total = 0;
        while(j < n)
            {
            total = total + (data[j] - d_mean[blkID]) * (d_samples[j * dim + i] - d_mean[i]);
            ++j;
            }
        d_scatter[blkID * dim + i] = total;
        i = i + num_of_threads;
        }
    }

__global__ static void kernel_compute_scatter_matrix_seq_col_maj(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    int blkID = blockIdx.x; //Fixed
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, col_element_index;
    double total;
    col_element_index = threadIdx.x * workload; // Index of first element

    i = 0;
    while(i < workload)
        {
        if(col_element_index >= dim)
            break;
        else
            {
            //printf("(%d, %d)\n", blkID, row_element_index);
            total = 0;
            j = 0;
            while(j < n)
                {
                total = total + (d_samples[j * dim + col_element_index] - d_mean[col_element_index]) * (d_samples[j * dim + blkID] - d_mean[blkID]);
                ++j;
                }
            d_scatter[col_element_index * dim + blkID] = total;
            }
        ++col_element_index;
        ++i;
        }
    }

__global__ static void kernel_compute_scatter_matrix_seq_col_maj_memory_sharing(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    extern __shared__ double data[];
    int blkID = blockIdx.x; //Fixed
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, col_element_index;
    double total;
    col_element_index = threadIdx.x * workload; // Index of first element

    i = threadIdx.x;
    while(i < n)
        {
        data[i] = d_samples[i * dim + blkID];
        i = i + num_of_threads;
        }

    __syncthreads();

    i = 0;
    while(i < workload)
        {
        if(col_element_index >= dim)
            break;
        else
            {
            //printf("(%d, %d)\n", blkID, row_element_index);
            total = 0;
            j = 0;
            while(j < n)
                {
                total = total + (d_samples[j * dim + col_element_index] - d_mean[col_element_index]) * (data[j] - d_mean[blkID]);
                ++j;
                }
            d_scatter[col_element_index * dim + blkID] = total;
            }
        ++col_element_index;
        ++i;
        }
    }

__global__ static void kernel_compute_scatter_matrix_interleve_col_maj(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    int blkID = blockIdx.x; //Fixed
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, col_element_index;
    double total;
    col_element_index = threadIdx.x * workload; // Index of first element

    i = threadIdx.x;
    while(i < dim)
        {
        j = 0;
        total = 0;
        while(j < n)
            {
            total = total + (d_samples[j * dim + i] - d_mean[i]) * (d_samples[j * dim + blkID] - d_mean[blkID]);
            ++j;
            }
        d_scatter[i * dim + blkID] = total;
        i = i + num_of_threads;
        }
    }

__global__ static void kernel_compute_scatter_matrix_interleve_col_maj_memory_sharing(double *d_scatter, const double *d_mean, const double *d_samples, const int workload, const int dim, const int n)
    {
    extern __shared__ double data[];
    int blkID = blockIdx.x; //Fixed
    int trdID = blkID * blockDim.x + threadIdx.x;
    int i, j, col_element_index;
    double total;
    col_element_index = threadIdx.x * workload; // Index of first element

    i = threadIdx.x;
    while(i < n)
        {
        data[i] = d_samples[i * dim + blkID];
        i = i + num_of_threads;
        }

    __syncthreads();

    i = threadIdx.x;
    while(i < dim)
        {
        j = 0;
        total = 0;
        while(j < n)
            {
            total = total + (d_samples[j * dim + i] - d_mean[i]) * (data[j] - d_mean[blkID]);
            ++j;
            }
        d_scatter[i * dim + blkID] = total;
        i = i + num_of_threads;
        }
    }

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv)
    {
    runTest( argc, argv);
	//system("PAUSE");
    return EXIT_SUCCESS;
    }

////////////////////////////////////////////////////////////////////////////////
//! Run a simple test
////////////////////////////////////////////////////////////////////////////////
void runTest( int argc, char** argv)
    {
    cudaError_t err = cudaSuccess;
    int n,dim;  //n = number   dim = dimension   command:./exe n dim
    int num_of_blocks, workload;
	double *samples, *scatter, *mean, *eigenvalue, *eigenvector, *wmatrix, *result;
    double *d_samples, *d_scatter, *d_mean;

    clock_gettime(CLOCK_REALTIME, &start_time);
	
    if (argc == 3)
        {
		n = atoi(argv[1]);
		dim = atoi(argv[2]);
        }
    else
        {
    	n=40;
		dim=3;
        }

	samples = new double [n * dim];

	if (!samples)
		fprintf(stderr, "error: can not allocate memory");


	//initialize random seed
    srand ( 7 );

    for( int i=0; i< n ; i++)
        {
		for(int j=0; j< dim ;j++)
            samples[i*dim + j] = rand() % 20 + 1;
        }

    //calculate the mean of each column 
    mean = new double [dim];

    for(int i=0; i < dim ;i++)
        mean[i]=0.0;

    for( int i=0; i< n ; i++)
        {
		for(int j=0; j< dim ;j++)
            mean[j] = mean[j] + samples[i*dim + j];
        }

	for(int i=0; i < dim ;i++)
        mean[i] = mean[i] / n;

    scatter = new double [dim*dim];// compute Scatter Matrix

    clock_gettime(CLOCK_REALTIME, &compute_scatter);

    err = cudaMalloc(&d_scatter, sizeof(double) * dim * dim);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaMalloc(&d_samples, sizeof(double) * n * dim);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaMalloc(&d_mean, sizeof(double) * dim);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }

    err = cudaMemcpy(d_samples, samples, n * dim * sizeof(double),cudaMemcpyHostToDevice);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaMemcpy(d_mean, mean, dim * sizeof(double),cudaMemcpyHostToDevice);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }

    num_of_blocks = dim;
    workload = (dim-1)/128 + 1;
    //kernel_compute_scatter_matrix_seq<<<num_of_blocks, num_of_threads>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_seq_memory_sharing<<<num_of_blocks, num_of_threads, sizeof(double) * n>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_interleve<<<num_of_blocks, num_of_threads>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    kernel_compute_scatter_matrix_interleve_memory_sharing<<<num_of_blocks, num_of_threads, sizeof(double) * n>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_seq_col_maj<<<num_of_blocks, num_of_threads>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_seq_col_maj_memory_sharing<<<num_of_blocks, num_of_threads, sizeof(double) * n>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_interleve_col_maj<<<num_of_blocks, num_of_threads>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    //kernel_compute_scatter_matrix_interleve_col_maj_memory_sharing<<<num_of_blocks, num_of_threads, sizeof(double) * n>>>(d_scatter, d_mean, d_samples, workload, dim, n);
    err = cudaGetLastError();
    if (err != cudaSuccess)
        {
        printf("Fail to lauch the kernel. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }

    err = cudaMemcpy(scatter, d_scatter, dim * dim * sizeof(double), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaFree(d_scatter);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaFree(d_samples);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    err = cudaFree(d_mean);
    if (err != cudaSuccess)
        {
        printf("Fail to malloc &ac. Error code: %s. \n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
        }
    //check
    /*cout << endl << "********** Scatter Matrix **********" << endl;
	for( int i=0; i< dim ; i++)
        {
		for(int j=0; j< dim ;j++)
            {
            cout << scatter[i*dim+j]<<"  ";
            }
        cout << endl;
	    }*/

	eigenvalue = new double [dim];
	eigenvector = new double [dim*dim];

	clock_gettime(CLOCK_REALTIME, &before_jacobi);
    //cout << "Before Jacobi" << endl;
    Jacobi(scatter, dim, eigenvector, eigenvalue, 0.01, 20); // compute eigenvectors + eigenvalues + sort
    //cout << "After Jacobi" << endl;
    clock_gettime(CLOCK_REALTIME, &after_jacobi);
    //check
    /*cout << endl << "********** Eigenvector **********" << endl;
	for( int i=0; i< dim ; i++)
        {
		for(int j=0; j< dim ;j++)
            {
            cout << eigenvector[i*dim+j] << "  ";
            }
        cout << endl;
        }

    cout << endl << "********** Eigenvalue **********" << endl;
	for( int i=0; i< dim ; i++)
        {
		cout << eigenvalue[i] << "  ";
        }*/

	wmatrix = new double [2*dim];   // find the biggest 2 eigenvectors

	for(int i=0 ; i<dim ; i++)
        {
		wmatrix[i]=eigenvector[i*dim];
		wmatrix[dim+i]=eigenvector[i*dim+1];
        }

    result = new double [2*n];  //result

	for(int i=0 ; i<n ; i++)
        {
		result[i]=0;
		result[i+n]=0;

		for(int j=0 ; j<dim ;j++)
            {
			result[i]+= wmatrix[j]*samples[i*dim + j];
			result[i+n]+= wmatrix[j+dim]*samples[i*dim + j];
            }
		//cout<<result[i]<<" "<<result[i+n]<<endl;    //check
        }

    clock_gettime(CLOCK_REALTIME, &end_time);


    printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n", start_time.tv_sec, start_time.tv_nsec);
    printf("e_time.tv_sec:%ld, e_time.tv_nsec:%09ld\n", end_time.tv_sec, end_time.tv_nsec);
    if(end_time.tv_nsec > start_time.tv_nsec)
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        end_time.tv_sec - start_time.tv_sec,
        end_time.tv_nsec - start_time.tv_nsec);
        }
    else
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        end_time.tv_sec - start_time.tv_sec - 1,
        end_time.tv_nsec - start_time.tv_nsec + 1000*1000*1000);
        }
    
    cout << endl << "Time breakdown: " << endl;
    cout << "========== Front Part ==========" << endl;
    printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n", start_time.tv_sec, start_time.tv_nsec);
    printf("e_time.tv_sec:%ld, e_time.tv_nsec:%09ld\n", compute_scatter.tv_sec, compute_scatter.tv_nsec);
    if(compute_scatter.tv_nsec > start_time.tv_nsec)
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        compute_scatter.tv_sec - start_time.tv_sec,
        compute_scatter.tv_nsec - start_time.tv_nsec);
        }
    else
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        compute_scatter.tv_sec - start_time.tv_sec - 1,
        compute_scatter.tv_nsec - start_time.tv_nsec + 1000*1000*1000);
        }

    cout << endl << "========== Computing Scatter Matrix ==========" << endl;
    printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n", compute_scatter.tv_sec, compute_scatter.tv_nsec);
    printf("e_time.tv_sec:%ld, e_time.tv_nsec:%09ld\n", before_jacobi.tv_sec, before_jacobi.tv_nsec);
    if(before_jacobi.tv_nsec > compute_scatter.tv_nsec)
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        before_jacobi.tv_sec - compute_scatter.tv_sec,
        before_jacobi.tv_nsec - compute_scatter.tv_nsec);
        }
    else
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        before_jacobi.tv_sec - compute_scatter.tv_sec - 1,
        before_jacobi.tv_nsec - compute_scatter.tv_nsec + 1000*1000*1000);
        }

    cout << endl << "========== Jacobi ==========" << endl;
    printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n", before_jacobi.tv_sec, before_jacobi.tv_nsec);
    printf("e_time.tv_sec:%ld, e_time.tv_nsec:%09ld\n", after_jacobi.tv_sec, after_jacobi.tv_nsec);
    if(after_jacobi.tv_nsec > before_jacobi.tv_nsec)
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        after_jacobi.tv_sec - before_jacobi.tv_sec,
        after_jacobi.tv_nsec - before_jacobi.tv_nsec);
        }
    else
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        after_jacobi.tv_sec - before_jacobi.tv_sec - 1,
        after_jacobi.tv_nsec - before_jacobi.tv_nsec + 1000*1000*1000);
        }

    cout << endl << "========== After Jacobi ==========" << endl;
    printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n", after_jacobi.tv_sec, after_jacobi.tv_nsec);
    printf("e_time.tv_sec:%ld, e_time.tv_nsec:%09ld\n", end_time.tv_sec, end_time.tv_nsec);
    if(end_time.tv_nsec > after_jacobi.tv_nsec)
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        end_time.tv_sec - after_jacobi.tv_sec,
        end_time.tv_nsec - after_jacobi.tv_nsec);
        }
    else
        {
        printf("[diff_time:%ld.%09ld sec]\n",
        end_time.tv_sec - after_jacobi.tv_sec - 1,
        end_time.tv_nsec - after_jacobi.tv_nsec + 1000*1000*1000);
        }

    cout << endl << "Waiting for file output......" << endl;
    
    ofstream fout;
    fout.open("result_CUDA.txt");

    for(int i=0 ; i < 2 * n ; i++)
        {
        fout << result[i] << " ";    //check
        }

    fout.close();
  //

#define TRACEBACK
#ifdef TRACEBACK

/*	FILE *fpo = fopen("result.txt","w");
	fprintf(fpo, "print traceback value GPU:\n");

	for (int i = max_cols- 2,  j = max_cols - 2; i>=0, j>=0;){
		int nw, n, w, traceback;
		if ( i == max_cols - 2 && j == max_cols - 2 )
			fprintf(fpo, "%d ", input_itemsets[ i * max_cols + j]); //print the first element
		if ( i == 0 && j == 0 )
           break;
		if ( i > 0 && j > 0 ){
			nw = input_itemsets[(i - 1) * max_cols + j - 1];
		    w  = input_itemsets[ i * max_cols + j - 1 ];
            n  = input_itemsets[(i - 1) * max_cols + j];
		}
		else if ( i == 0 ){
		    nw = n = LIMIT;
		    w  = input_itemsets[ i * max_cols + j - 1 ];
		}
		else if ( j == 0 ){
		    nw = w = LIMIT;
            n  = input_itemsets[(i - 1) * max_cols + j];
		}
		else{
		}

		//traceback = maximum(nw, w, n);
		int new_nw, new_w, new_n;
		new_nw = nw + referrence[i * max_cols + j];
		new_w = w - penalty;
		new_n = n - penalty;

		traceback = maximum(new_nw, new_w, new_n);
		if(traceback == new_nw)
			traceback = nw;
		if(traceback == new_w)
			traceback = w;
		if(traceback == new_n)
            traceback = n;

		fprintf(fpo, "%d ", traceback);

		if(traceback == nw )
		{i--; j--; continue;}

        else if(traceback == w )
		{j--; continue;}

        else if(traceback == n )
		{i--; continue;}

		else
		;
	}

	fclose(fpo);
*/
#endif

	delete scatter,eigenvalue,eigenvector,wmatrix,result,mean;


}

