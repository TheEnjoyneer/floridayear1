/* meat of the code is in here so read this file */

#include <capture.h>
#include <buffer.h>

#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

// Daniela
#include <tcp_connection.h>
#include <netinet/tcp.h>

#define PACKETSTOREAD 0
#define MAXPACKETS 262144
#define TESTVAL 65536
#define RATIOTHRESHOLD 3

typedef struct iphdr ip_header;
typedef struct ether_header ethernet_header;
typedef struct tcphdr tcp_header;

static ip_header ipType;
static tcp_header tcpType;

struct packetStats_s {
  typeof(ipType.saddr) srcAddr;
  typeof(ipType.daddr) destAddr;
  typeof(tcpType.th_sport) srcPort;
  long long int connectReqs;
  long long int successResps;
};

int main (int argc, char **argv){
  
  /* variables for network device */
  char errbuf[PCAP_ERRBUF_SIZE]; /* buffer for pcap to send its errors */
  pcap_t *descr; /* file descriptor for the network card */
  
  /* variables for packets we sniff */
  ethernet_header *eptr;                                /* pointer to the structure that represents ethernet header */
  ip_header *ipptr;                                     /* pointer to the structure that represents ip header */
  tcp_header *tcpptr;                                   /* pointer to the structure that represents the tcp header */
  unsigned int size_of_iphdr = sizeof(ip_header);       /* size of the ip header */
  unsigned int size_of_ehdr = sizeof(ethernet_header);  /* size of the ethernet header */
  unsigned int size_of_tcphdr = sizeof(tcp_header);     /* size of tcp header */

  /* variables for the simple buffer, ignore them */
  buffer buf;               /* that's my linked-list */
  item *tmp;                /* an item in the linked-list */
  u_char *ptr, *packet;     /* vars to store raw packets */
  register int i = 0;
  struct in_addr ipaddr_s;    /* you should know this one */
  struct in_addr ipaddr_d;
  struct in_addr arrIPAddr_s;
  struct in_addr arrIPAddr_d;
  struct packetStats_s *packetStats = malloc(sizeof(struct packetStats_s) * TESTVAL);
  long long int statsCount = 0;
  int j, found;
  unsigned long long testRatio;
  
 /* simple sanity check to make sure that the application is used properly */ 
  if (argc != 2){
      fprintf (stderr, "Usage: %s FILENAME\nFILENAME: file name of the trace file to read.\n", argv[0]);
      exit(-1);
  }

  /* remember, everything in unix is a file? so is a pcap file!
   * so all we have to do to sniff is:
   * 1) open the pcap trace file the same as any file, but with pcap_fopen_offline()
   * 2) read the pcap trace file the same as any file (like reading a file see read())
   * 3) display and count all the information in case we need to warn the user of a scan attack
   * and we are done!
   */
  
  /* step 1: open the pcap trace file to read
   * 0) first parameter is the trace file name
   * 1) second parameter is a pointer to the error buffer
   */
  FILE *tracefile = fopen(argv[1], "r");
  if((descr = pcap_fopen_offline(tracefile, errbuf))==NULL){
      fprintf (stderr, "%s: pcap_fopen_offline: %s\n", argv[0], errbuf);
      exit (-1);
  }


  /* so, i'm preparing a linked-list to store the packets i sniff */
  create_buffer(&buf, MAXPACKETS, MAXPACKETS / 2);


  /* hokey dokey, so we are now ready to sniff the wire! */
  /* step-3: read the network card (sniff in short!)
   * those who are not familiar with the concept of callback functions it
   * may look perplexing to pass a function as argument to another function
   * but that's how callback functions work. we will get to that soon.
   * about parameters:
   * 0) file descriptor of the network card so that the function can read it
   * 1) number of packets to sniff before this function stops, so in our case
   * we simply give it the first command line argument to our program. (try running
   * the program with  0 or -1) when this argument is 0 or -1 the function loops forever!
   * 2) third parameter is a pointer to the callback function. so, whenever pcap_loop
   *    reads a packet off the wire it calls the function pointed to by this pointer
   *    with the next argument as the first argument to the callback function. so, if
   *    you start the program with 0 or -1 then this program will gather packets for ever 
   *    and store them in the linked-list until the machine runs out of memory!!
   * 3) fourth parameter is the argument to the callback function
   *
   * pcap_loop returns numbers of packets captured, so i'm just printing it out.
   */
  pcap_loop(descr, PACKETSTOREAD, pcap_callback, (void *)&buf);
 
  /* sniffing is all done, at this point we have some packets in the linked-list so lets
   * go through the list and print some information about our captives
   */

  //fprintf(stdout, "\nDetail information about captured packets\n");
  /* just walk the list one item at a time and print some info about the packet */
  tmp = buf.header;
  while(tmp != NULL)
  {
    // Reset important vals
    found = 0;
    testRatio = 1;
    // fprintf(stdout, "actual length=%d captured length=%d\n",
		  //   tmp->packet_header->len,
		  //   tmp->packet_header->caplen);

    /* here we want to access the ethernet header of a packet, which looks like this
     *              ------------------------------------------
     *              | ethernet |   ip   |   tcp    | payload |
     *              |  header  | header |  header  |         |
     *              ------------------------------------------
     * since we are interested in the ethernet header we do a simple type cast and it gives 
     * us a right amount of bytes, that is, it'll automatically ignore everything beyond
     * ethernet header
     */ 

    packet = tmp->full_packet;
    eptr = (ethernet_header *) packet; /* ethernet header of current packet */

    /* print the source and destination from the ethernet frame */
    ptr = eptr->ether_dhost;
    i = ETHER_ADDR_LEN;
    // fprintf(stdout, "destination mac address= ");
    // do{
    //    fprintf(stdout, "%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
    //  }while(--i>0);
    //  fprintf(stdout, "\n");

    ptr = eptr->ether_shost;
    i = ETHER_ADDR_LEN;
    // fprintf(stdout, "source mac address= ");
    // do{
    //   fprintf(stdout, "%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
    //   }while(--i>0);
    // fprintf(stdout, "\n");

    // fprintf(stdout, "type of packet= ");

    /* what is the type of packet, that is, is it an IP packet inside or is it arp etc. */

    switch(ntohs(eptr->ether_type))
    {
	    
      case (ETHERTYPE_IP):
        //fprintf(stdout, "IP\n");

        /* now that we know this is an ip packet, lets examine its headers
         * ip header starts right after ethernet header, that's why we add 
         * the size of ethernet header before type casting the content
         */
        ipptr = (ip_header *) (packet + size_of_ehdr);
	
      	/* lets first check if we have a ip packet of valid length, if we don't
      	 * do this check the following instructions may lead to a buffer overflow
      	 * in our sniffer, cs392 gang take notes
      	 */
               
      	if((tmp->packet_header->len - size_of_ehdr) < size_of_iphdr)
        {
      	  fprintf(stderr, "not a valid IP packet\n");
      	  continue;
      	}

        /* we have a valid ip packet, so lets print out its fields */
      	//fprintf(stdout, "information about this IP packet:\n");
      	//fprintf(stdout, "length = %d\n", ntohs(ipptr->tot_len));
      	//fprintf(stdout, "header length = %d\n", ipptr->ihl);
      	//fprintf(stdout, "version = %d\n", ipptr->version);
      	//fprintf(stdout, "id = 0x%x\n", ntohs(ipptr->id));
      	//fprintf(stdout, "offset = %d\n", ipptr->frag_off);
      	//fprintf(stdout, "ttl = %d\n", ipptr->ttl);
      	//fprintf(stdout, "protocol = %d\n", ipptr->protocol);
      	
      	ipaddr_s.s_addr = (unsigned long int)ipptr->saddr;
        //fprintf(stdout, "source = %s\n", inet_ntoa(ipaddr_s)); /* source address */
      	
      	ipaddr_d.s_addr = (unsigned long int)ipptr->daddr;
      	//fprintf(stdout, "destination = %s\n", inet_ntoa(ipaddr_d));
        /* and so on, you got the idea */	

         /* So here is where we will look at the tcp header information
          * For each TCP packet source we need to record:
          *    - Connection requests: TCP SYN
          *    - Positive responses: TCP SYN/ACK
          * And if the ratio of ConReqs to PosResp exceeds 3:1, issue a warning
          */

        tcpptr = (tcp_header *) (packet + size_of_ehdr + size_of_iphdr);

        if ((tmp->packet_header->len - (size_of_ehdr + size_of_iphdr)) < size_of_tcphdr)
        {
          fprintf(stderr, "not a valid TCP packet\n");
          continue;
        }

        //fprintf(stdout, "information about this TCP packet:\n");
        //fprintf(stdout, "src port = %d\n", ntohs(tcpptr->th_sport));
        //fprintf(stdout, "dest port = %d\n", ntohs(tcpptr->th_dport));
        //fprintf(stdout, "seq number = %u\n", ntohl(tcpptr->th_seq));
        //fprintf(stdout, "ack number = %u\n", ntohl(tcpptr->th_ack));
        //fprintf(stdout, "tcp flags = %x\n", tcpptr->th_flags);
        //fprintf(stdout, "SYN flag = %d\n", (tcpptr->th_flags & 0x02) >> 1);
        //fprintf(stdout, "ACK flag = %d\n", (tcpptr->th_flags & 0x10) >> 4);

        // fprintf(stdout, "\n\nCurrent Packet in Linked List Info:\n");
        // fprintf(stdout, "source IP: %s\n", inet_ntoa(ipaddr_s));
        // fprintf(stdout, "source port: %d\n", ntohs(tcpptr->th_sport));
        // fprintf(stdout, "dest IP: %s\n", inet_ntoa(ipaddr_d));
        // fprintf(stdout, "SYN flag = %d\n", (tcpptr->th_flags & 0x02) >> 1);
        // fprintf(stdout, "ACK flag = %d\n", (tcpptr->th_flags & 0x10) >> 4);

        // SOMEHOW COLLECT INFORMATION FOR DIAGNOSTICS AND SCANNING HERE
        for (j = 0; j < statsCount; j++)
        {
          // TEST PRINTS
          arrIPAddr_s.s_addr = packetStats[j].srcAddr;
          arrIPAddr_d.s_addr = packetStats[j].destAddr;
          // fprintf(stdout, "-----------------------------------------------\n");
          // fprintf(stdout, "Current Packet in Array Info:\n");
          // fprintf(stdout, "source IP: %s\n", inet_ntoa(arrIPAddr_s));
          // fprintf(stdout, "source port: %d\n", ntohs(packetStats[j].srcPort));
          // fprintf(stdout, "dest IP: %s\n", inet_ntoa(arrIPAddr_d));
          // fprintf(stdout, "# of connection requests: %lld\n", packetStats[j].connectReqs);
          // fprintf(stdout, "# of sucessful responses: %lld\n", packetStats[j].successResps);

          // If the packet is already in the list of structs, then increment its statistics values
          if ((packetStats[j].srcAddr == ipptr->saddr) && (packetStats[j].destAddr == ipptr->daddr) && (packetStats[j].srcPort == tcpptr->th_sport))
          {
            // Set to be found
            found = 1;
            // If the packet is just a SYN packet, increment its connection requests value
            if (tcpptr->th_flags == 0x12)
              packetStats[j].successResps++;
            else if (tcpptr->th_flags == 0x02)
              packetStats[j].connectReqs++;

            // Check stats now too
            // if (packetStats[j].successResps != 0)
            // {
            //   testRatio = packetStats[j].connectReqs / packetStats[j].successResps;
            // }
            // else
            // {
            //   if (packetStats[j].connectReqs > RATIOTHRESHOLD)
            //     testRatio = RATIOTHRESHOLD + 1;
            // }

            // if (testRatio > RATIOTHRESHOLD)
            // {
            //   fprintf(stdout, "\nWARNING: PORT SCAN ATTEMPT from Source IP: %s", inet_ntoa(ipaddr_s));
            //   fprintf(stdout, ", port: %d", ntohs(packetStats[j].srcPort));
            //   fprintf(stdout, " on Destination IP: %s.\n", inet_ntoa(ipaddr_d));
            //   if (packetStats[j].successResps != 0)
            //     fprintf(stdout, "(Ratio of %llu:1) <1>\n", testRatio);
            //   else
            //     fprintf(stdout, "(Ratio of %llu:1) <2>\n", packetStats[j].connectReqs);
            }
          
            // Break from the loop if a match is found
            break;
          }
        }

        // If not found, add it to the array
        if (!found)
        {
          packetStats[statsCount].srcAddr = ipptr->saddr;
          packetStats[statsCount].destAddr = ipptr->daddr;
          packetStats[statsCount].srcPort = tcpptr->th_sport;
          packetStats[statsCount].connectReqs = 0;
          packetStats[statsCount++].successResps = 0;
        }

        break;

      case (ETHERTYPE_ARP):
        fprintf(stdout, "ARP\n");
	      break;
	
      case (ETHERTYPE_REVARP):
      	fprintf(stdout, "RARP\n");
      	break;

      case (ETHERTYPE_PUP):
      	fprintf(stdout, "Xerox PUP\n");
      	break;

      default:
      	fprintf(stdout, "Unknown type (%x)\n", ntohs(eptr->ether_type));
      	break;
    }
      
    //fprintf(stdout, "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
    
    /* next packet please */
    tmp = tmp->next;
  }

  for (j = 0; j < statsCount; j++)
  {
    // Check stats now too
    if (packetStats[j].successResps != 0)
    {
      testRatio = packetStats[j].connectReqs / packetStats[j].successResps;
    }
    else
    {
      if (packetStats[j].connectReqs > RATIOTHRESHOLD)
        testRatio = RATIOTHRESHOLD + 1;
    }

    if (testRatio > RATIOTHRESHOLD)
    {
      fprintf(stdout, "\nWARNING: PORT SCAN ATTEMPT from Source IP: %s", inet_ntoa(ipaddr_s));
      fprintf(stdout, ", port: %d", ntohs(packetStats[j].srcPort));
      fprintf(stdout, " on Destination IP: %s.\n", inet_ntoa(ipaddr_d));
      if (packetStats[j].successResps != 0)
        fprintf(stdout, "(Ratio of %llu:1) <1>\n", testRatio);
      else
        fprintf(stdout, "(Ratio of %llu:1) <2>\n", packetStats[j].connectReqs);
    }
  }

  //fprintf(stdout, "Number of items in this filtered buffer of packets = %lld\n", buf.items);

  // close the file
  pcap_close(descr);
  
  return 0;
}
