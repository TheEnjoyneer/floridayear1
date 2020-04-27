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
  u_char *packet;     /* vars to store raw packets */
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

  FILE *tracefile = fopen(argv[1], "r");
  if((descr = pcap_fopen_offline(tracefile, errbuf))==NULL){
      fprintf (stderr, "%s: pcap_fopen_offline: %s\n", argv[0], errbuf);
      exit (-1);
  }


  /* so, i'm preparing a linked-list to store the packets i sniff */
  create_buffer(&buf, MAXPACKETS, MAXPACKETS / 2);

  /* pcap_loop returns numbers of packets captured, so i'm just printing it out. */
  pcap_loop(descr, PACKETSTOREAD, pcap_callback, (void *)&buf);
 
  /* sniffing is all done, at this point we have some packets in the linked-list so lets
   * go through the list and print some information about our captives
   */

  /* just walk the list one item at a time and handle statistics as necessary */
  tmp = buf.header;
  while(tmp != NULL)
  {
    // Reset important vals
    found = 0;
    testRatio = 1;

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

    switch(ntohs(eptr->ether_type))
    {
	    
      case (ETHERTYPE_IP):

        ipptr = (ip_header *) (packet + size_of_ehdr);
               
      	if((tmp->packet_header->len - size_of_ehdr) < size_of_iphdr)
        {
      	  fprintf(stderr, "not a valid IP packet\n");
      	  continue;
      	}

        tcpptr = (tcp_header *) (packet + size_of_ehdr + size_of_iphdr);

        if ((tmp->packet_header->len - (size_of_ehdr + size_of_iphdr)) < size_of_tcphdr)
        {
          fprintf(stderr, "not a valid TCP packet\n");
          continue;
        }

        // SOMEHOW COLLECT INFORMATION FOR DIAGNOSTICS AND SCANNING HERE
        for (j = 0; j < statsCount; j++)
        {
          // If the packet is already in the list of structs, then increment its statistics values
          if (((packetStats[j].srcAddr == ipptr->saddr) && (packetStats[j].destAddr == ipptr->daddr)) || ((packetStats[j].destAddr == ipptr->saddr) && (packetStats[j].srcAddr == ipptr->daddr)))
          {
            // Set to be found
            found = 1;
            // If the packet is just a SYN packet, increment its connection requests value
            if ((tcpptr->th_flags & 0x12) == 0x12)
              packetStats[j].successResps++;
            else if ((tcpptr->th_flags & 0x02) == 0x02)
              packetStats[j].connectReqs++;
          
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
          if (tcpptr->th_flags == 0x12)
          {
              packetStats[statsCount].connectReqs = 0;
              packetStats[statsCount++].successResps = 1;
          }
          else if (tcpptr->th_flags == 0x02)
          {
            packetStats[statsCount].successResps = 0;
            packetStats[statsCount++].connectReqs = 1;
          }
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
        testRatio = packetStats[j].connectReqs;
    }

    if (testRatio > RATIOTHRESHOLD)
    {
      arrIPAddr_s.s_addr = packetStats[j].srcAddr;
      arrIPAddr_d.s_addr = packetStats[j].destAddr;
      fprintf(stdout, "\nWARNING: PORT SCAN ATTEMPT from Source IP: %s,", inet_ntoa(arrIPAddr_d));
      fprintf(stdout, " on Destination IP: %s.\n", inet_ntoa(arrIPAddr_s));
      fprintf(stdout, "(Ratio of %llu:1)\n", testRatio);
    }
  }

  // close the file
  pcap_close(descr);
  
  return 0;
}
