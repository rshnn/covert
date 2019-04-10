


# notes 


Cache info about dramamine:   
L1 instruction cache:   32768 B total, 8-way, 64 B lines  
L1 data cache:          same   
L3 cache:               4194304 B total, 16-way, 64 B lines, 4 slices  


```bash
% getconf -a | grep CACHE                                                                                  19-03-23 - 16:30:07
LEVEL1_ICACHE_SIZE                 32768
LEVEL1_ICACHE_ASSOC                8
LEVEL1_ICACHE_LINESIZE             64

LEVEL1_DCACHE_SIZE                 32768
LEVEL1_DCACHE_ASSOC                8
LEVEL1_DCACHE_LINESIZE             64

LEVEL2_CACHE_SIZE                  262144
LEVEL2_CACHE_ASSOC                 4
LEVEL2_CACHE_LINESIZE              64

LEVEL3_CACHE_SIZE                  4194304
LEVEL3_CACHE_ASSOC                 16
LEVEL3_CACHE_LINESIZE              64
LEVEL4_CACHE_SIZE                  0
LEVEL4_CACHE_ASSOC                 0
LEVEL4_CACHE_LINESIZE              0
```





    // evict_list.push_back((ADDR_PTR) 10);
    // evict_list.push_back((ADDR_PTR) 11);
    // evict_list.push_back((ADDR_PTR) 12);



    list<ADDR_PTR>::iterator i; 
    for (i=configuration->eviction_list.begin(); 
        i!= configuration->eviction_list.end(); i++)
    {
        cout << *i << endl; 
    }

    for (i=configuration->eviction_list.begin(); 
        i!= configuration->eviction_list.end(); i++)
    {
        cout << *i << endl; 
    }



do not put listeners into subroutines.  messes up timings 


----------------

Notes for readme.pdf

A.  How to use

B.  Description of how a covert channel is established 
    physical layer; 
        using L3 cache as covert channel 
        sender will flush L3 cache for 1.  nothing for 0.
        receiver will probe their L1.   


    protocol stack: 

C.  Which part2 was implemented 
    + accomplished
        * Speed
        * Portability
        * Portability++

D.  Challenges 

+ syncing up sender and receiver 
    * switched between clock() and cycles 
+ putting listeners into a another call stack messed up timings 


E.  Feedback 


