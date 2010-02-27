#ifndef FAKESWITCH_H
#define FAKESWITCH_H

#include <poll.h>

#include "msgbuf.h"

enum test_mode 
{
    MODE_LATENCY, MODE_THROUGHPUT
};

struct fakeswitch 
{
    int id;                             // switch number
    int debug;                          // do we print debug msgs?
    int sock;
    struct msgbuf * inbuf, * outbuf;    // input,output buffers
    enum test_mode mode;                // are we going for latency or throughput?
    int probe_state;                    // if mode=LATENCY, this is a flag: do we have a packet outstanding?
                                        // if mode=THROUGHPUT, this is the number of outstanding probes
    int count;                          // number of response's received
    int ready_to_send;                  // are we ready to start sending packet_in's?
};

/*** Initialize an already allocated fakeswitch
 * Fill in all of the parameters, 
 *  exchange OFP_HELLO, block waiting on features_request
 *  and send features reply
 * @param fs        Pointer to a fakeswitch
 * @param sock      A non-blocking socket already connected to 
 *                          the controller (will be non-blocking on return)
 * @param bufsize   The initial in and out buffer size
 * @param mode      Should we test throughput or latency?
 */
void fakeswitch_init(struct fakeswitch *fs, int sock, int bufsize, int debug, enum test_mode mode);


/*** Set the desired flags for poll()
 * @param fs    Pointer to initalized fakeswitch
 * @param pfd   Pointer to an allocated poll structure
 */
void fakeswitch_set_pollfd(struct fakeswitch *fs, struct pollfd *pfd);

/*** Call back to call on poll()
 *  If POLLOUT and not packet_sent, send a packet, set packet_sent
 *  If POLLIN, read it
 *      if the message is not complete, return
 *      if it's an echo request, then reply
 *      if it's a flow_mod, then incremenet count
 *      else ignore it
 * @param fs    Pointer to initalized fakeswitch
 * @param pfd   Pointer to an allocated poll structure
 */
void fakeswitch_handle_io(struct fakeswitch *fs, const struct pollfd *pfd);

/**** Get and reset count 
 * @param fs    Pointer to initialized fakeswitch
 * @return      Number of flow_mod responses since last call
 */
int fakeswitch_get_count(struct fakeswitch *fs);

#endif
