#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Alumno {
  string Nombre;
  string Apellidos;
  string Carrera;
  float mensualidad;
};

ostream &operator<<(ostream &stream, Alumno &p) {
  stream << p.Nombre << '|';
  stream << p.Apellidos << '|';
  stream << p.Carrera << '|';
  stream << to_string(p.mensualidad);
  stream << "\n";
  stream << flush;
  return stream;
}

class VariableRecord {
 private:
  string filename;

 public:
  VariableRecord(string filename) 
  { this->filename = filename; }

  vector<Alumno> load() {
    ifstream File(this->filename, ios::in);
    vector<Alumno> alumnos;
    if (!File.is_open()) cout << "No se pudo abrir el archivo. \n";
    string l;
    getline(File,l);
    while (getline(File, l)) {
      char *data = const_cast<char *>(l.c_str());
      data = strtok(data, "|");
      Alumno record;

      record.Nombre = data;
      data = strtok(NULL, "|");

      record.Apellidos = data;
      data = strtok(NULL, "|");

      record.Carrera = data;
      data = strtok(NULL, "|");

      record.mensualidad = atof(data);
      data = strtok(NULL, "|");

      alumnos.push_back(record);
    }

    return alumnos;
  }
  void add(Alumno record) {
    ofstream File(this->filename, ios::app | ios::out);
    if (!File.is_open()) cout << "No se pudo abrir el archivo. \n";

    File << record;

    File.close();
  }

  Alumno readRecord(int pos) {
    ifstream File(this->filename, ios::in);
    if (!File.is_open()) cout << "No se pudo abrir el archivo. \n";

    string line;

    getline(File, line);

    for (int i = 0; i < pos; ++i) getline(File, line);

    Alumno record;
    char *data = const_cast<char *>(line.c_str());

    record.Nombre = strtok(data, "|");
    record.Apellidos = strtok(NULL, "|");
    record.Carrera = strtok(NULL, "|");
    record.mensualidad = atoi(strtok(NULL, "|"));

    return record;
  }
};

void tests(){
  VariableRecord file("datos3.txt");
  Alumno alumno1={"x","y","a",0};
  Alumno alumno2={"x1","y1","a1",1};
  file.add(alumno1);
  file.add(alumno2);
  Alumno x = file.readRecord(1);
  for(auto i:file.load()){
    cout<<i.Nombre<<" "<<i.Apellidos<<" "<<i.Carrera<<" "<<i.mensualidad<<endl;
  }
}


int main() {
   tests();
}
