#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <chrono>

double ep;
double pts[1000][50];
int clusters[1000][1000];
int minpts, dim, num_pts;
__global__
void initialization(int *a, int *b, int N)
{

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
    for (int i = idx; i < N; i += stride)
    {
        a[i] = 0;
        b[i] = 0;
    }
}
__global__
void noise(int *a, int N)
{

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
    for (int i = idx; i < N; i += stride)
    {
        if (a[i] != 1)
            printf("%d ", i + 1);
    }
}
double sqrd_dist(int i, int j)
{
    double sum = 0;
    for (int k = 0; k < dim ; k++)
    {
        sum += pow(pts[i][k] - pts[j][k], 2);
    }
    return sqrt(sum);
}

void dfs(int i, int *siz, int* vis)
{
    vis[i] = 1;
    printf("%d ", i + 1);
    for (int a  = 0; a < siz[i] ; a++)
    {
        if (vis[clusters[i][a]] != 1)
            dfs(clusters[i][a], siz, vis);
    }
}



int main()
{
    //printf("Enter the ep distance:");
    //scanf("%lf", &ep);
    ep = 1;
    if (ep < 0)
    {
        printf("INVALID EPSILON DISTANCE");
        return 0;
    }

    //printf("Enter the minimum points:");
    //scanf("%d", &minpts);
    minpts = 2;
    if (minpts < 1)
    {
        printf("INVALID MIN PTS");
        return 0;
    }
    //printf("Enter the dimesions of the points:");
    //scanf("%d", &dim);
    dim = 3;
    if (dim < 1)
    {
        printf("INVALID DIMENSIONS");
        return 0;
    }
    //printf("Enter the number of points:");
    //scanf("%d", &num_pts);
    num_pts = 53;
    if (num_pts < 1)
    {
        printf("INVALID NUMBER OF PTS");
        return 0;
    }
    //printf("Enter points:");
    for (int i = 0 ; i < num_pts; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            //scanf("%lf", &pts[i][j]);
            pts[i][j] = rand() % 10;
            //printf("%lf ", pts[i][j]);
        }
        //printf("\n");
    }
    int block_thread[9][2] = {{1, 1}, {1, 10}, {1, 20}, {1, 30}, {1, 40},
        {10, 10}, {20, 10}, {1, num_pts}, {num_pts / 8, num_pts}
    };
    for (int thread = 0; thread < 9; thread++)
    {
        int *siz, *vis;
        size_t size = num_pts * sizeof(int);
        cudaMallocManaged(&siz, size);
        size = num_pts * sizeof(int);
        cudaMallocManaged(&vis, size);
        auto start = std::chrono::high_resolution_clock::now();
        initialization <<< block_thread[thread][0], block_thread[thread][1]>>>(siz, vis, num_pts);
        cudaDeviceSynchronize();
        printf("\n");
        for (int i = 0; i < num_pts - 1; i++)
        {
            for (int j = i + 1; j < num_pts; j++)
            {
                if (i == j)
                    continue;
                if (sqrd_dist(i, j) <= ep)
                {
                    clusters[i][siz[i]] = j;
                    clusters[j][siz[j]] = i;
                    siz[i]++;
                    siz[j]++;
                }
            }
        }

        int cnt = 0;
        for (int i = 0; i < num_pts; i++)
        {
            if (vis[i] != 1 && siz[i] >= minpts)
            {
                cnt++;
                printf("cluster %d : ", cnt);
                dfs(i, siz, vis);
                printf("\n");
            }

        }
        printf("NOISE :");
        noise <<< block_thread[thread][0], block_thread[thread][1]>>>(vis , num_pts);
        cudaDeviceSynchronize();
        printf("\n");
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        printf("Exec time : %ld \n", duration.count());
        cudaFree(vis);
        cudaFree(siz);
    }
    return 0;
}