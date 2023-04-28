#include <stdio.h>
#include <stdint.h>
#include "nx_api.h"

#define CERT_1ST_LINE_LENGTH 50
#define CERT_BASE64_DATA_MAX_LENGTH 2048

uint8_t certPEM[2048];
unsigned int certDER[2048];
unsigned int DecodeBASE64(char *incertbuff,unsigned char*outcerthexbuff);
void cli_thread_entry(void);
void cli_thread_entry(void)
{

	while (1)
		{
			main_display_menu();
			tx_thread_sleep(5);
		}
}


/* @brief  DecodeBASE64 function
*
* This function gets the rootCA certificate from the user and store in the internal flash.
*********************************************************************************************************************/
 unsigned int DecodeBASE64(char *incertbuff,unsigned char*outcerthexbuff)
{
	  memset(outcerthexbuff,0,CERT_BASE64_DATA_MAX_LENGTH);
      memset(certPEM,0,sizeof(certPEM));

      unsigned long int t=0;
      unsigned long int t2=0;
      unsigned long int t3=0;

/*Remove 1st line of certificate ,skip till \r */
      for (t=0;t<CERT_1ST_LINE_LENGTH;t++)
      {
          if(incertbuff[t]=='\r')
          {break;}
      }
   	  t++;

/*Copy BASE64 data till certificate's last line's 1st character '-' */
     for(;t<CERT_BASE64_DATA_MAX_LENGTH;t++)
      {
         if(incertbuff[t]=='-')
         {
             break;
         }
         if((incertbuff[t]!='\r')&&(incertbuff[t]!='\n'))
         {
             certPEM[t2]=incertbuff[t];

              t2++;
         }

       }

/*Generate hex data from BASE64 data*/
  t3= base64_decode(&certPEM[0], t2 ,&outcerthexbuff[0]);

   /*Return  hex data len*/
   return(t3);
}
