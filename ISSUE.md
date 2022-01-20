Por algum motivo, plugins gerados por install não funcionam.
Apenas funcionam plugins que são retirados manualmente do build dir.
Tenho que melhorar como fazer o setup de instalação no CMake...

Já tive problemas de leak memory e resolvi re compilando o plugin...

Corrigir os asserts e exits usados. Melhor usar return false.

com o obj_recognition sem photoneo eu nao tenho problemas em dar finish, ou seja, não tenho problemas em dar close na pipeline
contudo no photoneo_obj_recognition tenho problemas -> tenho que examinar o plugin

preciso fazer um script de arranque do 6dmimic remoto e que o processo seja reconhecido, por hora adaptei um processo VOID em lazarus e, o 6dmimic precisa arrancar manualmente

Primeiro rode e de start no 6dmimic e depois rode o qtcreator

Peeciso testar as posiçoies degrasp geradas
