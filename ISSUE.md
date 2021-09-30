Por algum motivo, plugins gerados por install não funcionam.
Apenas funcionam plugins que são retirados manualmente do build dir.
Tenho que melhorar como fazer o setup de intalação no CMake...

Já tive problemas de leak memory e resolvi re compilando o plugin...

Corrigir os asserts e exits usados. Melhor usar return false.

Corrigir o erro caso o object localization retorne error ou abort
