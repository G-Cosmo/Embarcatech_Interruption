# Embarcatech_Interruption
Repositório criado para a tarefa relacionada a interrupção com push button e matriz de leds.


O programa implementa um contador utilizando a matriz de LEDs WS2818B presente na placa BitDogLab.
Cada vez que o botão A é pressionado, a rotina de interrupção é acionada, e o contador é incrementado em 1.
Analogamente, cada vez que o botão B é pressionado, o contador é decrementado em 1. Além disso, o LED RGB 
pisca na cor vermelha cinco vezes por segundo.

# Instruções de compilação

Para compilar o código, são necessárias as seguintes extensões: 

*Wokwi Simulator*

*Raspberry Pi Pico*

*Cmake*

Após instalá-las basta buildar o projeto pelo CMake. A partir daí, abra o arquivo 
diagram.json e clique no botão verde para iniciar a simulação.

Enquanto na simulação, o usuário pode clicar nos dois botões presentes no circuito.

O botão inferior faz a matriz de leds incrementar 1, e o botão superior faz ela decrementar 1.

# Vídeo demonstrativo

https://youtube.com/shorts/xs4nkv09dj0?si=2qtVuS7WEPEJE6J-
