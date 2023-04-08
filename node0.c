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

bool neighborNodes[MAX_NODES];

void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr);
int minOfRow0(int rowNum);
void sendMessage0(struct RoutePacket packet);

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
    // printf("node 0 initial time: %f\n", clocktime);
    if (TraceLevel == 1)
        printf("\nAt time t=%.3f, rtinit0() called.\n", clocktime);

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
    //bool whichNodes[MAX_NODES];

    // Build arrays for determining neighbor nodes and minimal costs
    for (int i = 0; i < MAX_NODES; i++)
    {
        info_dt0.mincost[i] = neighbor0->NodeCosts[i];
        if (info_dt0.mincost[i] != INFINITY && info_dt0.mincost[i] > 0)
            neighborNodes[i] = true;
        else
            neighborNodes[i] = false;
    }

    // Start sending packets to immediate neighbors
    sendMessage0(info_dt0);
    return;
}

void rtupdate0(struct RoutePacket *rcvdpkt)
{
    if (TraceLevel == 1)
    {
        printf("\nAt time t=%.3f, rtupdate0() called.\n", clocktime);
        printf("Recieved a routing packet from node %i.\n", rcvdpkt->sourceid);
    }

    //printf("Source ID: %i\nDest ID: %i\n", rcvdpkt->sourceid, rcvdpkt->destid);
    //for (int i = 0; i < MAX_NODES; i++)
    //{
    //    printf("%i ", rcvdpkt->mincost[i]);
    //}
    //printf("\n\n");

    // Determines which nodes must be updated
    // This may need to be changed!!!! Since we want ALL the nodes to update when sent packets
    bool whichNodes[MAX_NODES];
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (rcvdpkt->mincost[i] != INFINITY && rcvdpkt->mincost[i] != 0)
            whichNodes[i] = true;
        else
            whichNodes[i] = false;
    }

    // Update distance table
    int checkVal;
    int shortPath;
    bool wasChanged = false;
    for (int dest_node = 0; dest_node < MAX_NODES; dest_node++)
    {
        for (int via = 0; via < MAX_NODES; via++)
        {
            //shortPath = minOfRow(via); 
            shortPath = minOfRow0(dest_node);
            checkVal = dt0.costs[via][via] + rcvdpkt->mincost[via] + rcvdpkt->mincost[dest_node];
            //if (rcvdpkt->sourceid != via || (via > dest_node && rcvdpkt->sourceid != (via - 1)))
            //{
            //    checkVal = checkVal + rcvdpkt->mincost[dest_node];
            //}
            if (dest_node != via && whichNodes[via] == true || whichNodes[dest_node] == true)
            {
                //checkVal = shortPath + rcvdpkt->mincost[via] + rcvdpkt->mincost[dest_node];
                if (checkVal < dt0.costs[dest_node][via])
                    dt0.costs[dest_node][via] = checkVal;
            }
            else if (dest_node == via && dest_node != NODE0 & dest_node < 4)
            {
                // Update node's minimal costs if there is a shorter path
                if (shortPath < dt0.costs[dest_node][via])
                {
                    dt0.costs[dest_node][via] = shortPath;
                    wasChanged = true;
                }
            }
        }
    }

    if (TraceLevel == 1)
    {
        printf("Distance table for node 0:\n");
        printdt0(NODE0, neighbor0, &dt0);
    }

    // If minimal cost in distance table was updated, send a packet
    struct RoutePacket info_dt0;
    if (wasChanged == true)
    {
        // Build array for determining updated minimal costs
        for (int i = 0; i < MAX_NODES; i++)
        {
            info_dt0.mincost[i] = dt0.costs[i][i];
        }

        info_dt0.sourceid = NODE0;
        sendMessage0(info_dt0);
    }
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

/**
 * @brief Get the minimum cost of a destination node in the distance table given a row/node number
 * @param rowNum
 * @return int
 */
int minOfRow0(int rowNum)
{
    int min = INFINITY;

    for (int i = 0; i < MAX_NODES; i++)
    {
        if (dt0.costs[rowNum][i] < min)
            min = dt0.costs[rowNum][i];
    }

    return min;
}

/**
 * @brief Takes an initialized routing packet and sets its destination before sending it
 * @param packet
 * @return void
 */
void sendMessage0(struct RoutePacket packet)
{
    for (int node_num = 0; node_num < MAX_NODES; node_num++)
    {
        if (neighborNodes[node_num] == true)
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