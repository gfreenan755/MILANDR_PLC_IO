/* EtherCAT driver v.1 for MILANDR Based on EasyCAT library V_2_0 by  */
// AB&T Tecnologie Informatiche - Ivrea Italy                                                *
// http://www.bausano.net                                                                    *
// https://www.ethercat.org/en/products/791FFAA126AD43859920EA64384AD4FD.htm                 *

#include "EtherCAT_driver.h"

// Create EtherCAT structure
ETHERCAT EtherCATDevice;

Status getStatusEnum(unsigned char input){

  switch(input){
  case 0x01:
    return INIT;
  case 0x02:
    return PRE_OP;
  case 0x03:
    return BOOT;
  case 0x04:
    return SAFE_OP;
  case 0x08:
    return OP;
  default:
    return UNDEFINED;
  }

}

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
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(MDR_PORTC, &PORT_InitStructure);
  
  PORT_InitStructure.PORT_Pin   = PORT_Pin_9 | PORT_Pin_11;
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_Init(MDR_PORTC, &PORT_InitStructure);
  
  PORT_InitStructure.PORT_Pin   = PORT_Pin_12;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_Init(MDR_PORTC, &PORT_InitStructure);
  
  CS_Pin_High();
  
  /* Enable peripheral clocks */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | PCLK_BIT(MDR_SSP2), ENABLE);
  
  /* Reset all SSP settings */
  SSP_DeInit(MDR_SSP2);
  SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv1);
  
  /* SSP MASTER configuration */
  SSP_InitTypeDef sSSP;
  SSP_StructInit(&sSSP);
  
  /* Скорость передачи: 144 / (8 + 1) / 2 = 8 МГц */
  sSSP.SSP_SCR = 8;
  sSSP.SSP_CPSDVSR = 2;
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

void CS_Pin_Low(void){

  MDR_PORTC->CLRTX = PORT_Pin_12;

} 

void CS_Pin_High(void){

  MDR_PORTC->SETTX = PORT_Pin_12;
  Delay(4);

} 

void SPI_TransferTx(unsigned char data){
  
  SSP_SendData(MDR_SSP2, data);
  while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
  SSP_ReceiveData(MDR_SSP2);
  
}

void SPI_TransferTxLast(unsigned char data){
  
  SSP_SendData(MDR_SSP2, data);
  while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
  SSP_ReceiveData(MDR_SSP2);
  
}

unsigned char SPI_TransferRx(unsigned char data){
  
  SSP_SendData(MDR_SSP2, data);
  while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
  
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
  
  CS_Pin_Low();                                             // SPI chip select enable
    
  SPI_TransferTx(COMM_SPI_READ);                            // SPI read command
  SPI_TransferTx(Addr.Byte[1]);                             // address of the register
  SPI_TransferTxLast(Addr.Byte[0]);                         // to read, MsByte first
  
  for (i=0; i<Len; i++)                                     // read the requested number of bytes
  {                                                         // LsByte first 
    Result.Byte[i] = SPI_TransferRx(DUMMY_BYTE);            //
  }                                                         //    
  
  CS_Pin_High();                                            // SPI chip select disable 
    
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
  
  
  CS_Pin_Low();                                             // SPI chip select enable  
    
  SPI_TransferTx(COMM_SPI_WRITE);                           // SPI write command
  SPI_TransferTx(Addr.Byte[1]);                             // address of the register
  SPI_TransferTx(Addr.Byte[0]);                             // to write MsByte first
  
  SPI_TransferTx(Data.Byte[0]);                             // data to write 
  SPI_TransferTx(Data.Byte[1]);                             // LsByte first
  SPI_TransferTx(Data.Byte[2]);                             //
  SPI_TransferTxLast(Data.Byte[3]);                         //
  
  CS_Pin_High();                                            // SPI chip select enable 
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
  
  SPIWriteRegisterDirect(ECAT_PRAM_RD_CMD, PRAM_ABORT);        // abort any possible pending transfer
  
  
  SPIWriteRegisterDirect(ECAT_PRAM_RD_ADDR_LEN, (0x00001000 | (((uint32_t)TOT_BYTE_NUM_OUT) << 16)));   
  // the high word is the num of bytes
  // to read 0xTOT_BYTE_NUM_OUT----
  // the low word is the output process        
  // ram offset 0x----1000 
  
  SPIWriteRegisterDirect(ECAT_PRAM_RD_CMD, 0x80000000);        // start command        
  
  
  //------- one round is enough if we have ----
  //------- to transfer up to 64 bytes --------
  
  do                                                            // wait for the data to be       
  {                                                             // transferred from the output  
    TempLong.Long = SPIReadRegisterDirect (ECAT_PRAM_RD_CMD,2); // process ram to the read fifo       
  }                                                             //    
  while (TempLong.Byte[1] != (FST_BYTE_NUM_ROUND_OUT/4));       // *CCC* 
  
  CS_Pin_Low();                                                 // enable SPI chip select 
    
  SPI_TransferTx(COMM_SPI_READ);                                // SPI read command
  SPI_TransferTx(0x00);                                         // address of the read  
  SPI_TransferTxLast(0x00);                                     // fifo MsByte first
  
  for (i=0; i< FST_BYTE_NUM_ROUND_OUT; i++)                     // transfer the data
  {                                                             //
   EtherCATDevice.BufferOut.Byte[i] = SPI_TransferRx(DUMMY_BYTE);             //
  }                                                             //
  
  CS_Pin_High();                                                // disable SPI chip select    
#endif  
    
    
#if SEC_BYTE_NUM_OUT > 0                    //-- if we have to transfer more then 64 bytes --
    //-- we must do another round -------------------
    //-- to transfer the remainig bytes -------------
    
    
    do                                                          // wait for the data to be       
    {                                                           // transferred from the output  
      TempLong.Long = SPIReadRegisterDirect(ECAT_PRAM_RD_CMD,2);// process ram to the read fifo 
    }                                                           //    
  while (TempLong.Byte[1] != SEC_BYTE_NUM_ROUND_OUT/4);       // *CCC*  
  
  CS_Pin_Low();                                               // enable SPI chip select   
    
  SPI_TransferTx(COMM_SPI_READ);                              // SPI read command
  SPI_TransferTx(0x00);                                       // address of the read  
  SPI_TransferTxLast(0x00);                                   // fifo MsByte first
  
  for (i=0; i< (SEC_BYTE_NUM_ROUND_OUT); i++)                 // transfer loop for the remaining 
  {                                                           // bytes
    EtherCATDevice.BufferOut.Byte[i+64] = SPI_TransferRx(DUMMY_BYTE);        // we transfer the second part of
  }                                                           // the buffer, so offset by 64
  
  CS_Pin_High();                                              // SPI chip select disable  
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
  
  CS_Pin_Low();                                                 // enable SPI chip select
    
  SPI_TransferTx(COMM_SPI_WRITE);                               // SPI write command
  SPI_TransferTx(0x00);                                         // address of the write fifo 
  SPI_TransferTx(0x20);                                         // MsByte first 
  
  for (i=0; i < (FST_BYTE_NUM_ROUND_IN - 1 ); i++)               // transfer the data
  {                                                             //
    SPI_TransferTx(EtherCATDevice.BufferIn.Byte[i]);                          //      
  }                                                             //
  //  
  SPI_TransferTxLast(EtherCATDevice.BufferIn.Byte[i]);                        // one last byte
  
  CS_Pin_High();                                                // disable SPI chip select  
#endif        
    
    
#if SEC_BYTE_NUM_IN > 0                     //-- if we have to transfer more then 64 bytes --
    //-- we must do another round -------------------
    //-- to transfer the remainig bytes -------------
    
    do                                                          // check that the fifo has     
    {                                                           // enough free space       
      TempLong.Long = SPIReadRegisterDirect(ECAT_PRAM_WR_CMD,2);// 
    }                                                           //  
  while (TempLong.Byte[1] < (SEC_BYTE_NUM_ROUND_IN/4));       //   *CCC*
  
  CS_Pin_Low();                                               // enable SPI chip select
    
  SPI_TransferTx(COMM_SPI_WRITE);                             // SPI write command
  SPI_TransferTx(0x00);                                       // address of the write fifo 
  SPI_TransferTx(0x20);                                       // MsByte first 
  
  for (i=0; i< (SEC_BYTE_NUM_ROUND_IN - 1); i++)              // transfer loop for the remaining 
  {                                                           // bytes
    SPI_TransferTx (BufferIn.Byte[i+64]);                     // we transfer the second part of
  }                                                           // the buffer, so offset by 64
  //  
  SPI_TransferTxLast (BufferIn.Byte[i+64]);                   // one last byte  
  
  CS_Pin_High();                                              // disable SPI chip select 
#endif     
}

//---- EtherCAT board initialization ---------------------------------------------------------------

void EtherCAT_Init(bool resetDevice, ETHERCAT* Device)
{
  
  ULONG TempLong;
  unsigned short i;
  
  SPI_Init();   
  
  Delay(50);
  
  if(resetDevice){
  
  SPIWriteRegisterDirect (RESET_CTL, DIGITAL_RST);        // LAN9252 reset 
  
  i = 0;                                                  // reset attempts 
  do                                                      // wait for reset to complete
  {                                                       //
    i++;                                                  //
    TempLong.Long = SPIReadRegisterDirect (RESET_CTL, 4); //
  }while (((TempLong.Byte[0] & 0x01) != 0x00) && (i != ATTEMPTS_NUM));    
  //                                                       
  if (i == ATTEMPTS_NUM)                                          // time out expired      
  {                                                       //
    EtherCATDevice.curState = RESET_FAIL;                                       // initialization failed  
  }                                                         
    
  }
 
  i = 0;                                                  // reset attempts  
  do                                                      // check the Byte Order Test Register
  {                                                       //
    i++;                                                  //      
    TempLong.Long = SPIReadRegisterDirect (BYTE_TEST, 4); //
  }while ((TempLong.Long != 0x87654321) && (i != ATTEMPTS_NUM));  //    
  //                                                            
  if (i == ATTEMPTS_NUM)                                          // time out expired      
  {                                                       //
    EtherCATDevice.curState = TEST_FAIL;                                        // initialization failed  
  }            
  
  i = 0;                                                  // reset attempts  
  do                                                      // check the Ready flag
  {                                                       //
    i++;                                                  //    
    TempLong.Long = SPIReadRegisterDirect (HW_CFG, 4);    //
  }while (((TempLong.Byte[3] & READY) == 0) && (i != ATTEMPTS_NUM));//
  //
  if (i == ATTEMPTS_NUM)                                          // time out expired      
  {                                                       //
    EtherCATDevice.curState = HW_FAIL;                                        // initialization failed  
  }        
  
  if ((Device->Sync == DC_SYNC) || (Device->Sync == SM_SYNC))           //--- if requested, enable --------   
  {                                                       //--- interrupt generation -------- 
  
    if (Device->Sync == DC_SYNC)
    {                                                     // enable interrupt from SYNC 0
      SPIWriteRegisterIndirect (0x00000004, AL_EVENT_MASK, 4);  
                                                          // in AL event mask register, and disable 
                                                          // other interrupt sources                                                          
    }                                                       
                                                                                                         
    else
    {                                                     
                                                          // enable interrupt from SM 0 event 
                                                          // (output synchronization manager)
      SPIWriteRegisterIndirect (0x00000100, AL_EVENT_MASK, 4);     
                                                          // in AL event mask register, and disable 
                                                          // other interrupt sources    
    }    
                                                         
    SPIWriteRegisterDirect (IRQ_CFG, 0x00000111);         // set LAN9252 interrupt pin driver  
                                                          // as push-pull active high
                                                          // (On the EasyCAT shield board the IRQ pin
                                                          // is inverted by a mosfet, so Arduino                                                        
                                                          // receives an active low signal)
                                                                        
    SPIWriteRegisterDirect (INT_EN, 0x00000001);          // enable LAN9252 interrupt      
  } 
  
  
  TempLong.Long = SPIReadRegisterDirect (ID_REV, 4);      // read the chip identification 
  EtherCATDevice.ChipID = TempLong.Word[1];                // and revision, and save it
  EtherCATDevice.Rev = TempLong.Word[0];                   // into union
  
  EtherCATDevice.curState = NORMAL_OP;                                       // initalization completed   
}

void EtherCAT_MainTask(){
  
  bool WatchDog = 0;
  bool Operational = 0; 
  unsigned char i;
  ULONG TempLong; 
  unsigned char Status;  
  
  TempLong.Long = SPIReadRegisterIndirect(WDOG_STATUS, 1);  // read watchdog status
  if ((TempLong.Byte[0] & 0x01) == 0x01)                    //
    WatchDog = 0;                                           // set/reset the corrisponding flag
  else                                                      //
    WatchDog = 1;                                           //
  
  TempLong.Long = SPIReadRegisterIndirect(AL_STATUS, 1);    // read the EtherCAT State Machine status
  Status = TempLong.Byte[0] & 0x0F;                         // to see if we are in operational state
  if (Status == ESM_OP)                                     // 
    Operational = 1;                                        //
  else                                                      // set/reset the corrisponding flag
    Operational = 0;                                        //    
  
  //--- process data transfer ----------
  //               
  if (WatchDog | !Operational)                              // if watchdog is active or we are 
  {                                                         // not in operational state, reset 
    for (i=0; i < TOT_BYTE_NUM_OUT ; i++)                   // the output buffer
    {                                                       //
      EtherCATDevice.BufferOut.Byte[i] = 0;                                //
    }                                                       //
  }  
  else                                                      
  {                                                         
    SPIReadProcRamFifo();                                   // otherwise transfer process data from 
  }                                                         // the EtherCAT core to the output buffer  
  
  SPIWriteProcRamFifo();                                    // we always transfer process data from
                                                            // the input buffer to the EtherCAT core  
  
EtherCATDevice.slaveStatus = getStatusEnum(Status);         // return the status of the State Machine      

       
  
}
