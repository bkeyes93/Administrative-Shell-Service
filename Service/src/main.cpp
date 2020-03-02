//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Set up the main entry point.
//
//====================================================================//

#include "service.h"

int main (int argc, char* argv[])
{
  if (startService())
      return 1;
  return 0;
}
