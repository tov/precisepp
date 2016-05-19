 - Figure out how to test it.
 
 - Track heap and allocation statistics.
 - Have it figure out when to collect based on stats.
 
 - Store traced<T>s in a doubly-linked list rather than a set
 - Or: Switch back to arena allocation, using a free-list.
 
 - Allow multiple Collectors. One per thread?
 
 - Should we free eagerly (when refcounts go to 0) or only lazily, after 
   tracing? Could this be an option?
