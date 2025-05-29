#include <fstream>
#include <stdio.h>
#include <cstdint>
#include <unistd.h>

#include "SimUdpSocket.cpp"

struct T_DISHeader
{
   double   Time;
   uint8_t  Version;
   uint8_t  Exercise;
   uint8_t  PduType;
   uint8_t  ProtocolFamily;
   uint32_t TimeStamp;
   uint16_t Length;
   uint16_t Padding;
   uint16_t Site;
   uint16_t App;
   uint16_t Entity;
};

int main()
{
   char*         buffer;
   uint32_t      size;
   uint32_t      index;
   uint32_t      frame = 0;
   double        prev_time = 0.0;
   std::fstream  in_file("dis.bin", std::ios::in | std::ios::binary);
   CSimUdpSocket socket;

   const char* ip_addr = "127.0.0.1";
   int port = 3000;
   printf("Opening UDP socket on %s:%d\n", ip_addr, port);
   socket.Open(ip_addr, port, port);

   if (in_file.is_open())
   {
      in_file.seekg(0, std::ios::end);
      size = in_file.tellg();
      in_file.seekg(0);

      buffer = new char[size];

      in_file.read(buffer, size);

      printf("Opened dis.bin, read %d bytes\n", size);

      while (index < size)
      {
         T_DISHeader* hdr = (T_DISHeader*)&buffer[index];

         if (index == 0)
            prev_time = hdr->Time;

         frame++;

         //printf("%d: %f - %d:%d:%d Size %d\n",
         //      frame,
         //      hdr->Time,
         //      hdr->Site,
         //      hdr->App,
         //      hdr->Entity,
         //      htons(hdr->Length));

         hdr->App = htons(5001);

         socket.SendToSocket(&buffer[index + 8], htons(hdr->Length));

         index += htons(hdr->Length) + 8;

         if (index >= size)
         {
            printf("End of file\n");
            index = 0;
            usleep(10000);
         }
         else
         {
            int sleep_time = (int)((hdr->Time - prev_time) * 1000000.0);
            prev_time = hdr->Time;
            usleep(sleep_time);
         }
      }
   }
}
