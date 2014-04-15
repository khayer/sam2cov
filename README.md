# sam2cov

This C program creates coverage files for sam files. The sam format is specified [here](http://samtools.sourceforge.net/SAMv1.pdf), while the output is in [BED format ](http://genome.ucsc.edu/FAQ/FAQformat.html#format1).

## Usage

      Usage: sam2cov [OPTIONS] fai_file sam_file
      Options:
        -r  Aligned with RUM? [0/1] Default: 0
        -s  Strand: 1 for fwd, 2 for rev [0/1/2] Default: 0
        -e  Paired end mode [0/1] Default: 1
        -p  Prefix for coverage files. Default: Unique.cov, NU.cov
        -u  Print header for UCSC Genome browser? [0/1] Default: 0
        -h  This helpful message.
        -v  Print Version.

### Example

* For a sam file produced by [STAR](https://code.google.com/p/rna-star/):

    `sam2cov -p coverage_ danRer7.fa.fai test.sam`

* For a sam file produced by [RUM](http://cbil.upenn.edu/RUM/):

    `sam2cov -p coverage_ -r 1 danRer7.fa.fai test.sam`

Note: To create the *fa.fai* use `samtools faidx danRer7.fa`. Samtools can be downloaded [here](http://samtools.sourceforge.net/).

## Installation

    git clone https://github.com/khayer/sam2cov.git
    cd sam2cov
    make

## Requirements

* You will need at least 15G of RAM, depending on the size of the genome.
* You will also need 'cc'.

## Limitations

* Reads in sam file can't be longer than 5,000 characters.
* Sam files produced by other aligners than [STAR](https://code.google.com/p/rna-star/) and [RUM](http://cbil.upenn.edu/RUM/) are currently not supported.
* Sam files have to be sorted by sequence name.
* Read pairs can't be separated. This is especially important for multi-mappers.

## troubleshoot

* Add -std=gnu99 to the makefile if you see this bug:

> sam2cov.c:103: warning: implicit declaration of function ‘getopt’