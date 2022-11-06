#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <omp.h>

double ep;
double pts[1000][50];
int clusters[1000][1000];
int siz[1000];
int minpts, dim, num_pts;
int vis[100000];
double sqrd_dist(int i, int j)
{
    double sum = 0;
    for (int k = 0; k < dim ; k++)
    {
        sum += pow(pts[i][k] - pts[j][k], 2);
    }
    return sqrt(sum);
}


int is_core_node(int i)
{
    if (siz[i] >= minpts - 1 )
        return 1;
    return 0;
}


void get_neighbours()
{
    #pragma omp for
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
}
void dfs(int i)
{
    vis[i] = 1;
    printf("%d ", i + 1);
    #pragma omp for
    for (int a  = 0; a < siz[i] ; a++)
    {
        if (vis[clusters[i][a]] != 1)
            dfs(clusters[i][a]);
    }
}
void dbscan()
{
    get_neighbours();

    int cnt = 0;
    for (int i = 0; i < num_pts; i++)
    {
        if (vis[i] != 1 && siz[i] >= minpts)
        {
            cnt++;
            printf("cluster %d : ", cnt);
            dfs(i);
            printf("\n");
        }

    }
    printf("NOISE :");
    #pragma omp for
    for (int i = 0; i < num_pts; i++)
    {
        if (vis[i] != 1)
            printf("%d ", i + 1);

    }
    printf("\n");
}



int main()
{
    printf("Enter the ep distance:");
    scanf("%lf", &ep);
    if (ep < 0)
    {
        printf("INVALID EPSILON DISTANCE");
        return 0;
    }
    printf("Enter the minimum points:");
    scanf("%d", &minpts);
    if (minpts < 1)
    {
        printf("INVALID MIN PTS");
        return 0;
    }
    printf("Enter the dimesions of the points:");
    scanf("%d", &dim);
    if (dim < 1)
    {
        printf("INVALID DIMENSIONS");
        return 0;
    }
    printf("Enter the number of points:");
    scanf("%d", &num_pts);
    if (num_pts < 1)
    {
        printf("INVALID NUMBER OF PTS");
        return 0;
    }
    printf("Enter points:");
    for (int i = 0 ; i < num_pts; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            scanf("%lf", &pts[i][j]);
        }
    }

    int threads[] = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128};
    int i = 0, j = 0, k = 0;
    for (int thread = 0; thread < 12; thread++)
    {
        omp_set_num_threads(threads[thread]);
        float startTime = omp_get_wtime();
        #pragma omp for
        for (int i = 0; i < num_pts; i++)
        {
            siz[i] = 0;
            vis[i] = 0;
        }
        dbscan();
        float endTime = omp_get_wtime();
        float execTime = endTime - startTime;
        printf("\n threads: %d -----> time taken : %f\n\n", threads[thread], execTime);
    }
    return 0;
}