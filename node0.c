/////
#include <stdio.h>
#include "dv.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts *neighbor0;

void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
              struct distance_table *dtptr);

// Part of the assignment asks that you write node4.c and node5.c so that you
// end up with a total of 6 nodes.  But your startoff code only has
// 4 nodes; node0.c, node1.c, node2.c, and node3.c
// So here are the stubs for routines that could appear in
// node4.c and node5.c should you choose to write them.  If you do so,
// you should eliminate the entries here.
void rtinit4() {}
void rtinit5() {}
void rtinit6() {}
void rtinit7() {}
void rtinit8() {}
void rtinit9() {}
void rtupdate4(struct RoutePacket *rcvdpkt) {}
void rtupdate5(struct RoutePacket *rcvdpkt) {}
void rtupdate6(struct RoutePacket *rcvdpkt) {}
void rtupdate7(struct RoutePacket *rcvdpkt) {}
void rtupdate8(struct RoutePacket *rcvdpkt) {}
void rtupdate9(struct RoutePacket *rcvdpkt) {}

/* students to write the following two routines, and maybe some others */

void rtinit0()
{
    //printf("node 0 initial time: %f\n", clocktime);
    if (TraceLevel == 1)
        printf("At time t=%.3f, rtinit0() called.\n", clocktime);

    // Get immediate neighbors' costs for node 0
    neighbor0 = getNeighborCosts(NODE0);

    // for (int i = 0; i < MAX_NODES; i++){
    //     printf("%i\n",neighbor0->NodeCosts[i]);
    // }

    for (int i = 0; i < MAX_NODES; i++)
    {
        for (int j = 0; j < MAX_NODES; j++)
        {
            // if in node 0's row, insert neighbor costs into table
            // if (i == NODE0)
            //     dt0.costs[i][j] = neighbor0->NodeCosts[j];
            // else if (j == NODE0)
            //     dt0.costs[i][j] = neighbor0->NodeCosts[i];
            // else 
            if (i == j)
                // dt0.costs[i][j] = 0;
                dt0.costs[i][j] = neighbor0->NodeCosts[j];
            else 
                dt0.costs[i][j] = INFINITY;

            // printf("%i ", dt0.costs[i][j]);
        }
        // printf("\n");
    }

    // Outputs node0 distance table if the trace level is 1
    if (TraceLevel == 1)
        printdt0(NODE0, neighbor0, &dt0);

    // Print information sent to neighbor nodes
    // Get neighbor node identities
    // For loop to build and send packets
    // Build the packet here
    struct RoutePacket info_dt0;
    info_dt0.sourceid = NODE0;
    bool whichNodes[MAX_NODES];
    
    // Build arrays for determining neighbor nodes and minimal costs
    for (int i = 0; i < MAX_NODES; i++)
    {
        info_dt0.mincost[i] = neighbor0->NodeCosts[i];
        if (info_dt0.mincost[i] != INFINITY && info_dt0.mincost[i] > 0)
            whichNodes[i] = true;
        else
            whichNodes[i] = false;
    }

    // Start sending packets to immediate neighbors
    for (int node_num = 0; node_num < MAX_NODES; node_num++)
    {
        if (whichNodes[node_num] == true)
        {
            info_dt0.destid = node_num;
            if (TraceLevel == 1) {
                printf("At time t=%.3f, node %i sends packet to node %i with:\t", clocktime, info_dt0.sourceid, info_dt0.destid);
                for (int i = 0; i < 4; i++)
                {
                    printf("%i ",info_dt0.mincost[i]);
                }
                printf("\n");
            }
            toLayer2(info_dt0);
        } 
    }
    return;
}

void rtupdate0(struct RoutePacket *rcvdpkt)
{
    if (TraceLevel == 1)
        printf("At time t=%.3f, rtupdate0() called.\n", clocktime);

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
void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
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
} // End of printdt0
