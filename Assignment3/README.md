# UVic CSC 360 Assignment 3
Student Name: Haidan Liu
Student Number: V00946959

#### Handle instruction from the provided file
When read an instruction with burst time is zero, which represents that the task was just created.
Then we create a newtask, and add it to the task table.
If we read an instruction saying the burst time is -1, then that means the task needs to be terminated. 
There is a int array acting as boolean array to indicate whether the task has completed or not.
If the task has no remaining burst time, we terminate it immediately by setting the corresponding switches array value to one;
if the task still has some burst time to go, then we wait for its current bust to finish and then terminate it. 

#### Enqueue and dequeue operation
There are three queues:the time quantum for queue1 is 2, for queue2 is 4 and for queue3 is 8. The time quantum is measured in ticks.
When the task is assigned with a burst time, it will be enqueued to the queue1 (the highest priority). 
For all task in queue1, after it runs out of the time quantum, it will be moved to queue2 and when a task comes into queue3, it will stay queue3 forever. 
As a result, when a task 'active time' > 2 means it should be scheduled to the second priority queue which is queue2. 
And if a task has'active time' > 6 and it is currently in queue2, then it should be moved to the third queue. 
When a task was selected to run, it will dequeue from its current queue.

#### Pick the right task to run
Check if there is any task in the three queue. Then pick the task with the highest priority. 
Execute the task and adjust the waiting time and the execution time for all task in three queue. 
When calculte the waiting time, if a task has never been in any queue, then it should be detected by the burst time is -2. 
If a task has completed but we have not receive the instruction to terminate it then its waiting time also should not be increased. 

