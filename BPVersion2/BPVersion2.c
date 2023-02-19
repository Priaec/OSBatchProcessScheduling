#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

/* declare global variables including a table structure to hold scheduling information */
/* optional: define a function that finds the maximum of two integers */
struct node
{
    // the unique identifier of the process
    int id;
    // the point in time when the process enters the ready list to be executed by the CPU
    int arrival;
    // A.K.A. Burst Time; The amount of CPU time the process will consume between arrival and departure
    int total_cpu;
    // the amount of CPU time remaining for a process to complete {used for SRT scheduling only}
    int total_remaining;
    // a flag that indicates whether the process has been successfully completed (1) or not (0)
    int done;
    // the time when the process has begun being executed by the CPU
    int start;
    // a flag that indicated whether the process has already begun (1) or not (0) {used for SRT scheduling only}
    int already_started;
    // the time when the process has been completed by the CPU
    int end;
    // the sum of the total CPU time and the waiting time (alternatively: the difference between the end time and the arrival time)
    int turnaround;
} *table = NULL;
typedef struct node table_type;

int number_processes;
// this is just a helper function for minimum not a general solution
int findMinArr(int a[])
{
    int result = INT_MAX;
    int index;
    for (int k = 0; k < number_processes; k++)
    {
        if (a[k] < result && a[k] != 0 && a[k] != -1)
            result = a[k];
    }
    return result;
}
// function to find duplicates in any array i.e. ramaining times (used for FCFS portion in SJF & SRT)
bool findDuplicates(int a[], int x)
{
    int count = 0;
    bool result = false;
    for (int i = 0; i < number_processes; i++)
    {
        if (x == a[i])
            count++;
        if (count > 1)
            return result = true;
    }
    return result;
}
/***************************************************************/
int setTurnaround(int arriv, int endd)
{
    return endd - arriv;
}
void printTable()
{
    int i = 0;
    printf("ID\tArrival\tTotal\tStart\tEnd\tTurnaround\n");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < number_processes; i++)
    {
        printf("%d\t%d\t%d\t%d\t%d\t%d", table[i].id, table[i].arrival, table[i].total_cpu, table[i].start, table[i].end, table[i].turnaround);
        printf("\n");
    }
    return;
}
/***************************************************************/
void parameter()
{
    int num_processes;
    int counter;
    printf("Enter total number of processes: ");
    scanf("%d", &num_processes);
    number_processes = num_processes;
    table = (table_type *)malloc(num_processes * sizeof(table_type));
    for (int i = 0; i < num_processes; i++)
    {
        printf("Enter process id: ");
        scanf("%d", &counter);
        table[counter - 1].id = counter;
        printf("Enter arrival cycle for process P[%d]: ", table[counter - 1].id);
        scanf("%d", &table[counter - 1].arrival);
        printf("Enter total CPU time for process P[%d]: ", table[counter - 1].id);
        scanf("%d", &table[counter - 1].total_cpu);
        table[i].total_remaining = 0;
        table[i].start = 0;
        table[i].end = 0;
        table[i].turnaround = 0;
    }
    printTable();
    return;
}
/***************************************************************/
void fifo() // Needs to be reviewed
{
    /* declare (and initilize when appropriate) local variables */
    int int_max = table[0].arrival;
    int z = 0;
    int current_total_time = 0;
    int temp;
    int arr[number_processes];
    int rem_processes = number_processes;
    for (int i = 0; i < number_processes; i++)
    {
        /* for each process, reset "done" field to 0 */
        // resetting processes to not completed hence starting FIFO algorithm
        table[i].done = 0;
        arr[i] = table[i].arrival;
        //  arrival array or essentially queue
    }
    // sort the arrival times in order
    for (int i = 0; i < number_processes - 1; i++)
    {
        for (int j = i + 1; j < number_processes; j++)
        {
            if (arr[i] > arr[j])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    /* while there are still processes to schedule */
    while (rem_processes > 0)
    {
        int current;
        /* for each process not yet scheduled */
        for (int i = 0; i < number_processes; i++)
        {
            for (int j = 0; j < number_processes; j++)
            {
                if (arr[i] == table[j].arrival && table[j].done == 0)
                {
                    current = j;
                    // we found the index of the next process in line skip to setting portion
                    goto exit;
                }
            }
        }
    exit:
        if (z < table[current].arrival)
        {
            // idle time set
            int temporary = abs(z - table[current].arrival);
            printf("Idle time from %d to %d: total Idle Time: %d seconds\n", z, table[current].arrival, temporary);
            z = table[current].arrival;
        }
        table[current].start = z;
        //  set end time
        z = z + table[current].total_cpu;
        table[current].end = z;
        // set turnaround time
        table[current].turnaround = setTurnaround(table[current].arrival, table[current].end);
        //  set done flag
        table[current].done = 1;
        rem_processes--;
    }
    printTable();
    return;
}
/***************************************************************/
void sjf()
{
    /* declare (and initilize when appropriate) local variables */
    int z = 0;
    int arr[number_processes];
    int turn[number_processes];
    int rem_processes = number_processes;
    int current;
    int minCurrent;
    /* for each process, reset "done" field to 0 */
    for (int i = 0; i < number_processes; i++)
    {
        /* for each process, reset "done" field to 0 */
        table[i].done = 0;
        arr[i] = table[i].arrival;
    }
    while (rem_processes > 0)
    {
        current = -1;
        minCurrent = INT_MAX;
        bool duplicate = false;
        int tally = 0;
        for (int i = 0; i < number_processes; i++)
        {
            // if local time meets with arrival time
            if (z >= arr[i] && table[i].done == 0 && arr[i] != -1) // if we do not hit this condition we go else at bottom
            {
                // add to ready queue
                for (int j = 0; j < number_processes; j++)
                {
                    // turnaround time cannot be zero so we can consider this a null value for turnaround time
                    if (turn[j] == 0)
                    {
                        // place turnaround time in the next open spot, then find the minimum in that array
                        turn[j] = table[i].total_cpu;
                        break;
                    }
                }
                continue;
            }
        }
        minCurrent = findMinArr(turn);
        duplicate = findDuplicates(turn, minCurrent);
        if (duplicate)
        {
            // check arrival times
            int fcfs[number_processes];
            for (int i = 0; i < number_processes; i++)
            {
                if (minCurrent == turn[i])
                    fcfs[i] = arr[i];
                else
                    fcfs[i] = -1;
            }
            minCurrent = findMinArr(fcfs);
            for (int x = 0; x < number_processes; x++)
            {
                if (minCurrent == fcfs[x])
                {
                    current = x;
                    goto exit;
                }
            }
        }
        else
            minCurrent = findMinArr(turn); // we do not have a duplicate remaining time
        for (int x = 0; x < number_processes; x++)
        {
            if (minCurrent == turn[x])
            {
                current = x;
                goto exit;
            }
        }
    exit:
        if (current != -1)
        {
            // we found a process to run! set its parameters!!!!
            table[current].start = z;
            z = z + table[current].total_cpu;
            table[current].end = z;
            table[current].turnaround = setTurnaround(table[current].arrival, table[current].end);
            table[current].done = 1;
            turn[current] = -1;
            arr[current] = -1;
            rem_processes--;
            if (rem_processes == 0) // can delete if we want it doesnt matter
                goto finish;
        }
        else
            z++;
    }
finish:
    printTable();
    return;
}
/***************************************************************/
void srt2()
{
    /*Declare Local Variables*/
    int z = 0;
    int rem_processes = number_processes;
    int arr[number_processes];
    int turn[number_processes];
    /* for each process, reset "done", "total_remaining" and "already_started" fields to 0 */
    for (int i = 0; i < number_processes; i++)
    {
        /* for each process, reset "done" field to 0 */
        table[i].done = 0;
        table[i].total_remaining = 0;
        table[i].already_started = 0;
        arr[i] = table[i].arrival;
    }
    for (int i = 0; i < number_processes; i++)
    {
        table[i].total_remaining = table[i].total_cpu;
    }
    while (rem_processes > 0)
    {
        // Find process who (has shortest burst time) && (process's arrival time is < = z)
        int initial_index;
        for (initial_index = 0; initial_index < number_processes; initial_index++)
        {
            // if process is not completed
            if (table[initial_index].done == 0)
            {
                // if process arrival time has hit
                if (z >= table[initial_index].arrival)
                {
                    break;
                }
            }
        }
        // if the inital index is the shortest remaining time it will continue through the whole foor loop
        int min_CPU_bust_p_i = initial_index;
        for (int i = initial_index + 1; i <= number_processes; i++)
        {
            if (table[i].done == 0 && z >= table[i].arrival)
            {
                // 1: process is not complete 2: table[i].total_cpu < min_CPU_burst_p_i
                if (table[i].total_remaining < table[min_CPU_bust_p_i].total_remaining)
                {
                    // set index to min_CPU_burst_p_i
                    min_CPU_bust_p_i = i;
                }
            }
        }
        // once were out we know we have found the next process to update
        // Update min_CPU_bust_p_i process variables
        // has min_CPU_burst_p_i not started?
        if (table[min_CPU_bust_p_i].already_started == 0)
        {
            table[min_CPU_bust_p_i].start = z;
            table[min_CPU_bust_p_i].already_started = 1;
        }
        // process has already started
        z++;
        table[min_CPU_bust_p_i].total_remaining--;
        if (table[min_CPU_bust_p_i].total_remaining == 0) // process's total remaining time is now 0
        {
            // mark process as done
            table[min_CPU_bust_p_i].end = z;
            table[min_CPU_bust_p_i].done = 1;
            table[min_CPU_bust_p_i].turnaround = setTurnaround(table[min_CPU_bust_p_i].arrival, table[min_CPU_bust_p_i].end);
            rem_processes--;
        }
        // if remaining time is now 0 for that process, we mark that process as completed and continue
    }
    printTable();
    return;
}
/***************************************************************/
void quit()
{
    if (table != NULL)
        table = NULL;
    printf("Quiting program...");
    return;
}
/***************************************************************/
int main()
{
    int choice;
    // while user has not choosen to quit
    while (choice != 5)
    {
        printf("Batch Process Scheduling\n");
        printf("--------------------------------\n");
        printf("1) Enter parameters\n");
        printf("2) Schedule processes with FIFO algorithm\n");
        printf("3) Schedule processes with SJF algorithm\n");
        printf("4) Schedule processes with SRT alrogirthm\n");
        printf("5) Quit program and free memory\n\n");
        printf("Enter selection: ");
        scanf("%d", &choice);
        if (choice == 1)
            parameter();
        else if (choice == 2)
            fifo();
        else if (choice == 3)
            sjf();
        else if (choice == 4)
            srt2();
        else if (choice == 5)
            quit();
    }
    return 1;
}
