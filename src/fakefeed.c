#ifdef __cplusplus
extern "C"{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fakefeed.h"
#ifdef __cplusplus
}
#endif

#define arraylen(x) (sizeof(x)/sizeof(*(x)))
#define itemfrom(x) x[rand()%arraylen(x)]

char *syms[5] = { "IBM", "APPL", "GOOG", "MSFT", "YHOO" };
char *exgs[3] = { "CME", "LSE", "BARK" };
char conds[6] = { 'C', 'F', 'I', '@', ' ', 'Z' };

void GenerateCore(FeedData *data)
{
    data->core.sym = itemfrom(syms);
    data->core.exg = itemfrom(exgs);

    for (int i = 0; i < 4; i++)
        data->core.cond[i] = itemfrom(conds);

    data->core.sequence = rand() % 200000;
}

void GenerateQuote(FeedData *data)
{
    data->type = FF_QUOTE_MSG;
    GenerateCore(data);
    
    data->msg.quote.asksize = rand() % 500;
    data->msg.quote.bidsize = rand() % 500;
    data->msg.quote.askprice = rand() % 1000 / 10.0;
    data->msg.quote.bidprice = (data->msg.quote.askprice) - (rand() % 1000 / 500.0);
}

void GenerateTrade(FeedData *data)
{
    data->type = FF_TRADE_MSG;
    GenerateCore(data);

    data->msg.trade.size = rand() % 500;
    data->msg.trade.volume = rand () % 1000;
    data->msg.trade.price = rand() % 5000 / 10.0;
}

void ProcessFeed(int (*fn) (const FeedData *))
{
    static FeedData feedData;
    srand(time(NULL));
   
   do {
        if (rand() % 10) {
            GenerateQuote(&feedData);
        } else {
            GenerateTrade(&feedData);
        }
   } while (fn(&feedData) == FF_CONTINUE_FEED); 
}
