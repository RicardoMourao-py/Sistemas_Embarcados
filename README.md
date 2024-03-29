# Projeto Embarcados

Desenvolvendo um controle remoto.

<div align="center" style="max-width:68rem;">

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)

</div>

## Entrega 1

### Integrantes


<div align="center" style="max-width:68rem;">
<table>
  <tr>
    <td align="center"><a href="https://github.com/Adneycm"><img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/Adneycm" width="100px;" alt=""/><br /><sub><b>Adney Costa</b></sub></a><br /><a href="https://github.com/Adneycm" title="Adney Costa"></a> Developer</td>
   <td align="center"><a href="https://github.com/RicardoMourao-py"><img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/RicardoMourao-py" width="100px;" alt=""/><br /><sub><b>Ricardo Mourão</b></sub></a><br /><a href="https://github.com/RicardoMourao-py" title="Ricardo Mourão"></a>Developer</td>

  </tr>
</table>
</div>

### Ideia

<img src="doc/gif.gif" alt="jornada do usuario 1" width="1000"/>

Criar o controle para controlar o dinossauro do chrome. <br>
Link da aplicação: https://chromedino.com/ ou chrome://dino/

### Nome

DinoJump

### Usuários 

Este sistema embarcado consiste em ajudar usuários sem internet ou entediados. Foi escolhido o jogo do dino ("famoso jogo dos sem wifi"). Sendo assim, o usuário pode se debruçar na aplicação, consequentemente, trazendo o seu dia menos CHATO!  

### Software/Jogo 

O controle servirá para guiar o dinossauro.

### Jornada do usuários 
#### Jornada 1
<img src="doc/p1_a.png" alt="jornada do usuario 1" width="1000"/>

#### Jornada 2
<img src="doc/p1_b.png" alt="jornada do usuario 1" width="1000"/>

### Comandos/ Feedbacks

Comandos:

- Botão para pular (Digital)
- Botão para abaixar (Digital)
- Botão para iniciar/reiniciar a aplicação (Digital)
- Botão para ligar/desligar o controle embarcado (Digital)
- Controle de som do computador (Analógico)

Feedbacks:

- Led de ON / OFF

## In/OUT


|    Botão      |      Pino     |      Função   |
| ------------- | ------------- | ------------- |
| Azul  | PA6  | Pular |
| Verde  | PC19  | Iniciar/Reiniciar jogo |
| Vermelho  | PA2  | Abaixar |
| Liga/Desliga  | PA3  | Ligar/Desligar controle |


- Botão para pular (Digital) - Azul:
  - Usuário clica e o dino pula, acendendo o led para identificação.
- Botão para abaixar (Digital) - vermelho:
  - Usuário clica e o dino abaixa, acendendo o led(cor diferente) para identificação
- Botão para iniciar/reiniciar a aplicação (Digital)  - Verde
  - Usuário clica e volta a jogar o DinoJump.
- Botão para ligar/desligar o controle embarcado (Digital) - preto
  - Quando energizado procura por um aparelho para se conectar
  - Caso o usuário aperte o botão de ligar/desligar, o controle desconecta do aparelho conectado e entra em estado de economia de energia
  - Quando ligado novamente, ele procura outro dispositivo para se conectar.
- Controle de som do computador (Analógico)
  - Usuário resolve aumentar ou diminuir o volume da aplicação, aparecendo o vlume no output.
  
### Design

<img src="doc/controle.jpg" alt="design do controle" width="500"/>

### Rodando a Aplicação

Link: https://youtu.be/C_ZQ3qkdAFU
