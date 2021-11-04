### Student name: Haidan Liu
### Student number: V00946959


## Part A The Vaccination Station  

The text file contains all information of people that will get vaccined at the vaccination ceter.

When a person arrives at the registaton desk, he/she will be put in a queue based on the time order. Each person has a service time, which is the time needed to get vaccination done for him/her. There are N stations and only one can dequeue the frist person of the queue. 

A mutex was employed to ensure only one thread can read or write to the thread at the same time. After a person was put into the queue, it should wake up waiting vaccination station thread. When we finish read the file, wake up all the waiting thread.

When a vaccination station was serving a person, the thread had to sleep for the corrsponding time. 


## Part B Meeting up 
Cosplay people meet by group of Boba Fett. The group size is N.  If the parameter meetfirst is passed, then they share the first person's code; otherwise, they share the last person's codword. The group can only leave when all N people arrived. 

The solution followes Pg 41 Reusable barrier solution in the book "LittleBookOfSemaphores". Using a two-phase barrier,the first phase is locked and the second is open. When all N Boba Fett meet together, the Nth Boba Fett lock the second one and unlock the first one. Then make sure every one know the code, lock the first one and unlock the second one. Then release the barrier for next group. 

