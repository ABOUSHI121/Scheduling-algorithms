#include <stdio.h>
#define MAX_TIME 200//the time interval
#define QT 5//quantum time for round-robin algorithm



int time;//the present time of CPU
int number_of_process;//count the total number of process to compute the average
double total_wait_time=0;
double total_turnaround_time=0;
double AVG_turnaround_time=0;
double AVG_wait_time=0;
//total number of process for every algorithm to make the gantt chart (not the real number)
int number_of_process_FCFS=0;
int number_of_process_SJF=0;
int number_of_process_SRTF=0;
int number_of_process_RR=0;
int number_of_process_NPP=0;
int number_of_process_PP=0;
int qt=QT;//temporary quantum time




struct process{
    int ID ;
    int arrival_time;
    int burst_time;
    int priority;
    int comes_back_after;
    //flags to indicate where the process is
    int in_ready_Q;
    int in_wait_Q;
    int in_processor;
    //
    int standard_burst_time;
    //how many times the process done
    int done_count;
    //priority*5 that to aging process
    int priority_times5;
};
typedef struct process process;
struct queue{
    //process ID
    int PROC[20];
    //number of process in the queue
    int num_of_process;
};
typedef struct queue queue;
struct heap{
    //process ID
    int PROC[20];
    //process burst time
    int BURST[20];
    //process priority
    int priority[20];
    //process priority*5
    int priority_times5[20];
    //number of process in the heap
    int num_of_process;
};
typedef struct heap heap;



void enqueue( queue *X,int A){
    //enqueue process
    X->PROC[X->num_of_process] = A;
    X->num_of_process++;
}
int dequeue( queue *X){
    //dequeue process(Add the process to the last index of queue)
    int temp=X->PROC[0];
    for (int i = 1; i < X->num_of_process; ++i) {
        X->PROC[i-1]=X->PROC[i];
    }
    X->num_of_process--;
    return temp;
}
int view(queue *X){
    //see the queue top without take it
    return X->PROC[0];
}
int get_min_burst(heap *H){
    //find the process with minimum burst time in the heap
    int temp_ID,temp_index,temp_min;
    temp_min=50;
    for (int i = 0; i < H->num_of_process; ++i) {
        if (H->BURST[i]<temp_min){
            temp_index=i;
            temp_ID=H->PROC[i];
            temp_min=H->BURST[i];
        }
    }
    for (int i = temp_index; i < H->num_of_process; ++i) {
        H->PROC[i]=H->PROC[i+1];
        H->BURST[i]=H->BURST[i+1];
    }
    H->num_of_process--;
    return temp_ID;

}
int view_min_burst(heap *H){
    //view the process with the minimum burst time and give the id without delete it
    int temp_min;
    temp_min=50;
    for (int i = 0; i < H->num_of_process; ++i) {
        if (H->BURST[i]<temp_min){
            temp_min=H->BURST[i];
        }
    }
    return temp_min;
}
int get_min_priority(heap *H){
    //find the process with minimum priority(which is the highest priority) in the heap
    int temp_ID,temp_index,temp_min;
    temp_min=50;
    for (int i = 0; i < H->num_of_process; ++i) {
        if (H->priority[i]<temp_min){
            temp_index=i;
            temp_ID=H->PROC[i];
            temp_min=H->priority[i];
        }
    }
    for (int i = temp_index; i < H->num_of_process; ++i) {
        H->PROC[i]=H->PROC[i+1];
        H->priority[i]=H->priority[i+1];
        H->priority_times5[i]=H->priority_times5[i+1];
    }
    H->num_of_process--;
    return temp_ID;
}
int view_min_priority(heap *H){
    //view the process with minimum priority(which is the highest priority) in the heap and return ID without delete it

    int temp_min;
    temp_min=50;
    for (int i = 0; i < H->num_of_process; ++i) {
        if (H->priority[i]<temp_min){
            temp_min=H->priority[i];
        }
    }
    return temp_min;
}


queue ready_queue;//for FCFS and round_robin
queue waiting_queue;//for all algorithms
heap ready_heap;//for STRF, SJF,NPP and PP




void initial_fill( process P[7]){
//fill the process with the default data
    P[0].ID=1;
    P[0].arrival_time=0;
    P[0].burst_time=10;
    P[0].comes_back_after=0;
    P[0].priority=3;
    P[0].in_ready_Q=0;
    P[0].in_wait_Q=0;

    P[1].ID=2;
    P[1].arrival_time=1;
    P[1].burst_time=8;
    P[1].comes_back_after=0;
    P[1].priority=2;
    P[1].in_ready_Q=0;
    P[1].in_wait_Q=0;

    P[2].ID=3;
    P[2].arrival_time=3;
    P[2].burst_time=14;
    P[2].comes_back_after=0;
    P[2].priority=3;
    P[2].in_ready_Q=0;
    P[2].in_wait_Q=0;

    P[3].ID=4;
    P[3].arrival_time=4;
    P[3].burst_time=7;
    P[3].comes_back_after=0;
    P[3].priority=1;
    P[3].in_ready_Q=0;
    P[3].in_wait_Q=0;

    P[4].ID=5;
    P[4].arrival_time=6;
    P[4].burst_time=5;
    P[4].comes_back_after=0;
    P[4].priority=0;
    P[4].in_ready_Q=0;
    P[4].in_wait_Q=0;

    P[5].ID=6;
    P[5].arrival_time=7;
    P[5].burst_time=4;
    P[5].comes_back_after=0;
    P[5].priority=1;
    P[5].in_ready_Q=0;
    P[5].in_wait_Q=0;

    P[6].ID=7;
    P[6].arrival_time=8;
    P[6].burst_time=6;
    P[6].comes_back_after=0;
    P[6].priority=2;
    P[6].in_ready_Q=0;
    P[6].in_wait_Q=0;

    for (int i = 0; i < 7; ++i) {
        P[i].in_wait_Q=0;
        P[i].in_ready_Q=0;
        P[i].in_processor=0;
        P[i].comes_back_after=0;
        P[i].standard_burst_time=P[i].burst_time;
        P[i].done_count=0;
        P[i].priority_times5=P[i].priority*5;
    }
}



int processor(process *PROC){
    //the function return flag if it is free or not
    //in the processor we decrement the burst time and if the process done we return flag that processor is free
    //if the process done we set the fields of process to the default one
    //we calculate the turn around time and waiting time in this function
    PROC->burst_time--;

    if (PROC->burst_time==0){
        total_turnaround_time+=time+1-PROC->arrival_time;
        if (PROC->ID==1)
            total_wait_time+=time-PROC->arrival_time+2-10-1;
        else if(PROC->ID==2)
            total_wait_time+=time-PROC->arrival_time+2-8-1;
        else if(PROC->ID==3)
            total_wait_time+=time-PROC->arrival_time+2-14-1;
        else if(PROC->ID==4)
            total_wait_time+=time-PROC->arrival_time+2-7-1;
        else if(PROC->ID==5)
            total_wait_time+=time-PROC->arrival_time+2-5-1;
        else if(PROC->ID==6)
            total_wait_time+=time-PROC->arrival_time+2-4-1;
        else if(PROC->ID==7)
            total_wait_time+=time-PROC->arrival_time+2-6-1;
    }

    if (PROC->burst_time>0) {
        PROC->in_processor=1;
        PROC->in_wait_Q=0;
        PROC->in_ready_Q=0;

        return 0;
    }
    else if (PROC->burst_time<=0){

        if (PROC->ID==1){
            PROC->burst_time=10;
            PROC->comes_back_after=2;
            PROC->priority=3;
        }
        else if (PROC->ID==2){
            PROC->burst_time=8;
            PROC->comes_back_after=4;
            PROC->priority=2;
        }
        else if (PROC->ID==3){
            PROC->burst_time=14;
            PROC->comes_back_after=6;
            PROC->priority=3;
        }
        else if (PROC->ID==4){
            PROC->burst_time=7;
            PROC->comes_back_after=8;
            PROC->priority=1;
        }
        else if (PROC->ID==5){
            PROC->burst_time=5;
            PROC->comes_back_after=3;
            PROC->priority=0;
        }
        else if (PROC->ID==6){
            PROC->burst_time=4;
            PROC->comes_back_after=6;
            PROC->priority=1;
        }
        else if (PROC->ID==7){
            PROC->burst_time=6;
            PROC->comes_back_after=9;
            PROC->priority=2;
        }
        PROC->in_ready_Q=0;
        PROC->in_wait_Q=0;
        PROC->arrival_time=time+1;
        PROC->in_processor=0;
        PROC->done_count++;
        PROC->priority_times5=PROC->priority*5;
        qt=5;
        return 1;
    }
    return 1;
}



void FCFS_driver(){
    //reset all variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //arrays to print the gantt chart
    int process_print[100];
    int time_print[100];

    //flag for the processor
    int processor_free=1;

    int current_process_ID=0;

    //array of process
    process P[7];

    //the queues are free
    waiting_queue.num_of_process=0;
    ready_queue.num_of_process=0;

    //fill the array with process with the initial data
    initial_fill(P);

    for (time=0; time <=MAX_TIME; ++time) {//loop for the time (200 in this project)
        for (int j = 0; j < 7; ++j) {//loop to add the process to the waiting queue
            if (P[j].arrival_time<=time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }

        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//loop to transfer the process from the waiting queue to ready queue according to comes_back_after
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                enqueue(&ready_queue, dequeue(&waiting_queue));
            }
        }

        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//decrement the comes_back_after for all the process in the waiting queue
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }

        if(processor_free){//if the processor free then we dequeue a process from the ready queue, and we give it to the processor
            current_process_ID= dequeue(&ready_queue);
            process_print[number_of_process_FCFS]=current_process_ID;
            time_print[number_of_process_FCFS]=time;
            number_of_process_FCFS++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }
        processor_free=processor(&P[current_process_ID-1]);
    }
    number_of_process=0;

    for (int i = 0; i < 7; ++i) {//loop to count how many process done
        number_of_process+=P[i].done_count;
    }

    printf("\n\nFCFS GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <number_of_process_FCFS ; ++i) {//printing process
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <number_of_process_FCFS ; ++i) {//printing time
        printf(" %3d",time_print[i]);
    }
    printf(" %3d",time-1);

    //printing avg turn around and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN FCFS IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN FCFS IS:    %.3lf\n\n\n",AVG_wait_time);
}



void SJF_driver(){
    //reset all variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //arrays to print the gantt chart
    int process_print[100];
    int time_print[100];

    //processor flag
    int processor_free=1;

    int current_process_ID=0;

    //array of process we will use
    process P[7];

    //reset queues
    waiting_queue.num_of_process=0;
    ready_heap.num_of_process=0;


    initial_fill(P);

    for (time=0; time <=MAX_TIME; ++time) {//loop for time(200)

        for (int j = 0; j < 7; ++j) {//loop to add process to waiting queue
            if (P[j].arrival_time<=time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }

        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//array to transfer the process from the waiting queue to the ready heap
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                ready_heap.BURST[ready_heap.num_of_process]= P[view(&waiting_queue)-1].burst_time;
                ready_heap.PROC[ready_heap.num_of_process]=dequeue(&waiting_queue);
                ready_heap.num_of_process++;
            }
        }
        //decrement the comes back after for all process in the waiting queue
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }

        if(processor_free){//if processor is free we give it the process with minimum burst time from the ready heap
            current_process_ID= get_min_burst(&ready_heap);
            process_print[number_of_process_SJF]=current_process_ID;
            time_print[number_of_process_SJF]=time;
            number_of_process_SJF++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
            //printf("(%d)", P[current_process_ID-1].arrival_time );
        }
        processor_free=processor(&P[current_process_ID-1]);

    }
    number_of_process=0;
    for (int i = 0; i < 7; ++i) {//loop to calculate the number of done process
        number_of_process+=P[i].done_count;
    }

    //printing gantt chart
    printf("\n\nSJF GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <number_of_process_SJF ; ++i) {
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <number_of_process_SJF ; ++i) {
        printf(" %3d",time_print[i]);
    }
    printf(" %3d",time-1);

    //calculate and print avg turn around time and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN SJF IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN SJF IS:    %.3lf\n\n\n",AVG_wait_time);
}



void SRTF_driver(){
    //reset all variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //temp to know if the process was replaced
    int temp=0;

    //arrays to print gantt chart
    int process_print[100];
    int time_print[100];

    //flag for processor
    int processor_free=1;

    int current_process_ID=0;

    //array of process
    process P[7];

    //reset queue and heap
    waiting_queue.num_of_process=0;
    ready_heap.num_of_process=0;

    //fill the process with initial data
    initial_fill(P);

    for (time=0; time <=MAX_TIME; ++time) {//loop for the time
        for (int j = 0; j < 7; ++j) {//loop to add the process to waiting queue
            if (P[j].arrival_time<=time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//transfer the process from the waiting queue to the ready heap
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                ready_heap.BURST[ready_heap.num_of_process]= P[view(&waiting_queue)-1].burst_time;
                ready_heap.PROC[ready_heap.num_of_process]=dequeue(&waiting_queue);
                ready_heap.num_of_process++;
            }

        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//decrment the comes back after for all the process in the waiting queue
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }

        if(P[current_process_ID-1].burst_time> view_min_burst(&ready_heap)&&!processor_free&&ready_heap.num_of_process>0){
            //if there is process in the ready heap with lower burst time than the process in CPU we exchange them
            int tempID,tempBURST;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_ready_Q=1;
            P[current_process_ID-1].in_processor=0;
            tempID=P[current_process_ID-1].ID;
            tempBURST=P[current_process_ID-1].burst_time;
            current_process_ID= get_min_burst(&ready_heap);
            ready_heap.PROC[ready_heap.num_of_process]=tempID;
            ready_heap.BURST[ready_heap.num_of_process]=tempBURST;
            ready_heap.num_of_process++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }

        if(processor_free&& ready_heap.num_of_process>0){
            //if the process free we fetch process from the ready heap with the least burst time
            current_process_ID= get_min_burst(&ready_heap);
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }


        processor_free=processor(&P[current_process_ID-1]);


        if (current_process_ID!=temp){
            //if we exchange the process then we edit the gantt chart
            process_print[number_of_process_SRTF]=current_process_ID;
            time_print[number_of_process_SRTF]=time;
            temp=current_process_ID;
            number_of_process_SRTF++;
        }
    }

    number_of_process=0;
    //count the done count
    for (int i = 0; i < 7; ++i) {
        number_of_process+=P[i].done_count;
    }
    //print the gantt chart
    printf("\n\nSRTF GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <=number_of_process_SRTF -1; ++i) {
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <=number_of_process_SRTF-1 ; ++i) {
        printf(" %3d",time_print[i]);
    }
    printf(" %3d",time-1);

    //calculate and print avg turn around time and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN SRTF IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN SRTF IS:    %.3lf\n\n\n",AVG_wait_time);
}



void RR_driver(){//round-robin

    //reset variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //arrays to print the gantt chart
    int process_print[100];
    int time_print[100];

    //flag for the processor
    int processor_free=1;

    int current_process_ID=0;

    //array of process
    process P[7];

    //reset queues
    waiting_queue.num_of_process=0;
    ready_queue.num_of_process=0;
    initial_fill(P);

    for (time=0; time<=MAX_TIME; ++time) {//loop for time
        for (int j = 0; j < 7; ++j) {//add process to the waiting queue
            if (P[j].arrival_time==time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//transfer the process from the waiting queue to ready queue
            //&&  waiting_queue.num_of_process>0
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                enqueue(&ready_queue, dequeue(&waiting_queue));
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//decrement comes back after for all the process in the waiting queue
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }

        if(processor_free){
            //if the process free then dequeue process from the ready queue and add it to the processor
            current_process_ID= dequeue(&ready_queue);
            process_print[number_of_process_RR]=current_process_ID;
            time_print[number_of_process_RR]=time;
            number_of_process_RR++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
            qt=QT;
        }
        if (qt==0){//if the process take the quantum time then return is the ready queue and get new process from the ready queue to the processor
            if (P[current_process_ID-1].burst_time<P[current_process_ID-1].standard_burst_time){
                P[current_process_ID-1].in_ready_Q=1;
                P[current_process_ID-1].in_wait_Q=0;
                P[current_process_ID-1].in_processor=0;
                enqueue(&ready_queue,current_process_ID);
            }
            current_process_ID= dequeue(&ready_queue);
            process_print[number_of_process_RR]=current_process_ID;
            time_print[number_of_process_RR]=time;
            number_of_process_RR++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
            qt=5;
        }
        processor_free=processor(&P[current_process_ID-1]);
        qt--;//decrement quantum time
    }
    number_of_process=0;
    //count the number of done process
    for (int i = 0; i < 7; ++i) {
        number_of_process+=P[i].done_count;
    }

    //printing the gantt chart
    printf("\n\nRR GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <number_of_process_RR -1; ++i) {
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <number_of_process_RR ; ++i) {
        printf(" %3d",time_print[i]);
    }

    //calculating and printing avg turn around time and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN RR IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN RR IS:    %.3lf\n\n\n",AVG_wait_time);
}



void NPP_driver(){
    //reset all variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //arrays to print the gantt chart
    int process_print[100];
    int time_print[100];

    //flag for processor
    int processor_free=1;
    int current_process_ID=0;

    //array of process
    process P[7];

    //reset queue and heap
    waiting_queue.num_of_process=0;
    ready_heap.num_of_process=0;
    initial_fill(P);
    for (time=0; time <=MAX_TIME; ++time) {//loop for time
        for (int j = 0; j < 7; ++j) {//add process to the waiting queue
            if (P[j].arrival_time<=time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }
        for (int i = 0; i < ready_heap.num_of_process; ++i) {//aging process
            //I used in aging variable called priority_times_5, I decrement this variable every cycle and when its become divisible by 5
            // I decrement the real priority
            if (ready_heap.priority_times5[i]>=0) {
                if (ready_heap.priority_times5[i]>0) {
                    ready_heap.priority_times5[i]--;
                    P[ready_heap.PROC[i]-1].priority_times5--;
                }
                if (ready_heap.priority_times5[i]%5==1&&ready_heap.priority[i]>0) {//1 because i so this before transfer the process from the waiting queue
                    P[ready_heap.PROC[i] - 1].priority--;
                    ready_heap.priority[i]--;
                    ready_heap.priority_times5[i]--;
                    P[ready_heap.PROC[i]-1].priority_times5--;
                }
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//transfer the process from the waiting queue to the ready heap
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                ready_heap.priority[ready_heap.num_of_process]= P[view(&waiting_queue)-1].priority;
                ready_heap.priority_times5[ready_heap.num_of_process]= P[view(&waiting_queue)-1].priority_times5;
                ready_heap.PROC[ready_heap.num_of_process]=dequeue(&waiting_queue);
                ready_heap.num_of_process++;
            }
        }
        //decrement the comes after back for all the process in the waiting queue
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }

        if(processor_free){//if process free we fitch instruction from the ready heap with highest priority(lowest as number)
            current_process_ID= get_min_priority(&ready_heap);
            process_print[number_of_process_NPP]=current_process_ID;
            time_print[number_of_process_NPP]=time;
            number_of_process_NPP++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }
        processor_free=processor(&P[current_process_ID-1]);
    }
    number_of_process=0;
    //count the number of done process
    for (int i = 0; i < 7; ++i) {
        number_of_process+=P[i].done_count;
    }
    //printing the gantt chart
    printf("\n\nNPP GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <number_of_process_NPP ; ++i) {
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <number_of_process_NPP ; ++i) {
        printf(" %3d",time_print[i]);
    }
    printf(" %3d",time-1);

    //calculating and printing avg turn around time and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN NPP IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN NPP IS:    %.3lf\n\n\n",AVG_wait_time);
}



void PP_driver(){
    //reset all the variables to zero
    number_of_process_FCFS=0;
    number_of_process_SJF=0;
    number_of_process_SRTF=0;
    number_of_process_RR=0;
    number_of_process_NPP=0;
    number_of_process_PP=0;
    total_wait_time=0;
    total_turnaround_time=0;
    AVG_turnaround_time=0;
    AVG_wait_time=0;

    //temp use to detect if the process was change or not
    int temp=0;

    //arrays to print the gantt chart
    int process_print[100];
    int time_print[100];

    //flag for the processor
    int processor_free=1;
    int current_process_ID=0;

    //array of process
    process P[7];

    //reset queue and heap
    waiting_queue.num_of_process=0;
    ready_heap.num_of_process=0;
    initial_fill(P);

    for (time=0; time <=MAX_TIME; ++time) {//loop for time
        for (int j = 0; j < 7; ++j) {//add the process to the waiting queue
            if (P[j].arrival_time<=time    &&  P[j].in_wait_Q==0   &&  P[j].in_ready_Q==0  &&  P[j].in_processor==0){
                enqueue(&waiting_queue,P[j].ID);
                P[j].in_wait_Q=1;
            }
        }
        for (int i = 0; i < ready_heap.num_of_process; ++i) {//aging process
            //I used in aging variable called priority_times_5, I decrement this variable every cycle and when its become divisible by 5
            // I decrement the real priority
            if (ready_heap.priority_times5[i]>=0) {
                if (ready_heap.priority_times5[i]>0) {
                    ready_heap.priority_times5[i]--;
                    P[ready_heap.PROC[i]-1].priority_times5--;
                }
                if (ready_heap.priority_times5[i]%5==1&&ready_heap.priority[i]>0) {
                    P[ready_heap.PROC[i] - 1].priority--;
                    ready_heap.priority[i]--;
                    ready_heap.priority_times5[i]--;
                    P[ready_heap.PROC[i]-1].priority_times5--;
                }
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//transfer the process from the waiting queue to the ready heap
            if (P[view(&waiting_queue)-1].comes_back_after==0 ) {
                P[view(&waiting_queue)-1].in_wait_Q=0;
                P[view(&waiting_queue)-1].in_ready_Q=1;
                ready_heap.priority[ready_heap.num_of_process]= P[view(&waiting_queue)-1].priority;
                ready_heap.priority_times5[ready_heap.num_of_process]= P[view(&waiting_queue)-1].priority_times5;
                ready_heap.PROC[ready_heap.num_of_process]=dequeue(&waiting_queue);
                ready_heap.num_of_process++;
            }
        }
        for (int i = 0; i < waiting_queue.num_of_process; ++i) {//decrement comes after back for all the process in the waiting haep
            if (P[waiting_queue.PROC[i] - 1].comes_back_after > 0) {
                P[waiting_queue.PROC[i] - 1].comes_back_after--;
            }
        }
        if(P[current_process_ID-1].priority> view_min_priority(&ready_heap)&&!processor_free&&ready_heap.num_of_process>0){
            //if there is a process in the ready heap with higher priority than the one in the process we exchange them
            int tempID,tempPRIORITY,temp_priority_times5;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_ready_Q=1;
            P[current_process_ID-1].in_processor=0;
            tempID=P[current_process_ID-1].ID;
            tempPRIORITY=P[current_process_ID-1].priority;
            temp_priority_times5=P[current_process_ID-1].priority_times5;
            current_process_ID= get_min_priority(&ready_heap);
            ready_heap.PROC[ready_heap.num_of_process]=tempID;
            ready_heap.priority[ready_heap.num_of_process]=tempPRIORITY;
            ready_heap.priority_times5[ready_heap.num_of_process]=temp_priority_times5;
            ready_heap.num_of_process++;
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }

        if(processor_free&& ready_heap.num_of_process>0){
            //if the processor free we fitch a process with highest priority from the ready heap
            current_process_ID= get_min_priority(&ready_heap);
            P[current_process_ID-1].in_ready_Q=0;
            P[current_process_ID-1].in_wait_Q=0;
            P[current_process_ID-1].in_processor=1;
        }


        processor_free=processor(&P[current_process_ID-1]);



        if (current_process_ID!=temp){//if the process was changed then edit the gantt chart
            process_print[number_of_process_PP]=current_process_ID;
            time_print[number_of_process_PP]=time;
            temp=current_process_ID;
            number_of_process_PP++;
        }
    }
    number_of_process=0;
    //count number of done process
    for (int i = 0; i < 7; ++i) {
        number_of_process+=P[i].done_count;
    }

    //printing the gantt chart
    printf("\n\nPP GANTT CHART:\n\n");
    printf("   |");
    for (int i = 0; i <number_of_process_PP; ++i) {
        printf("P%d |",process_print[i]);
    }
    printf("\n");
    for (int i = 0; i <number_of_process_PP ; ++i) {
        printf(" %3d",time_print[i]);
    }
    printf(" %3d",time-1);


    //calculating and printing avg turn around time and avg waiting time
    AVG_turnaround_time=total_turnaround_time/number_of_process;
    AVG_wait_time=total_wait_time/number_of_process;
    printf("\n\n\nTHE AVG TURNAROUND TIME IN PP IS: %.3lf\n",AVG_turnaround_time);
    printf("\n\nTHE AVG WAITING TIME IN PP IS:    %.3lf\n\n\n",AVG_wait_time);
}



int main() {
    int choice=0;//variable to read the input

    while (choice!=7){
        printf("1- FCFS\n2- SJF\n3- SRTF\n4- RR\n5- NPP\n6- PP\n7- EXIT\n");//menu
        scanf("%d",&choice);
        if (choice==1)
            FCFS_driver();
        else if (choice==2)
            SJF_driver();
        else if (choice==3)
            SRTF_driver();
        else if (choice==4)
            RR_driver();
        else if (choice==5)
            NPP_driver();
        else if (choice==6)
            PP_driver();
        else if (choice==7) {
            printf("\n\n\n*****************************************************   Salam !   *****************************************************\n");
            break;
        }
        else
            printf("invalid operation !\n");
    }

    return 0;
}
