#include<stdio.h>
#include<stdlib.h>
#include<math.h>

double ep;
double pts[1000][50];
int clusters[1000][1000];
int siz[1000];
int minpts,dim,num_pts;
int vis[100000] = {0};

double sqrd_dist(int i,int j)
{
    double sum = 0;
    for(int k = 0; k < dim ; k++)
    {
        sum += pow(pts[i][k] - pts[j][k],2);
    }
    return sqrt(sum);
}


int is_core_node(int i)
{
    if(siz[i] >= minpts-1 )
        return 1;
    return 0;
}


void get_neighbours()
{
    for(int i = 0; i+1 < num_pts; i++)
    {
        for(int j = i+1; j < num_pts; j++)
        {
            if(i == j)
            continue;
            if(sqrd_dist(i,j) <= ep)
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
    printf("%d ",i+1);
    for(int a  = 0; a < siz[i] ; a++)
    {
        if(vis[clusters[i][a]] != 1)
            dfs(clusters[i][a]);
    }
}
void dbscan()
{
    get_neighbours();

    int cnt = 0;
    for(int i = 0; i < num_pts; i++)
    {
        if(vis[i] != 1 && siz[i] >= minpts)
        {
            cnt++;
            printf("cluster %d : ",cnt);
            dfs(i); 
            printf("\n");
        }

    }
    printf("NOISE :");
    for(int i = 0; i < num_pts; i++)
    {
        if(vis[i] != 1)
            printf("%d ",i+1);
            
    }
}



int main()
{
    printf("Enter the ep distance:");
    scanf("%lf",&ep);
    if(ep < 0)
    {
        printf("INVALID EPSILON DISTANCE");
        return 0;
    }
    printf("Enter the minimum points:");
    scanf("%d",&minpts);
    if(minpts < 1)
    {
        printf("INVALID MIN PTS");
        return 0;
    }
    printf("Enter the dimesions of the points:");
    scanf("%d",&dim);
    if(dim < 1)
    {
        printf("INVALID DIMENSIONS");
        return 0;
    }
    printf("Enter the number of points:");
    scanf("%d",&num_pts);
    if(num_pts < 1)
    {
        printf("INVALID NUMBER OF PTS");
        return 0;
    }
    printf("Enter points:");
    for(int i = 0 ; i < num_pts; i++)
    {
        for(int j = 0; j < dim; j++)
        {
            scanf("%lf",&pts[i][j]);
        }
    }
    dbscan();
    return 0;
}