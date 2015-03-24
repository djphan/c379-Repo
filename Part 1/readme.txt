Note the following things:

1. In line 322 of pktgen. Sleep(1) was used to give the router.c program time to process packets as given in the forum specs
2. Pktgen also has an additional field: "Packet Routed Locally" which is reported in the packages output.
   The assignment specs say: (we assume that two hosts within a network can communicate directly)
   and the fourms say (The important things are the source IP and the destination IP which is picked 
   randomly and both should not be the same except if its from and for router A.)

   Due to these notes in the specs and fourms, A to A packets were made and recorded for completeness. 

