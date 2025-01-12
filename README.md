# rollback
 Single-header rollback util
=====

### Clean C++17 rollback util.

## Requirements:
 - std 17+
 
 
### Usage

## Prepare header-file: 

 1. Download .h file from this repository
 2. Add "runtime_rollback.h" in project
 3. Open header and add your entries in "rollback_type" enum
 4. Include header in code file
 ```
	#include "runtime_rollback.h"
 ```


## Integrate in code:

 1. Invoke ENABLE_ROLLBACK macros with your args (rollback_type::*your_type* - see 2. in 'Prepare header-file')
 ```
	ENABLE_ROLLBACK(rollback_type::*your_type*, float, &value, nullptr);
 ```
 2. In code:
 
	change
 ```
	value = 1.0f;
 ```
	to
 ```
	utils::capture_rollback_state<rollback_type::*your_type*>([&] { value = 1.0f; });
 ```
 3. For rollback use:
 ```
	utils::rollback<rollback_type::*your_type*>();
 ```
 
 
## Use on-rollback event with args:
 1. Invoke ENABLE_ROLLBACK macros with function pointer and args-of
 ```
	ENABLE_ROLLBACK(rollback_type::*your_type*, float, &value, *your_function*, float, bool, const char*);
 ```
 2. For rollback use:
 ```
	utils::rollback<rollback_type::*your_type*>(1.0f, true, "format");
 ```

## Rollback variables (see line 36 in runtime_rollback.h):
	- Size/capacity of rollback states buffer:
 ```
	utils::rollback_states_capacity
 ```
 	- Current rollback states count:
 ```
	utils::rollback_states_capacity
 ```
 
 
 ### mini-FAQ
 
 ## What if last state has been captured with different 'rollback_type'?
  - rollback will be applied to nearest available state (finded by ID).
 ## Can the state buffer be overflowing and cause an error?
  - If rollback states count = capacity - fisrt element in buffer will be removed and new element will be placed back.