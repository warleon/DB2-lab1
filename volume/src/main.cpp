#include <iostream>

#include "p2.cpp"
#include "p3.cpp"
#include "p4.cpp"

int main() {
  std::cout << "Iniciando tests ...\n";
  std::cout << "Test2 tiene " << p2Test() << " errores\n";
  std::cout << "Test3 tiene " << p3Test() << " errores\n";
  std::cout << "Test4 tiene " << p4Test() << " errores\n";
  return 0;
}