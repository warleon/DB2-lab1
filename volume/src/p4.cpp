#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace P4 {

typedef struct {
  int ciclo;
  float mensualidad;
  std::string codigo;
  std::string observaciones;
} Matricula;
typedef struct {
  int g;
  int size;
  int fieldSize[2];
} Metadata;

class Database {
  static const auto bin = std::fstream::binary;
  static const auto in = std::fstream::in;
  static const auto out = std::fstream::out;
  static const auto app = std::fstream::app;

  std::string filename;
  std::string metaname;

  Matricula parse(Metadata met, char* data) {}
  char* dump(Metadata met, Matricula data) {
    char* buff = new char[met.size];
    memcpy(buff, &data.ciclo, sizeof(int));
    memcpy(buff + offsetof(Matricula, mensualidad), &data.mensualidad,
           sizeof(float));
    memcpy(buff + offsetof(Matricula, codigo), data.codigo.c_str(),
           met.fieldSize[0]);
    memcpy(buff + offsetof(Matricula, codigo) + met.fieldSize[0],
           data.observaciones.c_str(), met.fieldSize[1]);
    return buff;
  }
  Metadata getSomeMeta(Matricula matri) {
    Metadata meta;
    meta.fieldSize[0] = matri.codigo.size();
    meta.fieldSize[1] = matri.observaciones.size();
    meta.size =
        sizeof(int) + sizeof(float) + meta.fieldSize[0] + meta.fieldSize[1];
    return meta;
  }

 public:
  Database(std::string filename_)
      : filename(filename_), metaname(filename_ + ".meta") {}
  std::vector<Matricula> load() {
    std::fstream file(filename, bin | in);
    std::fstream metafile(metaname, bin | in);
  }
  void add(Matricula record) {
    std::fstream file(filename, bin | out);
    file.seekp(0, std::ios::end);
    std::fstream metafile(metaname, bin | out | app);
    Metadata metaRecord = getSomeMeta(record);
    metaRecord.g = file.tellp();
    metafile.write((char*)&metaRecord, sizeof(Metadata));
    metafile.close();
    auto data = dump(metaRecord, record);
    file.write(data, metaRecord.size);
    file.close();
    delete[] data;
  }
  Matricula readRecord(int pos) {
    std::fstream file(filename, bin | in);
    std::fstream metafile(metaname, bin | in);
  }
};

int tests() {
  int errors = 0;
  return errors;
}
}  // namespace P4