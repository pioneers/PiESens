//******************************************************************************
//   MSP430F552x Demo - USCI_B0, I2C Master multiple byte TX/RX
//
//   Description: I2C master communicates to I2C slave sending and receiving
//   3 different messages of different length. I2C master will enter LPM0 mode
//   while waiting for the messages to be sent/receiving using I2C interrupt.
//   ACLK = NA, MCLK = SMCLK = DCO 16MHz.
//
//                                     /|\ /|\
//                   MSP430F5529       4.7k |
//                 -----------------    |  4.7k
//            /|\ |             P3.1|---+---|-- I2C Clock (UCB0SCL)
//             |  |                 |       |
//             ---|RST          P3.0|-------+-- I2C Data (UCB0SDA)
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//
//   Nima Eskandari
//   Texas Instruments Inc.
//   April 2017
//   Built with CCS V7.0
//******************************************************************************

#include <I2C_Lib.h>

//******************************************************************************
// General I2C State Machine ***************************************************
//******************************************************************************
typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;

/* Used to track the state of the software state machine*/
I2C_Mode MasterMode = IDLE_MODE;

/* The Register Address/Command to use*/
uint8_t TransmitRegAddr = 0;

/* ReceiveBuffer: Buffer used to receive data in the ISR
 * RXByteCtr: Number of bytes left to receive
 * ReceiveIndex: The index of the next byte to be received in ReceiveBuffer
 * TransmitBuffer: Buffer used to transmit data in the ISR
 * TXByteCtr: Number of bytes left to transfer
 * TransmitIndex: The index of the next byte to be transmitted in TransmitBuffer
 * */
uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t RXByteCtr = 0;
uint8_t ReceiveIndex = 0;
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;

/* I2C Write and Read Functions */

/* For slave device with dev_addr, writes the data specified in *reg_data
 *
 * dev_addr: The slave device address.
 *           Example: SLAVE_ADDR
 * reg_addr: The register or command to send to the slave.
 *           Example: CMD_TYPE_0_MASTER
 * *reg_data: The buffer to write
 *           Example: MasterType0
 * count: The length of *reg_data
 *           Example: TYPE_0_LENGTH
 *  */


/* For slave device with dev_addr, read the data specified in slaves reg_addr.
 * The received data is available in ReceiveBuffer
 *
 * dev_addr: The slave device address.
 *
 * reg_addr: The register or command to send to the slave.
 *
 * reg_data: An array of at least size "count" to write to. This
 *          is the output.
 *
 * count: The length of data to read
 *  */
void I2C_Master_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    /* Initialize state machine */
    MasterMode = TX_REG_ADDRESS_MODE;
    TransmitRegAddr = reg_addr;
    RXByteCtr = count;
    TXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB0IE |= UCTXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    CopyArray(ReceiveBuffer, reg_data, count);
}

/* For slave device with dev_addr, write the data specified to slaves reg_addr.
 *
 * dev_addr: The slave device address.
 *
 * reg_addr: The register or command to send to the slave.
 *
 * reg_data: The data to write
 *
 * count: The length of data to write
 *  */
void I2C_Master_WriteReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    /* Initialize state machine */
    MasterMode = TX_REG_ADDRESS_MODE;
    TransmitRegAddr = reg_addr;

    //Copy register data to TransmitBuffer
    CopyArray(reg_data, TransmitBuffer, count);

    TXByteCtr = count;
    RXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB0IE |= UCTXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts
}

void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count)
{
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

/*
 * initializes I2C communication as a Master
 */
void initI2C()
{
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
    UCB0BR0 = 160;                            // fSCL = SMCLK/160 = ~100kHz
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    UCB0IE |= UCNACKIE;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //Must read from UCB0RXBUF
  uint8_t rx_val = 0;

  switch(__even_in_range(UCB0IV,0xC))
  {
    case USCI_NONE:break;                             // Vector 0 - no interrupt
    case USCI_I2C_UCALIFG:break;                      // Interrupt Vector: I2C Mode: UCALIFG
    case USCI_I2C_UCNACKIFG:break;                    // Interrupt Vector: I2C Mode: UCNACKIFG
    case USCI_I2C_UCSTTIFG:break;                     // Interrupt Vector: I2C Mode: UCSTTIFG
    case USCI_I2C_UCSTPIFG:break;                     // Interrupt Vector: I2C Mode: UCSTPIFG
    case USCI_I2C_UCRXIFG:
        rx_val = UCB0RXBUF;
        if (RXByteCtr)
        {
          ReceiveBuffer[ReceiveIndex++] = rx_val;
          RXByteCtr--;
        }

        if (RXByteCtr == 1)
        {
          UCB0CTL1 |= UCTXSTP;
        }
        else if (RXByteCtr == 0)
        {
          UCB0IE &= ~UCRXIE;
          MasterMode = IDLE_MODE;
          __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
        }
        break;                      // Interrupt Vector: I2C Mode: UCRXIFG
    case USCI_I2C_UCTXIFG:
        switch (MasterMode)
        {
          case TX_REG_ADDRESS_MODE:
              UCB0TXBUF = TransmitRegAddr;
              if (RXByteCtr)
                  MasterMode = SWITCH_TO_RX_MODE;   // Need to start receiving now
              else
                  MasterMode = TX_DATA_MODE;        // Continue to transmission with the data in Transmit Buffer
              break;

          case SWITCH_TO_RX_MODE:
              UCB0IE |= UCRXIE;              // Enable RX interrupt
              UCB0IE &= ~UCTXIE;             // Disable TX interrupt
              UCB0CTL1 &= ~UCTR;            // Switch to receiver
              MasterMode = RX_DATA_MODE;    // State state is to receive data
              UCB0CTL1 |= UCTXSTT;          // Send repeated start
              if (RXByteCtr == 1)
              {
                  //Must send stop since this is the N-1 byte
                  while((UCB0CTL1 & UCTXSTT));
                  UCB0CTL1 |= UCTXSTP;      // Send stop condition
              }
              break;

          case TX_DATA_MODE:
              if (TXByteCtr)
              {
                  UCB0TXBUF = TransmitBuffer[TransmitIndex++];
                  TXByteCtr--;
              }
              else
              {
                  //Done with transmission
                  UCB0CTL1 |= UCTXSTP;     // Send stop condition
                  MasterMode = IDLE_MODE;
                  UCB0IE &= ~UCTXIE;                       // disable TX interrupt
                  __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
              }
              break;

          default:
              __no_operation();
              break;
        }
        break;                      // Interrupt Vector: I2C Mode: UCTXIFG
    default: break;
  }
}


