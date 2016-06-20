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

## Introduction

Program Kasm is designed for calculation of geomagnetic activity indices *K* on the basis of recorded geomagnetic field components, according to the ASm method.

The program is a console application working under both Windows and UNIX. To compile the program under UNIX, just run `make` inside the source repository.

The Kasm accepts two formats of input data:
* [IAF](http://www.intermagnet.org/data-donnee/formats/iaf-eng.php) (INTERMAGNET archive format) such as the Definitive data format in the [INTERMAGNET DVD/CD-ROM](http://www.intermagnet.org/data-donnee/cdrom/cddata-eng.php),
* [IMFV1.22](http://www.intermagnet.org/data-donnee/formats/imfv122-eng.php) text format.

The *K*-indices calculated with the use of program Kasm can be stored either in the `IAF` files or `*.DKA` text file, depending upon the parameters of program call.

The most important features of the program include the following:
* a possibility of choosing whether the indices are to be calculated from the horizontal components *X* and *Y* or *H* and *D*.
* a possibility of storing the calculated indices onto text files as well as to the `IAF` files.

Moreover, while calling the program with adequate parameters one can obtain additional interesting information, such as:
* a post-script plot representing, for a given day, the filtration of daily variations SR according to the ASm method
* information on differences between the indices calculated by the Kasm program and the indices contained in the input `IAF` files

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
| `K9|0`         |      | K<sub>9</sub> limit K<sub>9</sub>-index in nT, examples: *450*, *700*<br /> 0 means that K<sub>9</sub> stored in 1-min Intermagnet binary file(s) is to be used |
| `smth`         |      | smoothing coefficient for the observatory, default *1.0*
| `H|0|xy`       | `H`  | annual value of *H*-component in nT,<br />K-indices will be calculated from HD elements, e.g. *18720, 8245* |
|                | `0`  | means that *H* stored 1-min Intermagnet binary file(s) is to be used,<br /> K-indices will be calculated from *HD* components |
|                | `xy` | K-indices will be calculated from *XY* components |
| `out`          |      | name of output text file, this name will be supplemented with extensions `'dka'`, `'K'`, `'plo'`, `'ps'`, `'log'` |
| `path`         |      | directory with input data, e.g. `d:\data\binary\`,<br /> must end with backslash |
| `-0|-1|-2|-3 ` | `-0` | K's will be calculated from one day |
|                | `-1` | K's calculated from preceding and current day |
|                | `-2` | K's calculated from current and subsequent day |
|                | `-3` | K's calculated from preceding, current and subsequent day |
| `-b`           |      | program expects 1-min Intermagnet binary file(s) |
| `-p`           |      | program creates figure `*.ps`,<br /> this option works under condition that parameter `'days'` is missing |
| `-c`           |      | current K's are compared with previous K's stored in 1-min Intermagnet binary file(s), result of comparison is placed in output text file |
| `-u`           |      | current K's are stored to 1-min Intermagnet binary file(s) |
| `-l`           |      | causes creating `*.log` file containing debug information |

## Examples
The use of the Kasm program is illustrated below on 5 examples of its typical applications. (The syntax works on Windows.)

###  Example 1.
The IAF XYZF (or XYZG or XYZ) files are supplemented with the K-indices calculated from components *X* and *Y*.

### Example 2.
The IAF HDZF files are supplemented with the K-indices calculated from components *H* and *D*.

### Example 3.
The K-indices are calculated from components *H* and *D* on the basis of IAF *XYZF* (or *XYZG* or *XYZ*) files containing previously calculated indices. A comparison of indices calculated by Kasm and those contained in IAF files.

### Example 4.
Calculation of the indices from the Intermagnet text files in the IMFV1.22 format.

**INPUT:**
* The hard disk directory `D:\IMAG\IMF` contains 365 Intermagnet text files `?????99.BEL` written in format IMFV1.22.
* Files ``?????99.BEL`` contain data on components *X*, *Y*, *Z*, and *F*.

**OUTPUT:**
* The user requests that a text file `BEL99K.DKA` containing K-indices,
  similar to the `*.DKA` files from the INTERMAGNET CD-ROM, be created
  in the existing directory `D:\IMAG\REPORT`.
* The indices are to be calculated on the basis of components *X* and *Y*,
  according to the limit K<sub>9</sub>=450 nT specified in the command line.

**COMMAND:**
```
Kasm BEL:01JAN1999:365 450 xy D:\IMAG\REPORT\BEL99K D:\IMAG\IMF\
```

**NOTES:**
* Parameter `450` in the command line means that the value K<sub>9</sub>=450 nT is to be
  used for the K-index calculation.
* Parameter `xy` means that the K-indices are to be calculated from components
  *X* and *Y*, even if the text files IMFV1.22 contained components *H*, *D*, *Z*,
  and *F*.
* Parameter `D:\IMAG\IMF\` determining the path to the text files IMFV1.22
  must be obligatorily terminated by character `\`.
* Parameter `D:\IMAG\REPORT\BEL99K` means that a text file
  `BEL99K.DKA` containing K-indices will be created in directory `D:\IMAG\REPORT`.
  This file can be, for instance, used, upon modification of the header part,
  as a file to be placed on INTERMAGNET CD-ROM.
* For a more correct calculation of „boundary” K-indices for the two periods:

          1999-01-01 00-03 UTC
          1999-12-31 21-24 UTC

  files `DEC3198.BEL` and `JAN0100.BEL` are to be placed in the directory `D:\IMAG\IMF`.

### Example 5.
Creation of a post-script plot representing filtration of daily variations SR according to the ASm method.

**INPUT:**
* The hard disk directory `D:\IMAG\BIN` contains IAF file `BEL99JUL.BIN`.
  The file contains, among other things, the previously calculated K-indices.

**OUTPUT:**
* The user wants to be able to assess in a visual manner the performance
  of the ASm filtration for data of July 8, 1999, and to obtain a comparison
  of currently calculated indices with those contained in the IAF file.
  The indices are to be calculated according to the limit K<sub>9</sub>=450 nT.
* The K-indices are to be calculated from components *H* and *D*
  (in spite of the fact that the IAF files contain components *X*, *Y*, *Z*, *F*);
  the *D*-conversion factor is to be calculated with the use of *H*=19000 nT.
* The user requests that – whenever possible – the data from adjacent day be also used for the index calculation.

**COMMAND:**
```
Kasm BEL:08JUL1999 450 19000 D:\IMAG\REPORT\BELTEST D:\IMAG\BIN\ -3 -b -p
```

**NOTES:**
* Parameter `450` means that K-indices are to be calculated with the use of the
  value K<sub>9</sub>=450 nT given in the command line.
  If K<sub>9</sub>-value from the command line and that read from the IAF file disagree,
  the user will be notified about this fact.
  In that case, the program will calculate indices according to K<sub>9</sub>=450,
  since the data specified in the command line are preferential to those read from the IAF file.
* Parameter `19000` means that the K-indices are to be calculated from components *H* and *D*,
  and the conversion of component *D* into the changes expressed in nT is to be made
  with the use of the *D*-conversion factor calculated on the basis of *H*=19000 nT.
* Parameter `D:\IMAG\BIN\` determining the path to the IAF files must necessarily be terminated with character `\`
* Parameter `-3` means that the K-indices are to be calculated, whenever possible,
  with the use of the data of the adjacent day too. The neglect of the data from the
  adjacent day (e.g., the use of parameter `-0`) would produce greater errors of determining
  the “boundary” indices, i.e., those for the time intervals `00-03` and `21-24`.
* Parameter `-b` informs that the data for the index calculation are in IAF files; in our example, in file `BEL99JUL.BIN`.
* Parameter `-p` is in this case crucial, since it is responsible for the creation of
  postscript file named `BELTEST.PS` in the directory `D:\IMAG\REPORT`.
  The plot in the file `BELTEST.PS` contains, among other things, the following;
  - a plot illustrating the ASm filtration used in the program,
  - the K-indices calculated for each component (*H* and *D*) separately,
  - the currently calculated indices for each 3-hour interval and the indices contained in the IAF file.

To visualize the post-script plot one can use, for instance, a free program available from the internet address http://www.cs.wisc.edu/~ghost/gsview/.
