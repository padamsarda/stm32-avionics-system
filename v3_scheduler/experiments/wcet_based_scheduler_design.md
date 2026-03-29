	no. of cycles in main while loop:
	
	1.0- with interrupts enabled and initial values not discarded 
	t_min = 37
	t_max = 24229
	
	1.1- with interrupts enabled ,first 1000 values are discarded (warmup)
	t_min = 37
	t_max = 23289
	
	2.0- with interrupts disabled and initial values not discarded 
	t_min = 39
	t_max = 24125
	
	2.1- with interrupts disabled, first 1000 values are discarded (warmup)
	t_min = 39
	t_max = 22845
	
	
	t_mean = 128 cycles

	##Analysis
	
	ISR activity dominates execution variability while baseline execution remains ~constant (128 cycles) and close to the lowest case(~37–39 cycles)
	Neither warm-up discard nor EXTI button have negligible impact
	WCET is driven by other asynchronous interrupt preemption, not computation or user events.
	
	WCET ( worst case execution time) = 24k cycles ≈ 285 µs, add margin , then target period = 500µs , thus timer2 is set to 2khz
	
	##Scheduler Design & Overrun detection

	after this we check for any overruns using the following logic :
	(Overrun = next scheduler tick arrives before current execution finishes)
	if there are more overruns than the OVERRUN_LIMIT then we flag it, which can be further used for developing the state machine
	
	
	if (scheduler_tick)
	{
	    scheduler_tick = 0;
	
	    if (running_flag)
	    {
	        overrun_count++;
	
	        if (overrun_count > OVERRUN_LIMIT)
	        {
	            overrun_flag = 1;
	        }
	    }
	
	    running_flag = 1;
	
	    // tasks
	
	    running_flag = 0;
	}
