#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <utils.h>
#include <time.h>
#include <sys/stat.h>
struct haversine_pair
{
    f64 X0, Y0;
    f64 X1, Y1;
};
#include "listing_0068_buffer.cpp"
#include "listing_0069_lookup_json_parser.cpp"


static buffer ReadEntireFile(char *FileName) // From Casey
{
    buffer Result = {};
        
    FILE *File = fopen(FileName, "rb");
    if(File)
    {
        struct stat Stat;
        stat(FileName, &Stat);
        
        Result = AllocateBuffer(Stat.st_size);
        if(Result.Data)
        {
            if(fread(Result.Data, Result.Count, 1, File) != 1)
            {
                fprintf(stderr, "ERROR: Unable to read \"%s\".\n", FileName);
                FreeBuffer(&Result);
            }
        }
        
        fclose(File);
    }
    else
    {
        fprintf(stderr, "ERROR: Unable to open \"%s\".\n", FileName);
    }
    
    return Result;
}

static f64 SumHaversineDistances(u64 PairCount, haversine_pair *Pairs)
{
    f64 Sum = 0;
    
    f64 SumCoef = 1 / (f64)PairCount;
    for(u64 PairIndex = 0; PairIndex < PairCount; ++PairIndex)
    {
        haversine_pair Pair = Pairs[PairIndex];
        f64 EarthRadius = 6372.8;
        f64 Dist = ReferenceHaversine(Pair.X0, Pair.Y0, Pair.X1, Pair.Y1, EarthRadius);
        Sum += SumCoef*Dist;
    }
    
    return Sum;
}


int main(int ArgCount, char **Args){
    int Result = 1;
    if(ArgCount == 2)
    {
        buffer InputJSON = ReadEntireFile(Args[1]);

        u32 MinimumJSONPairEncoding = 6*4;
        u64 MaxPairCount = InputJSON.Count / MinimumJSONPairEncoding;
        // I don't understand what MaxPairCount represents
        if(MaxPairCount)
        {
            buffer ParsedValues = AllocateBuffer(MaxPairCount * sizeof(haversine_pair));
            if(ParsedValues.Count)
            {
                haversine_pair *Pairs = (haversine_pair *)ParsedValues.Data;
                u64 PairCount = ParseHaversinePairs(InputJSON, MaxPairCount, Pairs);
                f64 Sum = SumHaversineDistances(PairCount, Pairs);
                
                fprintf(stdout, "Input size: %llu\n", InputJSON.Count);
                fprintf(stdout, "Pair count: %llu\n", PairCount);
                fprintf(stdout, "Haversine sum: %.16f\n", Sum);
            }
            FreeBuffer(&ParsedValues);
        }
        else
        {
            fprintf(stderr, "ERROR: Malformed input JSON\n");
        }
        FreeBuffer(&InputJSON);
        Result = 0;
    }
    else
    {
        fprintf(stderr, "Usage: %s [haversine_input.json]\n", Args[0]);
        // fprintf(stderr, "       %s [haversine_input.json] [answers.f64]\n", Args[0]);
    }
    
    return 0;
}