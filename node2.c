////
#include <stdio.h>
#include "dv.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts *neighbor2;

bool neighborNodes2[MAX_NODES];

void printdt2(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr);
int minOfRow2(int rowNum);
void sendMessage2(struct RoutePacket packet);

/* students to write the following two routines, and maybe some others */

void rtinit2()
{
    // test for bo chen
    //  printf("node 2 initial time: %f\n", clocktime);
    if (TraceLevel == 1)
        printf("\nAt time t=%.3f, rtinit2() called.\n", clocktime);

    // Get immediate neighbors' costs for node 0
    neighbor2 = getNeighborCosts(NODE2);

    for (int i = 0; i < MAX_NODES; i++)
    {
        for (int j = 0; j < MAX_NODES; j++)
        {
            if (i == j)
                dt2.costs[i][j] = neighbor2->NodeCosts[j];
            else 
                dt2.costs[i][j] = INFINITY;
        }
    }

    // Outputs node0 distance table if the trace level is 1
    if (TraceLevel == 1)
        printdt2(NODE2, neighbor2, &dt2);

    // Build the packet here
    struct RoutePacket info_dt2;
    info_dt2.sourceid = NODE2;
    
    // Build arrays for determining neighbor nodes and minimal costs
    for (int i = 0; i < MAX_NODES; i++)
    {
        info_dt2.mincost[i] = neighbor2->NodeCosts[i];
        if (info_dt2.mincost[i] != INFINITY && info_dt2.mincost[i] > 0)
            neighborNodes2[i] = true;
        else
            neighborNodes2[i] = false;
    }

    // Start sending packets to immediate neighbors
    sendMessage2(info_dt2);
    return;
}

void rtupdate2(struct RoutePacket *rcvdpkt)
{
    if (TraceLevel == 1)
    {
        printf("\nAt time t=%.3f, rtupdate2() called.\n", clocktime);
        printf("Recieved a routing packet from node %i.\n", rcvdpkt->sourceid);
    }

    // // Determines which nodes must be updated
    // // This may need to be changed!!!! Since we want ALL the nodes to update when sent packets
    // bool whichNodes[MAX_NODES];
    // for (int i = 0; i < MAX_NODES; i++)
    // {
    //     if (rcvdpkt->mincost[i] != INFINITY && rcvdpkt->mincost[i] != 0)
    //         whichNodes[i] = true;
    //     else
    //         whichNodes[i] = false;
    // }

    // // Update distance table
    // int checkVal;
    // int shortPath;
    // bool wasChanged = false;
    // for (int dest_node = 0; dest_node < MAX_NODES; dest_node++)
    // {
    //     for (int via = 0; via < MAX_NODES; via++)
    //     {
    //         shortPath = minOfRow2(dest_node);
    //         checkVal = dt2.costs[via][via] + rcvdpkt->mincost[via] + rcvdpkt->mincost[dest_node];
    //         if (dest_node != via && whichNodes[via] == true || whichNodes[dest_node] == true)
    //         {
    //             if (checkVal < dt2.costs[dest_node][via])
    //                 dt2.costs[dest_node][via] = checkVal;
    //         }
    //         else if (dest_node == via && dest_node != NODE1 & dest_node < 4)
    //         {
    //             if (shortPath < dt2.costs[dest_node][via])
    //             {
    //                 dt2.costs[dest_node][via] = shortPath;
    //                 wasChanged = true;
    //             }
    //         }
    //     }
    // }

    // if (TraceLevel == 1)
    // {
    //     printf("Distance table for node 2:\n");
    //     printdt2(NODE2, neighbor2, &dt2);
    // }

    // // If minimal cost in distance table was updated, send a packet
    // struct RoutePacket info_dt2;
    // if (wasChanged == true)
    // {
    //     // Build array for determining updated minimal costs
    //     for (int i = 0; i < MAX_NODES; i++)
    //     {
    //         info_dt2.mincost[i] = dt2.costs[i][i];
    //     }

    //     info_dt2.sourceid = NODE2;
    //     sendMessage2(info_dt2);
    // }
    // return;
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
void printdt2(int MyNodeNumber, struct NeighborCosts *neighbor,
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
} // End of printdt2

/**
 * @brief Get the minimum cost of a destination node in the distance table given a row/node number
 * @param rowNum
 * @return int
 */
int minOfRow2(int rowNum)
{
    int min = INFINITY;

    for (int i = 0; i < MAX_NODES; i++)
    {
        if (dt2.costs[rowNum][i] < min)
            min = dt2.costs[rowNum][i];
    }

    return min;
}

/**
 * @brief Takes an initialized routing packet and sets its destination before sending it
 * @param packet
 * @return void
 */
void sendMessage2(struct RoutePacket packet)
{
    for (int node_num = 0; node_num < MAX_NODES; node_num++)
    {
        if (neighborNodes2[node_num] == true)
        {
            packet.destid = node_num;
            if (TraceLevel == 1)
            {
                printf("At time t=%.3f, node %i sends packet to node %i with:\t", clocktime, packet.sourceid, packet.destid);
                for (int i = 0; i < 4; i++)
                {
                    printf("%i ", packet.mincost[i]);
                }
                printf("\n");
            }
            toLayer2(packet);
        }
    }
}
