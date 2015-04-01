#ifndef fakefeed_h
#define fakefeed_h

#define FF_TRADE_MSG 0x01
#define FF_QUOTE_MSG 0x02 

#define FF_CONTINUE_FEED 0x40
#define FF_HALT_FEED 0x80

typedef struct _CoreMessage {
    char *sym;
    char *exg;
    int sequence;
    char cond[4];
} CoreMessage;

typedef struct _TradeMessage {
    int size;
    int volume;
    double price;
} TradeMessage;

typedef struct _QuoteMessage {
    int asksize;
    int bidsize;
    double askprice;
    double bidprice;
} QuoteMessage;

typedef struct _FeedData {
    int type;
    CoreMessage core;
    union {
        TradeMessage trade;
        QuoteMessage quote;
    } msg;
} FeedData;

/**
 * ProcessFeed is a simple function that populates the statically allocated
 * FeedData struct automatically and provides a pointer to it via a callback
 * function for each message.
 */
void ProcessFeed(int (*) (const FeedData *));

#endif
