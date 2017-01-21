# stcpp
C++11 wrapper of [state-threads library](http://state-threads.sourceforge.net/)

`state-threads library` is a network preferred co-routines library, it is
written in C, in here a C++11 wrapper is offered, to simply working with
morder C++.

the project is proposed to run under Linux.

make sure the compiled `state-threads library` lib/include files are placed in 
folder `st`.

to run the test, `gtest` is required.

to build the library and test, just:<br>
1. create a folder named `build` in the project root directory.<br>
2. change to the directory `build`.<br>
3. run `cmake .. && make`<br>

typical usage:<br>
```cpp
stcpp::JoinableThread*  threads[COUNT_LOOP];
for(int i = 0; i < COUNT_LOOP; i++){
    threads[i] = new stcpp::JoinableThread(
        [](int a ) {
            stcpp::Thread::usleep(1);
            somework();
            count++;
            stcpp::Thread::exit();
        }, 100 );
}

for(int i = 0; i < COUNT_LOOP; i++){
    threads[i]->join();
}
```
