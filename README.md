# Mitch Compression Algorithm
## Preface
This compression algoithm is written in C with only standard libraries and is intended for use on text files only. I started to learn C in January of 2017 and this is my attempt at a, not very useful, compression algorithm. On average the compression size of the algorithm is around 70% - 85% of the regular file size. I know this is not the most optimized, logical or a realistic way to compress data but I wanted to try and make a text file compression algorithm.
## The Algorithm
The program has 3 important information bytes when it is compressing files:

| Information Bits | Description |
|:----------------:|:-----------:|
| Standard      | 3 Bytes. Store the information that is used to uncompress the file. For a more detailed explanation of the three bytes see below and figure 1.1 | 
| Individual      | 1 Byte. An individual information byte is denoted with the check bit being 0. That means the program will just interpret the byte as an actual char and not something that has to be processed | 
| Special | 1 Byte. Is denoted by the first byte having the FIRST and THIRD bit from the left set to 1. This means that the following byte of information is to be interpretted as it is written without any processing. | 

![alt tag](https://cloud.githubusercontent.com/assets/8935913/23272492/130c96a6-f9c1-11e6-9713-81ed332a029f.png)
###### Figure 1.1 - The Information Bytes
