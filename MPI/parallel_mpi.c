#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <assert.h>
#include<time.h>

double ep;
double pts[1000][50];
int clusters[1000][1000];
int minpts, dim, num_pts;

double sqrd_dist(int i, int j)
{
    double sum = 0;
    for (int k = 0; k < dim ; k++)
    {
        sum += pow(pts[i][k] - pts[j][k], 2);
    }
    return sqrt(sum);
}


void dfs(int i,int* siz, int* vis)
{
    vis[i] = 1;
    printf("%d ", i + 1);
    for (int a  = 0; a < siz[i] ; a++)
    {
        if (vis[clusters[i][a]] != 1)
            dfs(clusters[i][a],siz,vis);
    }
}

int main(int argc, char **argv)
{
    int numProc, rank, numworkers;
    int source, dest, vals, offset, leftOver, nPerProcess, vals_to_consider;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    numworkers = numProc - 1;
    /*---------------------------- master ----------------------------*/
    if (rank == 0) {
        //printf("Enter the ep distance:");
        //scanf("%lf", &ep);
  	double startTime, endTime;
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
        int siz[num_pts], vis[num_pts];
        startTime = MPI_Wtime();
        /* send matrix data to the worker tasks */
        nPerProcess = (num_pts) / numworkers;
        leftOver = (num_pts) % numworkers;
        offset = 0;
        for (int dest = 1; dest <= numworkers; dest++)
        {
            vals = dest <= leftOver ? nPerProcess + 1 : nPerProcess;
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&vals, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            offset = offset + vals;
        }
        /* wait for results from all worker tasks */
        for (int i = 1; i <= numworkers; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            MPI_Recv(&vals, 1, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            MPI_Recv(&siz[offset], vals, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            MPI_Recv(&vis[offset], vals, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }
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
                dfs(i,siz,vis);
                printf("\n");
            }

        }
        printf("NOISE :");
        offset = 0;
        for (int dest = 1; dest <= numworkers; dest++)
        {
            vals = dest <= leftOver ? nPerProcess + 1 : nPerProcess;
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&vals, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&vis[offset], vals, MPI_INT, dest, 1, MPI_COMM_WORLD);
            offset = offset + vals;
        }
        /* wait for results from all worker tasks */
        for (int i = 1; i <= numworkers; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            MPI_Recv(&vals, 1, MPI_INT, source, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }

        printf("\n");
    endTime = MPI_Wtime();
    printf("%d %lf\n", numProc, endTime - startTime);
    }
    /*---------------------------- worker----------------------------*/
    if (rank > 0) {
        source = 0;
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(&vals, 1, MPI_INT, source, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        int siz[vals], vis[vals];
        for (int i = 0; i < vals; i++)
        {
            siz[i] = 0;
            vis[i] = 0;
        }

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&vals, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&siz, vals, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&vis, vals, MPI_INT, 0, 2, MPI_COMM_WORLD);


        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(&vals, 1, MPI_INT, source, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        int vis_2[vals];
        MPI_Recv(&vis_2, vals, MPI_INT, source, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        for (int i = 0; i < vals; i++)
        {
			if(vis_2[i] == 0)
            	printf("%d ", i + offset + 1);
        }

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&vals, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
