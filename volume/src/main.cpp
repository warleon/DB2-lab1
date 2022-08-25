#include <iostream>

#include "p2.cpp"
//#include "p3.cpp"
#include "p4.cpp"

int main() {
  std::cout << "Iniciando tests ...\n";
  // std::cout << "Test p2 ...\n";
  // int p2Err = P2::tests();
  // std::cout << "Test2 tiene " << p2Err << " errores\n";
  // std::cout << "Test p3 ...\n";
  // int p3Err = p3Test();
  // std::cout << "Test3 tiene " << p3Err << " errores\n";
  std::cout << "Test p4 ...\n";
  int p4Err = P4::tests();
  std::cout << "Test4 tiene " << p4Err << " errores\n";
  return 0;
}