# Kasm
Software to calculate geomagnetic activity K indices using the Adaptative Smoothing method

This repository contains the Kasm software written by Krzysztof Nowozynski
```
KAsm version 1.09 (09112015) Copyright (c) 2003-2015, Krzysztof Nowozynski.
```
and distributed via
* http://www.intermagnet.org/publication-software/software-eng.php

The original sources including the Windows binary are available in the [upstream branch](https://github.com/mojca/Kasm/tree/upstream).
The master branch contains minor modifications done by Mojca Miklavec, mainly structural ones (Makefile, file reorganisation etc.)

## Usage

```
> KAsm obs:ddmmmyyyy[:days] {K9|0}[:smth] H|0|xy out [path] [-0|-1|-2|-3] [-b] [-p] [-c] [-u] [-l]
```

| Parameter      |      | Description |
| -------------- | ---- | ----------------------------- |
| `|`            |      | separates permissible values of parameter |
| `[]`           |      | parameter is not obligatory |
| `{}`           |      | encloses list of permissible parameter values |
|                |      | |
| `obs`          |      | 3 char IAGA code, e.g. *HLP* |
| `dd`           |      | day of month (01..31), starting date |
| `mmm`          |      | month (JAN..DEC), starting date |
| `yyyy`         |      | year, starting date, e.g. *2001* |
| `days`         |      | number of days, obligatory, e.g. *365*<br /> *Note:  parameter 'days' is not allowed in case of use of switch `-p`* |
| `K9|0`         |      | K9 limit K9-index in nT, examples: *450*, *700*<br /> 0 means that K9 stored in 1-min Intermagnet binary file(s) is to be used |
| `smth`         |      | smoothing coefficient for the observatory, default *1.0*
| `H|0|xy`       | `H`  | annual value of *H*-component in nT,<br />K-indices will be calculated from HD elements, e.g. *18720, 8245* |
|                | `0`  | means that *H* stored 1-min Intermagnet binary file(s) is to be used,<br /> K-indices will be calculated from *HD* components |
|                | `xy` | K-indices will be calculated from *XY* components |
| `out`          |      | name of output text file, this name will be supplemented with extensions `'dka'`, `'K'`, `'plo'`, `'ps'`, `'log'` |
| `path`         |      | directory with input data, e.g. `d:\\data\\binary\\`,<br /> must end with backslash |
| `-0|-1|-2|-3 ` | `-0` | K's will be calculated from one day |
|                | `-1` | K's calculated from preceding and current day |
|                | `-2` | K's calculated from current and subsequent day |
|                | `-3` | K's calculated from preceding, current and subsequent day |
| `-b`           |      | program expects 1-min Intermagnet binary file(s) |
| `-p`           |      | program creates figure `*.ps`,<br /> this option works under condition that parameter `'days'` is missing |
| `-c`           |      | current K's are compared with previous K's stored in 1-min Intermagnet binary file(s), result of comparison is placed in output text file |
| `-u`           |      | current K's are stored to 1-min Intermagnet binary file(s) |
| `-l`           |      | causes creating `*.log` file containing debug information |
