# tp_synthese_autoradio

## 2 Le GPIO Expander et le VU-Metre

### 2.1 Configuration

1. GPIO Expander : **MCP23S17**
2. Sur le STM32, le SPI1 est déjà utilisé.  
   On utilse alors le **SPI3** en full-duplex master avec :
   - SCK -> PC10
   - MOSI -> PB5
   - MISO -> PC11
   - CS -> PB7
   - (A0 A1 A2 ) = 000
4. Paramètre pour configurer un SPI
     - Basic parameters
       - Frame Format : Motorola
       - Data Size : 8 bits
       - First Bit : MSB First
     - Clock Parameters
       - Prescaler -> Baudrate 10MHz
       - Clock Polarity : LOW (niveau logique au repos)
       - Clock Phase
