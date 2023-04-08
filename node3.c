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

bool neighborNodes3[MAX_NODES];

void printdt3(int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr);
int minOfRow3(int rowNum);
void sendMessage3(struct RoutePacket packet);

/* students to write the following two routines, and maybe some others */

void rtinit3()
{
    // test for bo chen
    //   printf("node 3 initial time: %f\n", clocktime);
    if (TraceLevel == 1)
        printf("\nAt time t=%.3f, rtinit3() called.\n", clocktime);

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
    
    // Build arrays for determining neighbor nodes and minimal costs
    for (int i = 0; i < MAX_NODES; i++)
    {
        info_dt3.mincost[i] = neighbor3->NodeCosts[i];
        if (info_dt3.mincost[i] != INFINITY && info_dt3.mincost[i] > 0)
            neighborNodes3[i] = true;
        else
            neighborNodes3[i] = false;
    }

    // Start sending packets to immediate neighbors
    sendMessage3(info_dt3);
    return;
}

void rtupdate3(struct RoutePacket *rcvdpkt)
{
    if (TraceLevel == 1)
    {
        printf("\nAt time t=%.3f, rtupdate3() called.\n", clocktime);
        printf("Recieved a routing packet from node %i.\n", rcvdpkt->sourceid);
    }

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
            shortPath = minOfRow3(dest_node);
            checkVal = dt3.costs[via][via] + rcvdpkt->mincost[via]; // + rcvdpkt->mincost[dest_node];
            if (rcvdpkt->mincost[dest_node] == 0 || rcvdpkt->mincost[dest_node] == INFINITY)
                checkVal = checkVal + neighbor3->NodeCosts[via];
            else
                checkVal = checkVal + rcvdpkt->mincost[dest_node];
            
            if (dest_node != via && whichNodes[via] == true || whichNodes[dest_node] == true)
            {
                if (checkVal < dt3.costs[dest_node][via])
                    dt3.costs[dest_node][via] = checkVal;
            }
            else if (dest_node == via && dest_node != NODE1 & dest_node < 4)
            {
                if (shortPath < dt3.costs[dest_node][via])
                {
                    dt3.costs[dest_node][via] = shortPath;
                    wasChanged = true;
                }
            }
        }
    }

    if (TraceLevel == 1)
    {
        printf("Distance table for node 3:\n");
        printdt3(NODE3, neighbor3, &dt3);
    }

    // If minimal cost in distance table was updated, send a packet
    struct RoutePacket info_dt3;
    if (wasChanged == true)
    {
        // Build array for determining updated minimal costs
        for (int i = 0; i < MAX_NODES; i++)
        {
            info_dt3.mincost[i] = dt3.costs[i][i];
        }

        info_dt3.sourceid = NODE3;
        sendMessage3(info_dt3);
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

/**
 * @brief Get the minimum cost of a destination node in the distance table given a row/node number
 * @param rowNum
 * @return int
 */
int minOfRow3(int rowNum)
{
    int min = INFINITY;

    for (int i = 0; i < MAX_NODES; i++)
    {
        if (dt3.costs[rowNum][i] < min)
            min = dt3.costs[rowNum][i];
    }

    return min;
}

/**
 * @brief Takes an initialized routing packet and sets its destination before sending it
 * @param packet
 * @return void
 */
void sendMessage3(struct RoutePacket packet)
{
    for (int node_num = 0; node_num < MAX_NODES; node_num++)
    {
        if (neighborNodes3[node_num] == true)
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
