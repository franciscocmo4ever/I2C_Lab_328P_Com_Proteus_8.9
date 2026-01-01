

I2C_Lab_328P_Com_Proteus_8.9








O que esse projeto faz
Ele cria um laboratório I2C com um único barramento (SDA/SCL) e dois dispositivos:
    1. DS1307 (RTC)
    • Guarda hora e data internamente.
    • Você pode gravar hora/data (no boot) e depois ler continuamente.
    2. PCF8574 (expansor I/O) usado como “ponte” pro LCD 20x4
    • O LCD não fala I2C direto.
    • O PCF8574 recebe 1 byte via I2C e vira sinais paralelos (RS, E, D4..D7, etc.)
Resultado: o ATmega328P conversa por I2C com os dois, e mostra hora/data no display (quando habilitado).

O que você aprende de I2C aqui (na prática)
1) Endereço 7-bit vs 8-bit (muita gente se confunde)
    • DS1307: 0x68 (7-bit)
        ◦ Write: 0xD0
        ◦ Read: 0xD1
Isso apareceu no teu analisador: D0 e D1.
2) Fluxo de ESCRITA no DS1307
A sequência é sempre:
    • START
    • SLA+W
    • Reg (ponteiro)
    • Dados...
    • STOP
No log:
S D0 A 00 A ... P
3) Fluxo de LEITURA (com Repeated Start)
Pra ler registradores “internos” do DS1307 você faz:
    • START
    • SLA+W
    • Reg (ponteiro)
    • Repeated START
    • SLA+R
    • Lê bytes...
    • NACK no último
    • STOP
No log:
S D0 A 00 A Sr D1 A ... N P
✅ Esse “Sr” é uma das coisas mais importantes pra entender.

Como o código está organizado (didático)
twi.c / twi.h (camada I2C baixa)
Aqui ficam as rotinas “cruas”:
    • twi_start(addr, read)
    • twi_write(byte)
    • twi_read_ack() / twi_read_nack()
    • twi_stop()
    • twi_status() (te dá o status 0x18, 0x28, 0x40, etc.)
Essa é a parte que realmente ensina I2C porque ela mexe direto nos registradores:
    • TWCR, TWDR, TWSR, TWBR
ds1307.c / ds1307.h (camada “média”)
Aqui você já trabalha com algo humano:
    • ds1307_set_datetime(&dt)
    • ds1307_read_datetime(&dt)
Ele faz por baixo:
    • aponta registrador 0x00
    • escreve ou faz repeated start e lê
    • converte BCD↔decimal
lcd_pcf8574.c / .h (camada “alta”)
Aqui você não aprende tanto I2C, mas aprende como um dispositivo I2C controla algo paralelo:
    • manda bytes no PCF8574
    • gera pulso no E
    • manda nibble alto/baixo (4-bit)

“Mudar valores no soft”
Isso é o bloco:
    • SET_CLOCK_ON_BOOT
    • SET_HOUR, SET_MIN, etc.
Quando SET_CLOCK_ON_BOOT = 1, ele grava no DS1307 logo ao iniciar (você viu isso no analisador).
Quando =0, ele só lê e mostra.

LCD_ENABLE (teste)
Esse recurso existe pra você testar I2C/RTC sem “bagunçar” o barramento com o LCD.
    • LCD_ENABLE 0 → nenhuma escrita no PCF8574 acontece
    • seu analisador fica “limpo” só com DS1307
