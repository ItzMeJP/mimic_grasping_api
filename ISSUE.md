Por algum motivo, plugins gerados por install não funcionam.
Apenas funcionam plugins que são retirados manualmente do build dir.
Tenho que melhorar como fazer o setup de instalação no CMake...

Já tive problemas de leak memory e resolvi re compilando o plugin...

Corrigir os asserts e exits usados. Melhor usar return false.

A ultima instrução dos scripts de arranque precisam ter &!!!

preciso fazer um script de arranque do 6dmimic remoto

Precisou criar um profile iilab nos seguintes modulos 6dmimic
VideoSync (copia do iilab profile)
CameraPlayer (copia do iilab profile)
VideoSync (copia do iilab profile)
Segmentation (copia do iilab profile)
Icosaedron 3D (copia iilab profile contudo com ficheiros de calib horizontal e vertical e tip diferentes)



Corrigir problema do load to localization.config

Apenas ONE_SHOOT estimation esta a funcionar

Preciso atualizar o firmware do manípulo para identificar os tipo de gripper com os nomes corretos!

Fazer a interface com firmware de ferramenta usando também plugins!