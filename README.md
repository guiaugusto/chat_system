# Chat FSO

## Informações sobre a implementação do Protocolo

&emsp;&emsp;Segue os tópicos solicitados que já estão implementados no projeto.

1. **Criação do Chat**: É possível criar uma chat personalizado com seu nome/apelido (chat-NAME).

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59007715-0c574680-87fe-11e9-9e2e-4b72fb779a14.png)

2. **Permissões**: Após a criação da fila do próprio usuário é lhe concedido permissão de leitura e escrita para sua própria fila. Para os demais usuários é dado somente a permissão para escrita.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59007785-5d673a80-87fe-11e9-96ab-686630890c85.png)


3. **Formato da mensagem**: Seguindo o solicitado, para enviar a mensagem deve-se escrever no seguinte formato _DE:PARA:MENSAGEM_. Onde o programa enviará a mensagem ao usuário que terá seu nome substituído no campo "PARA". Sendo então mostrado ao destinatário a mensagem no formato _REMETENTE: MENSAGEM_.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59007841-97d0d780-87fe-11e9-92e4-31ba10cbcd99.png)

&emsp;&emsp;**OBS 1**: Sendo a função de verificação de mensagem na fila é **blocante**, foi criado uma **thread** para que não haja disputa de prioridade e trave o programa enquanto o usuário não receba uma mensagem.

4. **Fila inexistente**: Caso o destinatário da mensagem não exista, será mostrado ao remetente a mensagem _UNKNOWUSER <DESTINATÁRIO>_.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008014-66a4d700-87ff-11e9-9356-ebf1aa78be0d.png)

5. **Retry**: Ao enviar uma mensagem, a fila do destinatário esteja cheia, ou por algum motivo de conexão não foi possível conectar a fila, são efetuadas três outras tentativas que espera um certo tempo para tentar reenviá-la novamente, isso tudo acontecendo em uma nova _thread_.

6. **Usuários onlines**: Um usuário é considerado _online_ quando sua fila está criada. Então, foi criado um comando chamado _List_ que quando acionado, ele lista todos os usuários que estão ativos naquele momento. Facilitando então o envio da mensagem para quem o usuário deseja.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008080-97850c00-87ff-11e9-9594-ff97bd27024c.png)

7. **Broadcast**: Ao digitar _DE:ALL:MENSAGEM_, a mensagem é enviada para todos os usuários que estão onlines naquele momento, aparecendo aos destinatários a seguinte mensagem _Broadcast de <REMETENTE>: MENSAGEM_.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008103-b4b9da80-87ff-11e9-964c-cdf03308bcf6.png)

8. **Terminar execução**: Foi bloqueado o comando Ctrl+C para finalizar a execução do programa. Assim, quando o usuário tenta finalizar, é apresentado uma mensagem solicitando que digite "Sair" para a finalizar o programa.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008152-df0b9800-87ff-11e9-9c8e-fe923242cff5.png)

## Bonus

1. Foram acrescentadas cores aos textos para diferenciar cada ação do chat.

&emsp;Segue então as cores do chat:

* **Verde**: Mensagem do usuário remetente
* **Rosa**: Mensagem do usuário destinatário
* **Amarelo**: Mensagem de validações, UNKNOWUSER, Mensagem vazia, etc.
* **Vermelho**: Mensagem de aviso, "Digite o comando sair para finalizar a execução"


2. Caso o usuário esteja com alguma dúvida sobre o que ele pode fazer, ele poderá digitar o _help_ que o auxiliará em como utilizar o programa.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008223-390c5d80-8800-11e9-98c8-98c5e6126db0.png)

3. E para saber mais sobre o estado da sua fila, basta o usuário digitar stats que conseguirá ver algumas informações sobre sua fila.

&emsp;&emsp;&emsp;![image](https://user-images.githubusercontent.com/26297247/59008268-635e1b00-8800-11e9-87f2-322a66780268.png)

## Problemas conhecidos

1. Quando você está digitando uma mensagem e recebe uma mensagem do _broadcast_, a mensagem a qual você está escrevendo é resetada.


## Dificuldades de implementação das Threads

1. Foi uma implementação um pouco mais tranquila, baseada no estudo que os membros do projeto já estavam fazendo. Então a maior dificuldade foi quando necessário fazer o **Retry** da mensagem, tendo que reenviá-la três vezes.

## Comandos para executar

1. Execute o comando seguinte para compilar e executar o código:
```
make run-all
```

&emsp;&emsp;Com o comando anterior você já consegue criar seu próprio chat e conversa com outras pessoas.

2. Caso queira abrir novos chats, não há necessidade de executar tudo novamente, então rode somente o seguinte comando em uma outra aba do terminal:
```
make run
```
