# Mitch Compression Algorithm
## Preface
This compression algoithm is written in C with only standard libraries and is intended for use on text files only. I started to learn C in January of 2017 and this is my attempt at a, not very useful, compression algorithm. On average the compression size of the algorithm is around 70% - 85% of the regular file size. I know this is not the most optimized, logical or a realistic way to compress data but I wanted to try and make a text file compression algorithm.
## The Algorithm
The program has 3 important information bytes when it is compressing files:
| Information Byte | Description |
|:----------------:|:-----------:|
| Standard      | right-aligned | 
| Individual      | centered      | 
| Special | are neat      | 
![alt tag](https://cloud.githubusercontent.com/assets/8935913/23272492/130c96a6-f9c1-11e6-9713-81ed332a029f.png)
###### Figure 1.1 - The Information Bytes
