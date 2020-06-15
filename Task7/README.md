Questions:
1. adding a mutex would not work, because 2 mutexes cannot be locked at the same time.

2. m is initialized with 1 so the first philosopher can check the chopsticks.
s is initialized with 0, so the test can unlock it.
If was initialized with 1, the first test() might fail, because a neigbour philospher is already using the chopsticks but since s with intialized with 1, the philoospsher would still be able to pick it up.
