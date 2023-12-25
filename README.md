# AoC 2023 in C


# TODO
- Logging (https://github.com/rxi/log.c?)
- Unit Tests!
- TUI? (at least command line args/config file)

# openmp

- use n threads

``` 
export OMP_NUM_THREADS = 12
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

# template day

```
pdm run jinja2 -Dday=9 -Dyear=2023 .\res\day_template.jinja2 > ..\src\2023\year23_day09.h
```

- UTF8 issues on windows:

```
$PSDefaultParameterValues['Out-File:Encoding'] = 'utf8'
```
