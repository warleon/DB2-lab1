#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

namespace P1 {

struct Alumno {
  char codigo[5];
  char nombre[11];
  char apellidos[20];
  char carrera[15];
};
ostream& operator<<(ostream& stream, Alumno& record) {
  stream.write(record.nombre, sizeof(record.nombre));
  stream.write(record.codigo, sizeof(record.codigo));
  stream.write(record.apellidos, sizeof(record.apellidos));
  stream.write(record.carrera, sizeof(record.carrera));
  stream << "\n";
  stream << flush;
  return stream;
}
istream& operator>>(istream& stream, Alumno& record) {
  stream.read(record.nombre, sizeof(record.nombre));
  stream.read(record.codigo, sizeof(record.codigo));
  stream.read(record.apellidos, sizeof(record.apellidos));
  stream.read(record.carrera, sizeof(record.carrera));
  stream.get();
  record.codigo[4] = '\0';
  record.nombre[10] = '\0';
  record.apellidos[19] = '\0';
  record.carrera[14] = '\0';
  return stream;
}

class FixedRecord {
  ofstream outFile;
  ifstream inFile;
  string fileName;

 public:
  FixedRecord(string fileName) : fileName(fileName) {}

  vector<Alumno> load() {
    inFile.open(fileName);
    Alumno flag;
    vector<Alumno> flags;
    while (inFile >> flag) {
      flags.push_back(flag);
    }
    inFile.close();
    return flags;
  }

  void add(Alumno record) {
    outFile.open(fileName, ios::app);
    outFile << record;
    outFile.close();
  }
  Alumno readRecord(int pos) {
    inFile.open(fileName);
    Alumno flag;
    inFile.seekg(pos * (sizeof(flag) + 1), ios::beg);
    inFile >> flag;
    return flag;
  }
};
void readFromConsole(char buffer[], int size) {
  string temp;
  getline(cin, temp);
  for (size_t i = 0; i < (size_t)size; i++)
    buffer[i] = (i < temp.size()) ? temp[i] : ' ';
  buffer[size - 1] = '\0';
  cin.clear();
}

int tests() {
  cout << "Antes del nuevo alumno" << endl;
  FixedRecord f("/data/datos12.txt");
  vector<Alumno> vec = f.load();
  for (size_t i = 0; i < vec.size(); i++)
    cout << vec[i].codigo << " " << vec[i].nombre << " " << vec[i].apellidos
         << " " << vec[i].carrera << endl;

  Alumno nuevo;
  cout << "Agregar un alumno" << endl;
  cout << "Ingresar codigo: ";
  readFromConsole(nuevo.codigo, sizeof(nuevo.codigo));
  cout << "Ingresar nombre: ";
  readFromConsole(nuevo.nombre, sizeof(nuevo.nombre));
  cout << "Ingresar apellidos: ";
  readFromConsole(nuevo.apellidos, sizeof(nuevo.apellidos));
  cout << "Ingresar carrera: ";
  readFromConsole(nuevo.carrera, sizeof(nuevo.carrera));
  f.add(nuevo);

  cout << "Despues de nuevo alumno" << endl;
  vec = f.load();
  for (size_t i = 0; i < vec.size(); i++)
    cout << vec[i].codigo << " " << vec[i].nombre << " " << vec[i].apellidos
         << " " << vec[i].carrera << endl;

  int pos;
  cout << "Obtener registro en la posición: ";
  cin >> pos;
  Alumno r = f.readRecord(pos);
  cout << r.codigo << " " << r.nombre << " " << r.apellidos << " " << r.carrera
       << endl;
  return 0;
}

}  // namespace P1