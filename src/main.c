////
// feedhandler.c
//
// A simple example of how to implement a feed handler that can be compiled as a shared
// library. It uses a background thread for work to keep the main q process thread responsive
// and then uses a socket to push updates to the main thread when it is ready. Error handling
// is kept to a minimum for demonstration purposes.
//
// @updated: 27/03/2015
// @authors: Mark Rooney
//
// AquaQ Analytics
// kdb+ consultancy, training and support

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// We define two custom thread and socket types (thread_t and socket_t based on the operating
// system that we are based on). 
#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <process.h>

    #define thread_t HANDLE;
    #define socket_t SOCKET;
#else
    #include <pthread.h>
    #include <unistd.h>

    #define thread_t pthread_t; 
    #define socket_t int;
#endif

#define KXVER 3
#include "k.h"
#include "socketpair.h"
#include "fakefeed.h"

thread_t thread;
socket_t sockets[2];

static int FEED_STATE = FF_CONTINUE_FEED;

#define col(name, type) ktn((type), 0)
#define BUFFER_SIZE 81920

K create_trade_schema()
{
    return knk(7, 
        col("sym",KS),
        col("exg",KS),
        col("size",KI),
        col("volume",KI),
        col("sequence",KI),
        col("price",KF),
        col("cond", 0));
}

K create_quote_schema()
{
    return knk(8,
        col("sym",KS),
        col("exg",KS),
        col("asksize",KI),
        col("bidsize",KI),
        col("askprice",KF),
        col("bidprice",KF),
        col("sequence", KI),
        col("cond", 0));
}

void SendToKDB(char *table, K data)
{
    static char buf[BUFFER_SIZE];

    K bytes = b9(-1, data);
    r0(data);
    
    int len = strlen(table);
    
    // write the table name into the buffer as a string first.
    memcpy(buf, (char *) &len, sizeof(int));
    memcpy(&buf[sizeof(int)], table, len);

    // copy the data into the buffer to be sent.
    memcpy(&buf[sizeof(int) + len], (char *) &bytes->n, sizeof(J));
    memcpy(&buf[sizeof(int) + len + sizeof(J)], kG(bytes), (size_t) bytes->n);
    
    send(sockets[0], buf, (int) (sizeof(int) + len + sizeof(J) + bytes->n), 0);
    r0(bytes);
}

static K trades = (K) 0;
static K quotes = (K) 0;

static int ProcessMessage(const FeedData *data)
{
    static int sent = 0;

    K condvec;
 
    if (trades == NULL)  {
        trades = create_trade_schema();
    }

    if (quotes == NULL) {
        quotes = create_quote_schema();
    }

    switch(data->type) {
        case FF_TRADE_MSG:
            js(&kK(trades)[0], ss(data->core.sym));
            js(&kK(trades)[1], ss(data->core.exg));
            ja(&kK(trades)[2], (void *) &data->msg.trade.size);
            ja(&kK(trades)[3], (void *) &data->msg.trade.volume);
            ja(&kK(trades)[4], (void *) &data->core.sequence);
            ja(&kK(trades)[5], (void *) &data->msg.trade.price); 
            
            condvec = ktn(4, KC);
            memcpy(&kC(condvec)[0], &data->core.cond, 4);
            jk(&kK(trades)[6], condvec); 
           
            break;
        case FF_QUOTE_MSG:
            js(&kK(quotes)[0], ss(data->core.sym));
            js(&kK(quotes)[1], ss(data->core.exg));
            ja(&kK(quotes)[2], (void *) &data->msg.quote.asksize);
            ja(&kK(quotes)[3], (void *) &data->msg.quote.bidsize);
            ja(&kK(quotes)[4], (void *) &data->msg.quote.askprice);
            ja(&kK(quotes)[5], (void *) &data->msg.quote.bidprice);
            ja(&kK(quotes)[6], (void *) &data->core.sequence);

            condvec = ktn(4, KC);
            memcpy(&kC(condvec)[0], &data->core.cond, 4);
            jk(&kK(quotes)[7], condvec);

            break;
    }
 
    if (++sent >= 10) {
        SendToKDB("quote", quotes); quotes = NULL;
        SendToKDB("trade", trades); trades = NULL;
        sent = 0;
    }

    return FEED_STATE;
}

void read_bytes(size_t numbytes, char (*buf)[BUFFER_SIZE])
{
    int total = 0;

    do {
        total += recv(sockets[1], &buf[total], (int)(numbytes - total), 0);
    } while (total < numbytes);
}

K recieve_data(I x)
{
    static char buf[BUFFER_SIZE];
    
    read_bytes(sizeof(int), &buf);
    int tnamesize = 0;
    memcpy(&tnamesize, buf, sizeof(int));
   
    read_bytes(tnamesize, &buf);
    buf[tnamesize] = '\0';
    
    K tname = ks(buf);

    read_bytes(sizeof(J), &buf);
    J size = 0;
    memcpy(&size, buf, sizeof(J));

    read_bytes(size, &buf);
    K bytes = ktn(KG, size);
    memcpy(kG(bytes), &buf, (size_t) size);

    K result = k(0, ".u.upd", tname, d9(bytes), (K) 0);
    r0(bytes);

    if (result != 0) {
        r0(result);
    }

    return (K) 0;
}

#ifdef WIN32
    static unsigned __stdcall process_feed(void *ptr)
    {
        ProcessFeed(ProcessMessage);
    }

    static void start_thread()
    {
        thread =  _beginthreadex(NULL, 0, process_feed, NULL, NULL, 0);
    }
#else
    static void *process_feed(void *input)
    {
        ProcessFeed(ProcessMessage);
        m9();
    }

    static void start_thread()
    {
        pthread_create(&thread, NULL, process_feed, (void *)sockets[0]);
    }
#endif

K init(K x)
{
    dumb_socketpair(sockets, 0);
    sd1(sockets[1], recieve_data);
    
    FEED_STATE = FF_CONTINUE_FEED;
    start_thread();

    return (K) 0;
}

K halt(K x)
{
    FEED_STATE = FF_HALT_FEED;

    // if (sockets[0] != -1) close(sockets[0]);
    if (sockets[1] != -1) sd0(sockets[1]);

    return (K) 0;
}
