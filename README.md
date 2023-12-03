
# openmp 
- use n threads
``` 
export OMP_NUM_THREADS = 24
```

# format
```
clang-format -i ./*.c ./*.h
```

# debug

## memory

```
valgrind --tool=memcheck  --leak-check=full --track-origins=yes ./aocc
```