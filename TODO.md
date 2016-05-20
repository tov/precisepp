 - Figure out how to test it.
 
 - Allow multiple Collectors. One per thread?
 
 - Should we free eagerly (when refcounts go to 0) or only lazily, after 
   tracing? Could this be an option?
