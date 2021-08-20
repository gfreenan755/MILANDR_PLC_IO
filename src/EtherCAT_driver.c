/* EtherCAT driver v.1 for MILANDR Based on EasyCAT library V_2_0 by  */
// AB&T Tecnologie Informatiche - Ivrea Italy                                                *
// http://www.bausano.net                                                                    *
// https://www.ethercat.org/en/products/791FFAA126AD43859920EA64384AD4FD.htm                 *

#include "EtherCAT_driver.h"

// create a union for data
ETHERCAT EtherCATDevice;

void SPI_Init(void){

    // Инициализируем пины SPI2
    /* Enable peripheral clocks */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | PCLK_BIT(MDR_PORTC)
                    | PCLK_BIT(MDR_PORTC), ENABLE);

    /* Configure SPI pins: CLK, RXD, TXD */
    PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_10;
    PORT_InitStructure.PORT_OE    = PORT_OE_IN;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_9 | PORT_Pin_11;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_12;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    /* Enable peripheral clocks */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | PCLK_BIT(MDR_SSP2), ENABLE);

    /* Reset all SSP settings */
    SSP_DeInit(MDR_SSP2);
    SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv1);

    /* SSP MASTER configuration */
    SSP_InitTypeDef sSSP;
    SSP_StructInit(&sSSP);

    /* Скорость передачи: 144 / (17 + 1) / 1 = 8 МГц */
    sSSP.SSP_SCR = 17;
    sSSP.SSP_CPSDVSR = 1;
    sSSP.SSP_Mode = SSP_ModeMaster;
    sSSP.SSP_WordLength = SSP_WordLength8b;
    sSSP.SSP_SPH = SSP_SPH_1Edge;
    sSSP.SSP_SPO = SSP_SPO_Low;
    sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
    sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
    SSP_Init(MDR_SSP2, &sSSP);

    /* Enable SSP */
    SSP_Cmd(MDR_SSP2, ENABLE);

}

void SPI_TransferTx(unsigned char data){
    
    SSP_SendData(MDR_SSP2, data);
    EtherCATDevice.Debug = MDR_SSP2->DR;
    
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}

}

void SPI_TransferTxLast(unsigned char data){
    
    SSP_SendData(MDR_SSP2, data);
    EtherCATDevice.Debug = MDR_SSP2->DR;
    
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}

}

unsigned char SPI_TransferRx(unsigned char data){
    
    SSP_SendData(MDR_SSP2, data);
    EtherCATDevice.Debug = MDR_SSP2->DR;
    
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
      
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_RNE) == RESET) {}
    EtherCATDevice.Debug = MDR_SSP2->DR;

    return SSP_ReceiveData(MDR_SSP2);

}

//---- EasyCAT Read/Write functions ---------------------------------------------------------------

//---- read a directly addressable registers  -----------------------------------------------------

unsigned long SPIReadRegisterDirect (unsigned short Address, unsigned char Len)

                                                   // Address = register to read
                                                   // Len = number of bytes to read (1,2,3,4)
                                                   //
                                                   // a long is returned but only the requested bytes
                                                   // are meaningful, starting from LsByte                                                 
{
  ULONG Result; 
  UWORD Addr;
  Addr.Word = Address; 
  unsigned char i; 
  
  SCS_Low_macro                                             // SPI chip select enable

  SPI_TransferTx(COMM_SPI_READ);                            // SPI read command
  SPI_TransferTx(Addr.Byte[1]);                             // address of the register
  SPI_TransferTxLast(Addr.Byte[0]);                         // to read, MsByte first
 
  for (i=0; i<Len; i++)                                     // read the requested number of bytes
  {                                                         // LsByte first 
    Result.Byte[i] = SPI_TransferRx(DUMMY_BYTE);            //
  }                                                         //    
  
  SCS_High_macro                                            // SPI chip select disable 
 
  return Result.Long;                                       // return the result
}


//---- write a directly addressable registers  ----------------------------------------------------

void SPIWriteRegisterDirect (unsigned short Address, unsigned long DataOut)

                                                   // Address = register to write
                                                   // DataOut = data to write
{ 
  ULONG Data; 
  UWORD Addr;
  Addr.Word = Address;
  Data.Long = DataOut;    

  
  SCS_Low_macro                                             // SPI chip select enable  
  
  SPI_TransferTx(COMM_SPI_WRITE);                           // SPI write command
  SPI_TransferTx(Addr.Byte[1]);                             // address of the register
  SPI_TransferTx(Addr.Byte[0]);                             // to write MsByte first

  SPI_TransferTx(Data.Byte[0]);                             // data to write 
  SPI_TransferTx(Data.Byte[1]);                             // LsByte first
  SPI_TransferTx(Data.Byte[2]);                             //
  SPI_TransferTxLast(Data.Byte[3]);                         //
 
  SCS_High_macro                                            // SPI chip select enable   
}


//---- read an indirectly addressable registers  --------------------------------------------------

unsigned long SPIReadRegisterIndirect (unsigned short Address, unsigned char Len)

                                                   // Address = register to read
                                                   // Len = number of bytes to read (1,2,3,4)
                                                   //
                                                   // a long is returned but only the requested bytes
                                                   // are meaningful, starting from LsByte                                                  
{
  ULONG TempLong;
  UWORD Addr;
  Addr.Word = Address;
                                                            // compose the command
                                                            //
  TempLong.Byte[0] = Addr.Byte[0];                          // address of the register
  TempLong.Byte[1] = Addr.Byte[1];                          // to read, LsByte first
  TempLong.Byte[2] = Len;                                   // number of bytes to read
  TempLong.Byte[3] = ESC_READ;                              // ESC read 

  SPIWriteRegisterDirect (ECAT_CSR_CMD, TempLong.Long);     // write the command

  do
  {                                                         // wait for command execution
    TempLong.Long = SPIReadRegisterDirect(ECAT_CSR_CMD,4);  //
  }                                                         //
  while(TempLong.Byte[3] & ECAT_CSR_BUSY);                  //
                                                             
                                                              
  TempLong.Long = SPIReadRegisterDirect(ECAT_CSR_DATA,Len); // read the requested register
  return TempLong.Long;                                     //
}


//---- write an indirectly addressable registers  -------------------------------------------------

void  SPIWriteRegisterIndirect (unsigned long DataOut, unsigned short Address, unsigned char Len)

                                                   // Address = register to write
                                                   // DataOut = data to write                                                    
{
  ULONG TempLong;
  UWORD Addr;
  Addr.Word = Address;

  
  SPIWriteRegisterDirect (ECAT_CSR_DATA, DataOut);            // write the data

                                                              // compose the command
                                                              //                                
  TempLong.Byte[0] = Addr.Byte[0];                            // address of the register  
  TempLong.Byte[1] = Addr.Byte[1];                            // to write, LsByte first
  TempLong.Byte[2] = Len;                                     // number of bytes to write
  TempLong.Byte[3] = ESC_WRITE;                               // ESC write

  SPIWriteRegisterDirect (ECAT_CSR_CMD, TempLong.Long);       // write the command

  do                                                          // wait for command execution
  {                                                           //
    TempLong.Long = SPIReadRegisterDirect (ECAT_CSR_CMD, 4);  //  
  }                                                           //  
  while (TempLong.Byte[3] & ECAT_CSR_BUSY);                   //
}


//---- read from process ram fifo ----------------------------------------------------------------

void SPIReadProcRamFifo()    // read data from the output process ram, through the fifo
                                      //        
                                      // these are the bytes received from the EtherCAT master and
                                      // that will be use by our application to write the outputs
{
  ULONG TempLong;
  unsigned char i;
  
  
  #if TOT_BYTE_NUM_OUT > 0

    SPIWriteRegisterDirect (ECAT_PRAM_RD_CMD, PRAM_ABORT);        // abort any possible pending transfer

  
    SPIWriteRegisterDirect (ECAT_PRAM_RD_ADDR_LEN, (0x00001000 | (((uint32_t)TOT_BYTE_NUM_OUT) << 16)));   
                                                                  // the high word is the num of bytes
                                                                  // to read 0xTOT_BYTE_NUM_OUT----
                                                                  // the low word is the output process        
                                                                  // ram offset 0x----1000 

    SPIWriteRegisterDirect (ECAT_PRAM_RD_CMD, 0x80000000);        // start command        
 
 
                                                //------- one round is enough if we have ----
                                                //------- to transfer up to 64 bytes --------
   
    do                                                            // wait for the data to be       
    {                                                             // transferred from the output  
      TempLong.Long = SPIReadRegisterDirect (ECAT_PRAM_RD_CMD,2); // process ram to the read fifo       
    }                                                             //    
    while (TempLong.Byte[1] != (FST_BYTE_NUM_ROUND_OUT/4));       // *CCC* 
  
    SCS_Low_macro                                                 // enable SPI chip select 
  
    SPI_TransferTx(COMM_SPI_READ);                                // SPI read command
    SPI_TransferTx(0x00);                                         // address of the read  
    SPI_TransferTxLast(0x00);                                     // fifo MsByte first
  
    for (i=0; i< FST_BYTE_NUM_ROUND_OUT; i++)                     // transfer the data
    {                                                             //
      EtherCATDevice.BufferOut.Byte[i] = SPI_TransferRx(DUMMY_BYTE);             //
    }                                                             //
    
    SCS_High_macro                                                // disable SPI chip select    
  #endif  

  
  #if SEC_BYTE_NUM_OUT > 0                    //-- if we have to transfer more then 64 bytes --
                                              //-- we must do another round -------------------
                                              //-- to transfer the remainig bytes -------------


    do                                                          // wait for the data to be       
    {                                                           // transferred from the output  
      TempLong.Long = SPIReadRegisterDirect(ECAT_PRAM_RD_CMD,2);// process ram to the read fifo 
    }                                                           //    
    while (TempLong.Byte[1] != SEC_BYTE_NUM_ROUND_OUT/4);       // *CCC*  

    SCS_Low_macro                                               // enable SPI chip select   
    
    SPI_TransferTx(COMM_SPI_READ);                              // SPI read command
    SPI_TransferTx(0x00);                                       // address of the read  
    SPI_TransferTxLast(0x00);                                   // fifo MsByte first
    
    for (i=0; i< (SEC_BYTE_NUM_ROUND_OUT); i++)                 // transfer loop for the remaining 
    {                                                           // bytes
      BufferOut.Byte[i+64] = SPI_TransferRx(DUMMY_BYTE);        // we transfer the second part of
    }                                                           // the buffer, so offset by 64
      
    SCS_High_macro                                              // SPI chip select disable  
  #endif  
}  


//---- write to the process ram fifo --------------------------------------------------------------

void SPIWriteProcRamFifo()    // write data to the input process ram, through the fifo
                                       //    
                                       // these are the bytes that we have read from the inputs of our                   
                                       // application and that will be sent to the EtherCAT master
{
  ULONG TempLong;
  unsigned char i;
  
  
  #if TOT_BYTE_NUM_IN > 0  
  
    SPIWriteRegisterDirect (ECAT_PRAM_WR_CMD, PRAM_ABORT);        // abort any possible pending transfer
  
 
    SPIWriteRegisterDirect (ECAT_PRAM_WR_ADDR_LEN, (0x00001200 | (((uint32_t)TOT_BYTE_NUM_IN) << 16)));   
                                                                  // the high word is the num of bytes
                                                                  // to write 0xTOT_BYTE_NUM_IN----
                                                                  // the low word is the input process        
                                                                  // ram offset  0x----1200
                                                                                               
    SPIWriteRegisterDirect (ECAT_PRAM_WR_CMD, 0x80000000);        // start command  
  
  
                                                //------- one round is enough if we have ----
                                                //------- to transfer up to 64 bytes --------
    
    do                                                            // check that the fifo has      
    {                                                             // enough free space 
      TempLong.Long = SPIReadRegisterDirect (ECAT_PRAM_WR_CMD,2); //  
    }                                                             //  
    while (TempLong.Byte[1] <   (FST_BYTE_NUM_ROUND_IN/4));       //    *CCC*
  
    SCS_Low_macro                                                 // enable SPI chip select
  
    SPI_TransferTx(COMM_SPI_WRITE);                               // SPI write command
    SPI_TransferTx(0x00);                                         // address of the write fifo 
    SPI_TransferTx(0x20);                                         // MsByte first 

    for (i=0; i< (FST_BYTE_NUM_ROUND_IN - 1 ); i++)               // transfer the data
    {                                                             //
      SPI_TransferTx (EtherCATDevice.BufferIn.Byte[i]);                          //      
    }                                                             //
                                                                  //  
    SPI_TransferTxLast (EtherCATDevice.BufferIn.Byte[i]);                        // one last byte
  
    SCS_High_macro                                                // disable SPI chip select           
  #endif        

  
  #if SEC_BYTE_NUM_IN > 0                     //-- if we have to transfer more then 64 bytes --
                                              //-- we must do another round -------------------
                                              //-- to transfer the remainig bytes -------------

    do                                                          // check that the fifo has     
    {                                                           // enough free space       
      TempLong.Long = SPIReadRegisterDirect(ECAT_PRAM_WR_CMD,2);// 
    }                                                           //  
    while (TempLong.Byte[1] < (SEC_BYTE_NUM_ROUND_IN/4));       //   *CCC*
                             
    SCS_Low_macro                                               // enable SPI chip select
    
    SPI_TransferTx(COMM_SPI_WRITE);                             // SPI write command
    SPI_TransferTx(0x00);                                       // address of the write fifo 
    SPI_TransferTx(0x20);                                       // MsByte first 

    for (i=0; i< (SEC_BYTE_NUM_ROUND_IN - 1); i++)              // transfer loop for the remaining 
    {                                                           // bytes
      SPI_TransferTx (BufferIn.Byte[i+64]);                     // we transfer the second part of
    }                                                           // the buffer, so offset by 64
                                                                //  
    SPI_TransferTxLast (BufferIn.Byte[i+64]);                   // one last byte  

    SCS_High_macro                                              // disable SPI chip select    
  #endif     
}

//---- EasyCAT board initialization ---------------------------------------------------------------

bool EtherCAT_Init()
{
  #define Tout 1000
  
  ULONG TempLong;
  unsigned short i;
  
  SPI_Init();     
  
  SPIWriteRegisterDirect (RESET_CTL, DIGITAL_RST);        // LAN9252 reset 
   
  i = 0;                                                  // reset timeout 
  do                                                      // wait for reset to complete
  {                                                       //
    i++;                                                  //
    TempLong.Long = SPIReadRegisterDirect (RESET_CTL, 4); //
  }while (((TempLong.Byte[0] & 0x01) != 0x00) && (i != Tout));    
                                                          //                                                       
  if (i == Tout)                                          // time out expired      
  {                                                       //
  
    return false;                                         // initialization failed  
  }                                                         
  
  i = 0;                                                  // reset timeout  
  do                                                      // check the Byte Order Test Register
  {                                                       //
    i++;                                                  //      
    TempLong.Long = SPIReadRegisterDirect (BYTE_TEST, 4); //
  }while ((TempLong.Long != 0x87654321) && (i != Tout));  //    
                                                          //                                                            
  if (i == Tout)                                          // time out expired      
  {                                                       //

    return false;                                         // initialization failed  
  }            
  
  i = 0;                                                  // reset timeout  
  do                                                      // check the Ready flag
  {                                                       //
    i++;                                                  //    
    TempLong.Long = SPIReadRegisterDirect (HW_CFG, 4);    //
  }while (((TempLong.Byte[3] & READY) == 0) && (i != Tout));//
                                                          //
  if (i == Tout)                                          // time out expired      
  {                                                       //

    return false;                                         // initialization failed  
  }              
  
  TempLong.Long = SPIReadRegisterDirect (ID_REV, 4);      // read the chip identification 
  EtherCATDevice.ChipID = TempLong.Word[1];                // and revision, and save it
  EtherCATDevice.Rev = TempLong.Word[0];                   // into union

  return true;                                        // initalization completed   
}
