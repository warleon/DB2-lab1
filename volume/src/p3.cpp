#include <iostream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
struct Alumno
{  
  string Nombre;
  string Apellidos;
  string Carrera;
  float mensualidad;
};
ostream &operator<<(ostream &stream, Alumno &alumno)
{
    stream << alumno.Nombre << '|';
    stream << alumno.Apellidos << '|';
    stream << alumno.Carrera << '|';
    stream << to_string(alumno.mensualidad);
    stream << "\n";
    stream << flush;
    return stream;
}

class VariableRecord{
  string filename;

  VariableRecord(string filename){
    this->filename=filename;
  }
  vector<Alumno> load(){
     
      
      while(getline)
    
  }
  void add(Alumno record)
    {
        ofstream File(this->filename, ios::app | ios::out);
        if (!File.is_open())
            cout << "No se pudo abrir el archivo. \n";

        File << record;

        File.close();
    }

};



int main() {
  std::cout << "Hello World!\n";
}

int p3Test() {
  int errors = 0;
  return errors;
}
