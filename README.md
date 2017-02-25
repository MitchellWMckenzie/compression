# Mitch Compression Algorithm
## Preface
This compression algoithm is written in C with only standard libraries and is intended for use on text files only. I started to learn C in January of 2017 and this is my attempt at a, not very useful, compression algorithm. On average the compression size of the algorithm is around 70% - 85% of the regular file size. I know this is not the most optimized, logical or a realistic way to compress data but I wanted to try and make a text file compression algorithm.

I noticed when looking at a text file in binary that the first 3 bits of information from the left was similar in many of the different bytes of information so this got me thinking about compressing that information.
## The Algorithm
The algorithm is quite simple:
* First it starts to read the file character by character
  * For each character, the program looks at the first three bits of information from the left and save it as a pattern
    * This ignores spaces (This is a special case which will be explained later on).
    * If the pattern of the first three bits match the current pattern, it adds the last 5 bits to the queue to save
      * Within this part, it checks for more compression of the 5 bits passed in (any repeating) and tries to compress it more
    * If the pattern does not match, the current pattern it is looking for is discard and replaced with the new pattern
      * It compresses the information that was previously and starts a new matching process

Details of the Information Bits:

| Information Bits | Description |
|:----------------:|:-----------:|
| Standard      | 3 Bytes. Store the information that is used to uncompress the file. For a more detailed explanation of the three bytes see below and figure 1.1 | 
| Individual      | 1 Byte. An individual information byte is denoted with the check bit being 0. That means the program will just interpret the byte as an actual char and not something that has to be processed | 
| Special | 1 Byte. Is denoted by the first byte having the FIRST and THIRD bit from the left set to 1. This means that the following byte of information is to be interpretted as it is written without any processing. | 

![alt tag](https://cloud.githubusercontent.com/assets/8935913/23272492/130c96a6-f9c1-11e6-9713-81ed332a029f.png)
###### Figure 1.1 - The Information Bytes

## Special Cases
There are some special cases in the compression, such as spaces are marked as a special bit so the compression algorithm is more effective. During decompression the program knows how to decompress spaces and they are returned to a normal space.

## Conclusion
The purpose of the program was to develop my skill in C and work in the Binary and Bit level of data. Overall the project might not be the most optimized but it was a great learning experience
