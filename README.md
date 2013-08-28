# sam2cov

This C programm creates coverage files for sam files. The sam format is specified [here](http://samtools.sourceforge.net/SAMv1.pdf), while the output is in [BED format](http://genome.ucsc.edu/FAQ/FAQformat.html#format1).

## Usage

  Usage: sam2cov fai_file sam_file unique_file non_unique_file rum?

## Installation

  git clone https://github.com/khayer/sam2cov.git
  cd sam2cov
  make

## Limitations

* Reads in sam file can't be longer than 5,000 characters.
* Sam files have to be sorted by sequence name.
* Read pairs can't be seperated. This is especially important for multi-mappers.
