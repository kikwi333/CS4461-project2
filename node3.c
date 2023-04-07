/////
#include <stdio.h>
#include "dv.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt3;
struct NeighborCosts *neighbor3;

void printdt3(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr);

/* students to write the following two routines, and maybe some others */

void rtinit3()
{
    // test for bo chen
    //   printf("node 3 initial time: %f\n", clocktime);
    if (TraceLevel == 1)
        printf("At time t=%.3f, rtinit3() called.\n", clocktime);

    // Get immediate neighbors' costs for node 0
    neighbor3 = getNeighborCosts(NODE3);

    for (int i = 0; i < MAX_NODES; i++)
    {
        for (int j = 0; j < MAX_NODES; j++)
        {
            if (i == j)
                dt3.costs[i][j] = neighbor3->NodeCosts[j];
            else 
                dt3.costs[i][j] = INFINITY;
        }
    }

    // Outputs node0 distance table if the trace level is 1
    if (TraceLevel == 1)
        printdt3(NODE3, neighbor3, &dt3);

    // Build the packet here
    struct RoutePacket info_dt3;
    info_dt3.sourceid = NODE3;
    bool whichNodes[MAX_NODES];
    
    // Build arrays for determining neighbor nodes and minimal costs
    for (int i = 0; i < MAX_NODES; i++)
    {
        info_dt3.mincost[i] = neighbor3->NodeCosts[i];
        if (info_dt3.mincost[i] != INFINITY && info_dt3.mincost[i] > 0)
            whichNodes[i] = true;
        else
            whichNodes[i] = false;
    }

    // Start sending packets to immediate neighbors
    for (int node_num = 0; node_num < MAX_NODES; node_num++)
    {
        if (whichNodes[node_num] == true)
        {
            info_dt3.destid = node_num;
            if (TraceLevel == 1) {
                printf("At time t=%.3f, node %i sends packet to node %i with:\t", clocktime, info_dt3.sourceid, info_dt3.destid);
                for (int i = 0; i < 4; i++)
                {
                    printf("%i ",info_dt3.mincost[i]);
                }
                printf("\n");
            }
            toLayer2(info_dt3);
        } 
    }
    return;
}

void rtupdate3(struct RoutePacket *rcvdpkt)
{
    if (TraceLevel == 1)
        printf("At time t=%.3f, rtupdate3() called.\n", clocktime);

    printf("Source ID: %i\nDest ID: %i\n", rcvdpkt->sourceid, rcvdpkt->destid);
    for (int i = 0; i < MAX_NODES; i++)
    {
        printf("%i ",rcvdpkt->mincost[i]);
    }
    printf("\n\n");
    return;
}

/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt3(int MyNodeNumber, struct NeighborCosts *neighbor,
              struct distance_table *dtptr)
{
    int i, j;
    int TotalNodes = neighbor->NodesInNetwork; // Total nodes in network
    int NumberOfNeighbors = 0;                 // How many neighbors
    int Neighbors[MAX_NODES];                  // Who are the neighbors

    // Determine our neighbors
    for (i = 0; i < TotalNodes; i++)
    {
        if ((neighbor->NodeCosts[i] != INFINITY) && i != MyNodeNumber)
        {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber);
    for (i = 0; i < NumberOfNeighbors; i++)
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for (i = 0; i < TotalNodes; i++)
    {
        if (i != MyNodeNumber)
        {
            printf("dest %d|", i);
            for (j = 0; j < NumberOfNeighbors; j++)
            {
                printf("  %4d", dtptr->costs[i][Neighbors[j]]);
            }
            printf("\n");
        }
    }
    printf("\n");
} // End of printdt3
