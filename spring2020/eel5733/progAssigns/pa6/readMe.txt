# readMe.txt
# Christopher Brant
# Programming Assignment 6
# EEL 5733 Advanced Systems Programming
# University of Florida
# 4/15/20

For Running My Test Program:
	Prerequisites
	- Compiled and loaded the modified device driver program
	- Compiled my testing program
	- Decide on which deadlock scenario you are wanting to test for with my program

	Running
	1) Run "./deadlockTest <deadlock scenario #>"

	* This program will execute a test for one of the following four scenarios. *

	* After describing each of the following four potential deadlock scenarios,
	  a brief description of the actual test procedure itself will be included. *


Deadlock Scenarios:

	Scenario 1) The first scenario I test for is the deadlock possibility when a process running 
				multiple threads is currently running in MODE1, and another process attempts to
				open the device while the driver is in the case E2_IOCMODE2 section.  If the process
				wanting to switch (P1) has to wait for the thread that is attempting to also open the
				device (P2), then P1 will still be holding devc->sem2, and P2 will be unable to acquire
				devc->sem2, but it will acquire devc->sem1 and open and increment devc->count1. So 
				P2 will block until it can acquire devc->sem2, and P1 will block until devc->count1 == 1,
				and since P2 won't release until it has been opened, then this creates a deadlock scenario.

				- The "wait" statements that I'm checking this for are:
					- In P1, Line <#>: wait_event_interruptible(devc->queue1, (devc->count1 == 1));
					- In P2, Line <#>: down_interruptible(&devc->sem2);

		General Test Procedure:
			- This will be tested by forking 2 processes that both attempt to open the driver, one will
			  sleep immediately so the other will be able to open the driver first (P1), then that process
			  will sleep so that the other will be able to attempt to open and be waiting for devc->sem2
			  which it cannot acquire (P2), then P1 will attempt to switch from MODE1 to MODE2 via e2_ioctl,
			  and at that time, a deadlock SHOULD occur.


	Scenario 2) The second scenario I test for is the deadlock possibility when there are two 
				processes running, both in MODE2, and then both of them at the same time attempt
				to switch to MODE1, however with a sleep() statement in the user code so that it
				is ensured that there are two processes open and both are in MODE2 before having 
				both of them then attempt to switch to MODE1.  When they both attempt to do so, 
				the devc->count2 variable will never be able to decrement to equal 0, and therefore
				we should see a deadlock scenario as both of them will wait forever in deadlock.

				- The "wait" statements that I'm checking this for are the same in both processes
				  since both processes will be waiting in the same spot in the same function, and
				  that place is Line <#>: wait_event_interruptible(devc->queue2, (devc->count2 == 0)); 

		General Test Procedure:
			- This will be tested similarly to Scenario 1, as two processes will be forked off and
			  the first process will open the device, but it will then immediately switch to MODE2
			  while the second process sleeps for a moment, and then the second process will open the
			  device and it will open in MODE2.  Then both processes will attempt to switch to MODE1
			  at the same time, and with devc->count2 being more than 0, and having either process
			  not being able to release and decrement devc->count2, a deadlock SHOULD occur.


	Scenario 3) The third scenario I test for is the deadlock possibility when a process is opened in 
				MODE1, then switches to MODE2.  Another process opens in MODE2 attempts to switch to MODE1
				and cannot since devc->count2 is currently equal to 1, so it will wait. Then the first
				process will close the device, but because after it decrements devc->count2 will be equal
				to 0, and will not call the wake_up_interruptable(&dev->queue2);

				- The "wait" statement that I'm checking this one for will be the same as in Scenario 2
				  which is Line <#>: wait_event_interruptible(devc->queue2, (devc->count2 == 0));

		General Test Procedure:
			- This will be tested basically the same as the scenario was given, so sleep statements
			  will be inserted in the first process to open after it has switched to MODE2 so that
			  second process to open will open in MODE2, and it is able to able to attempt to switch
			  to MODE1 first, then the first process will close the device.


	Scenario 4) The last scenario I test for is the deadlock possibility when a process opens the device
				in MODE1



		General Test Procedure:




Race Condition Code Reviews:
	
		* In this section, a series of critical regions of code will be described,
	  	  specifically, the first line numbers and the code on that line will be given
	  	  to indicate the first and last lines of the chosen critical regions, such that
	  	  there is no confusion. *

		* Pairs of the described critical regions will be listed, and once pairs are listed,
	  	  a code review will be given, including listings of the data accessed within that region,
	  	  the locks held at the time the regions are entered, and reasoning for the possibility
	  	  or absence of race conditions in the given critical region pair. *


	Critical Regions (CRs):

		1) In e2_read(), the conditional that executes if in MODE1:
			Beginning with line <#>...
				if (*f_pos + count > ramdisk_size)
	        	{
	            	printk("Trying to read past end of buffer!\n");
	            	return ret;
	        	}
		   		ret = count - copy_to_user(buf, devc->ramdisk, count);
	   		...Ending with line <#>

	   		- Locks held entering this region are:
	   			- devc->sem1 is explicitly NOT held in this region
	   			- Technically devc->sem2 is held at all times when in MODE1,
	   			  however it is not explicitly acquited in this place as it is held
	   			  by the process that opened it at the time of opening

	   		- Shared data accessed during this region:
	   			- *f_pos
	   			- devc->ramdisk

	   	2) In e2_write(), the conditional that executes if in MODE1:
	   		Beginning with line <#>...
		   		if (*f_pos + count > ramdisk_size)
	        	{
	            	printk("Trying to read past end of buffer!\n");
	            	return ret;
	        	}
	        	ret = count - copy_from_user(devc->ramdisk, buf, count);
	   		...Ending with line <#>

	   		- Locks held entering this region are:
	   			- devc->sem1 is explicitly NOT held in this region
	   			- Technically devc->sem2 is held at all times when in MODE1,
	   			  however it is not explicitly acquited in this place as it is held
	   			  by the process that opened it at the time of opening

	   		- Shared data accessed during this region:
	   			- *f_pos
	   			- devc->ramdisk

	   	3) In e2_ioctl(), case E2_IOCMODE1:
	   		Beginning with line <#>...
				if (devc->mode == MODE1)
				{
				   up(&devc->sem1);
				   break;
				}
				if (devc->count2 > 1)
				{
				   while (devc->count2 > 1)
				   {
				       up(&devc->sem1);
				       wait_event_interruptible(devc->queue2, (devc->count2 == 1));
				       down_interruptible(&devc->sem1);
				   }
				}
				devc->mode = MODE1;
				devc->count2--;
				devc->count1++;
				down_interruptible(&devc->sem2);
	   		...Ending with line <#>

	   		- Locks held entering this region are:
	   			- devc->sem1 is held across this region, except when waiting for
	   			  the conditional signal/event
	   			- devc->sem2 is seen to be acquired at the end as part of the sequence

	   		- Shared data accessed during this region:
	   			- devc->mode
	   			- devc->count2
	   			- devc->count1
	   			- &(devc->queue2)  **

	   	4) In e2_open(), basically the entire function:
	   		Beginning with line <#>...
			    if (devc->mode == MODE1)
			    {
			        devc->count1++;
			        up(&devc->sem1);
			        down_interruptible(&devc->sem2);
			        return 0;
			    }
			    else if (devc->mode == MODE2)
			    {
			        devc->count2++;
			    }
	   		...Ending with line <#>

	   		- Locks held entering this region are:
	   			- devc->sem1 is held across the region, until it is released
	   			  right before devc->sem2 is acquired
	   			- devc->sem2 is acquired if in MODE1 right before returning
	   			  from the function

	   		- Shared data accessed during this region:
	   			- devc->mode
	   			- devc->count1
	   			- devc->count2

	   	5) In e2_release(), basically the entire function:
	   		Beginning with line <#>...
			    if (devc->mode == MODE1)
			    {
			        devc->count1--;
			        if (devc->count1 == 1)
			            wake_up_interruptible(&(devc->queue1));
						up(&devc->sem2);
			    }
			    else if (devc->mode == MODE2)
			    {
			        devc->count2--;
			        if (devc->count2 == 1)
			            wake_up_interruptible(&(devc->queue2));
			    }
	   		...Ending with line <#>

	   		- Locks held entering this region are:
	   			- devc->sem1 is held across the region
	   			- devc->sem2 is held and subsequently ends up released right
	   			  before returning from the function if in MODE1

	   		- Shared data accessed during this region:
	   			- devc->mode
	   			- devc->count1
	   			- devc->count2
	   			- &(devc->queue1)  **
	   			- &(devc->queue2)  **

	   		** For these two, while they are shared data, they are just wait queues
	   		   and used for signaling/conditional events


	CR Pairs and their code reviews:

		1) The first pair of critical regions that we are going to look at are CR1 and CR2.
			- For this particular pair of critical regions, I believe there there IS indeed a
			  possibility of race conditions.
			- The reason for this is that since CR1 and CR2 do not hold any locks other than their
			  parent process holding devc->sem2, if that process were to open the driver file first,
			  then create two threads/fork a child process that then attempts to read and write at
			  the same time in MODE1, then there is a possibility for a data race for both the *f_pos
			  AND for the actual data in devc->ramdisk. 

		2) The second pair of critical regions that we are going to look at are CR2 and CR2 again.
			- For this particular pair of critical regions, I believe there there IS indeed a
			  possibility of race conditions.
			- The reason for this is virtually the same as that of the above critical region pair,
			  that since CR2 and CR2 do not hold any locks other than their parent process holding
			  devc->sem2, if that process were to open the driver file first, then create two threads
			  that then attempt to read and write at the same time in MODE1, then there is a possibility
			  for a data race for both the *f_pos AND for the actual data in devc->ramdisk. The only
			  difference is that this pair would be attempting two writes at the same time and not
			  a read and a write.

		3) The third pair of critical regions that we are going to look at are CR3 and CR4.
			- For this particular pair of critical regions, I believe there there is NOT a
			  possibility of race conditions.
			- The reason for this is is that the only shared data that is accessed within these
			  regions is accessed within the portions where devc->sem1 is held, and therefore
			  should not have the possibility for a race condition.  So that even if there is
			  an attempt to open another device while the e2_ioctl function is called, there
			  should be no conflict because the synchronization primitives are correctly placed.

		4) The last pair of critical regions that we are going to look at are CR4 and CR5.
			- For this particular pair of critical regions, I believe there there is NOT a
			  possibility of race conditions.
			- The reason for this is is that the only shared data that is accessed within these
			  regions is accessed within the portions where devc->sem1 is held, and therefore
			  should not have the possibility for a race condition.  So that even in the case that
			  there is an attempt to open and close a driver at the same time, the synchronization
			  primitives prevent any data races, and there should be no errors caused by this.





















