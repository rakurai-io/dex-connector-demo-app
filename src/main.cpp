#include "dex_handler.h"
#include "dex_listener_example.h"

#include <csignal>
#include <iostream>

void signal_handler(int signal_number)
{
   switch (signal_number)
   {
   case SIGUSR1: {
      std::cout << "SIGUSR1 Received\n";
      rakurai::dex_connector::dump_books();
   }
   break;
   }
}

int main()
{
   std::signal(SIGUSR1, signal_handler);

   dex_callbacks dex_listener;
   dex_listener.config();
   dex_listener.run();
   std::cout << "The End" << std::endl;
}