## UVic CSC 360 Assignment 4

### General strategy
The struct page table entry is provided by the instructor. Modified the page table entry struct by adding attributes lfu
and clock. The lfu can be used to record the frequency of each page and the clock is used for clock page replacement. 
There is a page table which includes all page table entry and each entry is indexed by the frame.
 
Since the methond used for calculating physical address is the same for different logical address so we dont need to change.
There are four global variables: size of frame, size of memory, page replacement scheme and victim frame. The first two are used for users to pass parameter and calculate the page and offset. The third one decides which algorithm to be used and the 
last one is prepared for FIFO or CLOCK. 

When a page fault happens, if it is written, then performs a swap out and set the dirty bit to false. The amount of swap out 
increases by one. Then bring the new page into the page table and increases the amount of swap in by one.  

### FIFO
The victim frame is set to be the first frame. Then when there is a page fault, we try to find a free frame. If we cannot 
find one, then we choose the oldest one and set the victim frame to the next frame. 

The variable victim frame is used to record the victim frame. It moves one forward every time. 

### LFU
When a page is referenced, its lfu amount will increase one. When a page fault happens, if there is a free frame, then we 
choose that frame, otherwise, we have to pick the page with the least frequency to be the victim frame. If two pages with the
same frequency, then the oldest one will be selected. Because when we put a new page to page table, we look through from index
is 0 to the end, the smaller index means longer time staying in the page table. 
 
The lfu attribute is used to record the amount of a page being referenced. 

### CLOCK
There is a attribute called clock which is used as a reference bit. When a page is put into the page table, its reference bit is set to 1. The front hand points to the oldest page (the first frame), and moving forward to find a free frame. In the 
process, if a page with reference bit is 1, it will set referece bit to zero and move forward. If it meets a page with 
reference bit is 0, then that is the victim frame. The front hand advances one position for next time. 

